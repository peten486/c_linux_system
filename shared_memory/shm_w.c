
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
	Shm *memory;
	char buf[255];	
	Shm *mem;
	void *shared_memory = (void *)0;

	FILE *f;
	f = fopen("./shm_item.ini", "r");
	while(fscanf(f, "%s", buf) != EOF){
		cnt++;
	}

	i = 0;
	mem_size = 0;


	Shm *items = (Shm*)malloc(sizeof(Shm) * cnt);
	f = fopen("./shm_item.ini","r");
	while(fscanf(f, "%s", buf) != EOF){
		int len = strlen(buf);
		strcpy(items[i].str, buf);
		items[i].len = len;
		i++;
	}
	
	mem_size = sizeof(Shm) * cnt;

	shmid = shmget(((key_t)0x4105), mem_size, 0666 | IPC_CREAT);
	if(shmid == -1){
		perror("shmget failed");
		return 1;
	}
	
	shared_memory = shmat(shmid, (void *)0, 0);
	if(shared_memory == (void *)-1){
		perror("shmat failed");
		return -1;
	}

	mem = (Shm *) shared_memory;
	mem = (Shm *)malloc(sizeof(Shm) * cnt);
	
	for(i=0; i<cnt; i++){
		mem[i].len = items[i].len;
		strcpy(mem[i].str, items[i].str);
	}
	free(items);

	for(i=0; i<cnt; i++){
	//	printf("len[%d] : %d\n", (i+1), mem[i].len);
		printf("str[%d] : %s\n", (i+1), mem[i].str);
	}

	
	if(-1 == shmdt(shared_memory)){
		printf("공유 메모리 분리 실패\n");
		return -1;
	}

/*
	if( -1 == shmctl(shmid, IPC_RMID, 0)){
		printf("공유 메모리 제거 실패 \n");
		return -1;
	}
*/	return 0;
}