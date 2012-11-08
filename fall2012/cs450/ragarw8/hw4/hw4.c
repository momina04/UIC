// end_game, play only once not all times
//imp todo: send bitfield to peer if this is a reconnect, if after handshake is rxed, missing_blocks()<total_pieces, form and send bitfield


/*problem: a peer has a piece but is not responding to our request_block for a long time either purposely OR
 * probably the peer got disconnected w/o informing TCP layer and TCP layer did not detect it.
*/
//TODO: when peer closes connection decrement availability of pieces on that peers
//TODO: remove memory leaks
//TODO: if peer is choked by us and we receive a piece from him, then we may want to unchoke him
//warning : remove reconect()...not required
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
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
#include <sys/select.h>


#include"hw4.h"

#ifndef FD_COPY
#define FD_COPY(f, t)   (void)(*(t) = *(f))
#endif

#define MAX_CONNECT_RETRY 0 /*Dont make it to 0, make it to at least 1*/
//#define myEND_GAME_THRESHOLD 1
int end_game_flag = 0;
int myEND_GAME_THRESHOLD = 10; /* 0 to disable */

int end_game_in_progress = 0;
/* Bittorent message common header */
struct header{
    int len;
    char type;
} __attribute__((packed));


FILE *fp;
int all_peers_down_flag;
fd_set readset;
fd_set writeset;
fd_set exceptionset;
fd_set readset_copy;
fd_set writeset_copy;
fd_set exceptionset_copy;
// computed in main(). This is where we get our peers from.
char announce_url[255];
char announce_url1[255];

enum {PIECE_EMPTY=0, PIECE_PENDING=1, PIECE_FINISHED=2} *piece_status;

int *piece_counts;

char *my_bitfield = NULL;
int total_pieces = 0;
int completed_pieces = 0;

#define BUFSIZE piece_length*4
struct peer_state *peers=0;

int debug=0;  //set this to zero if you don't want all the debugging messages 

char screenbuf[10000];

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
		if((peer->connected && !peer->choked) || peer->connecting)
			count++;
		peer = peer->next;
	}
	return count;
}
int active_peers2() {
    return active_peers();
/*
	struct peer_state *peer = peers;
	int count=0;
	while(peer) {
		if(peer->connected || peer->connecting)
			count++;
		peer = peer->next;
	}
	return count;
	*/
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

int peer_connected(in_addr_t addr, int sockid,unsigned int port) {
	struct peer_state *peer = peers;
	while(peer) {
		if(peer->ip == addr && peer->connected==1 && peer->sockid == sockid && peer->port == port) {
			return 1;
		}
		peer = peer->next;
	}	
	return 0;
}



void* draw_state() {
		printf("\033[2J\033[1;1H");
		int pieces = file_length/piece_length+1;

		printf("%d byte file, %d byte pieces(%d total pieces), %d pieces left, %d active, %d choked\n",file_length,piece_length,file_length/piece_length+1,missing_blocks(),active_peers(),choked_peers());
		for(int i=0;i<pieces;i++) {
			switch(piece_status[i]) {
			case PIECE_EMPTY: printf("."); break;
			case PIECE_PENDING: printf("x"); break;
			case PIECE_FINISHED: printf("X"); break;
			default: printf("?");
			}
		}
    printf("\n");
}


int missing_blocks() {
    //return (total_pieces - completed_pieces);
	int count=0;

	for(int i=0;i<file_length/piece_length+((file_length%piece_length)>0?1:0);i++) {
		if(piece_status[i]!=PIECE_FINISHED) {
			count++;
		}
	}
	return count;
}

/*See has_piece */
int isset(char *buf, int piece_index)
{
    int x;
    int bitfield_byte = piece_index/8;
    int bitfield_bit = piece_index%8;
    x = buf[bitfield_byte] >> (7 - bitfield_bit);
    x &= 0x01;
    return x;
}

/* Will return true if peer has the peice */
int has_piece(struct peer_state *peer ,int piece_index)
{
    return isset(peer->bitfield, piece_index);
}

/* If SEGV in this function increase BUFSIZE for outgoing OR block on send until some buffer is cleared */
void queue_send(struct peer_state *peer, char *buf, int count){
    int cnt ;
    int status = -1;
    //peer->send_count = 0;
    //cnt = 0;
   // while(status == -1 && cnt < 5){
   //     status = send(peer->socket, buf, count,0);
   //     cnt++;
   // }
    //printf("<RITESH> %d/%d bytes sent to ", status, count);
    //print_peer_ip(peer);
    memcpy(peer->outgoing + peer->send_count, buf, count);
    peer->send_count += count;
}


void send_choke(struct peer_state *peer)
{
    peer->choked_by_me = 1;
    char choke[5];
    choke[0]=0;
    choke[1]=0;
    choke[2]=0;
    choke[3]=1;
    choke[4]=0;/*CHOKE */
    queue_send(peer, choke, 5);
}

void send_unchoke(struct peer_state *peer)
{
    peer->choked_by_me = 0;
    char unchoke[5];
    unchoke[0]=0;
    unchoke[1]=0;
    unchoke[2]=0;
    unchoke[3]=1;
    unchoke[4]=1;/*UNchoke */
    queue_send(peer, unchoke, 5);
}


int do_we_have_piece(int piece_index){
    return isset(my_bitfield,piece_index);
}

/* Returns size of the piece, for invalid piece no returns -1 */
int get_piece_size_or_status(int piece_index){
    if(piece_index == (total_pieces - 1)){ /* Last piece */
        return (file_length % piece_length);
    }
    else if(piece_index >= (total_pieces)){ /* Invalid piece no. */
        return -1;
    }
    else{
        return piece_length;
    }
}

/* Bittorent message piece header */
struct piece_header{
    int len;
    char type;
    int index;
    int offset;
} __attribute__((packed));

/* This needs to be fixed to work properly for multi-file torrents. 
	 specifically, it needs to create the proper directory structure, rather than just concatenate directory and file names. 
 */
void read_block(char* data, int piece, int offset, int len) 
{


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
				
				int outfile = open(filename,O_RDONLY,0777);
				if(outfile == -1) {
					fprintf(stderr,"filename: %s\n",filename);
					perror("Couldn't open file for reading");
					printf("<RITESH> file open error for read1\n");
					exit(1);
				}
				
				int offset_into_file = block_start - accumulated_file_length;
				int remaining_file_length = file_length - offset_into_file;
				lseek(outfile,offset_into_file,SEEK_SET);

				if(remaining_file_length > len) {
					read(outfile,data,len);
					close(outfile);
					goto cleanup;
				}
				else {
					if(debug) {
						fprintf(stderr,"Uh-oh, write crossing file boundaries... watch out!\n");
						fprintf(stderr,"Len %d offset %d filelen %d remaining file len %d\n",len,offset_into_file,file_length,remaining_file_length);

					}

					read(outfile,data,remaining_file_length);
					close(outfile);
					read_block(data+remaining_file_length,piece,offset+remaining_file_length,len-remaining_file_length);
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
			FILE *outfile = fopen(name->s,"r");
			file_length = ((struct bencode_int*)ben_dict_get_by_str(info,"length"))->ll;			

			// write the data to the right spot in the file
			fseek(outfile,piece*piece_length+offset,SEEK_SET);
			fread(data,1,len,outfile);
			fclose(outfile);
	
		}
		else {
			printf("No name?\n");
			exit(1);
		}
	}
	
 cleanup:
    return; /* Make Compiler happy */
}
/* at the moment should be called from process_request_message, otherwise do all validations over here again */
void send_piece(struct peer_state *peer, int piece_index, int offset, int len)
{
    char *buf;
    struct piece_header *h;
    buf = malloc(len + 13 );
    h = (struct piece_header *) buf;
    h->len = htonl(len + 9);
    h->type = 7; /* PIECE */
    h->index = htonl(piece_index);
    h->offset = htonl(offset);
    read_block(buf + 13 , piece_index, offset, len);
    queue_send(peer, buf, len + 13);
    peer->pieces_sent++;
    free(buf);
    if((peer->pieces_sent > (peer->pieces_rxed + 10))){
        send_choke(peer);
    }
}

