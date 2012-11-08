#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "dns.h"
#include <errno.h>

static int debug=0, nameserver_flag=0;

int nameserver_count = 0;
char nameserver_list[100][16];
int currnameservercount = 0;

void usage() {
	printf("Usage: hw3 [-d] -i domain/ip_address\n\t-d: debug\n");
	exit(1);
	exit(1);
}

/* constructs a DNS query message for the provided hostname */
int construct_query(uint8_t* query, int max_query, char* hostname) {
	memset(query,0,max_query);

	in_addr_t rev_addr=inet_addr(hostname);
	if(rev_addr!=INADDR_NONE) {
		static char reverse_name[255];		
		sprintf(reverse_name,"%d.%d.%d.%d.in-addr.arpa",
						(rev_addr&0xff000000)>>24,
						(rev_addr&0xff0000)>>16,
						(rev_addr&0xff00)>>8,
						(rev_addr&0xff));
		hostname=reverse_name;
	}

	// first part of the query is a fixed size header
	struct dns_hdr *hdr = (struct dns_hdr*)query;

	// generate a random 16-bit number for session
	uint16_t query_id = (uint16_t) (random() & 0xffff);
	hdr->id = htons(query_id);
	// set header flags to request recursive query
	hdr->flags = htons(0x0000);	
	// 1 question, no answers or other records
	hdr->q_count=htons(1);

	// add the name
	int query_len = sizeof(struct dns_hdr); 
	int name_len=to_dns_style(hostname,query+query_len);
	query_len += name_len; 
	
	// now the query type: A or PTR. 
	uint16_t *type = (uint16_t*)(query+query_len);
	if(rev_addr!=INADDR_NONE)
		*type = htons(12);
	else
		*type = htons(1);
	query_len+=2;

	// finally the class: INET
	uint16_t *class = (uint16_t*)(query+query_len);
	*class = htons(1);
	query_len += 2;
 
	return query_len;	
}

unsigned int read_nameservers_from_file(char *filename)
{
    FILE *fp= NULL;
    unsigned int count = 0;

    fp = fopen(filename,"r");

    if(fp == NULL){
        return 0;
    }

    printf("Nameservers\n");
    while(EOF != fscanf(fp,"%s",nameserver_list[count])){
        printf("%d. %s\n",count+1,nameserver_list[count]);
        count++;
    }

    fclose(fp);
    return count;
}

char * get_next_nameserver(){
    char *ret = NULL;
    ret = nameserver_list[currnameservercount];
    currnameservercount = (currnameservercount + 1) % nameserver_count;
    return ret;
}

