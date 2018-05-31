/*
 * sp_record.h
 *
 *  Created on: 2013-11-1
 *      Author: lfx
 */

#ifndef SP_ALARM_H_
#define SP_ALARM_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "malarmout.h"


typedef struct{
	BOOL bEnable;			//�Ƿ�������������
	BOOL bNormallyClosed;	//�Ƿ񳣱ա���ֵΪ��ʱ���Ͽ���������֮�պϱ���

	unsigned char  u8AlarmNum;			//����·�����ƣ�ÿһλ��ʾһ·������1��0�أ���ֻ����·����,ֻ�к���λ��Ч

	BOOL bEnableRecord;		//�Ƿ�������¼��

	int nDelay;				//������ʱ��С�ڴ�ʱ��Ķ�α���ֻ����һ���ʼ����ͻ��˲��ܴ�����
	BOOL bStarting;			//�Ƿ����ڱ���,���ڿͻ��˱�����һֱ��������������Ƿ�����ͻ��˷��ͱ���

	BOOL bBuzzing;			//�Ƿ����������
	BOOL bSendtoClient;		//�Ƿ������ֿ�
	BOOL bSendEmail;		//�Ƿ����ʼ�
}SPAlarmIn_t;

/**
 *@brief ����
 *
 *@param channelid ��������ͨ��
 */
int sp_alarmin_start(int channelid);

/**
 *@brief ����
 *
 *@param channelid ��������ͨ��
 */
int sp_alarmin_stop(int channelid);

/**qlc20141124
 * @brief ��ȡ����������Ϣ
 *
 *@param channelid ��������ͨ��
 * @param alarm �����������
 *
 * @return 0 �ɹ�
 */
int sp_alarmin_get_param(int channelid, SPAlarmIn_t *param);

/**qlc20141124
 * @brief ��ȡ����������Ϣ
 *
 *@param channelid ��������ͨ��
 * @param alarm �����������
 *
 * @return 0 �ɹ�
 */
int sp_alarmin_set_param(int channelid, SPAlarmIn_t *param);

/**
 *@brief ����״̬
 *
 *@param channelid ��������ͨ��
 *
 *@return ����ʱΪ�棬����Ϊ��
 */
int sp_alarmin_b_onduty(int channelid);

/**
 *@brief ����״̬
 *
 *@param channelid ��������ͨ��
 *
 *@return ���ڱ���ʱΪ�棬����Ϊ��
 */
int sp_alarmin_b_alarming(int channelid);

typedef struct
{
	char tStart[16];		// ��ȫ������ʼʱ��(��ʽ: hh:mm:ss)
	char tEnd[16];			// ��ȫ��������ʱ��(��ʽ: hh:mm:ss)
}SPAlarmTime;

typedef struct
{
	int delay;				//���ʱ��
	char sender[64];		//������
	char server[64];		//������
	char username[64];		//�û���
	char passwd[64];		//����
	char receiver0[64];	    //�ռ���1
	char receiver1[64];
	char receiver2[64];
	char receiver3[64];

	//add by xianlt at 20120628
	int port;				//�������˿�
	char crypto[12];		//���ܴ��䷽ʽ
	//add by xianlt at 20120628

	BOOL bEnable;			//��ȫ�����ܿ���
	SPAlarmTime alarmTime[3];	// ��ȫ����ʱ�������
	
	char vmsServerIp[20];	//VMS������IP��ַ
	unsigned short vmsServerPort;		//VMS�������˿�
	
	BOOL bAlarmSoundEnable;	//������������
	BOOL bAlarmLightEnable;	//�����⿪��
}SPAlarmSet_t;

/**lk20131120
 * @brief ��ȡ������Ϣ
 *
 * @param alarm ������Ϣ���
 *
 * @return 0 �ɹ�
 */
int sp_alarm_get_param(SPAlarmSet_t *alarm);

/**lk20131120
 * @brief ���ñ�����Ϣ
 *
 * @param alarm �µı�����Ϣ
 *
 * @return 0 �ɹ�
 */
int sp_alarm_set_param(SPAlarmSet_t *alarm);

int sp_alarm_sound_start();

int sp_alarm_sound_stop();

int sp_alarm_light_start();

int sp_alarm_light_stop();

int sp_alarm_buzzing_open();

int sp_alarm_buzzing_close();

/**lk20131120
 * @brief ���Ͳ����ʼ�
 */
int  sp_mail_test(unsigned char *szIsSucceed);

/**
 * @brief ���ֻ��˷��Ͳ���������Ϣ
 *
 * @param 
 */
int sp_alarm_send_deployment();

#ifdef __cplusplus
}
#endif


#endif /* SP_RECORD_H_ */
