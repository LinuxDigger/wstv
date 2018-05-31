

/*	malarmout.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯���������ش���
	������ʷ���svn�汾����־
*/

#ifndef __MALARMOUT_H__
#define __MALARMOUT_H__

#include "jv_common.h"

#define MAX_ALATM_TIME_NUM	3
#define MAX_DEPLOY_POINT_NUM		(100)

typedef struct
{
	char tStart[16];		// ��ȫ������ʼʱ��(��ʽ: hh:mm:ss)
	char tEnd[16];			// ��ȫ��������ʱ��(��ʽ: hh:mm:ss)
}AlarmTime;

typedef struct
{
	int dayOfWeek;			//һ���еڼ���, 0:��Ч��1:����, 2:��һ, 3:�ܶ�...7:����
	BOOL bProtection;		// �Ƿ�����ȫ����
	int time;				// ������ȫ����ʱ��㣬���������
}DeployPoint;

typedef struct 
{
	int  bEnable;			//ʹ��
   	int  Schedule_time_H;	//����ʱ���
   	int  Schedule_time_M;
    int  num;				//����
	int  Interval;			//ʱ����
	
}MSchedule;

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

	MSchedule  m_Schedule[5]; //��ʱץ������
	//add by xianlt at 20120628
	int port;				//�������˿�
	char crypto[12];		//���ܴ��䷽ʽ
	//add by xianlt at 20120628

	BOOL bEnable;			//��ȫ�����ܿ���
	AlarmTime alarmTime[MAX_ALATM_TIME_NUM];	// ��ȫ����ʱ�������
	
	char vmsServerIp[20];	//VMS������IP��ַ
	unsigned short vmsServerPort;		//VMS�������˿�
	
	BOOL bAlarmSoundEnable;	//������������
	BOOL bAlarmLightEnable;	//�����ƹ⿪��
}ALARMSET;

//�����������ö��
typedef enum
{
	ALARM_OUT_CLIENT = 0x01,	//�������ֿ�
	ALARM_OUT_BUZZ = 0x02,		//������
	ALARM_OUT_EMAIL = 0x04,		//�����ʼ�
	ALARM_OUT_SOUND = 0x08,		//��������
	ALARM_OUT_LIGHT = 0x10,		//������
	ALARM_OUT_BUTT
}AlarmOutType_e;

//��������ö��
typedef enum
{
	ALARM_VMS_MDETECT 		= 0x01,		//�ƶ���ⱨ��
	ALARM_VMS_VIDEOCOVER 	= 0x02,		//��Ƶ�ڵ�����
	ALARM_VMS_VIDEOLOST 	= 0x03,		//��Ƶ��ʧ����
	ALARM_VMS_DISKLOST 		= 0x04,		//Ӳ�̶�ʧ����
	ALARM_VMS_ALARMIN 		= 0x05,		//��������
	ALARM_VMS_IVP 			= 0x06,		//���ܷ�������
	ALARM_VMS_DOOR 			= 0x07,		//�ſر���
	ALARM_VMS_BUTT
}AlarmType_e;

//��������ͨ������ö��
typedef enum
{
	ALARM_NONE 	= 0x00,
	ALARM_UDP 	= 0x01,
	ALARM_TCP 	= 0x02,
	ALARM_BUTT
}AlarmChannelType_e;

#define ALARM_TEXT			0x00
#define ALARM_PIC			0x01
#define ALARM_VIDEO 		0x02

typedef struct
{
	char cmd[4];			//��һλ:0x11�ֻ����ͷ����� 
							//�ڶ�λ:0x01����������Ϣ 0x02����ͼƬ��Ϣ 0x03������Ƶ��Ϣ
							//����λԤ��
	char ystNo[32]; 		//����ͨ��
	U16 nChannel;			//��1��ʼ
	U16 alarmType;			//ALARM_TYPEs
	long time;				//ʱ��
	char PicName[64];		//���������ı���ͼƬ����
	char VideoName[64];		//���������ı�����Ƶ����
	char cloudPicName[64];  //�����������ƴ洢ͼƬ����
	char cloudVideoName[64];//�����������ƴ洢��Ƶ����
	char uid[36];			//Ψһ��ʶ��
	char devName[32];		//�豸�ǳ�
	char alarmDevName[32];	//������豨�����ⲿ�����豸������
	U32 pushType;			//���ͷ�������AlarmPushType
	S32 errorCode;			//�ϴ������룬��AlarmErrorCode
	char cloudHost[64];		//�ƴ洢������
	char cloudBucket[32];	//�ƴ洢�ռ�
	int  cloudSt;			//�ƴ洢״̬
}JV_ALARM;

// ���ͷ���
enum AlarmPushType
{
	ALARM_PUSH_YST,
	ALARM_PUSH_CLOUD,
	ALARM_PUSH_YST_CLOUD,
	ALARM_PUSH_BUTT,
};


int malarm_init(void);

int malarm_flush(void);

int malarm_deinit(void);

int malarm_sendmail(int channelid, char * message);

void malarm_set_param(ALARMSET *alarm);

void malarm_get_param(ALARMSET *alarm);

U32 mail_test(char *szIsSucceed);

BOOL malarm_check_enable();
BOOL malarm_check_validtime();

/*
 * �������������أ��򿪷����������������룬�Զ��ر�
 */
int malarm_buzzing_open();
int malarm_buzzing_close();

void malarm_build_info(void *alarmInfo, ALARM_TYPEs type);

void malarm_sound_start();
int malarm_sound_stop();

void malarm_light_start();
void malarm_light_stop();

BOOL malarm_get_speakerFlag();



#endif

