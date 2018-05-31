/*
 * utl_queue.h
 *
 *  Created on: 2014��3��14��
 *      Author: lfx  20451250@qq.com
 *      Company:  www.jovision.com
 */
#ifndef UTL_QUEUE_H_
#define UTL_QUEUE_H_



/**
 *@brief ����һ����Ϣ����
 *@param name ��Ϣ���е�����
 *@param msgsize ÿ����Ϣ�ĳߴ�
 *@param queuesize ��Ϣ�������ܱ������Ϣ�ĸ���
 *@retval >=0 ��Ϣ���е�handle
 *@retval -1 ������Ϣ����ʱ���ڴ����ʧ��
 *
 */
int utl_queue_create(char *name, int msgsize, int queuesize);

/**
 *@brief ����һ����Ϣ����
 *@param handle ��Ϣ���еľ������ֵΪ#jv_queue_create �ķ���ֵ
 *@return 0
 *
 */
int utl_queue_destroy(int handle);

/**
 *@brief ������Ϣ
 *@param handle ��Ϣ���еľ������ֵΪ#jv_queue_create �ķ���ֵ
 *@param msg Ҫ���͵���Ϣ��ָ��
 *@note ��Ϣ�ĳ����� #jv_queue_create ʱ��ָ��
 *@return 0 �ɹ���-1 ��������ʱ���ܷ���
 *
 */
int utl_queue_send(int handle, void *msg);

/**
 *@brief ������Ϣ
 *@param handle ��Ϣ���еľ������ֵΪ#jv_queue_create �ķ���ֵ
 *@param msg ��������ڽ������ݵ�ָ��
 *@param timeout ��ʱʱ�䣬��λΪ���룬-1ʱ����ʱ
 *@note ��Ϣ�ĳ����� #jv_queue_create ʱ��ָ��
 *@return 0 �ɹ�������ű�ʾ��ʱ
 *
 */
int utl_queue_recv(int handle, void *msg, int timeout);
//��ѯ���ж��ٸ�����û����,����ʹ��
int utl_queue_get_count(int handle);

#endif /* UTL_QUEUE_H_ */
