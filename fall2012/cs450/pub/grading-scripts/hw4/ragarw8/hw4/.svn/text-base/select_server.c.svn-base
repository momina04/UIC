/*
 * =====================================================================================
 *
 *       Filename:  select_server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/07/2012 05:29:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */


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

#include<pthread.h>

#include"hw4.h"

#ifndef FD_COPY
#define FD_COPY(f, t)   (void)(*(t) = *(f))
#endif

int s[10];
int conns=0;
int conn_closed[10];

int main (int argc, char *argv[])
{

	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(server_sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}
	
	struct sockaddr_in addr; 	// internet socket address data structure
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080); // byte order is significant
	addr.sin_addr.s_addr = INADDR_ANY; // listen to all interfaces
	
	int res = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
	if(res < 0) {
		perror("Error binding to port");
		exit(1);
	}

    int yes=1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

	/* workaround for funny OS X bug - need to start listening without select */
    res=fcntl(server_sock, F_SETFL, O_NONBLOCK);
    if(res < 0) { perror("fcntl"); } 

	if (listen (server_sock, 1) < 0) { perror ("listen"); exit(1); } 
	fcntl(server_sock, F_SETFL, 0);

	/* initializing data structure for select call */
	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(server_sock,&readset);	
	while(1) {
		fd_set rdyset;
		FD_COPY(&readset,&rdyset);
		int rdy = select(FD_SETSIZE,&rdyset,0,0,0);

		/* if the server_sock has a new connection coming in, accept it */
		if(FD_ISSET(server_sock,&rdyset)) {
			int sock;
			struct sockaddr_in remote_addr;
			unsigned int socklen = sizeof(remote_addr); 
			
			sock = accept(server_sock, (struct sockaddr*)&remote_addr, &socklen);
			if(res < 0) { perror("Error accepting connection"); exit(1); }

			/* allocate and initialize new client state */

			s[conns]= sock;
			conns++;
			printf("Accepted connection, now %d concurrent clients\n",conns);
			/* add new socket to fd_set for select */
			FD_SET(sock,&readset);
		}

        int i;
		for(i=0;i<conns;i++) {
		    if(conn_closed[i] == 1) continue;
			if(FD_ISSET(s[i],&rdyset)) {
				char buf[255];
				memset(buf,0,255);
				int rec_count = recv(s[i],buf,255,0);
				if(rec_count > 0) {
				    printf("message recieved - %d bytes",rec_count);
				}
				/* if we got nothing, that means the connection is closed */
				else {
				    conn_closed[i]=1;

					shutdown(s[i],SHUT_RDWR);
					close(s[i]);						
					FD_CLR(s[i],&readset);
					continue;
				}
			} // end if(FD_ISSET...
		} // end while
	}
	shutdown(server_sock,SHUT_RDWR);

    return 0;
}/* main */
