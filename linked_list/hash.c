#include <stdio.h>
#include <stdlib.h>

typedef struct _list {
	struct _node *cur;
	struct _node *head;
	struct _node *tail;
} linkedList;

typedef struct _node {
	int data;
	struct _node *next;
} node;

void createNode(linkedList *L, int tdata){
	node *newNode = (node *)malloc(sizeof(node));
	newNode->data = tdata;
	newNode->next = NULL;
	if(L->head == NULL && L->tail == NULL){
		L->head = L->tail = newNode;
	} else {
		L->tail->next = newNode;
		L->tail = newNode;
	}

	L->cur = newNode;
}

void deleteLastNode(linkedList *L){
	node *p = L->head;
	while(p->next->next != NULL) p = p->next;
	p->next = p->next->next;
	L->tail = p;
}

void printNodes(linkedList *L){
	node *cur = L->head;
	while(cur != NULL){
		printf("%d", cur->data);
		if(cur->next != NULL) printf(", ");
		cur = cur->next;
	}
	printf("\n");
}

int main(void){
	linkedList *L = (linkedList *)malloc(sizeof(linkedList)); 
	L->cur = NULL; 
	L->head = NULL; 
	L->tail = NULL;

	createNode(L, 1);
	createNode(L, 2);
	createNode(L, 3);
	deleteLastNode(L);
	
	createNode(L, 4);
	createNode(L, 5);
	createNode(L, 6);
	deleteLastNode(L);
	deleteLastNode(L);
	createNode(L, 7);
	printNodes(L);
	
	return 0;
}