void send_bitfield(struct peer_state *peer)
{
/* Bittorent message common header */
    int len;
    char *buf;
    struct header *h;
    len = file_length/piece_length/8+1;
    buf = calloc(1,len + 5 );
    h = (struct header *) buf;
    h->len = htonl(len + 1);
    h->type = 5; /* BITFIELD */
    memcpy(buf+5, my_bitfield, len);
    queue_send(peer, buf, len + 5);
    free(buf);

    peer->choked_by_me = 0;
}
void close_peer(struct peer_state *peer){

    FD_CLR(peer->socket, &readset);
    FD_CLR(peer->socket, &writeset);
    FD_CLR(peer->socket, &exceptionset);
    peer->connected = 0;
    peer->connecting = 0;
    peer->choked = 0;

    peer->count = 0;
    peer->send_count =0;
    peer->requested_piece = -1;
    close(peer->socket);
    /* todo: free that is not required */
    /*todo optimization, delete from list OR move to last of linked list, while searching stop at first non connected peer*/
}
/* Will close connection and return -1 for invalid piece indexes */
int process_request_message(struct peer_state *peer, int piece_index, int offset, int len)
{
    if(peer->choked_by_me == 1)
        return;
    int piece_size = 0;
    piece_size = get_piece_size_or_status(piece_index);
    if(piece_size < 0 || do_we_have_piece(piece_index) == 0){
        printf("<RITESH> Received Request for invaliled piece index or piece we dont have piece = %d\n",piece_index);
        //exit(9); //remove
        return -1;
    }
    else if(len > piece_size){
        printf("<RITESH> Closing connection..peer misbehaving.asking for more bytes than available\n");
        exit(10); //remove
        return -2;
    }

    
    send_piece(peer, piece_index,offset,len);
    return 0;
}

void process_interested_message(struct peer_state *peer){
    /* SEND CHOKE OR UNCHOKE? */
    peer->not_interested = 0;
    if((peer->pieces_sent > (peer->pieces_rxed + 10))){
        send_choke(peer);
        //send_unchoke(peer);
    }
    else{
        send_unchoke(peer);
    }
}




/* Will set appropriate bit in bitfield */
void make_have_piece(int piece_index)
{
    int x;
    int bitfield_byte = piece_index/8;
    int bitfield_bit = piece_index%8;
    my_bitfield[bitfield_byte] |= 1 << (7 - bitfield_bit);


    /* Send HAVEs to all connected peers */
    struct peer_state *peer = NULL;
    char havemsg[9];
    struct header *h;
    h = (struct header *) &havemsg;
    //h->len = 5;
    havemsg[0] = 0;
    havemsg[1] = 0;
    havemsg[2] = 0;
    havemsg[3] = 5;
    h->type = 4; /* HAVE */
    *((int *)(&havemsg[5])) = htonl(piece_index);
    peer = peers;
    while(peer){
        if(peer->connected == 1){
            if(!has_piece(peer, piece_index) && !peer->not_interested){ /* Send HAVE message to peers only if it does not have that piece */
                //send(peer->socket, havemsg, 9, 0);
                queue_send(peer, havemsg, 9);
            }
        }
        peer = peer->next;
    }
}

/* so far, we're assuming that every peer actually have all pieces. That's not good! */
int next_piece2(int previous_piece, struct peer_state *peer){
	if(previous_piece!=-1){
		piece_status[previous_piece]=PIECE_FINISHED;
		make_have_piece(previous_piece);
		completed_pieces++;
    }
	
	draw_state();

	for(int i=0;i<(file_length/piece_length+1);i++) {
		if(piece_status[i]==PIECE_EMPTY) {
			if(debug)
				fprintf(stderr,"Next piece %d / %d\n",i,file_length/piece_length);
			piece_status[i]=PIECE_PENDING;			 
			return i;
		}
	}
	return -1;
}


