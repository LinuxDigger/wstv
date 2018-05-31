#ifndef M_DOORALARM_H
#define M_DOORALARM_H

#define DoorAlarm_FILE			CONFIG_PATH"dooralarm.dat"			//�Ŵű�����Ϣ

typedef enum
{
	DOOR_INSERT_OK = 0,
	DOOR_INSERT_TIMEDOUT = -1,
	DOOR_INSERT_FULL = -2,
	DOOR_REINSERT = -3,
}DOOR_INSERT_TYPE;

typedef struct
{
	BOOL bEnable;			//�Ƿ�������
	BOOL bSendtoClient;		//�Ƿ������ֿ�
	BOOL bSendEmail;		//�Ƿ����ʼ�
	BOOL bSendtoVMS;		//�Ƿ�����VMS������
	BOOL bBuzzing;			//�Ƿ����������
	BOOL bEnableRecord;		//�Ƿ�������¼��
}MDoorAlarm_t;
typedef enum{
	DOOR_ALARM_TYPE_DOOR = 1,	//�Ŵ�
	DOOR_ALARM_TYPE_SPIRE,		//�ֻ�
	DOOR_ALARM_TYPE_REMOTE_CTRL,//ң��
	DOOR_ALARM_TYPE_SMOKE,		//�̸�̽����
	DOOR_ALARM_TYPE_CURTAIN,	//Ļ��̽����
	DOOR_ALARM_TYPE_PIR,		//����̽����
	DOOR_ALARM_TYPE_GAS,		//ȼ��̽����
	DOOR_ALARM_TYPE_BUT
}DoorAlarmType_e;

typedef void (*DoorAlarmInsertSend)(signed char result, signed char index);
typedef void (*DoorAlarmOnSend)(unsigned char* name, int arg);
typedef void (*DoorAlarmOnStop)();

int mdooralarm_init(DoorAlarmInsertSend insert_send, DoorAlarmOnSend alarmon_send, DoorAlarmOnStop alarmon_stop);
int mdooralarm_insert(int dooralarm_type);
int mdooralarm_set(unsigned char index, unsigned char* name, unsigned char enable);
int mdooralarm_del(unsigned char index);
int mdooralarm_del_all();
int mdooralarm_select(char* buf);
BOOL mdooralarm_bSupport();	//�ж��豸�Ƿ�֧���Ŵű���

//#define DEMO_NOUSE_MIDIFIED_FUNC
#ifdef DEMO_NOUSE_MIDIFIED_FUNC

#include "jv_common.h"

typedef struct
{
	unsigned int id; //���߱����豸��ID��
	unsigned int cmd; //�����豸�������
	char name[64];				//����
	BOOL bEnable;		//�Ƿ�ʹ��
	DoorAlarmType_e type;			//����	�Ŵ�:0	�ֻ�:1

}DoorAlarm_t;

/**
 *@brief �����ص�����
 *@param bSearch �Ƿ��������Ľ�������������ű�ʾ�б������
 *@param ������Ϣ�������豸���ʱ����Ϣ
 */
typedef void (*DoorAlarmCallback_t)(BOOL bSearch, DoorAlarm_t *alarm);

/**
 *@brief ��ʼ��
 *@param callback ���ûص������������������ʱ�ص����Լ��豸���ʱ�ص�
 */
int mdooralarm_init(DoorAlarmCallback_t callback);

/**
 *@brief �豸������ֻҪ������һ���������ص����Һ������ٲ���
 */
int mdooralarm_scan(void);

/**
 *@brief ��ӱ���ģ��
 */
int mdooralarm_add(DoorAlarm_t *alarm);

/**
 *@brief ��ȡ����ģ�������
 */
int mdooralarm_get_cnt(void);

/**
 *@brief ��ȡ����ģ����Ϣ
 */
int mdooralarm_get(int index, DoorAlarm_t *alarm);

#endif

/**
 * @brief ��ȡ��������
 *
 * @param param �����������
 *
 * @return ����ָ��
 */
MDoorAlarm_t *mdooralarm_get_param(MDoorAlarm_t *param);

/**
 *@brief ���ò���
 *@param param ����ָ��
 *
 *@return 0�ɹ�  -1ʧ��
 */
int mdooralarm_set_param(MDoorAlarm_t *param);

//�豸����ʱ���Ŵű�����ϢҪ���
void mdooralarm_reset_info();


#endif
