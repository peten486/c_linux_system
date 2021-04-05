file_handling(파일 핸들링)
=============
C에서의 포맷이 정해진 파일을 핸들링.


간단한 INI파일
----------------------
아래와 같이 포맷이 정해진 test1.ini 파일이 있다. 
```
1001 ABCDE 00303 00310 ON
2002 ABCDF 10303 10310 ON
3003 ABCDG 20303 20310 OFF
4004 AACDE 30303 30310 ON
5005 DIEKF 40303 40310 OFF
6006 DOZEF 50303 50310 ON
7007 KBJDZ 60303 60310 ON
8008 DELCO 70303 70310 ON
9009 EOCLA 80303 80310 OFF
10010 AAAAA 90303 90310 OFF

```

해당 파일을 읽는 방법에는 여러가지가 있을 수 있지만, 여기서는 fgets으로 한 줄씩 읽어서, sscanf로 포맷으로 읽거나, strtok_r으로 공백을 기준으로 끊어서 읽는 방식을 구현하였다.


소스코드
------
```

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


```

출처
---
>https://man7.org/linux/man-pages/man3/fopen.3.html