/*RITESH Difference: now if there are no peers that have a piece it will return -1 */
/* so far, we're assuming that every peer actually have all pieces. That's not good! */
/*todo: try to use heap */
int next_piece(int previous_piece, struct peer_state *peer) 
{
    int min_index;
    int i;
	if(previous_piece!=-1){
		piece_status[previous_piece]=PIECE_FINISHED;
		peer->pieces_rxed ++;
		make_have_piece(previous_piece);
		peer->rarity_average *= peer->pieces_rxed;
		peer->pieces_rxed ++;
		peer->rarity_average += peer->last_piece_rarity;
		peer->rarity_average /= peer->pieces_rxed;
		completed_pieces++;
    }
	
	draw_state();

    /*Find first piece who at least has 1 peer and is not being downloaded and is not downloaded already */
    for(i=0; i<(file_length/piece_length+1); i++){
        if(piece_counts[i]>0 && 
            piece_status[i] == PIECE_EMPTY &&
             has_piece(peer,i)){
            break;
        }
    }
    if(i == file_length/piece_length+1)
        return -1;

    /* Find rarest piece which at least 1 peer has */
    min_index=i;
	for(i=i+1; i<(file_length/piece_length+1); i++){
	    if(piece_counts[i] > 0 &&
	         piece_status[i] == PIECE_EMPTY &&
              has_piece(peer,i)){
                if((piece_counts[i] == piece_counts[min_index]) && 
                    (random()%(total_pieces/4) == 0)){ /* Flip a coin */
                    min_index = i;
                }
                else if(piece_counts[i] < piece_counts[min_index]){
                    min_index = i;
                }
        }
    }
    peer->last_piece_rarity = piece_counts[min_index];
    piece_status[min_index]=PIECE_PENDING;			 
    fprintf(stderr,"Next piece %d / %d\n",min_index,file_length/piece_length);
    return min_index;
/*
	for(int i=0;i<(file_length/piece_length+1);i++) {
		if(piece_status[i]==PIECE_EMPTY) {
			if(debug)
				fprintf(stderr,"Next piece %d / %d\n",i,file_length/piece_length);
			piece_status[i]=PIECE_PENDING;			 
			return i;
		}
	}
	return -1;
*/
}

/* This needs to be fixed to work properly for multi-file torrents. 
	 specifically, it needs to create the proper directory structure, rather than just concatenate directory and file names. 
 */
void write_block(char* data, int piece, int offset, int len, int acquire_lock) {
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

					}

					write(outfile,data,remaining_file_length);
					close(outfile);
					write_block(data+remaining_file_length,piece,offset+remaining_file_length,len-remaining_file_length,0);
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
    return; /* Make Compiler happy */
}


// Drop the most recent message from the buffer. 
void drop_message(struct peer_state* peer) {
    if(peer->count == 0)
        return;
	int msglen = ntohl(((int*)peer->incoming)[0]); // size of length prefix is not part of the length
	if(peer->count < msglen+4) {
		printf("<RITESH>Trying to drop %d bytes, we have %d!\n",msglen+4,peer->count);
		fprintf(stderr,"Trying to drop %d bytes, we have %d!\n",msglen+4,peer->count);
		peer->connected=0;
		exit(1);
	}
	peer->count -= msglen+4; // size of length prefix is not part of the length
	if(peer->count) {
		memmove(peer->incoming,peer->incoming+msglen+4,peer->count);
	}
 }

 void cancel_request_block(struct peer_state* peer, int piece, int offset) 
 {	
    struct peer_state *p;
    p = peers;
    if(piece >= total_pieces || piece < 0) 
        return;
    while(p)
    {
        if(p == peer) {
            p = p->next;
            continue;
        }

        if(!has_piece(p,piece) || !p->connected || !p->choked)
        { 
            return;
        }
        /* requests have the following format */
        struct {
            int len;
            char id;
            int index;
            int begin;
            int length;
        } __attribute__((packed)) request;

        request.len=htonl(13);
        request.id=8;	
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
            //memcpy(peer->outgoing + peer->send_count, &request, sizeof(request));
            //peer->send_count += sizeof(request);
            queue_send(peer, &request, sizeof(request));
            //remove: send(peer->socket,&request,sizeof(request),0);		
        }
        else 
            fprintf(stderr,"Not sending, choked!\n");
        p = p->next;
    }
 }
 void request_block(struct peer_state* peer, int piece, int offset) {	
    if(piece >= total_pieces || piece < 0) 
        return;

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
	     //memcpy(peer->outgoing + peer->send_count, &request, sizeof(request));
	     //peer->send_count += sizeof(request);
		 queue_send(peer, &request, sizeof(request));
		 //remove: send(peer->socket,&request,sizeof(request),0);		
	 }
	 else 
		 fprintf(stderr,"Not sending, choked!\n");
 }

void send_not_interested(struct peer_state* peer) {
	struct {
		int len;
		char id;
	} __attribute__((packed)) msg;
	msg.len = htonl(1);
	msg.id = 3;

    queue_send(peer, &msg, sizeof(msg));
    //memcpy(peer->outgoing + peer->send_count, &msg, sizeof(msg));
    //peer->send_count += sizeof(msg);
	//remove: send(peer->socket,&msg,sizeof(msg),0);
}

void send_interested(struct peer_state* peer) {
	struct {
		int len;
		char id;
	} __attribute__((packed)) msg;
	msg.len = htonl(1);
	msg.id = 2;

    memcpy(peer->outgoing + peer->send_count, &msg, sizeof(msg));
    peer->send_count += sizeof(msg);
	//remove: send(peer->socket,&msg,sizeof(msg),0);
}

