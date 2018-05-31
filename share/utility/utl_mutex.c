/*
 * utl_mutex.c
 *
 *  Created on: 2013-11-19
 *      Author: lfx
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>

#include "utl_mutex.h"

static pthread_mutex_t simpleMutex;
static int bSimpleInited = 0;

/**
 *@brief ����������
 *
 *@param ��������������˴�ΪԤ�������ǵ��Ժ���ܻ�����õ������ӵĲ���
 *
 *@return handle or NULL
 */
MutexHandle utl_mutex_create(UtlMutex_t *param)
{
	pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
	if (!mutex)
	{
		return NULL;
	}
	pthread_mutex_init(mutex, NULL);

	return mutex;
}

/**
 *@brief �ͷŻ�����
 */
int utl_mutex_destroy(MutexHandle handle)
{
	pthread_mutex_destroy((pthread_mutex_t *)handle);
	free(handle);
	return 0;
}

/**
 *@brief ����
 *
 *@param handle #utl_mutex_create �������ľ��
 */
int utl_mutex_lock(MutexHandle handle)
{
	return pthread_mutex_lock((pthread_mutex_t *)handle);
}


/**
 *@brief �ͷ�
 *
 *@param handle #utl_mutex_create �������ľ��
 */
int utl_mutex_unlock(MutexHandle handle)
{
	return pthread_mutex_unlock((pthread_mutex_t *)handle);
}


/**
 *@brief һ�����Ѵ��ڵļ򵥵���
 *@note ������������������õġ����ڷǳ��򵥵Ļ��⡣
 *   ���ڴ˺���������һ��ȫ�ֵ��������ԣ�ʹ��ʱ����ر�֤
 *   ������Χ�ڵĴ��룬�������Լ�д�ģ������ڲ����е��ø�
 *   ���Ӷ����������ķ�����ҲҪ��֤�����Ĵ����㹻��
 *
 */
int utl_mutex_simple_lock()
{
	if (!bSimpleInited)
	{
		pthread_mutex_init(&simpleMutex, NULL);
		bSimpleInited = 1;
	}

	pthread_mutex_lock(&simpleMutex);

	return 0;
}

/**
 *@brief һ�����Ѵ��ڵļ򵥵���������
 *@note ���������#utl_mutex_simple_lock
 */
int utl_mutex_simple_unlock()
{
	if (!bSimpleInited)
	{
		pthread_mutex_init(&simpleMutex, NULL);
		bSimpleInited = 1;
	}
	pthread_mutex_unlock(&simpleMutex);

	return 0;

}
