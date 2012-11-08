#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <openssl/sha.h>
#include<bencodetools/bencode.h>
#include<curl/curl.h>
#include<arpa/inet.h>
#include<netinet/in.h>


#include"hw4.h"

#ifndef FD_COPY
#define FD_COPY(f, t)   (void)(*(t) = *(f))
#endif

// computed in main(). This is where we get our peers from.
char announce_url[255];



struct piece_state { 
	//enum {PIECE_AVAILABLE=0, PIECE_PENDING=1, PIECE_FINISHED=2} status;
	int downloaders; 
	unsigned int have_up_to; // bytes already downloaded 
	int announced;
} *piece_state;


#define BUFSIZE piece_length*2
struct peer_state *peers=0;

int debug=1;  //set this to zero if you don't want all the debugging messages 

char screenbuf[10000];

fd_set readset, writeset, errorset;

void print_bencode(struct bencode*);
void start_peers();

// The SHA1 digest of the document we're downloading this time. 
// using a global means we can only ever download a single torrent in a single process. That's ok.
unsigned char digest[20];

// we generate a new random peer id every time we start
char peer_id[21]="-UICCS450-";
struct bencode_dict *torrent;
struct bencode *info;

int file_length=0;
int piece_length;

int active_peers() {
	struct peer_state *peer = peers;
	int count=0;
	while(peer) {
		if(peer->connected && !peer->choked)
			count++;
		peer = peer->next;
	}
	return count;
}

int choked_peers() {
	struct peer_state *peer = peers;
	int count=0;
	while(peer) {
		if(peer->connected && peer->choked)
			count++;
		peer = peer->next;
	}
	return count;
}

int peer_connected(in_addr_t addr) {
	struct peer_state *peer = peers;
	while(peer) {
		if(peer->ip == addr && peer->connected==1) {
			return 1;
		}
		peer = peer->next;
	}	
	return 0;
}

void* draw_state() {
		printf("\033[2J\033[1;1H");
		int pieces = file_length/piece_length+1;

		printf("%d byte file, %d byte pieces, %d pieces, %d active, %d choked\n",file_length,piece_length,file_length/piece_length+1,active_peers(),choked_peers());
		for(int i=0;i<pieces;i++) {
			if(i%80 == 0) printf("\n");

			if(piece_state[i].downloaders == 0) {
				if(piece_state[i].announced) printf("X");
				else if(piece_state[i].have_up_to) printf(":"); 
				else printf("."); 
			}
			else 
				printf("%d",piece_state[i].downloaders); 
		}
		printf("\n\n");
		for(int i=0;i<pieces;i++) {
			if(i%10 == 0) printf("\n");
			if(!piece_state[i].announced) 
				printf("%d: %d, ",i,peers_have_piece(i));
		}
		fflush(stdout);
}

int add_downloader(int piece) {
	piece_state[piece].downloaders++;
	draw_state();

}

int sub_downloader(int piece) {
		if(piece_state[piece].downloaders < 1) {
			printf("Weird. Removing downloader for piece %d, but there were no downloaders...\n",piece);
			exit(1);
		}
		if(!piece_state[piece].announced)
			piece_state[piece].downloaders--;
		draw_state();
}

int missing_blocks() {
	int count=0;

	for(int i=0;i<file_length/piece_length+((file_length%piece_length)>0?1:0);i++) {
		if(!piece_state[i].announced) {
			count++;
		}
	}
	return count;
}

void shutdown_peer(struct peer_state *peer) {
	FD_CLR(peer->socket,&readset);
	FD_CLR(peer->socket,&writeset);
	FD_CLR(peer->socket,&errorset);
	peer->connected = 0;
	close(peer->socket);
	if(peer->requested_piece!=-1) 
		sub_downloader(peer->requested_piece);
	peer->socket = 0;
}

void enqueue_message(struct peer_state* peer, void *msg, int len) {
	if(peer->outgoing_count + len > BUFSIZE) {
		fprintf(stderr,"Tried to enqueue message beyond buffer size.\n");
		exit(1);
	}
	memcpy(peer->outgoing+peer->outgoing_count, msg, len);
	peer->outgoing_count+=len;

	FD_SET(peer->socket,&writeset); // get ready to send some!
}

int peer_has_piece(int piece, struct peer_state *peer) {
	return ((peer->bitfield[piece/8]>>(7-(piece%8)))&1)==1;
}

