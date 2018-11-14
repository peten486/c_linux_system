// signal_01.c
// 정재화
// 2018-11-15

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void ouch(int sig){
	printf("OUCH: I got a signal %d\n", sig);
}

int main(){
	signal(SIGINT, ouch); // SIGINT = Ctrl + C, Default action은 종료	
	while(1){
		printf("Hello world\n");
		sleep(1); // loop slowly
	}

	return 0;
}