void print_peer_ip(struct peer_state* peer)
{
    printf("%u.%u.%u.%u\n", *((unsigned char *)&(peer->ip)+0),
            *((unsigned char *)&(peer->ip)+1),
            *((unsigned char *)&(peer->ip)+2),
            *((unsigned char *)&(peer->ip)+3)
          );
}

int reconnect_peer(struct peer_state* peer) 
{
    struct in_addr a;
    a.s_addr = peer->ip;
    printf("\n<RITESH>Reconnecting to peer %s:%d\n",inet_ntoa(a),ntohs(peer->port));				 



	fprintf(stderr,"Connecting...\n");

	 if(peer->connected) {
		 fprintf(stderr,"Already connected\n");
		 return 0;
	 }

	 int s = socket(AF_INET, SOCK_STREAM, 0);
	 fcntl(s, F_SETFL, O_NONBLOCK); /* Make connect() non blocking */
	 struct sockaddr_in addr;
	 addr.sin_family = AF_INET;
	 addr.sin_addr.s_addr = peer->ip;
	 addr.sin_port = peer->port;

	 /* after 60 seconds of nothing, we probably should poke the peer to see if we can wake them up */
//	 struct timeval tv;
//	 tv.tv_sec = 60;
//	 if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv)) {
//		 perror("setsockopt");
//		 return 0;
//	 }	
    
     printf("<RITESH>Re-Connect called\n");


	 int res = connect(s, (struct sockaddr*)&addr, sizeof(addr));
	 if(res == -1) {
         printf("<RITESH>Re-Connect succeeded\n");
         FD_SET(s,&readset);	
         FD_SET(s,&writeset);	
         FD_SET(s,&exceptionset);	
         peer->connect_retries++;
	 }
	 else{
		 perror("Error while connecting.");		
		 return 1;
     }  

     /* update  peer in our list of peers */
     
     peer->socket = s;
     peer->connected = 0;
     peer->connecting = 1;
     peer->handshake_pending = 1;
     peer->choked = 1;
     printf("<RITESH> BUFSIZE = %d\n", BUFSIZE);
}



void process_bitfield(char *bitfield, unsigned int field_len /*No. of Bytes*/)
{
    char byte;
    char *p = NULL;
    p = bitfield;
    int piece_index = 0;

    while(p < (bitfield + field_len) ){
        int i = 0;
        for(i=7; i>=0; i--){
            if( (((*p) >> i) & 0x01) == 1)
                    piece_counts[piece_index]++;
            piece_index++;
        }
        p++;
    }

}

int end_game_count = 5;
/* Send request for remaining blocks to all peers that have it, later we can cancel it */
void play_end_game()
{
    static int flag = 0;
    //return;
    if(missing_blocks() > (total_pieces/10)) 
    //if(missing_blocks() > 2) 
    //    return;

    if(flag == 1) return;

    flag = 1;

    for(int i=0;i<file_length/piece_length+((file_length%piece_length)>0?1:0);i++) {
        if(piece_status[i]!=PIECE_FINISHED) {
            struct peer_state *p;
            p = peers;
            while(p){
                if(has_piece(p,i) && p->connected && !p->choked){
                    p->requested_piece = i;/* makes sense no more */
                    request_block(p,p->requested_piece,0);
                    piece_status[i] = PIECE_EMPTY;
                    //piece_status[i] = PIECE_PENDING;//look here...done this because if the connection closes, then we might piece_status mark incorrectly
                }
                p=p->next;
            }
            piece_status[i]!=PIECE_EMPTY;
            break;
        }
    }
    end_game_in_progress = 1;
}

