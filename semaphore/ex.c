// 세마포어 예제 프로그램
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void* read(void* arg);
void* accu(void* arg);

static sem_t sem_read;
static sem_t sem_accu;
static int num;

int main(){
	pthread_t t1, t2;
	sem_init(&sem_read, 0, 0);
	sem_init(&sem_accu, 0, 1);

	pthread_create(&t1, NULL, read, NULL);
	pthread_create(&t2, NULL, accu, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	sem_destroy(&sem_read);
	sem_destroy(&sem_accu);
	return 0;
}

void* read(void *arg){
	int i;
	
	for(i=0; i<5; i++){
		printf("input num : ");
		sem_wait(&sem_accu);
		scanf("%d", &num);
		sem_post(&sem_read);
	}
	return NULL;
}

void* accu(void *arg){
	int sum=0, i;
	
	for(i=0; i<5; i++){
		sem_wait(&sem_read);
		sum += num;
		sem_post(&sem_accu);
	}
	printf("sum : %d\n", sum);
	return NULL;
}
