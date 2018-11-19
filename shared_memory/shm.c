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
        // ���� ����
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

        // �����޸� ������ �����. 
        shmid = shmget((key_t)0x4105, sizeof(struct item), 0666|IPC_CREAT);

        if (shmid == -1){
                perror("shmget failed : ");
                exit(0);
        }

        // �����޸𸮸� ����ϱ� ���� ���μ����޸𸮿� ���δ�.  
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

                shared_memory = shmat(shmid, (void *)0, 0); // ������ �����޸𸮸� shared_memory������ ����
                if (shared_memory == (void *)-1){
                        perror("shmat failed : ");
                        exit(0);
                }

                printf("before print\n");
                for(i = 0; i < cnt; i++){
                        printf("[%d] %s\n", (i+1), value[i]);
                }

        } else if(pid > 0) {
                // �θ� ���μ����� �����޸��� ������ �����ش�.
                wait(&ret);
                printf("after print\n" );
                int len = shared_stuff->len;
                for(i = 0; i< len; i++){
                    printf("[%d] %s\n", (i+1), shared_stuff->buf[i]);
                }
                if(shmctl(shmid, IPC_RMID, 0) == -1){
                        perror("���� �޸� ���� ����");
                }
        }

        if( shmdt(shared_memory) == -1){
                perror("���� �޸� �и� ����\n");
        }
}
                   