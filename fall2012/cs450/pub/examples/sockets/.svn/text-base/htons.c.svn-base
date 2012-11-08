#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

/* Code snippet demonstrating byte order significance. */

main(int argc, char** argv) {
	if(argc<2) { perror("Missing input parameter"); exit(1); }

	int input = atoi(argv[1]);
	printf("Host order: %d (0x%02x%02x) network order: %d (0x%02x%02x)\n",
				 input,
				 input&0xff,(input&0xff00)>>8,
				 htons(input),
				 (input&0xff00)>>8,input&0xff);
	
}

