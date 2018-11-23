#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct _list {
	struct _node *cur;
	struct _node *head;
	struct _node *tail;
} linkedList;

typedef struct _node {
	int id;
	time_t cur_time;
	int expire_time;
	struct _node *next;
} node;

linkedList *L;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void insertNode(linkedList *L, int id, int expire_time);
int getListNumber(linkedList *L);
void deleteLastNode(linkedList *L);
int deleteNode(linkedList *L, node *delNode);
int deleteNodeId(linkedList *L, int id);
void printNodes(linkedList *L);
int expire(linkedList *L);
void printTime();
void* do_expire(void *data);

int main(void){
	L = (linkedList *)malloc(sizeof(linkedList)); 
	L->cur = NULL; 
	L->head = NULL; 
	L->tail = NULL;

	pthread_t pthread;
	int status, thr_id;
	int a = 0;
	printf("input id, expire time...(ex) 1 20\n");	
	
	while(1){
		int new_id = 0;
		int new_expire_time = 0;
		scanf("%d", &new_id);
		if(new_id == -1){
			break;
		}
		scanf("%d", &new_expire_time);

		insertNode(L, new_id, new_expire_time); 
	//	printNodes(L);
	}	
	printNodes(L);

	thr_id = pthread_create(&pthread, NULL, do_expire, (void *)&a);
	
	int menu = 0;
	int new_id, new_expire_time;
	int del_id;
	while(1){
		printf("menu ( 1 : insert, 2 : delete) : ");
		scanf("%d", &menu);
		switch(menu){
			case 1:
				printf("insert id, expire time ... (ex) 1 20\n");
				scanf("%d %d", &new_id, &new_expire_time);
				insertNode(L, new_id, new_expire_time);
				break;
			case 2:
				printf("delete id : ");
				scanf("%d",&del_id);
				deleteNodeId(L, del_id);
				break;
		}
		if(getListNumber(L) <= 0){
			break;
		}
		printNodes(L);
	}	
			
	pthread_join(pthread, (void*)&status);	
	status = pthread_mutex_destroy(&mutex);
	printf("end\n");
	return 0;
}

void insertNode(linkedList *L, int id, int expire_time){

	node *newNode = (node *)malloc(sizeof(node));
	newNode->next = NULL;
	newNode->id = id;
	newNode->expire_time = expire_time;
	time(&newNode->cur_time);

	pthread_mutex_lock(&mutex);
	if(L->head == NULL && L->tail == NULL){
		L->head = L->tail = newNode;
	} else {
		node *curNode = L->head;
		node *preNode = L->head;
		while(curNode != NULL){
			if(curNode == L->head && curNode->expire_time >= newNode->expire_time){
				newNode->next = curNode;
				L->head = newNode;
				break;
			} else if(curNode->next == NULL && newNode->expire_time > curNode->expire_time){
                                curNode->next = newNode;
                                L->tail = newNode;
                                break;
                        } else if(curNode != L->head && curNode->expire_time >= newNode->expire_time){
				newNode->next = curNode;
				preNode->next = newNode;
				break;
			}
			preNode = curNode;
			curNode = curNode->next;
		}
	}
	pthread_mutex_unlock(&mutex);
}


int getListNumber(linkedList *L){
	pthread_mutex_lock(&mutex);
	node *cur = L->head;
	if(L->head == NULL) {
		pthread_mutex_unlock(&mutex);
		return 0;
	}
	int cnt = 1;
	while(cur->next != NULL){
		cur = cur->next;
		cnt++;
	}
	pthread_mutex_unlock(&mutex);
	return cnt;
}


void deleteLastNode(linkedList *L){
	node *p = L->head;
	node *delNode;
	while(p->next->next != NULL) p = p->next;
	delNode = p->next;
	p->next = p->next->next;
	L->tail = p;
	free(delNode);
}

int deleteNode(linkedList *L, node *delNode){
	int chk = -1;

	pthread_mutex_lock(&mutex);
	if(delNode == L->head){
		L->head = delNode->next;
		free(delNode);
		pthread_mutex_unlock(&mutex);
		return 1;
	} else if(delNode->next == NULL){
		deleteLastNode(L);
		pthread_mutex_unlock(&mutex);
		return 1;	
	} else {
		node *cur = L->head;
		while(cur->next != delNode) cur = cur->next;
		cur->next = cur->next->next;
		free(delNode);
		pthread_mutex_unlock(&mutex);
		return 1;
	}
	pthread_mutex_unlock(&mutex);
	return chk;
}

int deleteNodeId(linkedList *L, int id){

	int chk = -1;

	pthread_mutex_lock(&mutex);
	node *cur = L->head;
	while(cur->id != id)cur = cur->next;
	pthread_mutex_unlock(&mutex);

	chk = deleteNode(L, cur);	
	return chk;
}


void printNodes(linkedList *L){
	//pthread_mutex_lock(&mutex);
	if(getListNumber(L) == 0){
		return;
	}
	pthread_mutex_lock(&mutex);
	node *cur = L->head;
	time_t now;
	time(&now);
	printf("현재 시간 : ");
	printf(ctime(&now));
	while(cur != NULL){
		printf("id : %d, expire time : %d\n", cur->id, cur->expire_time);
		cur = cur->next;
	}
	pthread_mutex_unlock(&mutex);
}

int expire(linkedList *L){
	int chk = 0;
	node *cur = L->head;
	time_t now;
	time(&now);
	
	pthread_mutex_lock(&mutex);
	while(cur != NULL){
		if( (now - cur->cur_time) > cur->expire_time){
			node* delNode = cur;
			cur = cur->next;
			pthread_mutex_unlock(&mutex);
			chk = deleteNode(L,delNode);
			pthread_mutex_lock(&mutex);
			continue;
			printfNodes(L);
		}
		cur = cur->next;
	}
	pthread_mutex_unlock(&mutex);
	return chk;
}	

void printTime(){
	time_t now;
	time(&now);
	printf(ctime(&now));
}

void* do_expire(void *data){
	while(1){
		sleep(1);
		if(getListNumber(L) <= 0) {
			printf("리스트에 데이터가 없습니다.\n");
			break;
		}
		if(expire(L) == 1){
			printNodes(L);
		}	
	}
}