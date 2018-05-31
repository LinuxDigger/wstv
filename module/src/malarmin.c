/*
 * malarmin.c
 *
 *  Created on: 2013-11-25
 *      Author: lfx
 */

#include "jv_common.h"
#include "jv_alarmin.h"
#include "msnapshot.h"
#include "malarmout.h"

#include "SYSFuncs.h"
#include "utl_timer.h"
#include "mrecord.h"
#include "mlog.h"
#include "mgb28181.h"

#include "malarmin.h"
#include "mipcinfo.h"
#include "mcloud.h"
#include "cgrpc.h"
#include "utl_iconv.h"
#include "alarm_service.h"
#include "mbizclient.h"

static MAlarmIn_t alarmincfg;

int ma_timer = -1;

static alarmin_alarming_callback_t callback_ptr = NULL;

/**
 * @brief ��������ı��������߳�
 */
int _malarmin_process(int tid, void *param)
{
	int ret = FALSE;

	static int timesended = 0;
	int timenow;
	int ignore_flag = 0; //���ڱ�־�Ƿ�ʱ������ڽ϶�ʱ���ڵڶ��δ���������������Ӧ

	ALARMSET stAlarm;
	malarm_get_param(&stAlarm);
	alarmincfg.nDelay = stAlarm.delay;

	static U32 alarmOutType = 0;
	BOOL bAlarmReportEnable = FALSE; // ��ȫ��������
	BOOL bCloudRecEnable = FALSE; // �Ƿ�ͨ�ƴ洢����
	BOOL bInValidTime = FALSE; // �Ƿ��ڰ�ȫ����ʱ�����
	JV_ALARM alarm;

	timenow = time(NULL);
	if (timenow - timesended < alarmincfg.nDelay)
	{
		ignore_flag = 1;
	}
	else if((int)param != 0)
		timesended = timenow;
		
	Printf("===================>>> AlarmIN-Type[%d]\n",(int)param);

	if ((int)param != 0)
	{
		bInValidTime = malarm_check_validtime();
		if(!bInValidTime)
		{
			if(alarmincfg.bStarting)
			{
				utl_timer_reset(tid, 0, _malarmin_process, (void *)0);
				ret = TRUE;
			}
			return ret;
		}

		alarmincfg.bStarting = TRUE;
		bAlarmReportEnable = malarm_check_enable();
		bCloudRecEnable = mcloud_check_enable();
		if (!ignore_flag)
		{
			mlog_write("MAlarmIn: AlarmIn Warning");
			malarm_build_info(&alarm, ALARM_GPIN);
			if(hwinfo.bHomeIPC && bAlarmReportEnable)
			{
				if(bCloudRecEnable)
					alarm.pushType = ALARM_PUSH_YST_CLOUD;
			}
		}
		if (alarmincfg.bEnableRecord)
		{
			if(hwinfo.bHomeIPC && bAlarmReportEnable && bCloudRecEnable)
			{
				if (!ignore_flag)
				{
					alarm.cmd[1] = ALARM_VIDEO;
					mrecord_alarming(0, ALARM_TYPE_ALARM, &alarm);
				}
			}
			else
			{
				mrecord_alarming(0, ALARM_TYPE_ALARM, NULL);
			}
		}
		if (!ignore_flag)
		{
			if(hwinfo.bHomeIPC && bAlarmReportEnable)
			{
				mrecord_alarm_get_attachfile(REC_ALARM, &alarm);
				if (alarm.cloudPicName[0])
				{
					/*printf("alarmJpgFile: %s\n", alarm.cloudPicName);*/
					if (msnapshot_get_file(0, alarm.cloudPicName) != 0)
					{
						alarm.cloudPicName[0] = '\0';
						alarm.PicName[0] = '\0';
					}
				}
				else if (alarm.PicName[0])
				{
					/*printf("alarmJpgFile: %s\n", alarm.PicName);*/
					if (msnapshot_get_file(0, alarm.PicName) != 0)
					{
						alarm.PicName[0] = '\0';
					}
				}
				else
					printf("cloudPicName and picName are none\n");
#ifdef BIZ_CLIENT_SUPPORT
				if (hwinfo.bXWNewServer)
				{
					int ret = -1;
					char *ppic = "";
					char tmppic[64] = {0};
					char *pvedio = "";
					if (alarm.cloudPicName[0])
					{
						ppic = strrchr(alarm.cloudPicName, '/');
						struct tm tmDate;
						localtime_r(&alarm.time, &tmDate);
						sprintf(tmppic,"%s/%s/%.4d%.2d%.2d%s",obss_info.days,alarm.ystNo,tmDate.tm_year + 1900,
							tmDate.tm_mon + 1, tmDate.tm_mday, ppic);	
						ppic = tmppic;
						pvedio = alarm.cloudVideoName;
					}
					else if (alarm.PicName[0])
					{
						ppic = alarm.PicName;
						pvedio = alarm.VideoName;
					}
					ret = mbizclient_PushAlarm("GPIN",alarm.ystNo,alarm.nChannel,ALARM_TEXT,alarm.uid,alarm.alarmType,alarm.time,ppic,pvedio);
				}
#endif
				if(bCloudRecEnable && alarm.cloudPicName[0])
				{
					alarm.cmd[1] = ALARM_PIC;
					mcloud_upload_alarm_file(&alarm);
				}
			}
		}
		if (alarmincfg.bSendtoClient)
		{
			//����ÿ�ζ����ֿط������źţ����ڸ����ӵķֿ����յ�
			if (callback_ptr != NULL)
			{
				callback_ptr(0, TRUE);
			}

			if (!ignore_flag)
				mlog_write("MAlarmIn: Client Alarm On");
			alarmOutType |= ALARM_OUT_CLIENT;
		}
		if (!ignore_flag && alarmincfg.bSendtoVMS)
		{
			ipcinfo_t info;
			ipcinfo_get_param(&info);
			AlarmInfo_t alarmInfo;
			alarmInfo.channel = 0;
			jv_ystNum_parse(alarmInfo.dev_id, info.nDeviceInfo[6], info.ystID);
			strcpy(alarmInfo.dev_type, "ipc");
			strcpy(alarmInfo.detector_id, "12345");
			strcpy(alarmInfo.type, "io");
			strcpy(alarmInfo.subtype, "pir");
			//strcpy(alarmInfo.content, "pir alarm");
			utl_iconv_gb2312toutf8("��������Ӧ����", alarmInfo.pir_code, sizeof(alarmInfo.pir_code));
			cgrpc_alarm_report(&alarmInfo);
		}
		if (!ignore_flag)
		{
			if (alarmincfg.bSendEmail)
			{
				malarm_sendmail(0, mlog_translate("MAlarmIn: AlarmIn Warning")); //����mlog�Ķ����Թ���
				mlog_write("MAlarmIn: Mail Sended");
			}
		}
		if (alarmincfg.bBuzzing)
		{
			//�������������140415
			Printf("LK TEST BUZZING ALARMING\n");
			malarm_buzzing_open();
			alarmOutType |= ALARM_OUT_BUZZ;
		}
#ifdef GB28181_SUPPORT
		if (!ignore_flag)
		{
			mgb_send_alarm(0, 2, 5);
		}
#endif
		if(hwinfo.bHomeIPC && stAlarm.bAlarmSoundEnable)
		{
			malarm_sound_start();
			alarmOutType |= ALARM_OUT_SOUND;
		}
		if(hwinfo.bHomeIPC && stAlarm.bAlarmLightEnable)
		{
			malarm_light_start();
			alarmOutType |= ALARM_OUT_LIGHT;
		}

		utl_timer_reset(tid, alarmincfg.nDelay * 1000, _malarmin_process, (void *)0);
		ret = TRUE;
	}
	else if ((int) param == 0)
	{
		Printf("stop alarm...\n");
		if (alarmincfg.bStarting)
		{
			if(alarmOutType & ALARM_OUT_CLIENT)
			{
				if (callback_ptr != NULL)
				{
					callback_ptr(0, FALSE);
				}
			}
			if(alarmOutType & ALARM_OUT_BUZZ)
			{
				malarm_buzzing_close();
			}
			if(alarmOutType & ALARM_OUT_SOUND)
			{
				malarm_sound_stop();
			}
			if(alarmOutType & ALARM_OUT_LIGHT)
			{
				malarm_light_stop();
			}
			alarmOutType = 0;

			alarmincfg.bStarting = FALSE;
			//mrecord_set(0, alarming, FALSE);
			if (!ignore_flag)
			{
				mlog_write("MAlarmIn: Client Alarm Off");
			}
		}
		ret = FALSE;
	}
	return ret;
}

