#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/*	用于检测内存安全的全局变量	*/
static int shm_buf_size = -1; 
static void *shm_buf_head = NULL;

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};

static int KEY_Obtain(const char *file, int proj, int *isCreate, size_t *keyid)
{
	char check[1024] = {0};
	int fd = -1;
	*isCreate = -1;

	if(access(file, F_OK)){
		*isCreate = 1;
		if((fd = open(file, O_RDWR | O_CREAT, 0666)) == -1){	return -1;}
		write(fd, file, strlen(file));
		close(fd);
	}else{
		*isCreate = 0;
		if((fd = open(file, O_RDWR)) == -1){	return -1;}
		read(fd, check, strlen(file));
		close(fd);
		if(strcmp(check, file)){	return -1;}
	}

	if((*keyid = ftok(file, proj)) == -1){	return -1;}

	return 0;
}

int SEM_Init(const char *file, int proj, int *semid)
{
	size_t keyid = -1;
	int isCreate = -1;
	if(KEY_Obtain(file, proj, &isCreate, &keyid)){	return -1;}

	if(isCreate){
		if((*semid = semget(keyid, 1, IPC_CREAT | IPC_EXCL | 0666)) == -1){	return -1;}

		union semun un_sem;
		un_sem.val = 1;
		if(semctl(*semid, 0, SETVAL, un_sem) == -1){	return -1;}
	}else{
		if((*semid = semget(keyid, 0, IPC_CREAT | 0666)) == -1){	return -1;}
	}

	return 0;
}

void SEM_Destroy(const char *file, int proj)
{
	size_t keyid = -1;
	int semid = -1;
	int isCreate = -1;
	if(KEY_Obtain(file, proj, &isCreate, &keyid)){	return;}

	if(!isCreate){
		if((semid = semget(keyid, 0, IPC_CREAT | 0666)) != -1){
			semctl(semid, 0, IPC_RMID);
		}
	}
	remove(file);
	return;	
}

int SEM_locked(int semid)
{
	struct sembuf str_sem;
	str_sem.sem_num = 0;
	str_sem.sem_op = -1;
	str_sem.sem_flg = SEM_UNDO;

	if(semop(semid, &str_sem, 1) == -1){	return -1;}

	return 0;
}

int SEM_unlock(int semid)
{
	struct sembuf str_sem;
	str_sem.sem_num = 0;
	str_sem.sem_op = 1;
	str_sem.sem_flg = 0;

	if(semop(semid, &str_sem, 1) == -1){	return -1;}

	return 0;
}

int SEM_Visit(int semid, void(* handle)(void *), void *arg)
{
	if(SEM_locked(semid)){	return -1;}
	handle(arg);
	if(SEM_unlock(semid)){	return -1;}

	return 0;
}

int SHM_Init(const char *file, int proj, int *shmsize, void **shmbuf)
{
	if(shmsize < 0 || shmbuf == NULL){	return -1;}
	size_t keyid = -1;
	int isCreate = -1;
	int shmid = -1;
	if(KEY_Obtain(file, proj, &isCreate, &keyid)){	return -1;}

	if(isCreate){
		int temp = 0;
		if((temp = *shmsize % 4096) != 0){
			*shmsize += 4096 - temp;
		}
		size_t size = *shmsize;
		int fd = open(file, O_WRONLY | O_APPEND);
		write(fd, &size, sizeof(size_t));
		close(fd);

		if((shmid = shmget(keyid, *shmsize, IPC_CREAT | IPC_EXCL | 0666)) == -1){	return -1;}

	}else{
		size_t size = 0;
		int fd = open(file, O_RDONLY);
		lseek(fd, strlen(file),SEEK_SET);
		read(fd, &size, sizeof(size_t));
		*shmsize = size;
		close(fd);		
		if((shmid = shmget(keyid, 0, 0)) == -1){	return -1;}
	}

	if((*shmbuf = shmat(shmid, NULL, 0)) == (void *)-1){	return -1;}
	shm_buf_head = *shmbuf;
	shm_buf_size = *shmsize;
	if(isCreate){
		memset(*shmbuf, 0x00, *shmsize);
	}

	return 0;	
}

void SHM_Secede(void **shmbuf)
{
	shmdt(*shmbuf);
	*shmbuf = NULL;
	return;
}

void SHM_Destroy(const char *file, int proj)
{
	size_t keyid = -1;
	int shmid = -1;
	int isCreate = -1;
	if(KEY_Obtain(file, proj, &isCreate, &keyid)){	return;}

	if(!isCreate){
		if((shmid = shmget(keyid, 0, 0)) != -1){
			shmctl(shmid, IPC_RMID, NULL);
		}
	}

	remove(file);
	return;	
}

int SHM_Read(void *shmbuf, int index, void *outbuf, int outbufSize)
{
	if(index + outbufSize > shm_buf_size || shmbuf != shm_buf_head){	return -1;}

	char *tempbuf = (char *)shmbuf + index;
	memcpy(outbuf, tempbuf, outbufSize);

	return 0;
}

int SHM_Write(void *shmbuf, int index, void *inbuf, int inbufSize)
{
	if(index + inbufSize > shm_buf_size || shmbuf != shm_buf_head){	return -1;}

	char *tempbuf = (char *)shmbuf + index;
	memcpy(tempbuf, inbuf, inbufSize);

	return 0;
}

