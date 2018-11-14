// fork를 이용하여 daemon 만들기
// 2018-11-14
// 정재화

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h> // fork
#include <sys/wait.h> // wait
#include <string.h>

int main(int argc, char *argv[]){
	int pid;
	int ret;

	// fork 생성
	pid = fork();
	printf("pid = [%d]\n", pid);

	// fork 에러 발생시 로그 남긴 후 종료
	if(pid < 0){
		printf("fork error : return is [%d] \n", pid);
		perror("fork error : ");
		exit(0);
	} else if(pid  > 0){
		// 부모 프로세스
		// 로그 남긴 후 종료
		printf("child process : [%d] - parent process : [%d]\n", pid, getpid());
		exit(0);	
	} else if(pid == 0){
		// 자식 프로세스
		// 정상 작동시 로그
		printf("process : [%d]\n", getpid());
	}

	// 터미널 종료 시 signal의 영향을 받지 않도록 처리
	signal(SIGHUP, SIG_IGN); // nohup 명령어와 유사
	close(0);
	close(1);
	close(2);

	// 실행위치를 홈 디렉토리로 변경
	chdir("~");

	// 새로운 세션 부여
	setsid();

	int cnt = 0;

	// 반복 실행이 필요한 코드면 while문 안에 삽입
	while(1){
		// daemon이 죽었을 경우 다시 실행
		if((pid = fork()) < 0){
			printf("fork error : restart daemon");
		} else if(pid == 0){
			break;
		} else if(pid > 0){
			wait(&ret);
		}

		// 10초 마다 체크
		sleep(10);
	}
}
