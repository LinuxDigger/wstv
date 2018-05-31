/*
 * malarmin.h
 *
 *  Created on: 2013-11-25
 *      Author: lfx
 */

#ifndef MALARMIN_H_
#define MALARMIN_H_
#include <jv_common.h>

typedef struct{
	BOOL bEnable;			//�Ƿ�������������
	BOOL bNormallyClosed;	//�Ƿ񳣱ա���ֵΪ��ʱ���Ͽ���������֮�պϱ���

	U8  u8AlarmNum;			//����·�����ƣ�ÿһλ��ʾһ·������1��0�أ���ֻ����·����,ֻ�к���λ��Ч

	BOOL bEnableRecord;		//�Ƿ�������¼��

	int nDelay;				//������ʱ��С�ڴ�ʱ��Ķ�α���ֻ����һ���ʼ����ͻ��˲��ܴ�����
	BOOL bStarting;			//�Ƿ����ڱ���,���ڿͻ��˱�����һֱ��������������Ƿ�����ͻ��˷��ͱ���

	BOOL bBuzzing;			//�Ƿ����������
	BOOL bSendtoClient;		//�Ƿ������ֿ�
	BOOL bSendEmail;		//�Ƿ����ʼ�
	BOOL bSendtoVMS;		//�Ƿ�����VMS������
}MAlarmIn_t;

extern int ma_timer;


/**
 *@brief ��Ҫ����ʱ�Ļص�����
 * �ú�����Ҫ����֪ͨ�ֿأ��Ƿ��о�������
 *@param channelid ����������ͨ����
 *@param bAlarmOn �����������߹ر�
 *
 */
typedef void (*alarmin_alarming_callback_t)(int channelid, BOOL bAlarmOn);

/**
 * @brief ע�ᱨ�����룬��ͻ��˷��;����Ļص�����
 */
int malarmin_set_callback(alarmin_alarming_callback_t callback);

/**
 * @brief ��ʼ��
 */
int malarmin_init(void);

/**
 * @brief ����
 */
int malarmin_deinit(void);

/**
 * @brief ���ñ�������
 * @param channel ͨ����
 * @param param ��������
 *
 * @return 0�ɹ�
 */
int malarmin_set_param(int channel, MAlarmIn_t *param);

/**
 * @brief ��ȡ��������
 *
 * @param channel ͨ����
 * @param param ������Ϣ���
 *
 * @return 0 �ɹ�
 */
int malarmin_get_param(int channel, MAlarmIn_t *param);

/**
 * @brief ��ʼ��ⱨ�������ź�
 *
 * @param channel ͨ����
 *
 * @return 0 �ɹ�
 */
int malarmin_start(int channel);

/**
 * @brief ֹͣ��ⱨ�������ź�
 *
 * @param channel ͨ����
 *
 * @return 0 �ɹ�
 */
int malarmin_stop(int channel);

/**
 *@brief ʹ������Ч
 *	��#malarmin_set_param֮������ı���ʹ��״̬�����ñ�����
 *@param channelid Ƶ����
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int malarmin_flush(int channelid);


int _malarmin_process(int tid, void *param);



#endif /* MALARMIN_H_ */