int peers_have_piece(int piece) {
	struct peer_state *peer = peers;
	int count = 0;
	while(peer) {
		if(peer->connected && peer_has_piece(piece,peer))
			count++;
		peer=peer->next;
	}
	return count;
}

int next_piece(int previous_piece, struct peer_state* peer) {
	draw_state();

	int best_piece = -1;
	int best_count = 10000;

	for(int i=0;i<(file_length/piece_length+1);i++) {
		if(piece_state[i].downloaders==0 && !piece_state[i].announced) {
			int count = peers_have_piece(i);
			if(peer_has_piece(i,peer) && count < best_count) {
				best_piece = i;
				best_count = count;
			}
		}
	}
		
	if(debug)
		fprintf(stderr,"Next piece %d / %d\n",best_piece,file_length/piece_length);				

	// if we're at the end, and are just waiting for one or two pending pieces, then go for the first pending piece we can get
	if(best_piece < 0) {
		for(int i=0;i<(file_length/piece_length+1);i++) {
			if(piece_state[i].downloaders>0 && !piece_state[i].announced &&
				 peer_has_piece(i,peer))
				return i;
		}
	}

	return best_piece;				
}

/* This needs to be fixed to work properly for multi-file torrents. 
	 specifically, it needs to create the proper directory structure, rather than just concatenate directory and file names. 
 */
void write_block(char* data, int piece, int offset, int len) {
	FILE *outfile;

	int accumulated_file_length = 0;
	int block_start = piece*piece_length+offset;

	struct bencode_list* files = (struct bencode_list*)ben_dict_get_by_str(info,"files");
	// multi-file case
	if(files) {
		for(int i=0;i<files->n;i++) {
			struct bencode* file = files->values[i];
			struct bencode_list* path = (struct bencode_list*)ben_dict_get_by_str(file,"path");
			//			printf("Filename %s/%s\n",((struct bencode_str*)ben_dict_get_by_str(info,"name"))->s,((struct bencode_str*)path->values[0])->s);
			// accumulate a total length so we know how many pieces there are 
			int file_length=((struct bencode_int*)ben_dict_get_by_str(file,"length"))->ll; 

			printf("start %d len %d accum %d filelen %d\n",block_start,len,accumulated_file_length,file_length);
			fflush(stdout);
			// at least part of the block belongs in this file
			if((block_start >= accumulated_file_length) && (block_start < accumulated_file_length+file_length)) {
				char filename[255];
				
				mkdir(((struct bencode_str*)ben_dict_get_by_str(info,"name"))->s,0777);
				chmod(((struct bencode_str*)ben_dict_get_by_str(info,"name"))->s,07777);
				
				sprintf(filename,"%s/",((struct bencode_str*)ben_dict_get_by_str(info,"name"))->s);
				for(int j=0;j<path->n;j++) {					
					if(j<(path->n-1)) {
						sprintf(filename+strlen(filename),"%s/",((struct bencode_str*)path->values[j])->s);
						mkdir(filename,0777);
						chmod(filename,07777);
					}
					else
						sprintf(filename+strlen(filename),"%s",((struct bencode_str*)path->values[j])->s);
				}	
				
				int outfile = open(filename,O_RDWR|O_CREAT,0777);
				if(outfile == -1) {
					fprintf(stderr,"filename: %s\n",filename);
					perror("Couldn't open file for writing");
					exit(1);
				}
				
				int offset_into_file = block_start - accumulated_file_length;
				int remaining_file_length = file_length - offset_into_file;
				lseek(outfile,offset_into_file,SEEK_SET);

				if(remaining_file_length > len) {
					write(outfile,data,len);
					close(outfile);
					goto cleanup;
				}
				else {
					if(debug) {
						fprintf(stderr,"Uh-oh, write crossing file boundaries... watch out!\n");
						fprintf(stderr,"Len %d offset %d filelen %d remaining file len %d\n",len,offset_into_file,file_length,remaining_file_length);
						fflush(stdout);
					}

					write(outfile,data,remaining_file_length);
					close(outfile);
					write_block(data+remaining_file_length,piece,offset+remaining_file_length,len-remaining_file_length);
					goto cleanup;
				}

			}
			accumulated_file_length+=file_length;
		}
	}
	// single-file case
	else {

		struct bencode_str* name = (struct bencode_str*)ben_dict_get_by_str(info,"name");
		if(name) {
			FILE *outfile = fopen(name->s,"r+");
			file_length = ((struct bencode_int*)ben_dict_get_by_str(info,"length"))->ll;			

			// write the data to the right spot in the file
			fseek(outfile,piece*piece_length+offset,SEEK_SET);
			fwrite(data,1,len,outfile);
			fclose(outfile);
	
		}
		else {
			printf("No name?\n");
			exit(1);
		}
	}
	
 cleanup:
	return;
}


