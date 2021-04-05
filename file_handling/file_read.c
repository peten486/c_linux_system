// file_read.c
// jae hwa jeong
// 2018-11-14

#include <stdio.h>
#include <string.h>

void read_sscanf(FILE* file);
void read_strtok(FILE* file);

int main(){
	FILE* file = fopen("./test1.ini", "r");
	read_sscanf(file);
 	
	file = fopen("./test1.ini", "r");
	read_strtok(file);			
	return 0;
}

void read_sscanf(FILE* file){
	char line[255];
	
	printf("------ sscanf -------\n");
	while(fgets(line, sizeof(line),file) != NULL){
		int id;
		char str[40];
		char temp[20];
		char num1[30], num2[30];
		sscanf(line, "%d %s %s %s %s", &id, str, num1, num2, temp);
		printf("%d %s %s %s %s\n", id, str, num1, num2, temp); 
	}
}

void read_strtok(FILE* file){
	char *context;
	char *pos;
	char line[255];
	
	printf("------ strtok ------\n");
	while(fgets(line, 255, file) != NULL){
		pos = strtok_r(line, " ", &context);
		while(pos){
			printf("%s", pos);
			pos = strtok_r(NULL, " ", &context);
			if(pos != NULL){
				printf(" ");
			}
		}


	}
}	
