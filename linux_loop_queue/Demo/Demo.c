#include "loop_queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//插入数据到队列
void *p_thread(void *arg)
{
	LoopQueue lQ = (LoopQueue)arg;
	int num = 0;
	while(1){
		num = rand()%100 + 1;
		int ret = LoopQueue_Push(lQ ,&num);
		if(!ret){
			printf("插入数据[%d]\n", num);
		}
		sleep(1);
	}
}

//从队列取出数据
void *c_thread(void *arg)
{
	LoopQueue lQ = (LoopQueue)arg;

	int num = 0;
	while(1){
		int ret = LoopQueue_Pop(lQ ,&num);
		if(!ret){
			printf("取出数据[%d]\n", num);
		}
		sleep(1);
	}
}

int main(int argc, char *argv[])
{
	LoopQueue lQ = LoopQueue_Init(5, sizeof(int), LOOP_QUEUE_CLOG | LOOP_QUEUE_MEM_INIT);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	int tidNum = 5;
	pthread_t p_tid[tidNum];
	pthread_t c_tid[tidNum];

	int i;
	for(i = 0; i < tidNum; ++i){
		pthread_create(p_tid + i, &attr, p_thread, lQ);
		pthread_create(c_tid + i, &attr, c_thread, lQ);
	}

	pthread_attr_destroy(&attr);

	while(1){
		sleep(30);
	}

	LoopQueue_Destroy(lQ);
	return 0;
}