void read_block(char* data, int piece, int offset, int len) {
	FILE *infile;

	int accumulated_file_length = 0;
	int block_start = piece*piece_length+offset;

	struct bencode_list* files = (struct bencode_list*)ben_dict_get_by_str(info,"files");
	// multi-file case
	if(files) {
		for(int i=0;i<files->n;i++) {
			struct bencode* file = files->values[i];
			struct bencode_list* path = (struct bencode_list*)ben_dict_get_by_str(file,"path");
			//			printf("Filename %s/%s\n",((struct bencode_str*)ben_dict_get_by_str(info,"name"))->s,((struct bencode_str*)path->values[0])->s);
			// accumulate a total length so we know how many pieces there are 
			int file_length=((struct bencode_int*)ben_dict_get_by_str(file,"length"))->ll; 

			printf("start %d len %d accum %d filelen %d\n",block_start,len,accumulated_file_length,file_length);
			fflush(stdout);
			// at least part of the block belongs in this file
			if((block_start >= accumulated_file_length) && (block_start < accumulated_file_length+file_length)) {
				char filename[255];
				
				sprintf(filename,"%s/",((struct bencode_str*)ben_dict_get_by_str(info,"name"))->s);
				for(int j=0;j<path->n;j++) {					
					if(j<(path->n-1)) {
						sprintf(filename+strlen(filename),"%s/",((struct bencode_str*)path->values[j])->s);
					}
					else
						sprintf(filename+strlen(filename),"%s",((struct bencode_str*)path->values[j])->s);
				}	
				
				int infile = open(filename,O_RDONLY);
				if(infile == -1) {
					fprintf(stderr,"filename: %s\n",filename);
					perror("Couldn't open file for writing");
					exit(1);
				}
				
				int offset_into_file = block_start - accumulated_file_length;
				int remaining_file_length = file_length - offset_into_file;
				lseek(infile,offset_into_file,SEEK_SET);

				if(remaining_file_length > len) {
					read(infile,data,len);
					close(infile);
					return;
				}
				else {
					if(debug) {
						fprintf(stderr,"Uh-oh, read crossing file boundaries... watch out!\n");
						fprintf(stderr,"Len %d offset %d filelen %d remaining file len %d\n",len,offset_into_file,file_length,remaining_file_length);
						fflush(stdout);
					}

					read(infile,data,remaining_file_length);
					close(infile);
					read_block(data+remaining_file_length,piece,offset+remaining_file_length,len-remaining_file_length);
					return;
				}

			}
			accumulated_file_length+=file_length;
		}
	}
	// single-file case
	else {

		struct bencode_str* name = (struct bencode_str*)ben_dict_get_by_str(info,"name");
		if(name) {
			FILE *infile = fopen(name->s,"r+");
			file_length = ((struct bencode_int*)ben_dict_get_by_str(info,"length"))->ll;			

			// write the data to the right spot in the file
			fseek(infile,piece*piece_length+offset,SEEK_SET);
			fread(data,1,len,infile);
			fclose(infile);
	
		}
		else {
			printf("No name?\n");
			exit(1);
		}
	}
}


// Wait for peer to send us another full message, then return the length of the new message.
int receive_message(struct peer_state* peer) {
	// if we already have a message, don't try to receive anything
	if(peer->incoming_count>4 && ntohl(((int*)peer->incoming)[0])+4 <= peer->incoming_count) 
		return ntohl(((int*)peer->incoming)[0]);
	
	int newbytes=recv(peer->socket,peer->incoming+peer->incoming_count,BUFSIZE-peer->incoming_count,0);
	if(newbytes == 0) {			
		fprintf(stderr,"Connection was closed by peer, count was %d, msg size %d\n",peer->incoming_count,ntohl(((int*)peer->incoming)[0]));
		return -1;
	}
	else if(newbytes < 0) {
		if(errno==EAGAIN)  // this happens when we call recv() twice in one select-round.
			return 0;
		perror("Problem when receiving more bytes, closing socket.");
		close(peer->socket);
		peer->connected = 0;			
		return -1;
	}
	peer->incoming_count+=newbytes;
	if(peer->incoming_count>4 && ntohl(((int*)peer->incoming)[0])+4 <= peer->incoming_count) 
		return ntohl(((int*)peer->incoming)[0]);
	else
		return 0;
}

