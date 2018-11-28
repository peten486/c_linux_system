// server.c
// 정재화
// 2018-11-26
// non block 모드와 select를 이용한 서버 구현

#include <sys/types.h>
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
#define MAX_IDLE_SECS 500000
#define FD_SIZE 10

int set_nonblock(int sockfd);
int is_nonblock(int sockfd);
int send_nonblock(int fd, void* data, size_t size, int flags);
int recv_nonblock(int fd, void* buffer, size_t size, int flags);

int main(){
	char str[MAX_SIZE];
	int listen_fd, comm_fd, sockfd;

	socklen_t clilen;
	struct sockaddr_in servaddr;
	struct sockaddr_in client_addr;
	int client_list[FD_SIZE];
	
	int msg_byte, n_byte;
	int i, on = 1;
	int fd_num, maxfd, maxi;
	struct timeval timeout;
	

	for(i=0; i<FD_SIZE; i++){
		client_list[i] = -1;
	}

	
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
	
	maxfd = listen_fd;
	maxi = -1;

	fd_set readfds, allfds;

	FD_ZERO(&readfds);
	FD_SET(listen_fd, &readfds);
	timeout.tv_sec = 0;
	timeout.tv_usec = MAX_IDLE_SECS;
        

	while(1){
		 
		allfds = readfds;
		fd_num = select(maxfd + 1, &allfds, (fd_set *)0, (fd_set *)0, &timeout);
		
		// 연결 요청이 들어오면 accept
		if(FD_ISSET(listen_fd, &allfds)){
			clilen = sizeof(client_addr);
			comm_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &clilen);
			printf("Server : client connected\n");
			printf("Accept Socket = %d\n", comm_fd);
			printf("Accept IP : %s, Port : %u\n", inet_ntoa(client_addr.sin_addr),(unsigned)ntohs(client_addr.sin_port));
			// non block 설정
			if(is_nonblock(comm_fd) != 0 && set_nonblock(comm_fd)){
				perror("2 : set_nonblock fail");
				exit(1);
			} 
			
			// client_list 추가
			for(i = 0; i< FD_SIZE; i++){
				if(client_list[i] < 0){
					client_list[i] = comm_fd;
					break;
				}
			}
			

			// 최대 사이즈가 넘기면 close 시킴
			if(i == FD_SIZE){
				close(comm_fd);
				client_list[i] = -1;
			}

			// FD_SET에 현재 들어오는 comm_fd 등록
			FD_SET(comm_fd, &readfds);
	
			if(comm_fd > maxfd){
				maxfd = comm_fd;
			}
	
			if(i > maxi)
				maxi = i;
	
			if(--fd_num <= 0)
				continue;
			
		}

		// client_list에 입력된 fd중
		// 연결이 된(-1이 아닌) fd에 대해서
		// 읽을 데이터가 있는지 확인후 처리

		for(i = 0; i<= maxi; i++){
			if((sockfd = client_list[i]) < 0){
				continue;
			}
			
			int msg_byte = 0;
			n_byte = -1;
			if(FD_ISSET(sockfd, &allfds)){
				memset(str, 0x00, MAX_SIZE);
			
				n_byte = recv_nonblock(sockfd, str, P_SIZE, 0);

        	//		printf("[%d] size : %s\n", i, str);
       				msg_byte = atol(str);
		
				memset(str, 0x00, MAX_SIZE);
			
				n_byte = -1;
				n_byte = recv_nonblock(sockfd, str, msg_byte, 0);
				
				printf("[%d] recv msg : %s\n", i, str);

				if( 0 < send_nonblock(sockfd, str, strlen(str), 0)){
					printf("[%d] sending!\n", i);
				}

				if(!strcmp(str, "quit")){
					n_byte =-1;
					FD_CLR(sockfd, &readfds);
					client_list[i] = -1; 
					printf("socket close() complete\n");
					break;
				}
				
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


int recv_nonblock(int fd, void* buffer, size_t size, int flags){
        int n, i, state, err;
	int total = size;
	char temp[MAX_SIZE];

	while(1){
		memset(temp, 0x00, MAX_SIZE);
		n = recv(fd, temp, MAX_SIZE, flags);
		
		if( n > 0 ){
			total -= n;
			strcat(buffer, temp);
		}
	
		if(total <= 0){
			return size;
		}
	}
        return n;
}


int send_nonblock(int fd, void* data, size_t size, int flags){
	int n = 0;
	
	while(n < size){
		int c = send(fd, data, size, flags);
		if(c < 0){
			perror("send_nonblock error : ");
			return -1;
		}
		n += c;
	}

	return n;
}