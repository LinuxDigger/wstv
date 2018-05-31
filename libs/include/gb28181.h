/*
 * gb28181.h
 *
 *  Created on: 2013-10-30
 *      Author: lfx
 */

#ifndef GB28181_H_
#define GB28181_H_

#ifndef MAX_CHN_CNT
	#define MAX_CHN_CNT 32
#endif

#ifndef MAX_ALARMIN_CNT
	#define MAX_ALARMIN_CNT 16
#endif

#define MAX_ID_LEN	21

typedef struct{
	int bEnable;	//�Ƿ�ʹ��
	char serverip[16];
	char localip[16];
	unsigned short serverport;
	unsigned short localport;

	char devid[MAX_ID_LEN]; //dev id. 34020000001320000001
	char devpasswd[32]; //12345678
	int expires;	//ע����Ϣ����ʱ�䣬��λΪ��
	int keepalive;	//��������
	int expires_refresh;
	int keepalive_outtimes;

	int chnCnt;	//ͨ������
	char chnID[MAX_CHN_CNT][MAX_ID_LEN];
	int alarminCnt;
	char alarminID[MAX_ALARMIN_CNT][MAX_ID_LEN];
}GBRegInfo_t;

int gb_get_default_param(GBRegInfo_t *info);

int gb_init(GBRegInfo_t *info);

int gb_deinit();

/**
 * ��ز��������仯���ڲ�������Ҫ����
 */
int gb_reset_param(GBRegInfo_t *info);

int gb_send_data_i(int channel, unsigned char *buffer, int len, int framerate, int width, int height, int bitrate, unsigned long long timestamp);

int gb_send_data_p(int channel, unsigned char *buffer, int len, unsigned long long timestamp);

int gb_send_data_a(int channel, unsigned char *buffer, int len, int btalk, unsigned long long timestamp);

/**
 *@brief ���ͱ�����Ϣ
 *
 *@param channel ͨ���š���methodΪ2ʱ����ʾalarmin��
 *@param priority 1Ϊһ�����飬2Ϊ�������飬3Ϊ�������飬4Ϊ�ļ�����
 *@param method 1Ϊ�绰������2Ϊ�豸������3Ϊ���ű�����4ΪGPS������5Ϊ��Ƶ������6Ϊ�豸���ϱ�����7����������
 */
int gb_send_alarm(int channel, int priority, int method);

#endif /* GB28181_H_ */
