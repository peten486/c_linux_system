daemon(데몬)
=============
__daemon__ : 리눅스 시스템이 처음 가동될 때 실행되는 백그라운드 프로세스의 일종, 사용자의 요청을 기다리고 있다가 요청이 발생하면 이에 적절히 대응하는 리스너와 같은 역할
	메모리에 상주하면서 특정 요청이 오면 즉시 대응할 수 있도록 대기중인 프로세스를 말함.
-> 윈도우의 Service 처럼 보이지 않는 곳에서 실행하고 있는 프로그램

데몬 프로그램 작성법
---------------
1. 소스코드 작성
2. 작성된 소스코드 컴파일
3. 데몬 파일 테스트
4. 컴파일된 데몬 파일 실행

standalone type daemon
----------------------
> 독립적으로 수행되며 서비스 요청에 응답하기 위해 항시 메모리에 상주하는 데몬이다.
> 데몬의 실행 스크립트 위치 : "/etc/inetd.d/" 


inetd type daemon
-----------------
> inetd라고 하는 특별한 데몬에 의해 간접적으로 실행되는 데몬이다. 보안상의 이유로 xinetd로 수정되었으며 슈퍼데몬이라고도 한다. inetd에 어떠한 요청이 들어오면 inetd는 해당되는 데몬을 메모리에 올리고 요청을 처리하게 된다.
> 데몬의 실행 스크립트 파일 : "/etc/xinetd.d/" 


실행결과
------
![image](https://user-images.githubusercontent.com/28287323/48488238-a1dfb880-e863-11e8-817c-4eec5e1cb723.png)

소스코드
------
```

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


```

출처
---
Link: [블로그][http://blog.freezner.com/archives/503]