#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main()
{
 
    char str[100];
    int listen_fd, comm_fd;
	socklen_t clilen;
    struct sockaddr_in servaddr, client_addr;
 
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    bzero( &servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 	servaddr.sin_port = htons(22000);

    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
 
    listen(listen_fd, 10);
 	clilen = sizeof(client_addr);
    if((comm_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &clilen))!= -1){
    	printf("Server : client connected\n");
    	printf("Accept Socket = %d\n", comm_fd);
    	printf("Accept IP : %s, Port : %u\n", inet_ntoa(client_addr.sin_addr),(unsigned)ntohs(client_addr.sin_port));
    }
 
    while(1)
    {
 
        bzero( str, 100);
 
        read(comm_fd,str,100);
 		if(!strcmp(str,"q")){
        	close(comm_fd);
        	//break;
        }
        printf("Echoing back - %s",str);
 
        write(comm_fd, str, strlen(str)+1);
 
    }

    //close(comm_fd);
}