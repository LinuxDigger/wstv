/*
 * utl_mutex.h
 *
 *  Created on: 2013-11-19
 *      Author: lfx
 */

#ifndef UTL_MUTEX_H_
#define UTL_MUTEX_H_

typedef struct{
	char dummy;
}UtlMutex_t;

typedef void * MutexHandle;

/**
 *@brief ����������
 *
 *@param ��������������˴�ΪԤ�������ǵ��Ժ���ܻ�����õ������ӵĲ���
 *
 *@return handle or NULL
 */
MutexHandle utl_mutex_create(UtlMutex_t *param);

/**
 *@brief �ͷŻ�����
 */
int utl_mutex_destroy(MutexHandle handle);

/**
 *@brief ����
 *
 *@param handle #utl_mutex_create �������ľ��
 */
int utl_mutex_lock(MutexHandle handle);


/**
 *@brief �ͷ�
 *
 *@param handle #utl_mutex_create �������ľ��
 */
int utl_mutex_unlock(MutexHandle handle);


/**
 *@brief һ�����Ѵ��ڵļ򵥵���
 *@note ������������������õġ����ڷǳ��򵥵Ļ��⡣
 *   ���ڴ˺���������һ��ȫ�ֵ��������ԣ�ʹ��ʱ����ر�֤
 *   ������Χ�ڵĴ��룬�������Լ�д�ģ������ڲ����е��ø�
 *   ���Ӷ����������ķ�����ҲҪ��֤�����Ĵ����㹻��
 *
 */
int utl_mutex_simple_lock();

/**
 *@brief һ�����Ѵ��ڵļ򵥵���������
 *@note ���������#utl_mutex_simple_lock
 */
int utl_mutex_simple_unlock();

#endif /* UTL_MUTEX_H_ */