// Drop the most recent message from the buffer. 
void drop_message(struct peer_state* peer) {
	if(peer->incoming_count < 4) {
		fprintf(stderr,"No message to drop. \n");
		return;
	}

	int msglen = ntohl(((int*)peer->incoming)[0]); // size of length prefix is not part of the length
	if(peer->incoming_count<msglen+4) {
		fprintf(stderr,"Trying to drop %d bytes, we have %d!\n",msglen+4,peer->incoming_count);
		peer->connected=0;
		exit(1);
	}
	peer->incoming_count -= msglen+4; // size of length prefix is not part of the length
	if(peer->incoming_count) {
		memmove(peer->incoming,peer->incoming+msglen+4,peer->incoming_count);
	}
 }

 void request_block(struct peer_state* peer, int piece, int offset) {	

	 /* requests have the following format */
	 struct {
		 int len;
		 char id;
		 int index;
		 int begin;
		 int length;
	 } __attribute__((packed)) request;

	 request.len=htonl(13);
	 request.id=6;	
	 request.index=htonl(piece);
	 request.begin=htonl(offset);
	 request.length=htonl(1<<14);						

	 // the last block is likely to be of non-standard size
	 int maxlen = file_length - (piece*piece_length+offset);
	 if(maxlen < (1<<14))
		 request.length = htonl(maxlen);

	 // no point in sending anything if we got choked. We'll restart on unchoke.
	 // WARNING: not handling the case where we get choked in the middle of a piece! Does this happen?
	 if(!peer->choked) {
		 enqueue_message(peer,&request,sizeof(request));
	 }
	 else 
		 fprintf(stderr,"Not sending, choked!\n");
 }

void announce_piece(int piece) {
	if(!piece_state[piece].announced) {
		// send this HAVE message to all connected peers
		struct {
			int len;
			char id;
			int index;
		} __attribute__((packed)) have;
		have.len = htonl(5);
		have.id = 4;
		have.index = htonl(piece);
		
		struct peer_state *peer = peers;
		fprintf(stderr,"Announcing piece %d to peers.\n",piece);
		while(peer) {
			if(peer->connected) {
				enqueue_message(peer,&have,sizeof(have));
			}
			peer = peer->next;
		}
	}
	piece_state[piece].announced=1;
}

void send_interested(struct peer_state* peer) {
	struct {
		int len;
		char id;
	} __attribute__((packed)) msg;
	msg.len = htonl(1);
	msg.id = 2;

	enqueue_message(peer,&msg,sizeof(msg));
}

void send_unchoke(struct peer_state* peer) {
	struct {
		int len;
		char id;
	} __attribute__((packed)) msg;
	msg.len = htonl(1);
	msg.id = 1;

	enqueue_message(peer,&msg,sizeof(msg));
}

void send_choke(struct peer_state* peer) {
	struct {
		int len;
		char id;
	} __attribute__((packed)) msg;
	msg.len = htonl(1);
	msg.id = 0;

	enqueue_message(peer,&msg,sizeof(msg));
}

void send_handshake(struct peer_state *peer) {
	 /* send the handshake message */
	 char protocol[] = "BitTorrent protocol";
	 unsigned char pstrlen = strlen(protocol); // not sure if this should be with or without terminator
	 unsigned char buf[pstrlen+49];
	 buf[0]=pstrlen;
	 memcpy(buf+1,protocol,pstrlen); 
	 memcpy(buf+1+pstrlen+8,digest,20);
	 memcpy(buf+1+pstrlen+8+20,peer_id,20);				 

	 enqueue_message(peer,buf,sizeof(buf));
}


/* establish a connection to the peer, assuming there isn't already a connection to it.

	 In this version, each peer gets a separate thread so connect_to_peer can be written as a
	 single peer-specific loop. For the homework solution, the loop needs to apply to all peers, 
	 not just the one.
 */
