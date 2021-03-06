﻿#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_SIZE 100
#define PORT 5400
#define P_SIZE 4

int set_nonblock(int sockfd);
int is_nonblock(int sockfd);

int main(){
	char str[MAX_SIZE];
	int listen_fd, comm_fd, on=1;
	socklen_t clilen;
	struct sockaddr_in servaddr;
	struct sockaddr_in client_addr;

	int n_byte = 0;
	
	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket fail");
		exit(1);
	}
	
	if((setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0){
		perror("set socket fail");
		close(listen_fd);
		exit(1);
	}
	
	bzero( &servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	
	bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	listen(listen_fd, 10);

	if(set_nonblock(listen_fd) == -1){
		perror("1 : set_nonblock fail");
		exit(1);
	}


	while(1){

		errno = 0;
		clilen = sizeof(client_addr);
		
		if((comm_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &clilen))!= -1){
			printf("Server : client connected\n");
			printf("Accept Socket = %d\n", comm_fd);
			printf("Accept IP : %s, Port : %u\n", inet_ntoa(client_addr.sin_addr),(unsigned)ntohs(client_addr.sin_port));
		
			if(is_nonblock(comm_fd) != 0 && set_nonblock(comm_fd)){
				perror("2 : set_nonblock fail");
				exit(1);
			} 
		} else if(errno == EAGAIN ){
			continue;   
		} else {
			perror("accept fail");
		}
	
		while(1){
	        	int msg_byte = 0;

			while(1){
				msg_byte = 0;
				n_byte = -1;
				bzero(str, MAX_SIZE);
				n_byte = recv(comm_fd, str, P_SIZE, 0);
				// 4byte를 먼저 읽어옴 : 받을 메시지의 길이
				if(n_byte == -1){
					if(errno == EWOULDBLOCK) continue;
				} else {
					break;
				} 
			}
        		printf("size : %s\n", str);
       		 	msg_byte = atol(str);
			bzero(str, MAX_SIZE);
	
			
			while(1){
				n_byte = -1;
				n_byte = recv(comm_fd, str, msg_byte, 0);
		
				if(n_byte == -1 && errno == EWOULDBLOCK){
					continue;
				} else { 
					printf("recv data : %s\n", str);
					printf("Echoing back - %s\n", str);
					if( 0 != send(comm_fd, str, strlen(str), 0)){
						break;	
					} else {
						perror("sending error");
					}
					break;
				}
			}
    
		        if(!strcmp(str, "quit")){
				//close(comm_fd);
				n_byte =-1; 
				printf("socket close() complete\n");
				break;
			}
		}
	}
	close(listen_fd);
}

int is_nonblock(int sockfd) {
    int val;
    val=fcntl(sockfd, F_GETFL,0);
    if(val & O_NONBLOCK)
        return 0;
    return -1;
}

int set_nonblock(int sockfd) {
    int val;
    val=fcntl(sockfd, F_GETFL,0);
    if(fcntl(sockfd, F_SETFL, val | O_NONBLOCK) == -1)
        return -1;
    return 0;
}