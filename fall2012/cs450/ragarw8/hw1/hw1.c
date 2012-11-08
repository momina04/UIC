/*
 * =====================================================================================
 *
 *       Filename:  hw1.c
 *
 *    Description: HTTP Client
 *
 *        Created:  08/31/2012 12:54:50 AM
 *       Compiler:  gcc
 *
 *         Author:  Ritesh Agarwal (Omi), ragarw8@uic.edu
 *                  University of Illinois, Chicago
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

#define MAXGETREQUESTSIZE 256
#define MAXURLSIZE 100
#define MAXFILENAMESIZE 50

void usage()
{
    printf("Usage: hw1 <url>\n");
    exit(100) ;
}/* usage */

void get_filename_from_URL(char *URL, char *filename);
void form_HTTP_GET_request(char *URL,char *getrequest, char *Hostname);
void writetofile(char *filename, char *buf, unsigned int bytes);
void process_HTTP_GET_response(char *getresponse, unsigned int response_length, char *filename);

int main (int argc, char *argv[])
{

    char getrequest[MAXGETREQUESTSIZE];
    char filename[MAXFILENAMESIZE];
    char URL[MAXURLSIZE]={(char)0};
    char hostname[60]={'\0'};
    struct hostent *hostentry = NULL;
    struct sockaddr_in addr;
    int sockfd = 0;
    int status = 0;
    char recvbuf[2*1024*1024]; /* 2MB to be safe */
    int bytesrecvd = 0;

    if(argc<2) 
        usage();

    strcpy(URL,argv[1]);
    /*
     * Convert http://www.google.com to http://www.google.com 
     * This is because of the assumption that was done in programming due to a statement in problem statement.
     */
    get_filename_from_URL(URL, filename);
    form_HTTP_GET_request(URL, getrequest, hostname);
    printf("%s\n%s\n%s\n", URL, getrequest, filename);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("TCP Socket creation failed\n");
        exit(100);
    }

    hostentry = gethostbyname(hostname);
    if(hostentry == NULL){
        /* Problem statement: The third and fourth* should exit with error i.e. exit(1). */
        perror("DNS lookup failed\n");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    memcpy(&(addr.sin_addr), *(hostentry->h_addr_list), sizeof(addr.sin_addr));
    status = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if(status < 0 ){
        perror("connect() failed.\n");
        exit(100);
    }

    send(sockfd, getrequest, strlen(getrequest), 0);
    
    bytesrecvd = 0;
    do{
        status = recv(sockfd, &recvbuf[bytesrecvd], 256, 0);
        if(status < 0){
            printf("recv() error occured %d", status);
            exit(100);
        }
        else if (status == 0){
            printf("Received total of %d bytes.\n", bytesrecvd);
        }
        bytesrecvd += status;
    }while(status > 0);

    process_HTTP_GET_response(recvbuf, bytesrecvd, filename);
    shutdown(sockfd, SHUT_RDWR);

    return 0;
}/* main */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_filename_from_URL
 *  Description:  Will populate filename with the name of file, index.html by default
 * =====================================================================================
 */
void get_filename_from_URL(char *URL, char *filename)
{
    char *last_directory = NULL;
    char *search_cursor = NULL;
    if(URL[strlen(URL)-1] == '/' || NULL == strchr(URL+7,'/')){
        strcpy(filename,"index.html");
    }
    else{
        search_cursor = URL;
        while(search_cursor){
            last_directory = search_cursor;
            search_cursor = strchr(search_cursor + 1, '/');
        }
        strcpy(filename,last_directory+1);
    }

    return ;
}/* get_filename_from_URL */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  form_HTTP_GET_request
 *  Description:  Will form a GET REQUEST to be sent to HTTP server, 
 *                  will also extract and return hostname from URL
 * =====================================================================================
 */
void form_HTTP_GET_request(char *URL,char *getrequest, char *Hostname)
{
  
    char *resource = NULL;
    char *host = NULL;
    resource = strchr(URL,'/');
    host = resource+2;
    resource = strchr(resource + 2 , '/');
    if(resource != NULL){
        *resource = '\0';
        resource++;
    }

    sprintf(getrequest,"GET /%s HTTP/1.0\r\n",resource == NULL? "":resource);
    sprintf(getrequest,"%sHost: %s\r\n",getrequest,host);
    strcpy(Hostname,host);
    sprintf(getrequest,"%s\r\n",getrequest);
    if(resource != NULL){
        resource--;
        *resource = '/';
    }
    return;
}/* form_HTTP_GET_request */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  writetofile
 *  Description:  Will write specified no. of bytes to file.
 * =====================================================================================
 */
void writetofile(char *filename, char *buf, unsigned int bytes)
{
    FILE *fp = NULL;
    fp = fopen(filename,"w");
    fwrite(buf,1,bytes,fp);
    if(fp == NULL){
        printf("Failed to open %s in write mode",filename);
        exit(100);
    }
    fclose(fp);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  process_HTTP_GET_response
 *  Description:  Will display the HTTP header of GET Response on STDOUT
 *                and download the webpage.
 *                For Case HTTP 404 Not found will exit(1) as per homework statement. 
 *                The web page returned will still be downloaded to be safe.
 * =====================================================================================
 */
void process_HTTP_GET_response(char *getresponse, unsigned int response_length, char *filename)
{
    int filesize = 0;
    char dummystring[20];
    char *start_of_file = NULL;
    int HTTP_response_code = 0;

    start_of_file = strstr(getresponse,"\r\n\r\n"); /* Search for end of HTTP header */
    *start_of_file = '\0';
    printf("<<< HTTP Header START >>>\n");
    printf("%s\n",getresponse);
    printf("<<< HTTP Header END >>>\n");
    *start_of_file = '\r';

    start_of_file += 4;
    filesize = response_length - (start_of_file - getresponse);
    writetofile(filename, start_of_file, filesize);

    sscanf(getresponse,"%s %d", dummystring, &HTTP_response_code); 
    if(HTTP_response_code == 404 ){
        /* Problem statement: The third* and fourth should exit with error i.e. exit(1).
         */
        perror("HTTP: 404 Page not found\n");
        exit(1); /*we will still write the file to be safe */
    }
    if(HTTP_response_code >=300 && HTTP_response_code <= 399 ){
        printf("HTTP (3xx): Response code %d....Continuing\n",HTTP_response_code);
        exit(1); /*we will still write the file to be safe */
    }

    return ;
}/* process_HTTP_GET_response */
