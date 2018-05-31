#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <jv_common.h>
#include "sp_define.h"
#include "sp_alarm.h"
#include "gb28181.h"
#include "sp_mdetect.h"
#include "malarmout.h"
#include "malarmin.h"
#include <SYSFuncs.h>
#if 1

/**
 *@brief ����
 *
 *@param channelid ���������
 */
int sp_alarmin_start(int channelid)
{
	__FUNC_DBG__();
	MAlarmIn_t ma;
	
	malarmin_get_param(channelid, &ma);
	ma.bEnable = TRUE;
	malarmin_set_param(channelid, &ma);
	WriteConfigInfo();
	malarmin_flush(channelid);
	return 0;
}

/**
 *@brief ����
 *
 *@param channelid ���������
 */
int sp_alarmin_stop(int channelid)
{
	__FUNC_DBG__();
	MAlarmIn_t ma;
	
	malarmin_get_param(channelid, &ma);
	ma.bEnable = FALSE;
	malarmin_set_param(channelid, &ma);
	WriteConfigInfo();
	malarmin_flush(channelid);
	return 0;
}
int sp_alarmin_get_param(int channelid, SPAlarmIn_t *param)
{
	MAlarmIn_t ma;
	
	malarmin_get_param(channelid, &ma);
	param->bEnable = ma.bEnable;
	param->bBuzzing = ma.bBuzzing;
	param->bEnableRecord = ma.bEnableRecord;
	param->bNormallyClosed = ma.bNormallyClosed;
	param->bSendEmail = ma.bSendEmail;
	param->bSendtoClient = ma.bSendtoClient;
	param->bStarting = ma.bStarting;
	param->u8AlarmNum = ma.u8AlarmNum;
	param->nDelay = ma.nDelay;
	return 0;
}
int sp_alarmin_set_param(int channelid, SPAlarmIn_t *param)
{
	MAlarmIn_t ma;
	
	malarmin_get_param(channelid, &ma);
	ma.bEnable = param->bEnable;
	ma.bBuzzing = param->bBuzzing;
	ma.bEnableRecord = param->bEnableRecord;
	ma.bNormallyClosed = param->bNormallyClosed;
	ma.bSendEmail = param->bSendEmail;
	ma.bSendtoClient = param->bSendtoClient;
	ma.bStarting = param->bStarting;
	ma.u8AlarmNum = param->u8AlarmNum;
	ma.nDelay = param->nDelay;
	malarmin_set_param(channelid, &ma);
	WriteConfigInfo();
	malarmin_flush(channelid);
	return 0;
}

/**
 *@brief ����״̬
 *
 *@param channelid ���������
 *
 *@return ����ʱΪ�棬����Ϊ��
 */
int sp_alarmin_b_onduty(int channelid)
{
	__FUNC_DBG__();
	MAlarmIn_t ma;
	
	malarmin_get_param(channelid, &ma);
	return ma.bEnable;
}

/**
 *@brief ����״̬
 *
 *@param channelid ���������
 *
 *@return ���ڱ���ʱΪ�棬����Ϊ��
 */
int sp_alarmin_b_alarming(int channelid)
{
	MAlarmIn_t ma;
	
	malarmin_get_param(channelid, &ma);
	return ma.bStarting;
}

/**lk20131120
 * @brief ��ȡ������Ϣ--�ñ���Ϊ�ʼ�����
 *
 * @param alarm ������Ϣ���
 */
int sp_alarm_get_param(SPAlarmSet_t *alarm)
{
	ALARMSET malarm;
	malarm_get_param(&malarm);

	alarm->delay = malarm.delay;
	alarm->port = malarm.port;
	strncpy(alarm->crypto, malarm.crypto, 12);
	strncpy(alarm->username, malarm.username, 64);
	strncpy(alarm->passwd, malarm.passwd, 64);
	strncpy(alarm->server, malarm.server, 64);
	strncpy(alarm->sender, malarm.sender, 64);
	strncpy(alarm->receiver0, malarm.receiver0, 64);
	strncpy(alarm->receiver1, malarm.receiver1, 64);
	strncpy(alarm->receiver2, malarm.receiver2, 64);
	strncpy(alarm->receiver3, malarm.receiver3, 64);
	alarm->bEnable = malarm.bEnable;
	memcpy(alarm->alarmTime, malarm.alarmTime, sizeof(alarm->alarmTime));
	strncpy(alarm->vmsServerIp, malarm.vmsServerIp, 20);
	alarm->vmsServerPort= malarm.vmsServerPort;
	alarm->bAlarmSoundEnable = malarm.bAlarmSoundEnable;
	alarm->bAlarmLightEnable = malarm.bAlarmLightEnable;
	return 0;
}

/**lk20131120
 * @brief ���ñ�����Ϣ--�ñ���Ϊ�ʼ�����
 *
 * @param alarm �µı�����Ϣ
 */
int sp_alarm_set_param(SPAlarmSet_t *alarm)
{
	ALARMSET malarm;
	malarm_get_param(&malarm);

	malarm.delay = alarm->delay;
	malarm.port = alarm->port;
	strncpy(malarm.crypto, alarm->crypto, 12);
	strncpy(malarm.username, alarm->username, 64);
	strncpy(malarm.passwd, alarm->passwd, 64);
	strncpy(malarm.server, alarm->server, 64);
	strncpy(malarm.sender, alarm->sender, 64);
	strncpy(malarm.receiver0, alarm->receiver0, 64);
	strncpy(malarm.receiver1, alarm->receiver1, 64);
	strncpy(malarm.receiver2, alarm->receiver2, 64);
	strncpy(malarm.receiver3, alarm->receiver3, 64);
	malarm.bEnable = alarm->bEnable;
	memcpy(malarm.alarmTime, alarm->alarmTime, sizeof(malarm.alarmTime));
	strncpy(malarm.vmsServerIp, alarm->vmsServerIp, 20);
	malarm.vmsServerPort= alarm->vmsServerPort;
	malarm.bAlarmSoundEnable = alarm->bAlarmSoundEnable;
	malarm.bAlarmLightEnable = alarm->bAlarmLightEnable;

	malarm_set_param(&malarm);
	WriteConfigInfo();

	return 0;
}

int sp_alarm_sound_start()
{
	malarm_sound_start();
	return 0;
}

int sp_alarm_sound_stop()
{
	malarm_sound_stop();
	return 0;
}

int sp_alarm_light_start()
{
	malarm_light_start();
	return 0;
}

int sp_alarm_light_stop()
{
	malarm_light_stop();
	return 0;
}

int sp_alarm_buzzing_open()
{
	malarm_buzzing_open();
	return 0;
}

int sp_alarm_buzzing_close()
{
	malarm_buzzing_close();
	return 0;
}

/**
 * @brief ���ֻ��˷��Ͳ���������Ϣ
 *
 * @param
 */
int sp_alarm_send_deployment()
{
	JV_ALARM alarm;
	malarm_build_info(&alarm, ALARM_TOTAL);
	alarm.cmd[1] = ALARM_TEXT;

	return 0;
}

/**lk20131120
 * @brief ���Ͳ����ʼ�
 */
int sp_mail_test(unsigned char *szIsSucceed)
{
	return (int)mail_test((char *)szIsSucceed);
}

#endif

