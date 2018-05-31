/*
 * utl_queue.c
 *
 *  Created on: 2014��3��14��
 *      Author: lfx  20451250@qq.com
 *      Company:  www.jovision.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#include "utl_queue.h"


//������Ϣ
typedef struct
{
	sem_t sSem;
	int iFront,iRear;
	int iCnt;
	int iMsgSize;
	int iQueueSize;
	char **ppQueue;
	char szName[32];
} QueueInfo;

#define MAX_QUEUE_CNT	32
static QueueInfo sQueues[MAX_QUEUE_CNT];//�����
static int iQueueCnt = 0;//������

static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 *@brief ����һ����Ϣ����
 *@param name ��Ϣ���е�����
 *@param msgsize ÿ����Ϣ�ĳߴ�
 *@param queuesize ��Ϣ�������ܱ������Ϣ�ĸ���
 *@retval >=0 ��Ϣ���е�handle
 *@retval -1 ������Ϣ����ʱ���ڴ����ʧ��
 *
 */
int utl_queue_create(char *name, int msgsize, int queuesize)
{
	int i = 0;
	pthread_mutex_lock(&_mutex);
	sQueues[iQueueCnt].iMsgSize = msgsize;
	sQueues[iQueueCnt].iQueueSize = queuesize;

	sQueues[iQueueCnt].ppQueue = (char **)malloc((sizeof(char *)*(sQueues[iQueueCnt].iQueueSize)));
	if(NULL == sQueues[iQueueCnt].ppQueue)
	{
		pthread_mutex_unlock(&_mutex);
		return -1;
	}
	for (i = 0; i < sQueues[iQueueCnt].iQueueSize; i++)
	{
		sQueues[iQueueCnt].ppQueue[i] = (char *)malloc(sQueues[iQueueCnt].iMsgSize);
		memset(sQueues[iQueueCnt].ppQueue[i], 0, sQueues[iQueueCnt].iMsgSize);
		if (NULL == sQueues[iQueueCnt].ppQueue[i])
		{
			int j = 0;
			for (j = 0; j < i ; j++)
			{
				free(sQueues[iQueueCnt].ppQueue[j]);
			}
			free(sQueues[iQueueCnt].ppQueue);
			pthread_mutex_unlock(&_mutex);
			return -1;
		}
	}

	strcpy(sQueues[iQueueCnt].szName, name);
	sQueues[iQueueCnt].iFront = 0;
	sQueues[iQueueCnt].iRear = 0;
	sQueues[iQueueCnt].iCnt = 0;
	sem_init(&sQueues[iQueueCnt].sSem, 0, 0);
	pthread_mutex_unlock(&_mutex);
	return 	iQueueCnt++;
}

/**
 *@brief ����һ����Ϣ����
 *@param handle ��Ϣ���еľ������ֵΪ#jv_queue_create �ķ���ֵ
 *@return 0
 *
 */
int utl_queue_destroy(int handle)
{
	pthread_mutex_lock(&_mutex);
	sem_destroy(&sQueues[handle].sSem);

	int i = 0;
	for (i = 0; i < sQueues[handle].iQueueSize; i++)
	{
		free(sQueues[handle].ppQueue[i]);
	}
	free(sQueues[handle].ppQueue);
	iQueueCnt--;
	pthread_mutex_unlock(&_mutex);
	return 0;
}

/**
 *@brief ������Ϣ
 *@param handle ��Ϣ���еľ������ֵΪ#jv_queue_create �ķ���ֵ
 *@param msg Ҫ���͵���Ϣ��ָ��
 *@note ��Ϣ�ĳ����� #jv_queue_create ʱ��ָ��
 *@return 0 �ɹ���-1 ��������ʱ���ܷ���
 *
 */
int utl_queue_send(int handle, void *msg)
{
	//pthread_mutex_lock(&sQueues[handle].mutex);
	pthread_mutex_lock(&_mutex);
	if (sQueues[handle].iCnt == sQueues[handle].iQueueSize)
	{
		//printf("the queue %s is full , can't send \n", sQueues[handle].szName);
		pthread_mutex_unlock(&_mutex);
		return -1;
	}
	memcpy(sQueues[handle].ppQueue[sQueues[handle].iFront], (char *)msg, sQueues[handle].iMsgSize);
	sQueues[handle].iFront = (sQueues[handle].iFront + 1) % (sQueues[handle].iQueueSize);
	(sQueues[handle].iCnt)++;
	pthread_mutex_unlock(&_mutex);
	sem_post(&sQueues[handle].sSem);
	return 0;
}

/**
 *@brief ������Ϣ
 *@param handle ��Ϣ���еľ������ֵΪ#jv_queue_create �ķ���ֵ
 *@param msg ��������ڽ������ݵ�ָ��
 *@param timeout ��ʱʱ�䣬��λΪ���룬-1ʱ����ʱ
 *@note ��Ϣ�ĳ����� #jv_queue_create ʱ��ָ��
 *@return 0 �ɹ�������ű�ʾ��ʱ
 *
 */
int utl_queue_recv(int handle, void *msg, int timeout)
{
	struct timespec ts;
	ts.tv_sec=timeout/1000;   // �ص�
	ts.tv_nsec=timeout*1000000%1000000000;
	char * buf = (char *)msg;
	//pthread_mutex_lock(&sQueues[handle].mutex);
	if (timeout == -1)
		sem_wait(&sQueues[handle].sSem);
	else
	{
		int sts =sem_timedwait(&sQueues[handle].sSem, &ts);
		if (-1 == sts)
		{
			return -1;
		}
	}
	pthread_mutex_lock(&_mutex);
	memcpy(buf, sQueues[handle].ppQueue[sQueues[handle].iRear], sQueues[handle].iMsgSize);
	sQueues[handle].iRear = (sQueues[handle].iRear + 1) % (sQueues[handle].iQueueSize); //������
	(sQueues[handle].iCnt)--;
	pthread_mutex_unlock(&_mutex);

	return 0;
}

int utl_queue_get_count(int handle)
{
	return sQueues[handle].iCnt;
}
