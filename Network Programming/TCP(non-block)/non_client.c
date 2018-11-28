// client.c
// 정재화
// 2018-11-26
// non block 모드의 클라이언트 구현

// 각 함수 파라미터들에 대해서 예외처리가 필요함
// 또한 모든 함수의 리턴값을 가지고 main문에서 한번더 에러 확인 작업이 필요함 

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
#define MAX_IDLE_SECS 500000000


int set_nonblock(int sockfd);
int is_nonblock(int sockfd);
int send_nonblock(int fd, void* data, size_t size, int flags);
int recv_nonblock(int fd, void* buffer, size_t size, int flags);
int connect_nonblock();
int conn();

int main(int argc,char **argv){
	int sockfd, n, errno;
	char sendline[MAX_SIZE];
	char recvline[MAX_SIZE];
	char len[MAX_SIZE];

//	sockfd = connect_nonblock();
	sockfd = conn();
	if(sockfd < 0 || is_nonblock(sockfd))
		return 0;

	while(1){
		bzero( sendline, MAX_SIZE);
		bzero( recvline, MAX_SIZE);
		bzero( len, MAX_SIZE);

		printf("send msg : "); 
		fgets(sendline,MAX_SIZE,stdin);
		sendline[strlen(sendline)-1] = '\0';
		sprintf(len, "%d", (int)strlen(sendline));

		send_nonblock(sockfd, len, 4, 0);
		send_nonblock(sockfd, sendline, strlen(sendline), 0);		
		recv_nonblock(sockfd, recvline, strlen(sendline), 0);
		if(!strcmp(recvline, "quit")){
			close(sockfd);
			break;
		}

		printf("echo msg : %s\n",recvline);
	}
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
	fd_set readfds;
        struct timeval timeout;
        int n, i, state, err;
        int total = size;
        char temp[MAX_SIZE];

        while(1){
                FD_ZERO(&readfds);
                FD_SET(fd, &readfds);
                timeout.tv_sec = 0;
                timeout.tv_usec = MAX_IDLE_SECS;
                state = select(fd+1, &readfds, NULL, NULL, &timeout);
                if(state == -1){
                        perror("recv_nonblock() select error : ");
                        exit(0);
                        break;
                } else {
                        err = errno;
                        switch(err){
                                case EAGAIN:
                                default:
                                        if(FD_ISSET(fd, &readfds)){
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
                                        }
                                        break;
                                }
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


int connect_nonblock(){
	int sockfd, flags, n, error, i;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;
	struct sockaddr_in servaddr;
	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("socket fail");
		exit(1);
	}

	bzero(&servaddr, sizeof servaddr);

	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	inet_pton(AF_INET,"10.10.3.180",&(servaddr.sin_addr));

	// nonblock set
	if(set_nonblock(sockfd) == -1){
		perror("set_nonblock fail");
		exit(1);
	}

	error = 0;
	if ( (n = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
		if (errno != EINPROGRESS)
			return -1;

	if (n == 0)
		goto done;

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset; 
	
	tval.tv_sec = 0; 
	tval.tv_usec = MAX_IDLE_SECS;

	if ( (n = select(sockfd+1, &rset, &wset, NULL, &tval)) == 0) { 
		close(sockfd);
		errno = ETIMEDOUT; // 연결시간 초과 
		return -1; 
	} 

	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) { 
		len = sizeof(error); 	
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) 
			return(-1);		
	} else { 
		perror("select error: sockfd not set"); 
		exit(-1); 
	} 
done:
	if (error) {
		close(sockfd);
		errno = error;
		return -1;
	}

	return sockfd;
}

int conn(){
	int sockfd, flags, n, error, i;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;
	struct sockaddr_in servaddr;
	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("socket fail");
		exit(1);
	}

	bzero(&servaddr, sizeof servaddr);

	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	inet_pton(AF_INET,"10.10.3.180",&(servaddr.sin_addr));

	// nonblock set
	if(set_nonblock(sockfd) == -1){
		perror("set_nonblock fail");
		printf("non block set fail\n");
		return -1;
	}

	n = 1;
	while(n != 0){
		n = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

		printf("dddd\n");



		if( n == 0 ){
			return sockfd;
		} else if(n < 0){
			printf("status : %i\n", n);
			if( errno == EINPROGRESS){
				fd_set w_fd, all_fd;
	
				FD_ZERO(&w_fd);
				FD_SET(sockfd, &w_fd);
				all_fd = w_fd; 
		
				tval.tv_sec = 0; 
				tval.tv_usec = MAX_IDLE_SECS;
		
				n = select(sockfd+1, NULL, &all_fd, NULL, &tval);
				printf("retVal : %i\n", n);

				if( n == 1 ){
					int so_error;
					socklen_t slen = sizeof(so_error);
					getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &slen);
					
					if(so_error == 0){
						return sockfd;
					}

				}
			}
		}
		printf("err\n");
	}

	close(sockfd);
	return -1;
}
