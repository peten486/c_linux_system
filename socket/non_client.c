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
#define MAX_IDLE_SECS 5


int set_nonblock(int sockfd);
int is_nonblock(int sockfd);
int send_nonblock(int fd, void* data, size_t size, int flags);
int recv_nonblock(int fd, void* buffer, size_t size, int flags);
int socket_connect(int fd, struct sockaddr* servaddr);
 
int main(int argc,char **argv){
	int sockfd,n;
	char sendline[MAX_SIZE];
	char recvline[MAX_SIZE];
	char len[MAX_SIZE];

	struct sockaddr_in servaddr;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("socket fail");
		exit(1);
	}

	bzero(&servaddr, sizeof servaddr);
	
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

	if(set_nonblock(sockfd) == -1){
		perror("set_nonblock fail");
		exit(1);
	}
	

//	int cc = socket_connect(sockfd,(struct sockaddr *)&servaddr);
	int cc = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(is_nonblock(sockfd) != 0){
		perror("set nonblock fail");
		exit(1);
	} else {
		printf("nonblock ok\n");
	}

	while(1){
		bzero( sendline, MAX_SIZE);
		bzero( recvline, MAX_SIZE);
		bzero( len, MAX_SIZE);

		printf("msg : "); 
		fgets(sendline,MAX_SIZE,stdin); /*stdin = 0 , for standard input */
		sendline[strlen(sendline)-1] = '\0';
		sprintf(len, "%d", (int)strlen(sendline));
		
		send_nonblock(sockfd, len, 4, 0);
//		write(sockfd,len,4);
		printf("msg len : %s\n", len);
//		write(sockfd,sendline,strlen(sendline));
		send_nonblock(sockfd, sendline, strlen(sendline), 0);		
		if(!strcmp(sendline, "quit")){
			close(sockfd);
			break;
		}
		recv_nonblock(sockfd, recvline, MAX_SIZE, 0);
//		read(sockfd,recvline,MAX_SIZE);
		printf("echo : %s\n",recvline);
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


int send_nonblock(int fd, void* data, size_t size, int flags){
        fd_set wset;
        struct timeval timeout;
        int n, i, err;

        n = send(fd, data, size, flags);
        err = errno;
        if(n < 0){
                switch(err){
                        case EINTR: return 0;
                        case EAGAIN:
                                FD_ZERO(&wset);
                                FD_SET(fd, &wset);
                                timeout.tv_sec = 0;
                                timeout.tv_usec = MAX_IDLE_SECS;
                                i = select(fd, NULL, &wset, NULL, &timeout);
                                if(i < 0){
                                        perror("ERROR in send_nonblock");
                                        return 0;
                                } else {
                                        if(i == 0) return 0;
                                }
                                break;
                        default:
                                perror("ERROR in send_nonblock");
                                return -1;
                }
        }
        return n;
}


int recv_nonblock(int fd, void* buffer, size_t size, int flags){
        fd_set rset;
        struct timeval timeout;
        int n, i;

        while(1){
	        n = recv(fd, buffer, size, flags);
	        int err = errno;
	        
	        if(n > 0){ break; }
	        if(n < 0){
	                switch(err){
	                        case EINTR: 
								printf("1 : %d, err : %d\n", n, err);
	                        	continue;
	                        case EAGAIN:
	                            printf("2 : %d, err : %d\n", n, err);
	                                FD_ZERO(&rset);
	                                FD_SET(fd, &rset);
	                                timeout.tv_sec = 0;
	                                timeout.tv_usec = MAX_IDLE_SECS;
	                                i = select(fd, &rset, NULL, NULL,&timeout);
	                                if(i < 0){
	                                        perror("ERROR in recv_nonblock");
	                                        continue;
	                                        //return 0;
	                                } else {
	                                        if(i == 0){
	                                                continue;
	                                                //return 0;
	                                        }
	                                }
	                                break;
	                        default:
		                        printf("3 :%d, err : %d\n", n, err);
	                                perror("ERROR in recv_nonblock");
	                                continue;
	                               break;
	                }

	        }
	    }
        return n;
}




int socket_connect(int fd, struct sockaddr* servaddr){
	int conn = 0;
	fd_set rset;
	struct timeval timeout;
	int n, i;

	while(1){
		conn = connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		int err = errno;
		if(conn == -1){
		//	perror("connect err");
			switch(err){
				case EINPROGRESS:
				case EAGAIN:
					FD_ZERO(&rset);
					FD_SET(fd, &rset);
					timeout.tv_sec = 0;
					timeout.tv_usec = MAX_IDLE_SECS;
					i = select(fd, &rset, NULL, NULL, &timeout);
					if(i < 0){
						perror(" 1: error is connect non block");
				//		return -1;
						continue;
					} else {
						if(i == 0){
							//return 0;
							continue;
						}
					}
					break;	
				default:
					perror("2 : error is connect non block");
					return -1;
			}
			continue;
		} else if( conn > 0){
			break;
		}
	}
	return conn;
}