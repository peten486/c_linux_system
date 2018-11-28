
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_STR 100
#define MAX_STR_NUM 30

typedef struct _shm{
	int len;
	char str[MAX_STR_NUM][MAX_STR];
} Shm;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int termination;
Shm *mem;

void* do_thread_1(void* key);
void* do_thread_2(void* key);

int getValueToFile(Shm *items);
int isOverlap(Shm *items, char* str);
int addValueToFile(char* str);

int main(int argc, char **argv){
	pthread_t pthread[2];
	int status, thr_id[2];
	int shmid;

	thr_id[0] = pthread_create(&pthread[0], NULL, do_thread_1, NULL);
	thr_id[1] = pthread_create(&pthread[1], NULL, do_thread_2, NULL);
	pthread_join(pthread[0],(void*)&status);
	pthread_join(pthread[1],(void*)&status);
	status = pthread_mutex_destroy(&mutex);

	pthread_mutex_lock(&mutex);
	shmid = shmget(((key_t)0x4105), sizeof(Shm), 0666 | IPC_CREAT);
	if(shmid == -1){
		perror("shmget failed");
		pthread_mutex_unlock(&mutex); 
		return;
	}
	
	if(shmctl(shmid, IPC_RMID, 0) == -1){
		perror("shmctl failed");
	}
	pthread_mutex_unlock(&mutex);

	printf("종료\n");
	return 0;
}

int getValueToFile(Shm *items){
	FILE *file;
	char buf[MAX_STR];
	int cnt = 0;

	file = fopen("./shm_item.ini", "r");
	while(fscanf(file, "%s", buf) != EOF){
		strcpy(items->str[cnt], buf); 
		cnt++;
	}

	fclose(file);
	items->len = cnt;
	return cnt;
}

int isOverlap(Shm* items, char *str){
	int i;
	
	for(i = 0; i < items->len; i++){
		if(!strcmp(str, items->str[i])){
			return 1;
		}
	}
	return 0;
}

int addValueToFile(char *str){
	int i;
	FILE *file;

	file = fopen("./shm_item.ini", "a");
	if(file == NULL) return 0;

	fprintf(file, "%s\n", str);
	fclose(file);

	return 1;	
}

void* do_thread_1(void* key){
	int shmid, i=0;
	int cnt;
	Shm *items;
	char newData[MAX_STR];

	// 현재 데이터를 저장할 items
	items = (Shm*)malloc(sizeof(Shm));
	cnt = getValueToFile(items);
	
	// 공유메모리 생성
	pthread_mutex_lock(&mutex);
	shmid = shmget(((key_t)0x4105), sizeof(Shm), 0666 | IPC_CREAT);
	if(shmid == -1){
		perror("shmget failed");
		pthread_mutex_unlock(&mutex); 
		return;
	}
	
	mem = shmat(shmid, (void *)0, 0);
	if(mem == (void *)-1){
		perror("shmat failed");
		pthread_mutex_unlock(&mutex);
		return;
	}

	// 공유메모리 데이터 추가
	memset((char *)mem, 0, (sizeof(Shm)));
	mem->len = items->len;
	for(i=0; i<items->len; i++){
		strcpy(mem->str[i], items->str[i]);
	}
	// 공유메모리 분리
	if(shmdt(mem) == -1){
	//	printf("공유 메모리 분리 실패\n");
		pthread_mutex_unlock(&mutex);
		return;
	}
	sleep(2);
	pthread_mutex_unlock(&mutex);
	while(1){
		sleep(1);
		// 새로운 데이터 입력
		memset(newData, 0x00, MAX_STR);
		printf("thread 1 : ");
		scanf("%s", &newData);
	
		if(!strcmp(newData, "-1")){
			// 종료
			pthread_mutex_lock(&mutex);
			termination = 1;
			pthread_mutex_unlock(&mutex);
			break;
		}

		// 중복 데이터 확인
		if(isOverlap(items, newData)){
			printf("중복된 데이터 입니다\n");
			continue;	
		}
	
	
		pthread_mutex_lock(&mutex);	
	
		// 공유메모리 가져오기	
		mem = shmat(shmid, (void*)0,0);
		if(mem == (void *)-1){
			perror("shmat failed");
			pthread_mutex_unlock(&mutex);
			return;
		}
		
		// 입력한 데이터 공유메모리 데이터 추가
		strcpy(mem->str[mem->len], newData);
		mem->len = (mem->len + 1);
		
		if(-1 == shmdt(mem)){
			printf("공유 메모리 분리 실패\n");
			pthread_mutex_unlock(&mutex);
			return;
		}
		pthread_mutex_unlock(&mutex);
	
		// 파일 끝에 입력한 데이터 추가
		addValueToFile(newData);
		strcpy(items->str[items->len], newData);
		items->len = (items->len + 1);
					
	}
}	



void* do_thread_2(void* key){
	int shmid, i, j, chk = 0;
	Shm *items;

	// 현재 데이터를 저장할 items
	items = (Shm*)malloc(sizeof(Shm));

	// 공유메모리 가져오기
	pthread_mutex_lock(&mutex);	
	shmid = shmget(((key_t)0x4105), (sizeof(Shm)), IPC_CREAT | 0666);
	if(shmid == -1){
		perror("shmget failed");
		pthread_mutex_unlock(&mutex);
		return;
	}

	mem = shmat(shmid, (void*)0,0);
	if(mem == (void *)-1){
		perror("shmat :wfailed");
		pthread_mutex_unlock(&mutex);
		return;
	}
	
	// 공유메모리 내용 출력
	for(i = 0; i < mem->len; i++){
		printf("[%d] : %s\n", (i+1), mem->str[i]);
		strcpy(items->str[i], mem->str[i]);
	}
	items->len = mem->len;

	// 공유메모리 분리
	if(shmdt(mem) == -1){
		printf("공유 메모리 분리 실패\n");
		pthread_mutex_unlock(&mutex);
		return;
	}
	
	pthread_mutex_unlock(&mutex);
	
	
	while(1){
		chk = 0;
		pthread_mutex_lock(&mutex);
		if(termination == 1){
			pthread_mutex_unlock(&mutex);
			break;
		}

		// 공유메모리 가져오기	
		mem = shmat(shmid, (void*)0,0);
		if(mem == (void *)-1){
			perror("shmat :wfailed");
			pthread_mutex_unlock(&mutex);
			return;
		}
	
		// 공유메모리 변경 확인
		for(i = 0; i < mem->len; i++){
			if(!isOverlap(items, mem->str[i])){
				printf("thread 2 : %s\n", mem->str[i]);	
				chk = 1;
			}
		}
		if(chk == 1){
			printf("thread 2(cnt) : %d\n", mem->len);
		}
	
		// 공유메모리 백업
		for(i = 0; i < mem->len; i++){
			strcpy(items->str[i], mem->str[i]);
		}
		items->len = mem->len;
	
		// 공유메모리 분리
		if(shmdt(mem) == -1){
			printf("공유 메모리 분리 실패\n");
			pthread_mutex_unlock(&mutex);
			return;
		}

		pthread_mutex_unlock(&mutex);
		sleep(1);
	}

}
