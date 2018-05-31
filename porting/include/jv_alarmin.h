/*
 * jv_alarmin.h
 *
 *  Created on: 2013-11-30
 *      Author: LK
 */

#ifndef JV_ALARMIN_H_
#define JV_ALARMIN_H_

#include "jv_common.h"

typedef struct{
	BOOL bNormallyClosed;	//�Ƿ񳣱ա���ֵΪ��ʱ���Ͽ���������֮�պϱ���
	U8  u8AlarmNum;			//����·�����ƣ�ÿһλ��ʾһ·������1��0�أ���ֻ����·����,ֻ�к���λ��Ч
}jv_alarmin_attr_t;

typedef void (*jv_alarmin_callback_t)(int channelid, void *param);

/**
 *@brief  ��ʼ��
 *@return 0 �ɹ�
 */
int jv_alarmin_init(void);

/**
 *@brief ����
 *@return 0 �ɹ�
 */
int jv_alarmin_deinit(void);

/**
 * @brief ��ȡ��������������Ϣ
 *
 * @param channelid ͨ����
 * @param attr ������Ϣ���
 *
 * @return 0 �ɹ�
 */
int jv_alarmin_get_param(int channelid, jv_alarmin_attr_t *attr);

/**
 * @brief ���ñ�������������Ϣ
 *
 * @param channelid ͨ����
 * @param attr ������Ϣ
 *
 * @return 0 �ɹ�
 */
int jv_alarmin_set_param(int channelid, jv_alarmin_attr_t *attr);

/**
 *@brief ��ʼ����������
 *@param ͨ����
 *@param �ص���������⵽���������ź�ʱ������
 *@param �ص���������
 *@return 0 �ɹ�
 */
int jv_alarmin_start(int channelid, jv_alarmin_callback_t callback, void *param);

/**
 *@brief ֹͣ�����������
 *@param ͨ����
 *@return 0 �ɹ�
 */
int jv_alarmin_stop(int channelid);

/*
 * ����������ȷ�������ɣ�ֻ��һ���ӿ�
 */
int jv_alarm_buzzing_on(int bON);


#endif /* JV_ALARMIN_H_ */