void *connect_to_peer(void* pa) {
	fprintf(stderr,"Connecting...\n");
	struct peer_addr* peeraddr = (struct peer_addr*)pa;

	 if(peer_connected(peeraddr->addr)) {
		 fprintf(stderr,"Already connected\n");
		 return 0;
	 }

	 int s = socket(AF_INET, SOCK_STREAM, 0);
	 struct sockaddr_in addr;
	 addr.sin_family = AF_INET;
	 addr.sin_addr.s_addr = peeraddr->addr;
	 addr.sin_port = peeraddr->port;


	 fcntl(s,F_SETFL,O_NONBLOCK);

	 int res = connect(s, (struct sockaddr*)&addr, sizeof(addr));

	 /* register the new peer in our list of peers */
	 struct peer_state* peer = calloc(1,sizeof(struct peer_state));
	 peer->socket = s;
	 peer->ip=peeraddr->addr;
	 peer->next = peers;
	 peer->connected = 0;
	 peer->choked=1;
	 peer->incoming=malloc(BUFSIZE);

	 peer->outgoing=malloc(BUFSIZE);
	 peer->outgoing_count=0;
	 peer->requested_piece = -1;
	 peer->requested_block = 0;

	 peer->bitfield = calloc(1,file_length/piece_length/8+1); //start with an empty bitfield
	 peers=peer;

	 FD_SET(peer->socket,&readset);
	 send_handshake(peer);
}
	 
int recv_handshake(struct peer_state *peer) {

	 /* receive the handshake message. This is different from all other messages, making things ugly. */
	int newbytes = recv(peer->socket,peer->incoming+peer->incoming_count,BUFSIZE-peer->incoming_count,0);
	if(newbytes<=0) {
		perror("receiving handshake");
		return -1;
	}
	peer->incoming_count+=newbytes;

	if(peer->incoming_count < 4 || peer->incoming_count < peer->incoming[0]+49) {
		fprintf(stderr,"Not enough bytes, wanted %d\n",peer->incoming[0]+49);
		return -1;
	}

	if(memcmp(peer->incoming+peer->incoming[0]+8+20+1,"-UICCS450-",strlen("-UICCS450-"))==0) {
		fprintf(stderr,"Caught a CS450 peer, exiting.\n");
		exit(1); // warning - need to handle this correctly
		return -1;
	}
	
	// forget handshake packet
	if(debug)
		fprintf(stderr,"handshake message is %d bytes\n",peer->incoming_count);

	peer->incoming_count -= peer->incoming[0]+49;
	if(peer->incoming_count) 
		memmove(peer->incoming,peer->incoming+peer->incoming[0]+49,peer->incoming_count);

	peer->connected = 1;
}

