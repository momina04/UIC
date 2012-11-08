/*
 * =====================================================================================
 *
 *       Filename:  hw2.c
 *
 *    Description:  Multithreaded HTTP Server
 *
 *        Version:  1.0
 *        Created:  09/07/2012 06:14:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *        Company:  University of Illinois, Chicago
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>       /* for AF_INET */
#include <netdb.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>

typedef struct encaps{
    int sockfd_connection;
    char directory_name[64];
}encaps;

void *thread_handler(void *ptr);

void handle_connection(unsigned int sockfd_connection,char *directoryname);

void usage()
{
    printf("Usage: hw2 <port> <local_directory_name>\n");
    exit(1);
}/* usage */

int main (int argc, char *argv[])
{
    int sockfd_server = 0;
    int sockfd_connection = 0;
    int status = 0;
    int listen_port = 0;
    char directory_name[64] = {'\0'};
	struct sockaddr_in server_addr;
	struct sockaddr_in remote_addr;
	unsigned int socklen = sizeof(remote_addr);
	pthread_t threads[100];
	int threadcount = 0;
	struct encaps *e;
	int sockopts = 1;


    if(argc < 3) 
        usage();

    listen_port = atoi(argv[1]);
    strcpy(directory_name, argv[2]);

    sockfd_server = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt( sockfd_server, SOL_SOCKET, SO_REUSEADDR, &sockopts, sizeof(sockopts) );
    if(sockfd_server < 0){
        perror("TCP Socket creation failed\n");
        exit(100);
    }

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(listen_port); 
	server_addr.sin_addr.s_addr = INADDR_ANY;

	status = bind(sockfd_server, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(status < 0) {
        perror("Could not bind to port.");
        exit(100);
    }

	do {
		status = listen(sockfd_server,0);
		if(status < 0) {
			perror("listen() failed");
			exit(100);
		}
		
		sockfd_connection = accept(sockfd_server, (struct sockaddr*)&remote_addr, &socklen);
		if(sockfd_connection < 0) {
			perror("accept failed()");
			exit(100);
		}

		e = (struct encaps*)malloc(sizeof(struct encaps));
		if(e == NULL){
		    perror("Malloc for encaps failed.");
		    exit(100);
        }
		e->sockfd_connection = sockfd_connection;
		strcpy(e->directory_name, directory_name);
		pthread_create(&threads[threadcount], NULL, thread_handler, (void *) e);
		threadcount = (threadcount + 1) % 100;
	} while(1);

	shutdown(sockfd_server, SHUT_RDWR);
    return 0;
}/* main */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  thread_handler
 *  Description:  Thread handler function
 * =====================================================================================
 */
void* thread_handler(void *message)
{
    struct encaps* e;
    e = (struct encaps *) message;
    handle_connection(e->sockfd_connection, e->directory_name);
    free(e);
    return NULL;
}/* thread_handler */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  handle_connection
 *  Description:  Handles a single HTTP connection.
 * =====================================================================================
 */
void handle_connection(unsigned int sockfd_connection,char *directory_name)
{
    char resource[100];
    char filename[200];
    FILE *fp;
    int bytes_received = 0;
    char recvbuffer[1024];
    char sendbuffer[1024*1024*2];
    struct stat statbuf;
    int offset = 0;
    int status = 0;
    do{
        status = recv(sockfd_connection, &recvbuffer[bytes_received], 256, 0);
        if(status < 0 ) { 
            perror("recv() failed");
            exit(100); 
        }
        bytes_received += status;
    }while(status < 0);

    printf("Received %d bytes\n", bytes_received);
    printf("<<< START of HTTP GET >>>\n");
    printf("%s",recvbuffer);																							
    printf("<<< END of HTTP GET >>>\n");

    sscanf(recvbuffer,"GET %s",resource);
    sprintf(filename,"./%s%s",directory_name,resource);
    printf("Request for %s being processed\n", filename);
    status = stat(filename, &statbuf);
    if(status == -1){
        sprintf(sendbuffer,"HTTP/1.0 404 Not Found\r\n\r\n");
        strcat(sendbuffer,"<HTML><BODY><H1>404 PAGE NOT FOUND</H1></BODY></HTML>");
        offset = strlen(sendbuffer);
        printf("<<< HTTP RESPONSE START >>> \n");
        printf("%s", sendbuffer);
        printf("<<< HTTP RESPONSE END >>> \n");
    }
    else{
        switch (statbuf.st_mode & S_IFMT) {
            default:
                printf("ERROR: Some other st_mode in stat()\n");

            case S_IFDIR:
                strcat(filename,"index.html");
                break;

            case S_IFREG:
                /* Do Nothing */
                break;

        }

        fp = fopen(filename,"r");

        if(fp == NULL){
            sprintf(sendbuffer, "HTTP/1.0 404 Not Found\r\n\r\n");
            strcat(sendbuffer, "<HTML><BODY><H1>404 PAGE NOT FOUND</H1></BODY></HTML>");
            offset = strlen(sendbuffer);
            printf("<<< HTTP RESPONSE START >>> \n");
            printf("%s", sendbuffer);
            printf("<<< HTTP RESPONSE END >>> \n");
        }
        else{
            char *ext,*tmp;
            sprintf(sendbuffer, "HTTP/1.0 200 OK\r\n");

            tmp = filename + 1;
            do {
                ext = tmp;
                tmp = strchr(tmp + 1, '.');
            }while(tmp != '\0');

            if(*ext == '.' ){
                ext++;
                printf("Extension: %s\n", ext);
                if(strcmp(ext,"html") == 0){
                    strcat(sendbuffer, "Content-Type: ");
                    strcat(sendbuffer,"text/html\r\n");
                }
                else if(strcmp(ext,"gif") == 0){
                    strcat(sendbuffer, "Content-Type: ");
                    strcat(sendbuffer,"image/gif\r\n");
                }
                else if(strcmp(ext,"png") == 0){
                    strcat(sendbuffer, "Content-Type: ");
                    strcat(sendbuffer,"image/png\r\n");
                }
                else if(strcmp(ext,"jpeg") == 0 || strcmp(ext,"jpg") == 0){
                    strcat(sendbuffer,"image/jpeg\r\n");
                }
            }

            strcat(sendbuffer, "\r\n");
            printf("<<< HTTP RESPONSE START >>> \n");
            printf("%s", sendbuffer);
            printf("<<< HTTP RESPONSE END >>> \n");
            offset = strlen(sendbuffer);
            do{
                status = fread(&sendbuffer[offset], 1, 1, fp);
                offset += 1;
            }while(status != 0);
            offset--;

            fclose(fp);
        }
    }
    send(sockfd_connection, sendbuffer, offset, 0);
    shutdown(sockfd_connection, SHUT_RDWR);
    return;
}/* handle_connection */

