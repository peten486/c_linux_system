// signal_02.c
// 2018-11-15
// sigaction
// 정재화

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ouch(int sig){
	printf("OUCH: I got a signal %d\n", sig);
}

int main(){
	struct sigaction sa;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = ouch;
	
	if(sigaction(SIGINT, &sa, NULL) == -1){
		perror("sigaction");
		exit(0);
	}
	
	while(1){
		printf("hello world\n");
		sleep(1); //loop slowly
	}
	return 0;
}
