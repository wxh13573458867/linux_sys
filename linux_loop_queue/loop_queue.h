#ifndef LOOP_QUEUE_H 
#define LOOP_QUEUE_H

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define LOOP_QUEUE_CLOG		(0x01)	//队列阻塞
#define LOOP_QUEUE_NO_CLOG	(0x02)	//队列非阻塞
#define LOOP_QUEUE_MEM_INIT	(0x04)	//内存初始时构建
#define LOOP_QUEUE_MEM_TEMP	(0x08)	//内存使用时构建

typedef struct _LoopQueue
{
	int capacity;
	int size;
	int producer;
	int consumer;

	void **nodeArr;
	int nodeSize;
	
	int flags;
	pthread_mutex_t mutex;
	pthread_cond_t condProducer;
	pthread_cond_t condConsumer;	
} *LoopQueue;

/*
 *@brief	初始化循环队列
 *@param:capacity	循环队列容量
 *@param:nodeSize	单个节点大小
 *@param:flags	队列属性
 *@reruen	成功:循环队列指针	失败:NULL
 **/
extern LoopQueue LoopQueue_Init(const int capacity, const int nodeSize, const int flags);

/*
 *@brief	销毁循环队列
 *@param:lQ	循环队列指针
 * return	无
 **/
extern void LoopQueue_Destroy(LoopQueue lQ);

/*
 *@brief	向循环队列插入节点
 *@param:lQ	循环队列指针
 *@param:data	节点数据
 *return	成功:0	失败:-1
 **/
extern int LoopQueue_Push(LoopQueue lQ, const void *data);

/*
 *@brief	从循环队列取出节点
 *@param:lQ	循环队列指针
 *@param:data	节点数据
 *return	成功:0	失败:-1
 **/
extern int LoopQueue_Pop(LoopQueue lQ, void *data);

#endif