void handle_messages() 
{
    struct peer_state* peer = peers;
    unsigned int msglen = 0;
    while(missing_blocks() > 0)
    {
        int all_peers_not_choked = 0;

        if(missing_blocks() == end_game_count) 
        {
            play_end_game();
        }
        //if(missing_blocks() <= myEND_GAME_THRESHOLD) 
        if(missing_blocks() == myEND_GAME_THRESHOLD && end_game_flag == 0) 
        {
            end_game_flag = 1;
            return;
        }
        //draw_state();
        //sleep(1);
        all_peers_down_flag = 0;
        FD_COPY(&readset,&readset_copy);
        FD_COPY(&writeset,&writeset_copy);
        FD_COPY(&readset,&readset_copy);
        struct timeval timespec;
        timespec.tv_sec = 2;
        /*
        int rdy = select(FD_SETSIZE,NULL,&writeset_copy,&exceptionset_copy, &timespec);
        int rdy = select(FD_SETSIZE,&readset_copy,NULL,&exceptionset_copy, &timespec);
        */
        int rdy = select(FD_SETSIZE,&readset_copy,&writeset_copy,&exceptionset_copy, &timespec);
        if(rdy == 0) {
            /* Time out expired */
            struct peer_state *p;
            p = peers;
            while(p){
                if(p->connected && !p->choked){
                    all_peers_not_choked = 1;
                    break;
                }
                p = p->next;
            }

            //?send interested to all choked peers and check
            //disconnect all choked peers..done at break;
            if(!all_peers_not_choked){ /* If all peers are choked */
                all_peers_down_flag =1;
                break;
            }
            else{
                /* END GAME */
                //why did timer expire then? why was there no incoming data.
                //what to do?
                //return; //might create problem
                //play_end_game();
            }
            continue;

        }
        peer = peers;
        if(active_peers2() == 0){
            printf("<RITESH> All peers down/disconnected.\n");
            break;
        }
        while(peer){
            /* Readset */
            if(FD_ISSET(peer->socket,&readset_copy)) 
            {
                int newbytes = recv(peer->socket,peer->incoming+peer->count,BUFSIZE-peer->count,0);
                printf("<RITESH> %d bytes received from peer ",newbytes);
                printf("%u.%u.%u.%u\n", *((unsigned char *)&(peer->ip)+0),
                        *((unsigned char *)&(peer->ip)+1),
                        *((unsigned char *)&(peer->ip)+2),
                        *((unsigned char *)&(peer->ip)+3)
                      );
                if(newbytes <= 0){ /* Connection refused/closed */
                    peer->connecting = 0;
                    if(newbytes == 0){
                        printf("\nConnection closed remotely by %x -- %u.%u.%u.%u\n",peer->ip, *((unsigned char *)&(peer->ip)+0),
                            *((unsigned char *)&(peer->ip)+1),
                            *((unsigned char *)&(peer->ip)+2),
                            *((unsigned char *)&(peer->ip)+3)
                            );
                        /* Mark the piece requested from this host as empty */
                        if(piece_status[peer->requested_piece] != PIECE_FINISHED) //double check during end game..might be finished by other peer
                        {
                            piece_status[peer->requested_piece] = PIECE_EMPTY;
                        }

                    }
                    else{
                        printf("\nConnection refused for %x -- %u.%u.%u.%u\n",peer->ip, *((unsigned char *)&(peer->ip)+0),
                            *((unsigned char *)&(peer->ip)+1),
                            *((unsigned char *)&(peer->ip)+2),
                            *((unsigned char *)&(peer->ip)+3)
                            );
                    }
                    close_peer(peer);


                    /* Delete peer from linked list */
                    /* If there is only one node in the list */
                    printf("<RITESH> Peer count before = %d ", active_peers2());
                    //free(peer->incoming);
                    //free(peer->outgoing);
                    //free(peer->bitfield);
                    //
                    //if(newbytes == 0 && peer->connect_retries < MAX_CONNECT_RETRY) 
                    if(peer->connect_retries < MAX_CONNECT_RETRY) {
                        reconnect_peer(peer);
                    }
                    if(active_peers2() == 0){
                        all_peers_down_flag = 1;
                        break;
                    }
                    peer = peer->next;
                    continue; //Otherwise writeset will be set for this socket
                } /* Connection refused */
                peer->count += newbytes;
                if(peer->handshake_pending){
                    // forget handshake packet
                    if(debug)
                        printf("handshake message is %d bytes",peer->count);
                    printf("<RITESH>handshake message is %d bytes for peer ",peer->count);
                    printf("%u.%u.%u.%u\n", *((unsigned char *)&(peer->ip)+0),
                            *((unsigned char *)&(peer->ip)+1),
                            *((unsigned char *)&(peer->ip)+2),
                            *((unsigned char *)&(peer->ip)+3)
                          );
                    peer->count -= peer->incoming[0]+49;
                    if(peer->count) 
                        memmove(peer->incoming, 
                                peer->incoming + peer->incoming[0]+49,
                                peer->count);
                    peer->handshake_pending = 0;
                    if(missing_blocks() < total_pieces){ /*If this is a reconnection most likely */
                        send_bitfield(peer);
                    }
                }

                if(peer->count == 0){ /*This means this stream had only handshake message, move to next peer*/
                    peer = peer->next;
                    continue;
                }
                
                if(peer->handshake_pending == 1){
                    printf("<RITESH> Something went wrong...handshake pending was NOT unset..exiting\n");
                    exit(200);
                }
                    

                /* FSM for message_pending as follows
                 *************************************************************************************************
                 *  State *             Event                 * Next State * Comments                            *
                 *************************************************************************************************
                 *   0    * Half message                      *     1      * Dont process                        *
                 *   0    * Full message + no xtra bytes      *     0      * Process message and reset peer_count*
                 *   0    * Full message + xtra bytes         *     1      * Process message recalculate, realign*
                 *   1    * Incomplete message                *     1      * Dont process                        *
                 *   1    * Complete Message + no xtra bytes  *     0      * Process Complete message, reset     *
                 *   1    * Complete Message + xtra bytes     *     1      * Process message, realign recalculate*
                 *        *                                   *            *                                     *
                 *************************************************************************************************
                 */
                /* 
                 * If atleast 4 bytes(size of length field) are not recived OR at least one message is not received then 
                 * wait till next recv() from this peer.
                 */
                if(peer->count < 4 || peer->count < (ntohl(((int*)peer->incoming)[0]) + 4) ) {
                    printf("<RITESH>Received half message (%d bytes out of %u total bytes) from peer ",peer->count,
                            ntohl(((int*)peer->incoming)[0])+4
                          );
                    printf("%u.%u.%u.%u\n", *((unsigned char *)&(peer->ip)+0),
                            *((unsigned char *)&(peer->ip)+1),
                            *((unsigned char *)&(peer->ip)+2),
                            *((unsigned char *)&(peer->ip)+3)
                          );
                    peer = peer->next;
                    continue;
                }

                /* If a stream consists of more than one complete message we need to process all of them */
                int peer_closed;
                peer_closed = 0;

                int next_peer;
                next_peer = 0;  /* TODO: not used so far */
                while(peer->count >= (ntohl(((int*)peer->incoming)[0])) + 4){ /* RITESH: this is extra , if problems are seen remove this */
                printf("<RITESH>Processing one full message (total %u bytes) from peer ",ntohl(((int*)peer->incoming)[0])+4);
                printf("%u.%u.%u.%u\n", *((unsigned char *)&(peer->ip)+0),
                        *((unsigned char *)&(peer->ip)+1),
                        *((unsigned char *)&(peer->ip)+2),
                        *((unsigned char *)&(peer->ip)+3)
                      );
                msglen = ntohl(((int*)peer->incoming)[0]);

//#if 0
                    switch(peer->incoming[4]) {
                        // CHOKE
                        case 0: {
                                    if(debug)
                                        fprintf(stderr,"Choke\n");
                                    peer->choked = 1;
                                    piece_status[peer->requested_piece]=PIECE_EMPTY;
                                    completed_pieces--;
                                    peer->requested_piece = -1;
                                    break;
                                }
                                // UNCHOKE
                        case 1: {
                                    if(debug)
                                        fprintf(stderr,"Unchoke\n");
                                    peer->choked = 0;

                                    // grab a new piece - WARNING: this assumes that you don't get choked mid-piece!
                                    peer->requested_piece = next_piece(-1,peer);	
                                    request_block(peer,peer->requested_piece,0);
                                    break;
                                }
                                // HAVE -- update the bitfield for this peer
                        case 4: {
                                    int piece_index = ntohl(*((int*)&peer->incoming[5]));
                                    int bitfield_byte = piece_index/8;
                                    int bitfield_bit = piece_index%8;
                                    if(debug)
                                        fprintf(stderr,"Have %d\n",piece_index);
                                    // OR the appropriate mask byte with a byte with the appropriate single bit set
                                    if(has_piece(peer, piece_index)){ //remove...test..assert
                                        printf("1-Something is wrong pc = %d\n",piece_index); //todo-fix
                                    //    exit(600);
                                    }
                                    peer->bitfield[bitfield_byte] |= 1 << (7 - bitfield_bit);
                                    piece_counts[piece_index]++;
                                    if(!has_piece(peer, piece_index)){ //remove...test..assert
                                        printf("2- Something is wromng pc =%d", piece_index);
                                        exit(700);
                                    }
                                    if(!peer->choked)
                                        send_interested(peer);
                                    break;
                                }
                                // BITFIELD -- set the bitfield for this peer
                        case 5:
                                peer->choked = 0; // let's assume a bitfield means we're allowed to go...
                                if(debug) 
                                    printf("Bitfield of length %d\n",msglen-1);
                                int fieldlen = msglen - 1;
                                if(fieldlen != (file_length/piece_length/8+1)) {
                                    fprintf(stderr,"Incorrect bitfield size, expected %d\n",file_length/piece_length/8+1);
                                    //goto shutdown;
                                    close_peer(peer);
                                    if(active_peers2() == 0){
                                        all_peers_down_flag = 1;
                                        break;
                                    }
                                    peer = peer->next;
                                    continue;
                                }				
                                memcpy(peer->bitfield,peer->incoming+5,fieldlen);
                                process_bitfield(peer->bitfield,fieldlen);

                                send_interested(peer);
                                break;
                                // PIECE
                        case 7: {
                                    int piece = ntohl(*((int*)&peer->incoming[5]));
                                    int offset = ntohl(*((int*)&peer->incoming[9]));
                                    int datalen = msglen - 9;

                                    piece_status[piece] = PIECE_FINISHED; //double check
                                    if(do_we_have_piece(piece)){
                                        //peer->requested_piece=next_piece(piece,peer);
                                        piece_status[piece] = PIECE_FINISHED; //double check
                                        peer->requested_piece=next_piece(-1,peer);
                                        draw_state();
                                        if(peer->requested_piece != -1)
                                            request_block(peer,peer->requested_piece,0);
                                        break;
                                    }

                                    fprintf(stderr,"Writing piece %d, offset %d, ending at %d\n",piece,offset,piece*piece_length+offset+datalen);
                                    write_block(peer->incoming+13,piece,offset,datalen,1);
                                    draw_state();
                                    offset+=datalen;
                                    if(offset==piece_length || (piece*piece_length+offset == file_length) ) {

                                        if(debug) 
                                            fprintf(stderr,"Reached end of piece %d at offset %d\n",piece,offset);

                                        peer->requested_piece=next_piece(piece,peer);
                                        offset = 0;

                                        if(peer->requested_piece==-1) {
                                            /*
                                               int all_peice_downloaded_flag;
                                               fprintf(stderr,"No more pieces to download!\n");
                                               all_peice_downloaded_flag = 0;
                                            // don't exit if some piece is still being downloaded
                                            for(int i=0;i<file_length/piece_length+1;i++) 
                                            if(piece_status[i]!=2) {
                                            all_peice_downloaded_flag = 1;
                                            //goto shutdown;
                                            }
                                            if(all_peice_downloaded_flag == 1){
                                            //goto shutdown;
                                            close_peer(peer);
                                            if(active_peers2() == 0){
                                            all_peers_down_flag = 1;
                                            break;
                                            }
                                            }
                                            */
                                            break;
                                            //drop_message(peer); /*being careful*/
                                            //continue; /* If peer->incoming has any pending message process that */
                                        }
                                    }

                                    request_block(peer,peer->requested_piece,offset);
                                    if(end_game_in_progress){
                                        cancel_request_block(peer,peer->requested_piece,offset);
                                        end_game_count--;
                                    }
                                    break;									
                                }

                        case 20:
                                printf("Extended type is %d\n",peer->incoming[5]);
                                struct bencode *extended = ben_decode(peer->incoming,msglen);
                                print_bencode(extended);
                                break;
                    }

                    /*Upload part */
                    switch(peer->incoming[4]) {
                        // CHOKE
                        case 0: {
                                    break;
                                }
                                // UNCHOKE
                        case 1: {
                                    break;
                                }
                                // INTERESTED*
                        case 2: {
                                    //send choke or unchoke?
                                    process_interested_message(peer);
                                    break;
                                }
                                // NOT INTERESTED*
                        case 3: {
                                    //send_choke(peer);
                                    peer->not_interested = 1;
                                    break;
                                }
                                // HAVE -- update the bitfield for this peer
                        case 4: {
                                    break;
                                }
                                // BITFIELD -- set the bitfield for this peer
                        case 5:
                                break;
                                // Request_block*
                        case 6: {
                                    int status; 
                                    if(peer->choked_by_me == 1) break;
                                    /* requests have the following format */
                                    struct request{
                                        int length;
                                        char id;
                                        int index;
                                        int begin;
                                        int len;
                                    } __attribute__((packed)) *request;
                                    request = (struct request *) peer->incoming;
                                    request->index = ntohl(request->index);
                                    request->begin = ntohl(request->begin);
                                    request->len = ntohl(request->len);

                                    status = process_request_message(peer, request->index, request->begin, request->len);
                                    if(status < 0){
                                        printf("<RITESH> Closing connection..peer misbehaving\n");
                                        close_peer(peer);
                                        peer = peer->next;
                                        peer_closed = 1;
                                        break;
                                    }
                                    //may choke or unchoke depending on whether we do it in case 2
                                    break;
                                }
                                // PIECE
                        case 7: {
                                    /* may choke after sending piece */
                                    break;
                                }

                        case 20:
                                break;
                    }
                    //#endif
                    drop_message(peer); /* Realign and reset/recalculate*/			 
                }
                if(peer_closed == 1){
                    peer = peer->next;
                    continue;
                }
            }
            /* Writeset */
            if(FD_ISSET(peer->socket,&writeset_copy)){
                if(peer->connected == 0){
                    peer->connecting = 0;
                    printf("\nConnect() accepted by  %x -- %u.%u.%u.%u\n",peer->ip, *((unsigned char *)&(peer->ip)+0),
                            *((unsigned char *)&(peer->ip)+1),
                            *((unsigned char *)&(peer->ip)+2),
                            *((unsigned char *)&(peer->ip)+3)
                          );
                    //peer->connect_retries = 0;

                    printf("<RITESH> Sending Handshake\n");

                    /* send the handshake message */
                    char protocol[] = "BitTorrent protocol";
                    unsigned char pstrlen = strlen(protocol); // not sure if this should be with or without terminator
                    unsigned char buf[pstrlen+49];
                    buf[0]=pstrlen;
                    memcpy(buf+1,protocol,pstrlen); 
                    memcpy(buf+1+pstrlen+8,digest,20);
                    memcpy(buf+1+pstrlen+8+20,peer_id,20);				 

                    send(peer->socket,buf,sizeof(buf),0);
                    peer->connected = 1;
                    peer->connecting = 0;
                }
                else if(peer->send_count > 0) {
                    int bytes_sent;
                    /* send data for this peer */
                    bytes_sent = send(peer->socket, peer->outgoing, peer->send_count, 0);
                    if(bytes_sent  == -1){
                        fprintf(stderr,"<RITESH> send() returned -1..exitting\n");
                        peer = peer->next;
                        continue;
                        //int x;
                        //scanf("%d",&x);
                        //exit(300);
                    }
                    if(bytes_sent > peer->send_count){
                        fprintf(stderr,"<RITESH> send() returned more than sent bytes..exitting\n");
                        //int x;
                        //scanf("%d",&x);
                        //exit(300);
                    }
                    printf("<RITESH> %d bytes sent to peer ",bytes_sent);
                    print_peer_ip(peer);
                    /* recalculate */
                    peer->send_count -= bytes_sent;
                    if(peer->send_count){
                        /* realign */
                        memmove(peer->outgoing, peer->outgoing + bytes_sent, peer->send_count);
                    }
                }
            }
            peer = peer->next;
        }
        if(all_peers_down_flag == 1){
            break;
        }

        /*
           int all_peers_disconnected_flag;
           all_peers_disconnected_flag = 1;
           peer = peers;
           while(peer){
           if(FD_ISSET(peer->socket,&writeset_copy)){
           all_peers_disconnected_flag = 0;
           break;
           }

           peer = peer->next;
           }
           if(all_peers_disconnected_flag == 1){
           printf("<RITESH>All peers are disconnected\n");
           break;
           }
           */

        /*
        // HACK request pieces that can be requested 
        struct peer_state *r;
        r = peers;
        while(r){
        if(r->connected && !r->choked && r->requested_piece == -1){
        r->requested_piece = next_piece(-1,r);	
        if(r->requested_piece != -1)
        request_block(r,r->requested_piece,0);
        }
        r = r->next;
        }
        */
    }

shutdown:
    /* Either all peers are down or all peices are downloaded */
    printf("<RITESH> info: all_peers_down_flag = %d\n", all_peers_down_flag);
    peer = peers;
    while(peer){
        if(peer->connected){
            if(all_peers_down_flag == 1){
                printf("<RITESH> all_peers_down_flag set even when a  peer->connected == 1 peer: ");
                /* this would happen if connected peers are choking */
                print_peer_ip(peer);
                //exit(400);//remove
            }
            close_peer(peer);
        }
        peer = peer->next;
    }
    fprintf(stderr,"All peer connections closed.\n");
    return;
}