int handle_message(struct peer_state* peer) {
	int msglen = ntohl(((int*)peer->incoming)[0]);
	switch(peer->incoming[4]) {
		// CHOKE
	case 0: {
		if(debug)
			fprintf(stderr,"Choke\n");
		peer->choked = 1;
		if(peer->requested_piece != -1) 
			sub_downloader(peer->requested_piece);

		peer->requested_piece = -1;	 
		break;
	}
		// UNCHOKE
	case 1: {
		if(debug)
			fprintf(stderr,"Unchoke\n");
		peer->choked = 0;
		
		if(peer->requested_piece==-1 || piece_state[peer->requested_piece].announced) {
			peer->requested_piece = next_piece(-1, peer);	
			peer->requested_block = piece_state[peer->requested_piece].have_up_to;
			add_downloader(peer->requested_piece);
			request_block(peer,peer->requested_piece,peer->requested_block);			
		}
		else {
			printf("Hmm... got unchoked, but had a pending request!\n");
			exit(1);
		}
		break;
	}
		// INTERESTED: -- unchoke the peer if they want some
	case 2: {
		send_unchoke(peer);
	}
		// HAVE -- update the bitfield for this peer
	case 4: {
	  unsigned int piece_index = ntohl(*((int*)&peer->incoming[5]));
		if(piece_index > file_length/piece_length+1) {
			fprintf(stderr,"Got bad message, shutting down peer!\n");
			return -1;
		}
			
		int bitfield_byte = piece_index/8;
		int bitfield_bit = 7-(piece_index%8);
		//		if(debug)
			//			fprintf(stderr,"Have %d\n",piece_index);
		// OR the appropriate mask byte with a byte with the appropriate single bit set
		peer->bitfield[bitfield_byte]|=1<<bitfield_bit;
		
		send_interested(peer);
		break;
	}
		// BITFIELD -- set the bitfield for this peer
	case 5:
		peer->choked = 1; 
		if(debug) 
			printf("Bitfield of length %d\n",msglen-1);
		int fieldlen = msglen - 1;
		if(fieldlen != (file_length/piece_length/8+1)) {
			fprintf(stderr,"Incorrect bitfield size, expected %d\n",file_length/piece_length/8+1);
			return -1;
		}				
		memcpy(peer->bitfield,peer->incoming+5,fieldlen);
		
		send_interested(peer);
		break;
		// REQUEST
	case 6: {
		
		int piece =  ntohl(*((int*)&peer->incoming[5]));
		int offset = ntohl(*((int*)&peer->incoming[9]));
		int length = ntohl(*((int*)&peer->incoming[13]));
		if(debug)
			fprintf(stderr,"Peer wants piece %d offset %d length %d.\n",piece, offset, length);
		
		if(peer->outgoing_count+length < BUFSIZE && peer->balance > -2) {
			char buf[length+13];
			*((int*)buf)=htonl(9+length);
			buf[4]=7;
			*((int*)(buf+5))=htonl(piece);
			*((int*)(buf+9))=htonl(offset);
			
			fflush(stderr);
			read_block(buf+13,piece,offset,length);
			fflush(stderr);
			enqueue_message(peer,buf,length+13);
			fflush(stderr);

			peer->balance--;
			if(peer->balance < -2)
				send_choke(peer);
		}
		break;
	}
		// PIECE
	case 7: {
		int piece = ntohl(*((int*)&peer->incoming[5]));
		int offset = ntohl(*((int*)&peer->incoming[9]));
		int datalen = msglen - 9;
		
		fprintf(stderr,"Writing piece %d, offset %d, ending at %d\n",piece,offset,piece*piece_length+offset+datalen);
		write_block(peer->incoming+13,piece,offset,datalen);
		peer->balance++;
		if(peer->balance==-1) 
			send_unchoke(peer);

		draw_state();
		offset+=datalen;
		piece_state[piece].have_up_to = offset;

		if(peer->requested_piece != piece) {
			printf("Weird... got a piece we didn't request?\n");
			exit(1);
		}
		sub_downloader(piece);
			
		if(!piece_state[piece].announced && (offset==piece_length || (piece*piece_length+offset == file_length))) {
			announce_piece(piece);
		}

		if(piece_state[piece].announced || offset==piece_length || (piece*piece_length+offset == file_length) ) {
			
			if(debug) 
				fprintf(stderr,"Reached end of piece %d at offset %d\n",piece,offset);
			
			peer->requested_piece=next_piece(piece,peer);

			offset = piece_state[peer->requested_piece].have_up_to;
			
			if(peer->requested_piece==-1) {
				if(missing_blocks()==0) {
					fprintf(stderr,"Download complete.\n");
					exit(1);
				}
				else {
					fprintf(stderr,"No more pieces to download. %d missing!\n",missing_blocks());
					if(active_peers()<2) {
						fprintf(stderr,"Restarting peers.\n");
						start_peers();
					}
					return -1;
				}
			}		 
		}

		if(peer->requested_piece!=-1)
			add_downloader(peer->requested_piece);
		
		peer->requested_block = offset;
		request_block(peer,peer->requested_piece,offset);
		break;									
	}
		
	case 20:
		printf("Extended type is %d\n",peer->incoming[5]);
		struct bencode *extended = ben_decode(peer->incoming,msglen);
		print_bencode(extended);
		break;
	}
	drop_message(peer);			 
}

void send_some(struct peer_state* peer) {
	int txsize = peer->outgoing_count;
	if(txsize > 1400) txsize = 1400;
	//	fprintf(stderr,"Sending some %d.\n",txsize);
	int sent_bytes = send(peer->socket,peer->outgoing,txsize,0);
	if(sent_bytes == -1) {
		perror("Error when sending some");
		exit(1);
	}
	memmove(peer->outgoing,peer->outgoing+txsize,peer->outgoing_count - txsize);
	peer->outgoing_count -= txsize;

	if(peer->outgoing_count == 0)
		FD_CLR(peer->socket, &writeset);
}

