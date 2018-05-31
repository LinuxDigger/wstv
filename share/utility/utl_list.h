/*
 * utl_list.h
 *
 *  Created on: 2013-11-19
 *      Author: lfx
 */

#ifndef UTL_LIST_H_
#define UTL_LIST_H_

typedef void * ListHandle_t;

typedef struct{
	int bNeedMutex; //�Ƿ���Ҫ���̲߳���
}UtlListParam_t;

typedef struct _UtlNode_t{
	void *param;
	struct _UtlNode_t *next;
}UtlNode_t;

/**
 *@brief ����һ��LIST
 */
ListHandle_t utl_list_create(UtlListParam_t *param);

/**
 *@brief ����һ���ڵ�
 *
 *@param handle
 *
 *@return
 */
int utl_list_add(ListHandle_t handle, void *param);

/**
 *@brief ɾ��һ�����
 *
 *@param handle
 *
 *@return
 */
int utl_list_del(ListHandle_t handle, void *param);

/**
 *@brief ��ȡ������ͬʱ����#utl_list_seek_set ��Ч��
 *
 *@param handle
 *
 *@return
 */
int utl_list_get_cnt(ListHandle_t handle);

/**
 *@brief ��LISTָ��ָ��ͷ
 *
 *@param handle
 *
 *@return
 */
void utl_list_seek_set(ListHandle_t handle);

/**
 *@brief ��ȡ��һ���ڵ�
 *
 *@param handle
 *
 *@return
 */
void *utl_list_get_next(ListHandle_t handle);

/**
 *@brief ��ȡ��һ���ڵ�
 *
 *@param handle
 *
 *@return ��һ���ڵ��ָ��
 */
UtlNode_t *utl_list_get_first(ListHandle_t handle);

/**
 *@brief ɾ����һ���ڵ�
 *
 *@param handle
 *
 *@return ��һ���ڵ������
 */
void *utl_list_pop(ListHandle_t handle);

/**
 *@brief
 *
 *@param handle
 *
 *@return
 */
int utl_list_lock(ListHandle_t handle);

/**
 *@brief
 *
 *@param handle
 *
 *@return
 */
int utl_list_unlock(ListHandle_t handle);

#endif /* UTL_LIST_H_ */
