#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct item{
        int len;
        char **buf;
};

int main(){
        int shmid;
        int i, pid, ret, cnt = 0;

        void *shared_memory = (void *)0;
        char buf[255];
        char **value;

        int n = 0;
        int l = 0;

        struct item *shared_stuff;
        // 파일 오픈
        FILE* file = fopen("./shm_item.ini","r");
        while(fscanf(file, "%s", buf) != EOF){
                cnt++;
        }

        value = (char**)malloc(sizeof(char*) * cnt);
        file = fopen("./shm_item.ini","r");
        while(fscanf(file, "%s", buf) != EOF){
                l = strlen(buf);
                if(l > 0){
                        char* newstrptr = (char*)malloc(sizeof(char) * (l+1));
                        strcpy(newstrptr, buf);
                        value[n] = newstrptr;
                        n++;
                } else {
                        break;
                }
        }

        // 공유메모리 공간을 만든다. 
        shmid = shmget((key_t)0x4105, sizeof(struct item), 0666|IPC_CREAT);

        if (shmid == -1){
                perror("shmget failed : ");
                exit(0);
        }

        // 공유메모리를 사용하기 위해 프로세스메모리에 붙인다.  
        shared_memory = shmat(shmid, (void *)0, 0);

        if (shared_memory == (void *) - 1){
                perror("shmat failed : ");
                exit(0);
        }

        shared_stuff = (struct item *)shared_memory;
        shared_stuff->buf = value;
        shared_stuff->len = cnt;

        pid = fork();
        if (pid == 0){
                shmid = shmget((key_t)0x4105, sizeof(struct item), IPC_CREAT|0666);
                if (shmid == -1){
                        perror("shmget failed : ");
                        exit(0);
                }

                shared_memory = shmat(shmid, (void *)0, 0); // 생성한 공유메모리를 shared_memory변수에 붙임
                if (shared_memory == (void *)-1){
                        perror("shmat failed : ");
                        exit(0);
                }

                printf("before print\n");
                for(i = 0; i < cnt; i++){
                        printf("[%d] %s\n", (i+1), value[i]);
                }

        } else if(pid > 0) {
                // 부모 프로세스로 공유메모리의 내용을 보여준다.
                wait(&ret);
                printf("after print\n" );
                int len = shared_stuff->len;
                for(i = 0; i< len; i++){
                    printf("[%d] %s\n", (i+1), shared_stuff->buf[i]);
                }
                if(shmctl(shmid, IPC_RMID, 0) == -1){
                        perror("공유 메모리 제거 실패");
                }
        }

        if( shmdt(shared_memory) == -1){
                perror("공유 메모리 분리 실패\n");
        }
}
                   