int main(int argc, char** argv)
{
	if(argc<2) usage();
	
	char *hostname=0;
	char *nameserver=0;
	unsigned int status = 0;
	
	char *optString = "-d-i:";
 	int opt = getopt( argc, argv, optString );
	// using a constant name server address for now.
	
	while( opt != -1 ) {
		switch( opt ) {      
		case 'd':
			debug = 1; 
			break;
		case 'i':
			hostname = optarg;
			break;	
		case '?':
			usage();
			exit(1);               
		default:
			usage();
			exit(1);
		}
		opt = getopt( argc, argv, optString );
	}
		
	if(!hostname) {
		usage();
		exit(1);
	}

    nameserver_count = read_nameservers_from_file("./root-servers.txt");
    if(nameserver_count == 0){
        perror("root-servers.txt empty or not present\n");
        exit(100);
    }

    nameserver = get_next_nameserver();

	in_addr_t nameserver_addr;
	int sock;
	
retry:
	nameserver_addr = inet_addr(nameserver);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}
	
	// construct the query message
	uint8_t query[1500];
	int query_len=construct_query(query,1500,hostname);

	struct sockaddr_in addr; 	// internet socket address data structure
	addr.sin_family = AF_INET;
	addr.sin_port = htons(53); // port 53 for DNS
	addr.sin_addr.s_addr = nameserver_addr; // destination address (any local for now)
	
	int send_count = sendto(sock, query, query_len, 0,
													(struct sockaddr*)&addr,sizeof(addr));
	if(send_count<0) { perror("Send failed");	exit(1); }	

	// await the response 
	uint8_t answerbuf[1500];
	struct timeval timeout;
	timeout.tv_sec = 5;

	if(0 > setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
    {
        perror("Failed to set sockopts timeout\n");
        exit(100);
    }
	int rec_count = recv(sock,answerbuf,1500,0);
	printf("rec_count = %d\n",rec_count);
	if(rec_count < 0){
        nameserver = get_next_nameserver();
        if(errno =  EAGAIN){
            printf("Nameserver not responding, moving to next one: %s\n",nameserver);
        }
        else{
            printf("RECV() error other than timeout, moving to next one: %s\n",nameserver);
        }
        goto retry;
    }
	
	// parse the response to get our answer
	struct dns_hdr *ans_hdr=(struct dns_hdr*)answerbuf;
	uint8_t *answer_ptr = answerbuf + sizeof(struct dns_hdr);
	
	// now answer_ptr points at the first question. 
	int question_count = ntohs(ans_hdr->q_count);
	int answer_count = ntohs(ans_hdr->a_count);
	int auth_count = ntohs(ans_hdr->auth_count);
	int other_count = ntohs(ans_hdr->other_count);

	// skip past all questions
	int q;
	for(q=0;q<question_count;q++) {
		char string_name[255];
		memset(string_name,0,255);
		int size=from_dns_style(answerbuf,answer_ptr,string_name);
		answer_ptr+=size;
		answer_ptr+=4; //2 for type, 2 for class
	}

	int a;
	int got_answer=0;

	// now answer_ptr points at the first answer. loop through
	// all answers in all sections
	for(a=0;a<answer_count+auth_count+other_count;a++) {
		// first the name this answer is referring to 
		char string_name[255];
		int dnsnamelen=from_dns_style(answerbuf,answer_ptr,string_name);
		answer_ptr += dnsnamelen;

		// then fixed part of the RR record
		struct dns_rr* rr = (struct dns_rr*)answer_ptr;
		answer_ptr+=sizeof(struct dns_rr);

		const uint8_t RECTYPE_A=1;
		const uint8_t RECTYPE_NS=2;
		const uint8_t RECTYPE_CNAME=5;
		const uint8_t RECTYPE_SOA=6;
		const uint8_t RECTYPE_PTR=12;
		const uint8_t RECTYPE_AAAA=28;

		if(htons(rr->type)==RECTYPE_A) {
			printf("The name %s resolves to IP addr: %s\n",
						 string_name,
						 inet_ntoa(*((struct in_addr *)answer_ptr)));
			got_answer=1;
		}
		// NS record
		else if(htons(rr->type)==RECTYPE_NS) {
			char ns_string[255];
			int ns_len=from_dns_style(answerbuf,answer_ptr,ns_string);
			if(debug)
				printf("The name %s can be resolved by NS: %s\n",
							 string_name, ns_string);					
			got_answer=1;
		}
		// CNAME record
		else if(htons(rr->type)==RECTYPE_CNAME) {
			char ns_string[255];
			int ns_len=from_dns_style(answerbuf,answer_ptr,ns_string);
			if(debug)
				printf("The name %s is also known as %s.\n",
							 string_name, ns_string);								
			got_answer=1;
		}
		// PTR record
		else if(htons(rr->type)==RECTYPE_PTR) {
			char ns_string[255];
			int ns_len=from_dns_style(answerbuf,answer_ptr,ns_string);
			printf("The host at %s is also known as %s\n",
						 string_name, ns_string);								
			got_answer=1;
		}
		// SOA record
		else if(htons(rr->type)==RECTYPE_SOA) {
			if(debug)
				printf("Ignoring SOA record\n");
		}
		// AAAA record
		else if(htons(rr->type)==RECTYPE_AAAA)  {
			if(debug)
				printf("Ignoring IPv6 record\n");
		}
		else {
			if(debug)
				printf("got unknown record type %hu\n",htons(rr->type));
		} 

		answer_ptr+=htons(rr->datalen);
	}
	
	if(!got_answer) printf("Host %s not found.\n",argv[2]);
	
	shutdown(sock,SHUT_RDWR);
	close(sock);
}
