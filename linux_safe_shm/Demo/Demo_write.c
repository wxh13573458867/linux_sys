#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "safe_shm.h"

void *shmbuf = NULL;
void *pShm = NULL;

void handle(void *arg)
{
	SHM_Write(shmbuf, 128,  arg, 1024);
	sleep(1);
	printf("[写入] [%s]\n", (char *)arg);
}

void sighandle(int signo)
{
	if(pShm != NULL){
		SHM_Secede(pShm);
	}

	SHM_Destroy("./ipc_sys_key.shm", 0x00);	
	SEM_Destroy("./ipc_sys_key.sem", 0x00);	
	
	exit(0);
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sighandle);
	int semid = -1;
	int shmbufSize = 40000;
	SEM_Init("./ipc_sys_key.sem", 0x00, &semid);
	SHM_Init("./ipc_sys_key.shm", 0x00, &shmbufSize, &shmbuf);
	printf("[%d][%d]\n", semid, shmbufSize);
	pShm = shmbuf;
	
	char tempbuf[1024] = { 0 };
	int index = 0;
	while(1)
	{
		memset(tempbuf, 0x00, 1024);
		sprintf(tempbuf,"数字--%d", index);
	
		int ret = SEM_Visit(semid, handle, tempbuf);
		if(ret)
		{
			printf("[%d]\n", ret);
			sleep(1);
		}

		index++;	
	}	

	return 0;
}
