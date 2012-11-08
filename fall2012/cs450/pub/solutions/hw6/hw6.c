#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include "hw6.h"

float rtt;
float deviation;
int timeout;

struct sockaddr_in peeraddr;

int sequence_number;
int expected_sequence_number = 0;
int last_ack_number = -1;

#define MAX_WINDOW 1000
#define MAX_TIMEOUT 2000
int window_size = 1;

int congestion_avoidance = 0;
int ca_last_increase = 0;
int ssthresh = 1;

int packets_outstanding = 0;

struct packet {
	struct hw6_hdr *data;
	unsigned sent_at;
	int retx;
	int len; 
};
struct packet window[MAX_WINDOW];

/* add packet to tail of queue */
int push_packet(struct hw6_hdr* data, int len, int sent_at) {
	window[packets_outstanding].data=malloc(len); 	// freed in pop_packet
	memcpy(window[packets_outstanding].data,data,len);	
	window[packets_outstanding].len = len;
	window[packets_outstanding].sent_at = sent_at;
	window[packets_outstanding].retx = 0;
	packets_outstanding++;
}

void resend_from_tail(int sock) {
	if(packets_outstanding >= window_size) {
		fprintf(stderr,"Resending Packet %d (from tail) with rtt %f dev %f timeout %d ms window %d  to %s         \n",ntohl(window[window_size-1].data->sequence_number),rtt, deviation,timeout,window_size,inet_ntoa(peeraddr.sin_addr));
		if(sendto(sock, window[window_size-1].data, window[window_size-1].len, 0,(struct sockaddr*)&peeraddr,sizeof(peeraddr)) <=0)
			perror("Couldn't send!");
		window[window_size-1].retx = 1;
		window[window_size-1].sent_at = current_msec(); // update the timestamp
	}
}

/* remove packet from head of queue */
void pop_packet(int sock) {
	
	if(packets_outstanding > 0)
		free(window[0].data);

	for(int i=0;i<packets_outstanding-1;i++) 
		window[i]=window[i+1];		
	packets_outstanding--;

	// we just made space for another transmission
	resend_from_tail(sock);

	/* window size management a'la TCP slow-start */
	if(!congestion_avoidance) {
		window_size++;
		resend_from_tail(sock); // if we grew the window, there's now more room
	}
	else { 
		if (current_msec() - ca_last_increase > rtt) {
			ca_last_increase = current_msec();
			window_size++;
			resend_from_tail(sock);  // if we grew the window, there's now more room
		}
	}
		
	if(window_size > MAX_WINDOW)
			window_size = MAX_WINDOW;
}



int min(int a, int b){
	return a<b?a:b;
}

unsigned timeval_to_msec(struct timeval *t) { 
	return t->tv_sec*1000+t->tv_usec/1000;
}

void msec_to_timeval(unsigned millis, struct timeval *out_timeval) {
	out_timeval->tv_sec = millis/1000;
	out_timeval->tv_usec = (millis%1000)*1000;
}

unsigned current_msec() {
	struct timeval t;
	gettimeofday(&t,0);
	return timeval_to_msec(&t);
}

/* updates rtt and deviation estimates based on new sample */
void update_rtt(int this_rtt) {
	deviation = 0.7*deviation + 0.3*(fabs(this_rtt - rtt));
	rtt = 0.75*rtt + 0.25*this_rtt;
	timeout = (int)(2*rtt+4*deviation);
}

int rel_connect(int socket,struct sockaddr_in *toaddr,int addrsize) {
		 peeraddr=*toaddr;
		 return 0;
}

int rel_rtt(int sock) {
		 return rtt;
}