int connect_to_peer_new(struct peer_addr* peeraddr,int sockid) 
{
    struct in_addr a;
    a.s_addr = peeraddr->addr;
    printf("\n<RITESH>connecting to peer %s:%d\n",inet_ntoa(a),ntohs(peeraddr->port));				 



    fprintf(stderr,"Connecting...\n");

    if(peer_connected(peeraddr->addr,sockid,peeraddr->port)) {
        fprintf(stderr,"Already connected\n");
        return 0;
    }

    int s = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(s, F_SETFL, O_NONBLOCK); /* Make connect() non blocking */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = peeraddr->addr;
    addr.sin_port = peeraddr->port;

    /* after 60 seconds of nothing, we probably should poke the peer to see if we can wake them up */
    //	 struct timeval tv;
    //	 tv.tv_sec = 60;
    //	 if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv)) {
    //		 perror("setsockopt");
    //		 return 0;
    //	 }	

    printf("<RITESH>Connect called\n");


    int res = connect(s, (struct sockaddr*)&addr, sizeof(addr));
    if(res == -1) {
        printf("<RITESH>Connect succeeded\n");
        FD_SET(s,&readset);	
        FD_SET(s,&writeset);	
        FD_SET(s,&exceptionset);	
    }
    else{
        perror("Error while connecting.");		
        return 1;
    }  

    /* register the new peer in our list of peers */
    struct peer_state* peer = calloc(1,sizeof(struct peer_state));
    peer->socket = s;
    peer->ip = peeraddr->addr;
    peer->port = peeraddr ->port;
    peer->connected = 0;
    peer->handshake_pending = 1;
    peer->choked = 1;
    peer->incoming = malloc(BUFSIZE);
    peer->outgoing = malloc(BUFSIZE*16);
    printf("<RITESH> BUFSIZE = %d\n", BUFSIZE);
    peer->bitfield = calloc(1,file_length/piece_length/8+1); //start with an empty bitfield
    peer->next = peers;
    peers=peer;
    peer->connecting = 1;
    peer->sockid = sockid;

}

