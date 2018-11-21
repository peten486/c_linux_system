#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>

struct item {
	int len;
	char ** buf;
};

int shm_create(int mem_size, int* d_size){
	int shm_id = 0;
	*d_size = sizeof(struct item) + mem_size;
	shm_id = shmget((key_t)0x4105, sizeof(struct item) + mem_size, IPC_CREAT | 0666);
	
	if(shm_id == -1){
		printf("shmget failed\n");
		exit(1);
	}
	return shm_id;
}

int shm_remove(int shm_id){
	int result = 0;
	result = shmctl(shm_id, IPC_RMID, 0);
	if(result == -1){
		printf("shm detach failed\n");
	}
	return result;
}

void* shm_attach(int shm_id){
	return shmat(shm_id, (void *)0,0);
}

int shm_detach(void* memory){
	int result = 0;
	if((result = shmdt(memory)) == -1){
		printf("shmdt failed\n");
	}
	return result;
}

char** get_file_contents(int *size){
	int cnt = 0, len = 0;
	int n =0;
	char **value;
	char buf[255];
	*size = 0;

	FILE *file = fopen("./shm_item.ini","r");
	while(fscanf(file, "%s", buf) != EOF){
		cnt++;
	}
	
	value = (char**)malloc(sizeof(char*) * cnt);	
	int d_cnt = 0;
	
	file = fopen("./shm_item.ini","r");	
	while(fscanf(file, "%s", buf) != EOF){
		len = strlen(buf);
		*size += len;
		d_cnt += len;
		if(len > 0){
			char* newStr = (char *)malloc(sizeof(char) * (len + 1));	
			strcpy(newStr, buf);
			value[n] = newStr;
			n++;
		} else {
			break;
		}
	}
printf("%d\n", d_cnt);
	return value;
} 	