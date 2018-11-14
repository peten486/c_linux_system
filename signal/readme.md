signal(시그널)
============

__signal__ : software interrupt로서, 프로세스에 무엇인가 발생했음을 알리는 간단한 메시지를 비동기적으로 보내는 것
> 정해진 시간이 지나면 발생하는 알람
> Child process가 종료되었다고 parent process에게 알려주는 이벤트
> User에 의한 프로그램 종료(Ctrl + C)
> User에 의한 프로그램 일시 중단(Ctrl + Z)
> Shell에서의 KILL 명령어

- 이와 같은 비동기적(asynchronous) 사건들의 발생을 프로세스에게 알려줌(인터럽트)
> 비동기적 : 시그널을 보낸 쪽에서 받는 쪽의 응답을 기다리지 않음
> 인터럽트 : 수행하고 있는 프로세스 중단(일시 중단)
- 예를 들어, 사용자가 프로세스 진행중에 Ctrl + C를 누르거나, 프로세스에 kill 명령이 내릴 때와 같은 비동기적인 사건에서 Signal이 발생
- 시그널들은 프로그램이 수행하는 도중에 발생할 수 있고, 명시적으로 발생시켜 특정 프로세스에 전달할 수 도 있다.(Signal Dispatching)
- 시그널 함수
> Void *signal(int sig, void(*func) (int))) (int);
> Sig : 신호번호, func : 신호처리 핸들러 함수
>	Signal을 처리하기 위해서 이전에 설정되어 있던 핸들러 함수를 변환하거나 SIG_INT 또는 SIG_DFL를 반환한다.
>	<sys/types.h> <signal.h>

1) Signal의 종류
---------------
| Name | Description | Default action |
|:-------------|:------------------------------------:|------------:|
| SIGALRM | 설정한 시간이 지나면 발생하는 시그널 | term |
| SIGCHLD | Child process가 종료되었을 때 parent process가 받는 시그널 | ignore |
| SIGCONT | 프로세스가 중지되었을 경우 다시 resume하는 시그널 | cont |
| SIGKILL | 이 시그널을 받은 프로세스는 무조건 종료됨(막을수 없음) | term |
| SIGPWR | 곧 파워가 꺼질 것이라는 걸 알리는 시그널 | term |
| SIGSEGV | 포인터를 잘못쓰면 나타나는 시그널(잘못된 메모리 접근) | core |
| SIGSTOP | 프로세스 중지(Ctrl+Z) | stop |
| SIGTERM | 프로세스 종료(SIGKILL과는 다르게 막을 수 있음) | term |
| SIGPIPE | 읽는 프로세스가 없는 상황에서의 PIPE에 대한 쓰기 발생시 | stop |
| SIGINT | 사용자 인터럽트를 커널에 의하여 단말기와 연결된 프로세스에 보내짐(Ctrl+C) | stop |

2) Signal에 대응하는 action
-------------------------
- 시그널을 받았을 때 3가지 행동
1. 시그널을 처리할 등록된 함수(signal handler)를 호출(catch)
2. 시그널을 무시(ignore)
3. 시그널을 무시하지 않지만, 그렇다고 함수를 호출하지도 않음(시그널의 기본행동을 취함)(default)
- 시그널의 기본행동
> 프로세스 종료
> 프로세스를 종료할 때 프로세스의 상태(레지스터, 메모리의 값 등)를 저장하는 core dump 파일을 생성한 뒤 종료.(core dump파일은 .swp형이며 디버깅시 사용할 수 있다.)
> 프로세스 일시 중지
> 시그널을 받을 당시에 프로세스가 정지되어 있다면 다시 수행하는 동작

3) Signal Handler
-----------------
- __함수원형__

```
int sigemptyset(sigset_t *set); : 시그널이 없는 비어있는 상태로 초기화
int sigfillset(sigset_t *set); : 모든 시그널이 포함된 상태로 초기화
int sigaddset(sigset_t *set); : 시그널 추가
int sigdelset(sigset_t *set); : 시그널 삭제
int sigismember(const sigset_t *set, int signo) : 시그널 찾기
```

![image](https://user-images.githubusercontent.com/28287323/48496684-e7a57c80-e875-11e8-8bce-952fe2854e11.png)


- 시그널은 비동기적으로 발생하므로 언제 발생할지 예측할 수 없다. 
(따라서 프로그램이 수행되는 도중 어느때나 발생할 수 있다)

>① : 프로그램 수행 중 시그널이 발생하면 그 시점에서 잠시 수행을 중단
>② ~ ③ : 시그널을 처리해주기 위한 Signal handler가 실행됨
>④ : 시그널이 처리되면 프로그램이 중단되었던 시점에서 다시 다음 명령어가 수행됨 (term같은 경우는 종료됨)


- Sigaction : 특정 시그널을 받을 때 프로세스가 취해야 할 행동 지정
``` 
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```
signum : 시그널 번호, act : 프로세스가 지정한 행동, oldact : 이전에 지정된 시그널 행동
반환값 : 호출 성공 시 0, 실패 시 -1

>signum으로 지정한 시그널에 대해서 act로 지정된 행동을 취함
>새로운 행동인 act가 등록되면서 기존의 행동은 oldact에 저장됨
>Signal과 같은 일을 수행하지만 스레드나 리얼타임시그널 등 좀 더 정밀한 작업을 할 때 사용됨
>Sigaction 구조체 내의 sa_mask를 통해 signal handler가 동작하는 도중엔 sa_mask에 지정된 signal이 발생하는 것을 block시킬 수 있다.
>sa_flags와 (*sa_restorer)(void)의 경우 스레드나 리얼 타임 시그널에 의미가 있는 변수


