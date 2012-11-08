/* 

Minimal TCP client example. The client connects to port 8080 on the 
local machine and prints up to 255 received characters to the console, 
then exits. To test it, try running a minimal server like this on your
local machine:

echo "Here is a message" | nc -l 8080

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

int main(int argc, char** argv) 
{	
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}
	
	struct sockaddr_in addr; 	// internet socket address data structure
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);    // byte order is significant
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	
	int res = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	if(res < 0) {
		perror("Error connecting: ");
		exit(1);
	}

	char buf[255];
	memset(&buf,0,sizeof(buf));
	int recv_count = recv(sock, buf, 255, 0);
	if(recv_count<0) { perror("Receive failed");	exit(1); }

	printf("%s",buf);																							

	shutdown(sock,SHUT_RDWR);
}
