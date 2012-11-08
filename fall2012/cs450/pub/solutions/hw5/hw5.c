#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <math.h>
#include "hw5.h"

unsigned int rtt;
unsigned int deviation;
struct timeval timeout;

struct sockaddr_in peeraddr;

int sequence_number;
int expected_sequence_number = 0;

int min(int a, int b){
	return a<b?a:b;
}

unsigned timeval_to_msec(struct timeval *t) { 
	return t->tv_sec*1000+t->tv_usec/1000;
}

void msec_to_timeval(int millis, struct timeval *out_timeval) {
	out_timeval->tv_sec = millis/1000;
	out_timeval->tv_usec = (millis%1000)*1000;
}

unsigned current_msec() {
	struct timeval t;
	gettimeofday(&t,0);
	return timeval_to_msec(&t);
}

/* updates rtt and deviation estimates based on new sample */
void update_rtt(unsigned this_rtt) {
	// if this is the first packet, make an 'educated guess' as to the rtt and deviation
	if(sequence_number==0) {
		rtt = this_rtt;
		deviation = this_rtt/2;
	}
	else {
	  deviation = 0.7*deviation + 0.3*(abs(this_rtt - rtt));
	  rtt = 0.8*rtt + 0.2*this_rtt;
	}
	msec_to_timeval(rtt+4*deviation,&timeout);
}

int rel_connect(int socket,struct sockaddr_in *toaddr,int addrsize) {
		 peeraddr=*toaddr;
		 return 0;
}

int rel_rtt(int sock) {
		 return rtt;
}

void rel_send(int sock, void *buf, int len)
{
 	// make the packet = header + buf
	char packet[1400];
	struct hw5_hdr *hdr = (struct hw5_hdr*)packet;
	memset(hdr,0,sizeof(struct hw5_hdr));
	hdr->sequence_number = htonl(sequence_number);
	memcpy(hdr+1,buf,len);

	fprintf(stderr,"\rPacket %d with rtt %d dev %d timeout %d ms           \n",sequence_number,rtt, deviation,timeval_to_msec(&timeout));
	sendto(sock, packet, sizeof(struct hw5_hdr)+len, 0,(struct sockaddr*)&peeraddr,sizeof(peeraddr));
	int retx = 0;
	// repeatedly send it until we get an ack
	while(1) {
		int sent_time = current_msec();
		
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(sock,&readset); 

		struct timeval t = timeout; // select changes the timeout parameter on some platforms, so make a copy

		int rdy = select(FD_SETSIZE,&readset,0,0,&t);

		if(rdy==0) {
			// if we timed out, send again double the timeout value
			msec_to_timeval(min(5000,2*timeval_to_msec(&timeout)), &timeout);
			fprintf(stderr,"\rPacket %d with rtt %d dev %d timeout %d ms           ",sequence_number,rtt, deviation,timeval_to_msec(&timeout));
			sendto(sock, packet, sizeof(struct hw5_hdr)+len, 0,(struct sockaddr*)&peeraddr,sizeof(peeraddr));
			retx=1;
		}
		else if(rdy==-1) {
			perror("select error");
		}
		else {
			char incoming[1400];
			struct sockaddr_in from_addr;
			unsigned int addrsize = sizeof(from_addr);
			int recv_count=recvfrom(sock,incoming,1400,0,(struct sockaddr*)&from_addr,&addrsize);
			if(recv_count<0) {
				perror("When receiving packet.");
				return;
			}
			
			struct hw5_hdr *hdr = (struct hw5_hdr*)incoming;
			// if this is an ack for our present packet, update the rtt and exit
			if(ntohl(hdr->ack_number) == sequence_number) {
				if(!retx)
					update_rtt(current_msec() - sent_time);
				sequence_number++;
				break;
			}
			
			// if it's not an ack, it's the end of the stream. ACK it. 
			if(! (hdr->flags & ACK)) {
				// ack whatever we have so far
				struct hw5_hdr ack;
				ack.flags = ACK;
				if(ntohl(hdr->sequence_number) == expected_sequence_number) {
					expected_sequence_number++;
				}
				else {
					fprintf(stderr,"Unexpected non-ACK in rel_send(), size %d. Acking what we have so far. \n",recv_count);
				}
				ack.ack_number = htonl(expected_sequence_number-1);
				sendto(sock, &ack, sizeof(ack), 0,(struct sockaddr*)&peeraddr,sizeof(peeraddr));
			}		 
		}
	}
}

int rel_socket(int domain, int type, int protocol) {
	/* start out with large timeout and rtt values */
	rtt = 500;
	deviation = 50;
	timeout.tv_sec = 0; 
	timeout.tv_usec = 700000; // rtt + 4*deviation ms
	sequence_number = 0;

	return socket(domain, type, protocol);
}

int rel_recv(int sock, void * buffer, size_t length) {
	char packet[MAX_PACKET];
	memset(&packet,0,sizeof(packet));
	struct hw5_hdr* hdr=(struct hw5_hdr*)packet;	

	while(1) {
		// remember these so that we can close an incoming socket as well as outgoing sockets
		unsigned int addrlen=sizeof(peeraddr);

		int recv_count = recvfrom(sock, packet, MAX_PACKET, 0, (struct sockaddr*)&peeraddr, &addrlen);		
		if(recv_count<0) { break; }

 		// if we got the expected packet, send an ACK and return data
 		if(ntohl(hdr->sequence_number) == expected_sequence_number) {
 						
 			struct hw5_hdr ack;
			ack.flags = ACK;
			ack.ack_number = hdr->sequence_number;
 			sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)&peeraddr, addrlen);
 			
 			expected_sequence_number++;
//			fprintf(stderr,"Next sequence number: %d                  \r\n",expected_sequence_number);
 			
 			memcpy(buffer, packet+sizeof(struct hw5_hdr), recv_count-sizeof(struct hw5_hdr));
 			return recv_count - sizeof(struct hw5_hdr);
 		}
		else {
			// ack whatever we have so far
			struct hw5_hdr ack;
			ack.flags = ACK;
			ack.ack_number = htonl(expected_sequence_number-1);
			sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)&peeraddr, addrlen);	 
		}
	}
}


int rel_close(int sock) {

	// an empty packet signifies end of file
	rel_send(sock,0,0);

	fprintf(stderr,"\nSent EOF. Now in final wait state.\n");

	struct timeval t;
	t.tv_sec = 2;
	t.tv_usec = 0;

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
	int wait_start = current_msec();
	
	// wait for 2 seconds
	while(current_msec() - wait_start < 2000) {		
		rel_recv(sock,0,0);
	}

	close(sock);
}

