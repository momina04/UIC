
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

fd_set readset;
fd_set writeset;
fd_set exceptionset;
fd_set readset1;
fd_set writeset1;
fd_set exceptionset1;

#ifndef FD_COPY
#define FD_COPY(f, t)   (void)(*(t) = *(f))
#endif

int s[10];
int main (int argc, char *argv[])
{
    int i=0;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    for(i=0;i<2;i++){
        s[i] = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_port = htons(8080+i);
        fcntl(s[i], F_SETFL, O_NONBLOCK);
        int res = connect(s[i], (struct sockaddr*)&addr, sizeof(addr));

        if(res != -1){
            printf("Connect failed.\n");
            fflush(stdout);
            exit(1);
        }

        FD_SET(s[i],&readset);	
        FD_SET(s[i],&writeset);	
        FD_SET(s[i],&exceptionset);	
    }

    while(1){

        FD_COPY(&readset,&readset1);
        FD_COPY(&writeset,&writeset1);
        FD_COPY(&readset,&readset1);
        int rdy = select(FD_SETSIZE,&readset1,&writeset1,&exceptionset1,0);

        for(i=0;i<2;i++){
            if(FD_ISSET(s[i],&readset1)) {
                char buf[2048];
				int rec_count = recv(s[i],buf,255,0);
                printf("Readset for port %d---=%d bytes read\n", 8080+i, rec_count);
                if(rec_count==-1){
					shutdown(s[i],SHUT_RDWR);
					close(s[i]);						
					FD_CLR(s[i],&readset);
					FD_CLR(s[i],&writeset);
					FD_CLR(s[i],&exceptionset);
					printf("Connection closed.\n");
					continue;
                }
            }
            if(FD_ISSET(s[i],&writeset1)) {
                printf("Writeset for port %d\n", 8080+i);
                int status=0;
                char buf[2048]="Ritesh is the god of programming";
                status = send(s[i],buf,strlen(buf)+1,0);		
                if(status < 0)
                    printf("xxxxWriteset for port %d, send status = %d\n", 8080+i, status);
                printf("Writeset for port %d, send status = %d\n", 8080+i, status);
            }
            if(FD_ISSET(s[i],&exceptionset1)) {
                printf("Exceptionset for port %d\n", 8080+i);
            }
        }
    }
    
}/* main */