void event_loop() {
	
	while(1) {
		fd_set rcset,wcset,ecset;
		FD_COPY(&readset,&rcset);
		FD_COPY(&writeset,&wcset);
		FD_COPY(&errorset,&ecset);

		int selected = select(FD_SETSIZE,&rcset,&wcset,&ecset,0);
		struct peer_state* peer=peers;
		while(peer) {
			if(FD_ISSET(peer->socket,&rcset)) {
				if(!peer->connected) {
					if(recv_handshake(peer)==-1) {
						fprintf(stderr,"Problem receiving handshake.\n");
						shutdown_peer(peer);				
					}
				}
				else {	
					int msglen=0;				
					while((msglen = receive_message(peer))>0) {
						int result = handle_message(peer);
						if(result == -1) {
							fprintf(stderr,"Problem handling message.\n");
							shutdown_peer(peer);
							break;
						};	
					}
					if(msglen == -1) {
						fprintf(stderr,"Problem receiving message.\n");
						shutdown_peer(peer);
					}						
				}
			}	
			if(FD_ISSET(peer->socket,&wcset)) {
				if(peer->socket)
					send_some(peer);
			}
			peer=peer->next;
		}

		draw_state();
		fflush(stdout);		
	}
}


/* handle_announcement reads an announcement document to find some peers to download from.
	 start a new tread for each peer.
 */
void handle_announcement(char *ptr, size_t size) {
	struct bencode* anno = ben_decode(ptr,size);

	printf("Torrent has %lld seeds and %lld downloading peers. \n",
				 ((struct bencode_int*)ben_dict_get_by_str(anno,"complete"))->ll,
				 ((struct bencode_int*)ben_dict_get_by_str(anno,"incomplete"))->ll);
		 
	struct bencode_list *peers = (struct bencode_list*)ben_dict_get_by_str(anno,"peers");

	/* unfortunately, the list of peers could be either in bencoded format, or in binary format. 
		 the binary format value is passed as a string, bencoded format is passed as a list. 
		 We handle the two cases separately below.

		 For each peer, we start a new thread to connect to the peer. Your solution is not allowed 
		 to use threads.
	 */
	// handle the binary case
	if(peers->type == BENCODE_STR) {
		printf("Got binary list of peers\n");

		// the "string" in peers is really a list of peer_addr structs, so we'll just cast it as such
		struct peer_addr *peerlist = (struct peer_addr*)((struct bencode_str*)peers)->s;
		for(int i=0;i<((struct bencode_str*)peers)->len/6;i++) {				
			struct in_addr a;
			a.s_addr = peerlist[i].addr;
			printf("Found peer %s:%d\n",inet_ntoa(a),ntohs(peerlist[i].port));				 
			
			connect_to_peer(&peerlist[i]);
		}			 
	}
	// handle the bencoded case
	else {
		for(int i=0;i<peers->n;i++) {
			printf("Got bencoded list of peers\n");
			struct bencode *peer = peers->values[i];
			char *address = ((struct bencode_str*)ben_dict_get_by_str(peer,"ip"))->s;
			unsigned short port = ((struct bencode_int*)ben_dict_get_by_str(peer,"port"))->ll;
			printf("Found peer %s:%d\n",address,port);

			struct peer_addr *peeraddr = malloc(sizeof(struct peer_addr));
			peeraddr->addr=inet_addr(address);
			peeraddr->port=htons(port);

			connect_to_peer(&peeraddr);
		}
	}
}

/* contact the tracker to get announcement, call handle_announcement on the result */
void start_peers() {
	/* now download the announcement document using libcurl. 
		 because of the way curl does things, it's easiest to just throw the entire document into a file first, 
		 and then just read the file. the alternative would be to buffer up all the data in memory using a
		 custom callback function. Let's stick with the KISS principle. 
	 */
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, announce_url);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);

		FILE *anno = fopen("/tmp/anno.tmp","w+");
		if(!anno) {
			perror("couldn't create temporary file\n");
		}

		int attempts=0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, anno); 
		while((res = curl_easy_perform(curl)) !=CURLE_OK && 
					attempts < 5) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
							curl_easy_strerror(res));
			attempts++;
		}
		fclose(anno);

		if (attempts<5) {
			struct stat anno_stat;
			if(stat("/tmp/anno.tmp",&anno_stat)) {
				perror("couldn't stat /tmp/anno.tmp");
				exit(1);
			}
			// the announcement document is in /tmp/anno.tmp. 
			// so map that into memory, then call handle_announcement on the returned pointer
			handle_announcement(mmap(0,anno_stat.st_size,PROT_READ,MAP_SHARED,open("/tmp/anno.tmp",O_RDONLY),0),anno_stat.st_size);
		}
		curl_easy_cleanup(curl);
	}
}

