#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "linux_sys.h"

void *shmbuf = NULL;

void handle(void *arg)
{
	int ret = SHM_Write(shmbuf, 128,  arg, 1024);
	printf("[%s]\n", (char *)arg);
	sleep(1);
}

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
	SEM_Init("./ipc_sys_key.sem", 0x00, &semid);
	SHM_Init("./ipc_sys_key.shm", 0x00, &shmbufSize, &shmbuf);
	printf("%d\n", shmbufSize);
	
	char tempbuf[1024] = { 0 };
	int index = 0;
	while(1)
	{
		memset(tempbuf, 0x00, 1024);
		sprintf(tempbuf,"数字--%d", index);
	
		SEM_Visit(semid, handle, tempbuf);

		index++;	
	}	

	return 0;
}