void wait_for_ack(int sock) {

	// repeatedly send it until we get an ack
	while(1) {
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(sock,&readset); 

		struct timeval t;
		if(window[0].sent_at + timeout > current_msec()) {			
			unsigned msec_until_expiry = window[0].sent_at + timeout - current_msec(); 
			msec_to_timeval(msec_until_expiry,&t);
		}
		else {
			msec_to_timeval(10,&t);
		}

		/*		// always allow a 1 msec wait
		if(msec_until_expiry<0) {
			t.tv_sec = 0;
			t.tv_usec = 100;
			//			memset(&t,0,sizeof(t));
			}*/

		int rdy = select(FD_SETSIZE,&readset,0,0,&t);

		char incoming[1400];
		struct hw6_hdr *hdr = (struct hw6_hdr*)incoming;
		if(rdy>0) {
			struct sockaddr_in from_addr;
			unsigned int addrsize = sizeof(from_addr);
			int recv_count=recvfrom(sock,incoming,1400,0,(struct sockaddr*)&from_addr,&addrsize);
			if(recv_count<0) {
				perror("When receiving packet.");
				return;
			}
		}

		// if we timed out, or got a double acknowledgment, double the timeout value and send again
		if(rdy==0 || 
			 // double acknowledgment only triggers retransmission once per round-trip time
			 (ntohl(hdr->ack_number)==ntohl(window[0].data->sequence_number)-1 && 
				((current_msec() - window[0].sent_at) > rtt))) { //last_ack_number) {
			if(rdy==0)
				fprintf(stderr,"\nTimed out on packet %d, msec %u\n",ntohl(window[0].data->sequence_number),timeval_to_msec(&t));//,msec_until_expiry);
			else
				fprintf(stderr,"\nDouble ack indicating loss of packet %d, msec %u\n",ntohl(window[0].data->sequence_number),timeval_to_msec(&t));//,msec_until_expiry);

			fprintf(stderr,"Window packets: ");
			for(int p=0;p<packets_outstanding;p++) {
				fprintf(stderr,"%d, ",ntohl(window[p].data->sequence_number));				
			}
			fprintf(stderr,"\n");

			timeout *= 2;
			if(timeout > MAX_TIMEOUT) 
				timeout = MAX_TIMEOUT;

			window_size /=2;
			if(window_size < 1) 
				window_size=1;
			
			ssthresh = window_size;
			congestion_avoidance = 1;
			ca_last_increase = current_msec();

			for(int i=0;i<packets_outstanding && i<window_size;i++)  {
				fprintf(stderr,"Resending Packet %d with rtt %f dev %f timeout %d ms window %d  to %s         \n",ntohl(window[i].data->sequence_number),rtt, deviation,timeout,window_size,inet_ntoa(peeraddr.sin_addr));
				if(sendto(sock, window[i].data, window[i].len, 0,(struct sockaddr*)&peeraddr,sizeof(peeraddr)) <=0)
						perror("Couldn't send!");
				window[i].retx = 1;
				window[i].sent_at = current_msec(); // update the timestamp
			}
			fprintf(stderr,"Done resending, for now\n");

		}
		else if(rdy==-1) {
			perror("select error");
		}
		else {
			
			/* blow away all the packets acked here */
			// fprintf(stderr,"Got ack for %d, time left %d, rtt %d, retx %d\n",ntohl(hdr->ack_number), timeval_to_msec(&t), current_msec() - window[0].sent_at, window[0].retx);
			int got_ack=0;
			last_ack_number = ntohl(hdr->ack_number);
			while(packets_outstanding > 0 && ntohl(hdr->ack_number) >= ntohl(window[0].data->sequence_number)) {
				if(!window[0].retx)
					update_rtt(current_msec() - window[0].sent_at);
				pop_packet(sock);

				got_ack=1;
			}

			if(got_ack) break;
			
			if(! (hdr->flags & ACK)) {
				// ack whatever we have so far
				struct hw6_hdr ack;
				ack.flags = ACK;
				if(ntohl(hdr->sequence_number) == expected_sequence_number) {
					expected_sequence_number++;
				}
				else {
					fprintf(stderr,"Unexpected non-ACK in rel_send(). Acking what we have so far. \n");
				}
				ack.ack_number = htonl(expected_sequence_number-1);
				sendto(sock, &ack, sizeof(ack), 0,(struct sockaddr*)&peeraddr,sizeof(peeraddr));
			}		 
		}
	}

}

void rel_send(int sock, void *buf, int len)
{
	while(packets_outstanding>=window_size) {
		wait_for_ack(sock);
	}
	
 	// make the packet = header + buf	
	char packet[1400];
	struct hw6_hdr *hdr = (struct hw6_hdr*)packet;
	memset(hdr,0,sizeof(struct hw6_hdr));
	hdr->sequence_number = htonl(sequence_number);
	memcpy(hdr+1,buf,len);
	sequence_number++;

	push_packet(hdr,sizeof(struct hw6_hdr)+len,current_msec());
	sendto(sock, packet, sizeof(struct hw6_hdr)+len, 0,(struct sockaddr*)&peeraddr,sizeof(peeraddr));
	//	fprintf(stderr,"Packet %d with rtt %f dev %f wsize %d timeout %d ms     to %s      \n",sequence_number-1,rtt, deviation,window_size,timeout,inet_ntoa(peeraddr.sin_addr));
}

int rel_socket(int domain, int type, int protocol) {
	/* start out with large timeout and rtt values */
	rtt = 300;
	deviation = 100;
	timeout = 1000;
	sequence_number = 0;

	return socket(domain, type, protocol);
}

int rel_recv(int sock, void * buffer, size_t length) {
	char packet[MAX_PACKET];
	memset(&packet,0,sizeof(packet));
	struct hw6_hdr* hdr=(struct hw6_hdr*)packet;	

	while(1) {
		// remember these so that we can close an incoming socket as well as outgoing sockets
		unsigned int addrlen=sizeof(peeraddr);

		int recv_count = recvfrom(sock, packet, MAX_PACKET, 0, (struct sockaddr*)&peeraddr, &addrlen);		
		if(recv_count<0) { break; }

		if(ACK & hdr->flags) {
			continue;
		}
		
		//		fprintf(stderr,"Got packet %d\n",ntohl(hdr->sequence_number));		 

 		// if we got the expected packet, send an ACK and return data
 		if(ntohl(hdr->sequence_number) == expected_sequence_number) {			
			//			fprintf(stderr,"Got %d expected %d, size %d\n",ntohl(hdr->sequence_number),expected_sequence_number,recv_count);
 			struct hw6_hdr ack;
			ack.flags = ACK;
			ack.ack_number = hdr->sequence_number;
			
 			sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)&peeraddr, addrlen);
 			
 			expected_sequence_number++;
 			memcpy(buffer, packet+sizeof(struct hw6_hdr), recv_count-sizeof(struct hw6_hdr));
 			return recv_count - sizeof(struct hw6_hdr);
 		}
		else {
			// ack whatever we have so far			
			struct hw6_hdr ack;
			ack.flags = ACK;
			ack.ack_number = htonl(expected_sequence_number-1);			
			sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)&peeraddr, addrlen);	 
		}
	}
}


int rel_close(int sock) {

	// an empty packet signifies end of file
	rel_send(sock,0,0);
	
	while(packets_outstanding > 0) {
		wait_for_ack(sock);
	}

	fprintf(stderr,"\nSent EOF. Now in final wait state.\n");

	struct timeval t;
	t.tv_sec = 2;
	t.tv_usec = 0;

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
	int wait_start = current_msec();
	
	// wait for 5 seconds
	while(current_msec() - wait_start < 5000) {		
		rel_recv(sock,0,0);
	}

	close(sock);
}

