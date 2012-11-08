#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

int main(int argc, char** argv) {	

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}
	
	struct sockaddr_in addr; 	// internet socket address data structure
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080); // byte order is significant
	addr.sin_addr.s_addr = INADDR_ANY;
	
	int res = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	if(res < 0) {
		perror("Error binding: ");
		exit(1);
	}

	char buf[255];
	memset(&buf,0,sizeof(buf));

	int recv_count = recv(sock, buf, 255, 0);
	if(recv_count<0) { perror("Receive failed");	exit(1); }

	printf("%s",buf);																							

	shutdown(sock,SHUT_RDWR);
}


