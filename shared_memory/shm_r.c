#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 255

typedef struct _shm{
	int len;
	char str[MAX_STR];
} Shm;

int main(int argc, char **argv){
	int shmid, i;
	int mem_size, cnt;
	FILE *file;
	char buf[MAX_STR];
	Shm *mem;
	void *shared_memory = (void *)0;

	file = fopen("./shm_item.ini", "r");
	while(fscanf(file, "%s", buf) != EOF){
		cnt++;
	}

	mem_size = sizeof(Shm) * cnt;
	shmid = shmget((key_t)0x4105, mem_size, IPC_CREAT | IPC_EXCL | 0666);
	if(shmid == -1){
	
		shmid = shmget((key_t)0x4105, mem_size, 0);
		if(shmid == -1){
			perror("shmget failed");
			return 1;
		}
	}

	shared_memory = shmat(shmid, (void *)0, 0);
	if(shared_memory == (void*)-1){
		perror("shmat failed");
		return -1;
	}
	
	mem = (Shm *) shared_memory;
	mem = malloc(sizeof(Shm) * cnt);

	for(i=0; i<cnt; i++){
		printf("str[%d] : %s\n", (i+1), mem[i].str);
	}
	
	if(-1 == shmdt(shared_memory)){
		printf("공유메모리 분리 실패\n");
		return -1;
	}

	if(-1 == shmctl(shmid, IPC_RMID, 0)){
		printf("공유 메모리 제거 실패\n");
		return -1;
	}

	return 0;
}	
