#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void usage() {
	printf("Usage: hw1 <URL>\n");
	exit(1);
}

int main(int argc, char** argv) 
{
	if(argc<2) usage();
	
	/* grab the host and path from the URL */
	char host[100], path[100], file[100];
	memset(path,0,sizeof(path));
	int scanned=sscanf(argv[1],"http://%[^/]/%s",host,path);
		
	/* parse out a nice filename to use */
	char *filepart=strrchr(path,'/');
	if(filepart && strlen(filepart+1)) // the strlen call checks if there's a filename after the slash
		strcpy(file,filepart+1);
	else 
		strcpy(file,"index.html");	

	/* resolve the host name to an IP address */
	struct addrinfo hints;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* addrinfo;
	int lookup_result=getaddrinfo(host,"http",&hints,&addrinfo);
	if(lookup_result!=0) { 		
		printf("Error: no such host %s\n",host);
		perror(gai_strerror(lookup_result));
		exit(1);
	}

	printf("Connecting to host %s (%s) to retrieve document %s. \n",
				 host,
				 inet_ntoa( ((struct sockaddr_in*)addrinfo->ai_addr)->sin_addr),
				 path);	

	/* create a new socket for talking to remote host */
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}
	
	/* connect to the remote host */
	int res = connect(sock, 
										(struct sockaddr*)addrinfo->ai_addr, 
										sizeof(struct sockaddr_in));
	if(res < 0) {
		perror("Error connecting.");
		exit(1);
	}
	freeaddrinfo(addrinfo);

	/* formulate and send the request */
	char request[255];
	sprintf(request,"GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n",path,host);
	send(sock,request,strlen(request),0);

	/* start receiving the response */
	const int MAX_HDR = 1024*1024;
 	char buf[MAX_HDR]; // expect headers to be no more than a megabyte! 
	memset(buf,0,MAX_HDR); 
	int recv_total = 0;
	int recv_count = 0;

	// keep receiving request until empty line is encountered. This way we don't 
	// have to worry about packet boundaries in the parsing.
	while(!strstr(buf,"\r\n\r\n")) {
		recv_count = recv(sock, buf+recv_total, sizeof(buf)-1-recv_total, 0);
		if(recv_count==0) {
			printf("receiving request failed\n");
			exit(1);
		}
		recv_total+=recv_count;
	}

	// now parse the contents
	int code;
	sscanf(buf,"HTTP/1.%*[01] %d ",&code);
	if(code==200) 
		printf("Receiving file.\n");
	else {
		printf("Got error code %d\n",code);
		char errorline[100];
		sscanf(buf,"%[^\r]",errorline);
		printf("%s\n",errorline);
		exit(1);
	}

	// skip to the empty line in the response, then start writing data to file
	char *end_of_headers;
	if(end_of_headers=strstr(buf, "\r\n\r\n")) {
		printf("Saving to file %s\n",file);

		int bytes_left=recv_total-((end_of_headers-buf)+4);

		FILE *outfile=fopen(file,"w+");
		fwrite(end_of_headers+4,1,bytes_left,outfile);

		while((recv_count=recv(sock,buf,10000,0))!=0) {
			fwrite(buf,recv_count,1,outfile);		
		}
		fclose(outfile);
	}
	else {
		printf("Could not find end of headers. Quitting.\n");
	}

	shutdown(sock,SHUT_RDWR);
}