/**
 * @brief ��������Ļص���������⵽���������ź�ʱ���ô˺���
 */
static void _malarmin_callback(int channelid, void *param)
{
	if (ma_timer == -1)
		ma_timer = utl_timer_create("malarmIn", 0, _malarmin_process, param);
	else
		utl_timer_reset(ma_timer, 0, _malarmin_process, param);
}

/**
 *@brief ���ñ����ص�����
 */
int malarmin_set_callback(alarmin_alarming_callback_t callback)
{
	callback_ptr = callback;
	return 0;
}

/**
 * @brief ��ʼ��
 */
int malarmin_init(void)
{
	jv_alarmin_init();
	return 0;
}

/**
 * @brief ����
 */
int malarmin_deinit(void)
{
	malarmin_stop(0);
	if (ma_timer != -1)
		utl_timer_destroy(ma_timer);
	jv_alarmin_deinit();
	return 0;
}

/**
 * @brief ���ñ�������
 * @param channel ͨ����
 * @param param ��������
 *
 * @return 0�ɹ�
 */
int malarmin_set_param(int channel, MAlarmIn_t *param)
{
	MAlarmIn_t malarmin;
	malarmin_get_param(0, &malarmin);
	malarmin.bEnable = param->bEnable;
	malarmin.bNormallyClosed = param->bNormallyClosed;
	malarmin.bSendEmail = param->bSendEmail;
	malarmin.bSendtoVMS= param->bSendtoVMS;
	malarmin.bSendtoClient = param->bSendtoClient;
	malarmin.bEnableRecord = param->bEnableRecord;
	malarmin.u8AlarmNum = param->u8AlarmNum;
	malarmin.bBuzzing = param->bBuzzing;

	if (param->bEnable == TRUE)
	{
		if (alarmincfg.bEnable == FALSE)
		{
			mlog_write("MAlarmIn Start");
		}
		if (alarmincfg.bSendEmail == 0 && param->bSendEmail == 1)
		{
			mlog_write("MA sendmail Enabled");
		}
		else if (alarmincfg.bSendEmail == 1 && param->bSendEmail == 0)
		{
			mlog_write("MA sendmail Disabled");
		}
	}
	else
	{
		if (alarmincfg.bEnable == TRUE)
		{
			mlog_write("MAlarmIn Stop");
		}
	}

	memcpy(&alarmincfg, &malarmin, sizeof(MAlarmIn_t));
	return 0;
}

