// shm_write.c
// ¡§¿Á»≠
// 2018-11-21

#include "shm.h"

int main(int argc, char*argv[]){
	int shm_id = 0;
	void *shared_memory = (void *)0;
	int d_size = 0, mem_size = 0;
	char **value = get_file_contents(&mem_size);
//	shm_id = shm_create(mem_size, &d_size);
//	shared_memory = shm_attach(shm_id);
//	shm_detach(&shared_memory);
//	shm_remove(shm_id);
	printf("mem_size : %d\n", mem_size);
	return 0;
}
