#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

void usage() {
	printf("Usage: addrinfo <hostname>\n");
	exit(1);
}

int main(int argc, char** argv)
{
	if(argc<2) usage();

	struct addrinfo *result=0; 		
	int return_code=getaddrinfo(argv[1],0,0,&result);
	if(result!=NULL) {
		printf("The name %s resolves to IP address %s\n",
					 argv[1],inet_ntoa(((struct sockaddr_in*)result->ai_addr)->sin_addr));
	}
	else {
		herror("An error occured during host name lookup\n");
		exit(1);
	}

	return 0;
}

