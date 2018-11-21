// client.c
// 정재화
// 2018-11-21

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAX_SIZE 100
#define PORT 5400
 
int main(int argc,char **argv){
    int sockfd,n;
    char sendline[MAX_SIZE];
    char recvline[MAX_SIZE];
    char len[MAX_SIZE];

    struct sockaddr_in servaddr;
 
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);
 
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
 
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));
 
    int cc = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(cc < 0){
        printf("err");
        close(1);
    }

    while(1){
        bzero( sendline, MAX_SIZE);
        bzero( recvline, MAX_SIZE);
        bzero( len, MAX_SIZE);
       
	printf("msg : "); 
        fgets(sendline,MAX_SIZE,stdin); /*stdin = 0 , for standard input */
        sendline[strlen(sendline)-1] = '\0';
        sprintf(len, "%d", (int)strlen(sendline));
        write(sockfd,len,4);
        printf("msg len : %s\n", len);
        write(sockfd,sendline,strlen(sendline));
        if(!strcmp(sendline, "quit")){
            close(sockfd);
            break;
        }
       
        read(sockfd,recvline,MAX_SIZE);
        printf("echo : %s\n",recvline);
    }
 
}