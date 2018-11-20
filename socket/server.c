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

int set_nonblock(int sockfd);
int is_nonblock(int sockfd);
int get_recv(int type, int size, int sockfd, char* buf);

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
         printf("non block ... ok\n");
      } else if(errno == EAGAIN ){
         continue;   
      } else {
         perror("accept fail");
   //      exit(1);   
      }
      
      while(1){
         //printf("들어감");
         int msg_byte = 0;
         bzero(str, MAX_SIZE);
         //msg_byte = get_recv(0, P_SIZE, comm_fd, str);
         
         n_byte = recv(comm_fd, str, 4, 0);
         if(n_byte == -1 && errno == EWOULDBLOCK){
            continue;
         } else {

            printf("size : %s\n", str);
            msg_byte = atol(str);  

            n_byte = recv(comm_fd, str, msg_byte, 0);
            printf("nbyte : %d\n", n_byte);
            if(n_byte == -1 && errno == EWOULDBLOCK){
               continue;
            }   
         }
         //msg_byte = get_recv(0, P_SIZE, comm_fd, str);
      //   msg_byte = atol(str);
         //if(msg_byte == -1){
         //   continue;
      //   }
         //printf("msg size : %d\n", msg_byte);
         //bzero(str, msg_byte);
      
         //n_byte = get_recv(1, msg_byte, comm_fd, str);   
         //if(n_byte == -1){
         //   continue;
         //}

         if(!strcmp(str, "quit")){
            close(comm_fd);
            continue;
         }
   
         printf("recv : %s\n", str);   
         printf("Echoing back - %s",str);                 
      
         if(0 != send(comm_fd, str, strlen(str)+1, 0)){
            //break;
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

int get_recv(int type, int size, int sockfd, char* buf){
   int recv_size = 0;
   recv_size = recv(sockfd, buf, size, 0);
   
   if(recv_size == 0){
      close(sockfd);
      return -1; // continue
   } else if(recv_size == -1 || errno == EWOULDBLOCK){
      return -1; // continue
   }
   
   if(type == 0){
      recv_size = atol(buf);
   }

   return recv_size;
}