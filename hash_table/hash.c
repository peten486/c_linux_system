// hash table 구현
// 정재화 
// 2018-11-26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STR_SIZE 255

typedef struct _node{
	char key[MAX_STR_SIZE];
	char value[MAX_STR_SIZE];
} node;

node **hash_table;
int m, s;

int getHash(char* str);
int insertHash(char* key, char* value);
char* getValue(char* key);
int deleteHash(char* key);
int isKey(char* key);

int main(){
	int i, j;
	char temp[MAX_STR_SIZE];

	printf("bucket num ? ");
	scanf("%d", &m);
	printf("slot num ? ");
	scanf("%d", &s);

	hash_table = (node **)malloc(sizeof(node *) * m);
	for(i = 0; i < m; i++){
		hash_table[i] = (node *)malloc(sizeof(node) * s);
	}

	// 입력부 
	while(1){
		char new_key[MAX_STR_SIZE];
		memset(new_key, 0x00, MAX_STR_SIZE);
		memset(temp, 0x00, MAX_STR_SIZE);
		scanf("%s", temp);
		if(!strcmp(temp, "-1")){
			break;
		}	
		
		strcpy(new_key, temp);	
		scanf(" %[^\n]", temp);
		insertHash(new_key, temp);
		rewind(stdin); 
	}

	// 검색, 삭제, 추가 입력
	while(1){
		memset(temp, 0x00, MAX_STR_SIZE);
		printf("입력 키 : ");
		scanf("%s", temp);
		if(!strcmp(temp, "-1")) break;
		rewind(stdin);
			
		if(isKey(temp) == 1){
			printf("key : %s, value : %s\n", temp, getValue(temp));
			deleteHash(temp);
		} else {
			char new_value[MAX_STR_SIZE];
			printf("새로운 value값 : ");
			scanf(" %[^\n]", new_value);
			insertHash(temp, new_value);
			rewind(stdin); 
		}
	}
	
	for(i=0; i<m; i++){
		free(hash_table[i]);
	}
	free(hash_table);
	return 0;
}
			 

int insertHash(char* key, char* value){
	int i, hash;
	hash = getHash(key);
	for(i = 0; i < s; i++){
		if(!strcmp(hash_table[hash][i].key, key)){
			printf("중복 키 입니다.\n");
			return -1;
		}

		if(strlen(hash_table[hash][i].key) == 0){
			strcpy(hash_table[hash][i].key, key);
			strcpy(hash_table[hash][i].value, value);
			return 1;			
		}
		
		if((i == s - 1) && (strlen(hash_table[hash][i].key) != 0)){
			printf("해당 Bucket의 Slot이 모두 차있습니다.\n");
		}
	}
	return 0;
}

int getHash(char* str){
	int result = 0;
	int i = 0;
	for(i = 0; i < strlen(str); i++){
		int temp = str[i];
		result = (result * 31) + temp;
	}
	return abs(result % (m));
}

char* getValue(char* key){
	int i, key_idx;
	key_idx = getHash(key);
	for(i = 0; i < s; i++){
		if(!strcmp(key, hash_table[key_idx][i].key)){
			return hash_table[key_idx][i].value;
		}
	}
	return "";
}

int deleteHash(char* key){
	int i, key_idx, j;
	key_idx = getHash(key);
	for(i = 0; i < s; i++){
		if(!strcmp(key,hash_table[key_idx][i].key)){
			strcpy(hash_table[key_idx][i].key, "");
			strcpy(hash_table[key_idx][i].value, "");
			return 1;
		}
	}
	return -1;
}

int isKey(char* key){
	int i, key_idx;
	key_idx = getHash(key);
	for(i = 0; i < s; i++){
		if(!strcmp(key, hash_table[key_idx][i].key)){
			return 1;
		}
	}
	return 0;
}
