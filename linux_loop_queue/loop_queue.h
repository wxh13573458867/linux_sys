#ifndef LOOP_QUEUE_H 
#define LOOP_QUEUE_H

#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define LOOP_QUEUE_CLOG (1)	//队列阻塞
#define LOOP_QUEUE_NO_CLOG (0)	//队列非阻塞

typedef struct _LoopQueue
{
	int capacity;
	int size;
	int producer;
	int consumer;

	void **nodeArr;
	int nodeSize;
	
	int isClog;
	pthread_mutex_t mutex;
	pthread_cond_t condProducer;
	pthread_cond_t condConsumer;	
} *LoopQueue;


extern LoopQueue LoopQueue_Init(const int capacity, const int nodeSize, const int isClog);

extern void LoopQueue_Destroy(LoopQueue lQ);

extern int LoopQueue_Push(LoopQueue lQ, const void *data);

extern int LoopQueue_Pop(LoopQueue lQ, void *data);

#endif
