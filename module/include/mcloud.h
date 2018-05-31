#ifndef __MCLOUD_H__
#define __MCLOUD_H__

#include "jv_common.h"
#include "malarmout.h"

typedef enum
{
	CLOUD_ON,
	CLOUD_RUNNING,
	CLOUD_OFF,
	CLOUD_BUTT
}CloudStatus_e;

typedef struct
{
	BOOL bEnable;			//�Ƿ����ƴ洢
	char acID[64];			//�Ʒ����û���
	char acPwd[64];			//�Ʒ�������
	char host[64];			//�ƴ洢������
	char bucket[32];		//�ƿռ�
	int  type;				//������
	char expireTime[20];	//����ʱ��
	long deadline;			//����ʱ��
}CLOUD;

typedef struct
{
	char bucketName[32];
	char endpoint[64];
	char deviceGuid[16];
	char keyId[64];
	char expiration[64];
	char keySecret[128];
	char token[1024];
	char days[16];
	char type[8];
}OBSS_INFO;

extern OBSS_INFO obss_info;

/**
 *@brief ��ʼ��
 *
 *@return 0
 */
int mcloud_init(void);

/**
 *@brief ����
 *
 *@return 0
 */
int mcloud_deinit(void);

/**
 *@brief ��ȡ����
 *@param cloud ������ԵĽṹ��
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mcloud_get_param(CLOUD *cloud);

/**
 *@brief ���ò���
 *@param cloud Ҫ���õ����Խṹ��
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mcloud_set_param(CLOUD *cloud);

/**
 *@brief ����ƴ洢�����Ƿ����
 *@param 
 *@return TRUE ���ã�FALSE ����
 *
 */
BOOL mcloud_check_enable();

int mcloud_parse_obssstate(char *data);

void mcloud_upload_alarm_file(JV_ALARM *jvAlarm);


#endif  //end of __MCLOUD_H__

