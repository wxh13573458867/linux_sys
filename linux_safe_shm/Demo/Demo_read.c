#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "safe_shm.h"

void *pShm = NULL;

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
	void *shmbuf = NULL;
	SEM_Init("./ipc_sys_key.sem", 0x00, &semid);
	SHM_Init("./ipc_sys_key.shm", 0x00, &shmbufSize, &shmbuf);
	printf("[%d][%d]\n", semid, shmbufSize);
	pShm = shmbuf;
	
	char tempbuf[1024] = { 0 };
	while(1)
	{
		memset(tempbuf, 0x00, 1024);
	
		int ret = SEM_locked(semid);	
		if(ret){
			printf("[%d]\n", ret);
			usleep(1000000);
			continue;
		}

		SHM_Read(shmbuf, 128,  tempbuf, 1024);
		printf("[读取] [%s]\n", tempbuf);
		usleep(1000000);
		SEM_unlock(semid);	

	}	

	return 0;
}
