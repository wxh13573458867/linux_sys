#include "loop_queue.h"

LoopQueue LoopQueue_Init(const int capacity, const int nodeSize, const int isClog)
{
	LoopQueue lQ = (LoopQueue)malloc(sizeof(struct _LoopQueue));
	if(lQ){
		lQ->capacity = capacity;
		lQ->size = 0;
		lQ->producer = 0;
		lQ->consumer = 0;

		lQ->nodeArr = (void **)malloc(sizeof(void *) * capacity);
		if(!lQ->nodeArr){
			free(lQ);
			return NULL;
		}
		lQ->nodeSize = nodeSize;

		lQ->isClog = isClog;
		pthread_mutex_init(&lQ->mutex, NULL);
		pthread_cond_init(&lQ->condProducer, NULL);
		pthread_cond_init(&lQ->condConsumer, NULL);
	}
	return lQ;
}

void LoopQueue_Destroy(LoopQueue lQ)
{
	if(!lQ){
		return;
	}

	pthread_mutex_lock(&lQ->mutex);
	while(lQ->size != 0){
		free(lQ->nodeArr[lQ->consumer]);
		lQ->consumer = ++lQ->consumer % lQ->capacity;

		lQ->size--;
	}

	pthread_mutex_destroy(&lQ->mutex);
	pthread_cond_destroy(&lQ->condProducer);
	pthread_cond_destroy(&lQ->condConsumer);

	free(lQ->nodeArr);
	free(lQ);
	return;	
}



int LoopQueue_Push(LoopQueue lQ, const void *data)
{
	if(!lQ || !data){
		return -1;
	}

	while(1){
		pthread_mutex_lock(&lQ->mutex);

		if(lQ->size == lQ->capacity){
			switch(lQ->attr & 0xF)
			{
				case LOOP_QUEUE_IS_CLOG:
					{
						pthread_cond_wait(&lQ->condProducer, &lQ->mutex);
					}break;
				case LOOP_QUEUE_NO_CLOG:
					{
						pthread_mutex_unlock(&lQ->mutex);
						return -1;			
					}break;
				default:
					{
						pthread_mutex_unlock(&lQ->mutex);
						return -1;
					}break;
			}
		}

		if(lQ->size == lQ->capacity){
			pthread_mutex_unlock(&lQ->mutex);
			continue;	
		}

		lQ->nodeArr[lQ->producer] = (void *)malloc(lQ->nodeSize);
		if(!lQ->nodeArr[lQ->producer]){
			pthread_mutex_unlock(&(lQ->mutex));
			return -1;
		}
		memcpy(lQ->nodeArr[lQ->producer], data, lQ->nodeSize);

		lQ->producer = ++lQ->producer % lQ->capacity;

		lQ->size++;
		pthread_cond_signal(&lQ->condConsumer);
		pthread_mutex_unlock(&lQ->mutex);
		break;
	}

	return 0;
}

int LoopQueue_Pop(LoopQueue lQ, void *data)
{
	if(!lQ || !data){
		return -1;
	}

	while(1){
		pthread_mutex_lock(&lQ->mutex);

		if(lQ->size == 0){
			switch(lQ->attr & 0xF)
			{
				case LOOP_QUEUE_IS_CLOG:
					{
						pthread_cond_wait(&lQ->condConsumer, &lQ->mutex);
					}break;
				case LOOP_QUEUE_NO_CLOG:
					{
						pthread_mutex_unlock(&lQ->mutex);
						return -1;
					}break;
				default:
					{
						pthread_mutex_unlock(&lQ->mutex);
						return -1;
					}break;
			}
		}

		if(lQ->size == 0)
		{
			pthread_mutex_unlock(&lQ->mutex);
			continue;	
		}

		memcpy(data, lQ->nodeArr[lQ->consumer], lQ->nodeSize);
		free(lQ->nodeArr[lQ->consumer]);

		lQ->consumer = ++lQ->consumer % lQ->capacity;

		lQ->size--;
		pthread_cond_signal(&lQ->condProducer);
		pthread_mutex_unlock(&lQ->mutex);
		break;
	}

	return 0;	
}

