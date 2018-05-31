#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "utl_thread.h"


//���������߳�,����ͬpthread_create
//ע��:������pthread_attr_t����
int pthread_create_detached(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg)
{
	int ret;
	size_t stacksize = LINUX_THREAD_STACK_SIZE;
	pthread_attr_t attr;
	pthread_t tmpid;
	pthread_t* p_tid = NULL;

	p_tid = (pid == NULL) ? &tmpid : pid;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);	//�����߳�
	pthread_attr_setstacksize(&attr, stacksize);					//ջ��С
	
	ret = pthread_create(p_tid, &attr, (void*)pFunction, (void *)(arg));
	if (ret != 0)
	{
		printf("Error!!!!! pthread_create failed with %d, %s(%d)\n", ret, strerror(errno), errno);
	}
	
	pthread_attr_destroy (&attr);
	return ret;
}

//�����߳�,ջ��С��ΪLINUX_THREAD_STACK_SIZE������ͬpthread_create
//ע��:������pthread_attr_t����
int pthread_create_normal(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg)
{
	int ret;
	size_t stacksize = LINUX_THREAD_STACK_SIZE;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, stacksize);					//ջ��С
	
	ret = pthread_create(pid, &attr, (void*)pFunction, (void *)(arg));
	
	pthread_attr_destroy (&attr);
	return ret;
}

//�����߳�,ջ��С��ΪLINUX_THREAD_STACK_SIZE������ͬpthread_create
//���ȼ�����Ϊ51
//ע��:������pthread_attr_t����
int pthread_create_normal_priority(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg)
{
	int ret;
	size_t stacksize = LINUX_THREAD_STACK_SIZE;
	pthread_attr_t attr;
	struct sched_param param;
	
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, stacksize);					//ջ��С

	param.sched_priority = 51;	//���ȼ�
	pthread_attr_setschedpolicy(&attr,SCHED_RR);
	pthread_attr_setschedparam(&attr,&param);
	pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);//Ҫʹ���ȼ������ñ���Ҫ����仰

	ret = pthread_create(pid, &attr, (void*)pFunction, (void *)(arg));
	
	pthread_attr_destroy (&attr);
	return ret;
}


