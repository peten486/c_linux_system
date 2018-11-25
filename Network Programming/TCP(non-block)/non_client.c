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
#define MAX_IDLE_SECS 500000


int set_nonblock(int sockfd);
int is_nonblock(int sockfd);
int send_nonblock(int fd, void* data, size_t size, int flags);
int recv_nonblock(int fd, void* buffer, size_t size, int flags);
int connect_nonb(int sockfd, struct sockaddr_in saptr, size_t salen);

int main(int argc,char **argv){
	int sockfd, n, errno, cc;
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

	// cc = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	cc = connect_nonb(sockfd, servaddr, sizeof(servaddr));
	if(cc < 0)
		return 0;

	if(is_nonblock(sockfd) != 0){
		perror("set nonblock fail");
		exit(1);
	} else {
		//		printf("nonblock ok\n");
	}

	// 1.  connect error - PROGRESS , 
	// 2. select() , timeout ( 500msec, 2초간 )
	// 3. while() {/woking .../}  <-- 연결끊어짐.. -> Reconnect
	while(1){
		bzero( sendline, MAX_SIZE);
		bzero( recvline, MAX_SIZE);
		bzero( len, MAX_SIZE);

		printf("send msg : "); 
		fgets(sendline,MAX_SIZE,stdin);
		sendline[strlen(sendline)-1] = '\0';
		sprintf(len, "%d", (int)strlen(sendline));

		send_nonblock(sockfd, len, 4, 0);
		//		printf("msg len : %s\n", len);
		send_nonblock(sockfd, sendline, strlen(sendline), 0);		
		if(!strcmp(sendline, "quit")){
			close(sockfd);
			break;
		}
		recv_nonblock(sockfd, recvline, MAX_SIZE, 0);
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
				//			case EWOULDBLOCK:
				case EAGAIN:
				default:
					if(FD_ISSET(fd, &readfds)){
						while((n = recv(fd, buffer, size, flags)) > 0){
							return n;
						}
					}
					break;
			}
		}
	}
	return n;
}

int send_nonblock(int fd, void* data, size_t size, int flags){
/*    fd_set sendfds;
	struct timeval timeout;
	int n, i, err, state;

	while(1){
		FD_ZERO(&sendfds);
		FD_SET(fd, &sendfds);
		timeout.tv_sec = 0;
		timeout.tv_usec = MAX_IDLE_SECS;
		state = select(fd+1, NULL, &sendfds, NULL, &timeout);
		if(state == -1){
			perror("send_nonblock() select error : ");
			exit(0);
			break;
		} else {
			err = errno;
			switch(err){
				//			case EWOULDBLOCK:
				case EAGAIN:
				default:
					if(FD_ISSET(fd, &sendfds)){
						while((n = send(fd, data, size, flags)) > 0){
							return n;
						}
					}
					break;
			}
		}
	}
*/
	int n = send(fd, data, size, flags);
	if(n < 0){
		perror("send_nonblock error : ");
		return -1;

	}
	return n;
}



int connect_nonb(int sockfd, struct sockaddr_in saptr, size_t salen){
	int flags, n, error;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;

	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	error = 0;
	if ( (n = connect(sockfd, (struct sockaddr *)&saptr, salen)) < 0)
		if (errno != EINPROGRESS)
			return(-1);

	/* Do whatever we want while the connect is taking place. */

	if (n == 0)
		goto done;	/* connect completed immediately */

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = 0;
	tval.tv_usec = MAX_IDLE_SECS;

	if ( (n = select(sockfd+1, &rset, &wset, NULL, &tval)) == 0) {
		close(sockfd);		/* timeout */
		errno = ETIMEDOUT;
		return(-1);
	}

	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			return(-1);			/* Solaris pending error */
	} else {
		perror("select error: sockfd not set");
		exit(-1);
	}

done:
	fcntl(sockfd, F_SETFL, flags);	/* restore file status flags */

	if (error) {
		close(sockfd);		/* just in case */
		errno = error;
		return(-1);
	}
	return(0);
}
