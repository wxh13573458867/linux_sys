#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "linux_sys.h"

void sighandle(int signo)
{
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
	printf("%d\n", shmbufSize);
	
	char tempbuf[1024] = { 0 };
	while(1)
	{
		memset(tempbuf, 0x00, 1024);
	
		int ret = SEM_locked(semid);	
		SHM_Read(shmbuf, 128,  tempbuf, 1024);
		usleep(1000000);
		printf("[%d][%s]\n",ret, tempbuf);
		SEM_unlock(semid);	

	}	

	return 0;
}
