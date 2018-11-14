daemon(데몬)
=============
__daemon__ : 리눅스 시스템이 처음 가동될 때 실행되는 백그라운드 프로세스의 일종, 사용자의 요청을 기다리고 있다가 요청이 발생하면 이에 적절히 대응하는 리스너와 같은 역할
	메모리에 상주하면서 특정 요청이 오면 즉시 대응할 수 있도록 대기중인 프로세스를 말함.
-> 윈도우의 Service 처럼 보이지 않는 곳에서 실행하고 있는 프로그램

데몬 프로그램 작성법
---------------
1. 우선 fork를 호출하여 자식프로세스를 생성시킨 다음 부모프로세스를 종료시킨다.
> 부모프로세스를 먼저 죽임으로써 자식 프로세스의 ppid는 pid 1번은 init이 담당

2. setsid()를 이용하여 새로운 세션을 만들고, 현재 프로세스(자식)의 pid가 세션의 제어권을 가지도록 한다.

3. chdir을 이용하여 프로세스가 루트디렉토리에서 작업을 수행하도록 변경시켜준다.
> (선택사항) 데몬 프로그램에서 여러가지 파일을 읽고 쓰는 작업을 할 때, 상대 경로를 명시함으로써 일어나는 혼동을 피하기 위해 권장하는 방법.


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

int work(){
	FILE *fp;
	unsigned long count = 0;

	fp = fopen("./daemon.out", "wt");
	if(fp == NULL){
		printf("file open error\n");
		return -1;
	}

	while(1){
		sleep(1); // 1초씩 카운트를 증가시켜 파일에 적기 위함.
		fprintf(fp, "%lu\n", count);
		fflush(fp);
		fprintf(stdout, "%lu\n", count++);
	}
	fclose(fp);
	return 0;
}

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

	// 새로운 세션 부여
	setsid();

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

	}
	work();
	return 0;
}


```

출처
---
http://blog.freezner.com/archives/503
http://kiaak.tistory.com/entry/%EB%A6%AC%EB%88%85%EC%8A%A4-%EB%8D%B0%EB%AA%AC-%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%A8-%EB%A7%8C%EB%93%A4%EA%B8%B0