/**
 * @brief ��ȡ��������
 *
 * @param channel ͨ����
 * @param param ������Ϣ���
 *
 * @return 0 �ɹ�
 */
int malarmin_get_param(int channel, MAlarmIn_t *param)
{
	memcpy(param, &alarmincfg, sizeof(MAlarmIn_t));
	return 0;
}

/**
 * @brief ��ʼ��ⱨ�������ź�
 *
 * @param channel ͨ����
 *
 * @return 0 �ɹ�
 */
int malarmin_start(int channel)
{
	int ret;
	jv_alarmin_attr_t attr;
	jv_alarmin_get_param(0, &attr);
	attr.bNormallyClosed = alarmincfg.bNormallyClosed;
	attr.u8AlarmNum = alarmincfg.u8AlarmNum;
	ret = jv_alarmin_set_param(0, &attr);
	if (ret != 0)
	{
		Printf("jv_malarmin_set_attr failed: %d\n", ret);
		mlog_write("MAlarmIn Start: Failed");
		return -1;
	}
	if(alarmincfg.bEnableRecord)
		mrecord_set(0, alarming, TRUE);//SD��¼����
	else
		mrecord_set(0, alarming, FALSE);
	WriteConfigInfo();
	ret = jv_alarmin_start(0, _malarmin_callback, NULL);
	return ret;
}

/**
 * @brief ֹͣ��ⱨ�������ź�
 *
 * @param channel ͨ����
 *
 * @return 0 �ɹ�
 */
int malarmin_stop(int channel)
{
	int ret;

	mrecord_set(0, alarming, FALSE);//SD��¼��ر�
	WriteConfigInfo();
	ret = jv_alarmin_stop(channel);
	if (ret != 0)
	{
		Printf("jv_alarmin_stop failed: %d\n", ret);
		mlog_write("MAlarmIn Stop: Failed");
		return -1;
	}
	Printf("malarmin_stop over %d\n", channel);
	return 0;
}

/**
 *@brief ʹ������Ч
 *	��#malarmin_set_param֮������ı���ʹ��״̬�����ñ�����
 *@param channelid Ƶ����
 *
 *@return 0 �ɹ���-1 ʧ��
 */
int malarmin_flush(int channelid)
{
	if (alarmincfg.bEnable)
		malarmin_start(channelid);
	else
		malarmin_stop(channelid);
	return 0;
}