void delete_all_peers()
{
    struct peer_state *p;
    p = peers;
    while(p)
    {
        struct peer_state *to_free;
        to_free = p;
        free(p->incoming);
        free(p->outgoing);
        free(p->bitfield);

        if(p->connected || p->connecting){
            close(p->socket);
        }
        if(piece_status[p->requested_piece] != PIECE_FINISHED)
            piece_status[p->requested_piece] = PIECE_EMPTY;
        p = p->next;
        free(to_free);
    }
    peers = NULL;
}

/* handle_announcement reads an announcement document to find some peers to download from.
	 start a new tread for each peer.
 */
void handle_announcement(char *ptr, size_t size) 
{
	struct bencode* anno = ben_decode(ptr,size);

	printf("Torrent has %lld seeds and %lld downloading peers. \n",
				 ((struct bencode_int*)ben_dict_get_by_str(anno,"complete"))->ll,
				 ((struct bencode_int*)ben_dict_get_by_str(anno,"incomplete"))->ll);
		 
	struct bencode_list *peers = (struct bencode_list*)ben_dict_get_by_str(anno,"peers");

	/* unfortunately, the list of peers could be either in bencoded format, or in binary format. 
		 the binary format value is passed as a string, bencoded format is passed as a list. 
		 We handle the two cases separately below.
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

			//pthread_create(&thread,0,connect_to_peer,&peerlist[i]);
			connect_to_peer_new(&peerlist[i],1);
			//connect_to_peer_new(&peerlist[i],2);
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

			// pthread_create allows us to pass in one pointer. For the bencoded case, we allocate a new
			// peer_addr struct, and pass that in. Note that we can't allocate this on the stack, as it would
			// be immediately overwritten before the thread even got started. 

			struct peer_addr *peeraddr = malloc(sizeof(struct peer_addr));
			peeraddr->addr=inet_addr(address);
			peeraddr->port=htons(port);

			//pthread_t thread;
			//pthread_create(&thread,0,connect_to_peer,&peeraddr);
			connect_to_peer_new(peeraddr,1);
			//connect_to_peer_new(peeraddr,2);
		}
	}
	
	handle_messages();
	delete_all_peers();

	
	// wait for a signal that all the downloading is done
	while(missing_blocks()>0) {
		fprintf(stderr,"One thread finished, %d active peers, %d missing blocks\n",active_peers(),missing_blocks());

		//if(active_peers()==0 && missing_blocks()>0) {
		if(missing_blocks()>0) { /*hack*/
		    int d;
			printf("Ran out of active peers, reconnecting.Press a number and ENTER key\n");
			//scanf("%d",&d);
			start_peers();
		}

	}
	printf("<RITESH>Download complete\n");
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
	// compile a suitable announce URL for our document
	{
	    int bytes_left = 0;
	    bytes_left = missing_blocks() * piece_length;
	    if(piece_status[total_pieces-1] == PIECE_EMPTY){
	        bytes_left -= piece_length;
	        bytes_left += file_length%piece_length;
        }
	sprintf(announce_url,"%s%d",announce_url1, file_length);
	printf("Announce URL: %s\n",announce_url);
    }
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
    fp = fopen("stdritesh","w+");
    if(fp==NULL){
        printf("Error opening stdritesh\n");
        exit(900);
    }
	if(argc<2) {
		fprintf(stderr,"Usage: ./hw4 <torrent file>\n");
		exit(1);
	}

	//setvbuf(stdout,screenbuf,_IOFBF,10000);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
		
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
		FILE *outfile;
		struct bencode_str* name = (struct bencode_str*)ben_dict_get_by_str(info,"name");
		if(name) {
			outfile = fopen(name->s,"r+");
			file_length = ((struct bencode_int*)ben_dict_get_by_str(info,"length"))->ll;			
		}
	}

	piece_length = ((struct bencode_int*)ben_dict_get_by_str(info,"piece length"))->ll;

	// create our output file, and set up a piece_status array, and a couple of pthread sync. variables
	piece_status = calloc(1,sizeof(int)*(int)(file_length/piece_length+1)); //start with an empty bitfield
	total_pieces = (int)(file_length/piece_length+1);
    my_bitfield = calloc(1,file_length/piece_length/8+1); //start with an empty bitfield

	piece_counts = calloc(1,sizeof(int)*(int)(file_length/piece_length+1));//initialized to 0

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
	sprintf(announce_url,"%s?info_hash=%s&peer_id=%s&port=6882&left=%d",((struct bencode_str*)ben_dict_get_by_str((struct bencode*)torrent,"announce"))->s,info_hash,peer_id,file_length);
	sprintf(announce_url1,"%s?info_hash=%s&peer_id=%s&port=6882&left=",((struct bencode_str*)ben_dict_get_by_str((struct bencode*)torrent,"announce"))->s,info_hash,peer_id);
	printf("Announce URL: %s\n",announce_url);


	FD_ZERO(&readset);
	FD_ZERO(&writeset);
	FD_ZERO(&exceptionset);
	start_peers();
	fclose(fp);
}

