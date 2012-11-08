#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

void usage() {
	printf("Usage: gethostbyname <hostname>\n");
	exit(1);
}

int main(int argc, char** argv)
{
	if(argc<2) usage();

	struct hostent *result; 		
	result=gethostbyname(argv[1]);
	if(result!=NULL) {

		/* The third argument to printf looks a little complicated.
			 inet_ntoa takes a 'struct in_addr' and produces a string 
			 containing the IP address written out in plaintext.

			 But what's with all the casting? The h_addr_list contains
			 a list of IP addresses, each address 4 bytes long, but in
			 true C style, it's untyped: just a char array. On the
			 other hand, a 'struct in_addr' is just 4 bytes too. So
			 we first cast the char* to the first address as a 
			 'struct in_addr *', and then de-reference the pointer. 
			 In the end we have the required 'struct in_addr', passed
			 to inet_ntoa.

			 A longer (and some would argue, better) way to write it:
			 char *rawaddr = result->h_addr_list[0]; // points to 4 byte address
			 struct in_addr *addr = (struct in_addr*)rawaddr; // cast to correct type
			 char *straddr = inet_ntoa(*addr); // pass the dereferenced value to ntoa
		*/

		printf("The name %s resolves to IP address %s\n",
					 argv[1],inet_ntoa(*((struct in_addr*)result->h_addr_list[0])));
	}
	else {
		herror("An error occured during host name lookup\n");
		exit(1);
	}

	return 0;
}