int main(int argc, char** argv) {
	if(argc<2) {
		fprintf(stderr,"Usage: ./hw4 <torrent file>\n");
		exit(1);
	}

	FD_ZERO(&readset);
	FD_ZERO(&writeset);
	FD_ZERO(&errorset);

	setvbuf(stdout,screenbuf,_IOFBF,10000);
		
	// create a global peer_id for this session. Every peer_id should include -CS450-.
	for(int i=strlen(peer_id);i<20;i++)
		peer_id[i]='0'+random()%('Z'-'0'); // random numbers/letters between 0 and Z
	
	// make sure the torrent file exists
	struct stat file_stat;
	if(stat(argv[1],&file_stat)) {
		perror("Error opening file.");
		exit(1);
	}

	// map .torrent file into memory, and parse contents
	int fd = open(argv[1],O_RDONLY);
	char *buf = mmap(0,file_stat.st_size,PROT_READ,MAP_SHARED,fd,0);
	if(buf==(void*)-1) {
		perror("couldn't mmap file");
		exit(1);
	}		 
	size_t off = 0;
	int error = 0;
	torrent = (struct bencode_dict*)ben_decode2(buf,file_stat.st_size,&off,&error);
	if(!torrent) {
		printf("Got error %d, perhaps a malformed torrent file?\n",error);
		exit(1);
	}

	// pull out the .info part, which has stuff about the file we're downloading
	info = (struct bencode*)ben_dict_get_by_str((struct bencode*)torrent,"info");
	
	struct bencode_list* files = (struct bencode_list*)ben_dict_get_by_str(info,"files");
	// multi-file case
	if(files) {
		for(int i=0;i<files->n;i++) {
			struct bencode* file = files->values[i];
			struct bencode_list* path = (struct bencode_list*)ben_dict_get_by_str(file,"path");
			printf("Filename %s/%s\n",((struct bencode_str*)ben_dict_get_by_str(info,"name"))->s,((struct bencode_str*)path->values[0])->s);

			// accumulate a total length so we know how many pieces there are 
			file_length+=((struct bencode_int*)ben_dict_get_by_str(file,"length"))->ll; 
		}
	}
	// single-file case
	else {
		struct bencode_str* name = (struct bencode_str*)ben_dict_get_by_str(info,"name");
		if(name) {
			file_length = ((struct bencode_int*)ben_dict_get_by_str(info,"length"))->ll;			
		}
	}
	fflush(stdout);
	piece_length = ((struct bencode_int*)ben_dict_get_by_str(info,"piece length"))->ll;

	// create our output file, and set up a piece_state array, and a couple of pthread sync. variables
	piece_state = calloc(1,sizeof(struct piece_state)*(int)(file_length/piece_length+1)); //start with an empty bitfield

	/* compute the message digest and info_hash from the "info" field in the torrent */
	size_t len;
	char info_hash[100];  
	char* encoded = ben_encode(&len,(struct bencode*)info);
	SHA1(encoded,len,digest); // digest is a global that holds the raw 20 bytes
	
	// info_hash is a stringified version of the digest, for use in the announce URL
	memset(info_hash,0,100);
	for(int i=0;i<20;i++)
		sprintf(info_hash+3*i,"%%%02x",digest[i]);
		 

	// compile a suitable announce URL for our document
	sprintf(announce_url,"%s?info_hash=%s&peer_id=%s&port=6881&left=%d",((struct bencode_str*)ben_dict_get_by_str((struct bencode*)torrent,"announce"))->s,info_hash,peer_id,file_length);
	printf("Announce URL: %s\n",announce_url);
	fflush(stdout);

	start_peers();		
	event_loop();	
}

