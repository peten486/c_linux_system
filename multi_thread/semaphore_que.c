// 세마포어 예제 프로그램(큐)
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct _node{
	int data;
	struct _node* next;
} Queue;

int dequeue();
void enqueue(int data);
void* de(void* arg);
void* en(void* arg);

static sem_t sem_de;
static sem_t sem_en;
static int num;
static Queue *front, *rear;

int main(){
	
	pthread_t t1, t2;

	front = NULL;
	rear = NULL;

	sem_init(&sem_de, 0, 1);
	sem_init(&sem_en, 0, 0);

	pthread_create(&t1, NULL, en, NULL);
//	sleep(5);
	pthread_create(&t2, NULL, de, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	sem_destroy(&sem_de);
	sem_destroy(&sem_en); 

	return 0;
}

void* en(void *arg){
	int i;
	
	for(i=0; i<5; i++){
		sem_wait(&sem_de);
		enqueue(i);
		sem_post(&sem_en);
	}
	return NULL;
}

void* de(void *arg){
	int i;
	
	for(i=0; i<5; i++){
		sem_wait(&sem_en);
		printf("%d\n",dequeue());
		sem_post(&sem_de);
	}
	return NULL;
}

int dequeue(){
	struct _node *curNode = front;
	int result = front->data;
	if(front == NULL){
		printf("Queue is Empty\n");
		return -1;
	}

	if(front == rear){
		front = rear = NULL;
	} else {
		front = front->next;
	}
	free(curNode);
	return result;
}

void enqueue(int data){
	struct _node *newNode = (struct _node*)malloc(sizeof(struct _node));
	newNode->data = data;
	newNode->next = NULL;
	
	if(front == NULL && rear == NULL){
		front = newNode;
		rear = newNode;
	} else {
		rear->next = newNode;
		rear = newNode;
	}
}

