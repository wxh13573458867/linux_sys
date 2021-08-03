#include "loop_queue.h"

LoopQueue LoopQueue_Init(const int capacity, const int nodeSize, const int flags)
{
	if(capacity <= 0 || nodeSize <= 0 || !(flags & (LOOP_QUEUE_NO_CLOG | LOOP_QUEUE_CLOG))){
		return NULL;
	}

	LoopQueue lQ = (LoopQueue)malloc(sizeof(struct _LoopQueue));
	memset(lQ, 0x00, sizeof(struct _LoopQueue));
	if(lQ){
		lQ->capacity = capacity;
		lQ->size = 0;
		lQ->producer = 0;
		lQ->consumer = 0;
		lQ->flags = flags;

		pthread_mutex_init(&lQ->mutex, NULL);
		pthread_cond_init(&lQ->condProducer, NULL);
		pthread_cond_init(&lQ->condConsumer, NULL);

		lQ->nodeSize = nodeSize;
		lQ->nodeArr = (void **)malloc(sizeof(void *) * capacity);
		if(!lQ->nodeArr){
			LoopQueue_Destroy(lQ);
			return NULL;
		}else{
			memset(lQ->nodeArr, 0x00, sizeof(void *) * capacity);
			if(lQ->flags & LOOP_QUEUE_MEM_TEMP){
				for(int i = 0; i < lQ->capacity; ++i){
					lQ->nodeArr[i] = NULL;
				}
			}else if(lQ->flags & LOOP_QUEUE_MEM_INIT){
				for(int i = 0; i < lQ->capacity; ++i){
					lQ->nodeArr[i] = (void*)malloc(lQ->nodeSize);
					if (!lQ->nodeArr[i]) {
						LoopQueue_Destroy(lQ);
						return NULL;
					}
				}
			}
		}
	}
	return lQ;
}

void LoopQueue_Destroy(LoopQueue lQ)
{
	if(!lQ){
		return;
	}

	pthread_mutex_lock(&lQ->mutex);
	for(int i = 0; i < lQ->capacity; ++i){
		if (!(lQ->nodeArr[i])) {
			free(lQ->nodeArr[i]);
			lQ->nodeArr[i] = NULL;	
		}
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
			if(lQ->flags & LOOP_QUEUE_CLOG){
				pthread_cond_wait(&lQ->condProducer, &lQ->mutex);
				if(lQ->size == lQ->capacity){
					pthread_mutex_unlock(&lQ->mutex);
					continue;	
				}
			}else if(lQ->flags & LOOP_QUEUE_NO_CLOG){
				pthread_mutex_unlock(&lQ->mutex);
				return -1;
			}
		}

		if (!(lQ->nodeArr[lQ->producer])) {
			lQ->nodeArr[lQ->producer] = (void*)malloc(lQ->nodeSize);
			if (!lQ->nodeArr[lQ->producer]) {
				pthread_mutex_unlock(&(lQ->mutex));
				return -1;
			}
		}

		memcpy(lQ->nodeArr[lQ->producer], data, lQ->nodeSize);

		lQ->producer = (++lQ->producer) % lQ->capacity;

		lQ->size++;
		if(lQ->flags & LOOP_QUEUE_CLOG){
			pthread_cond_signal(&lQ->condConsumer);
		}	
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
			if(lQ->flags & LOOP_QUEUE_CLOG){
				pthread_cond_wait(&lQ->condConsumer, &lQ->mutex);
				if(lQ->size == 0){
					pthread_mutex_unlock(&lQ->mutex);
					continue;	
				}
			}else if(lQ->flags & LOOP_QUEUE_NO_CLOG){
				pthread_mutex_unlock(&lQ->mutex);
				return -1;
			}
		}


		memcpy(data, lQ->nodeArr[lQ->consumer], lQ->nodeSize);
		memset(lQ->nodeArr[lQ->consumer], 0x00, lQ->nodeSize);

		lQ->consumer = (++lQ->consumer) % lQ->capacity;

		lQ->size--;
		if(lQ->flags & LOOP_QUEUE_CLOG){
			pthread_cond_signal(&lQ->condProducer);
		}
		pthread_mutex_unlock(&lQ->mutex);
		break;
	}

	return 0;	
}

