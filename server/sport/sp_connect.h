/*
 * sp_connect.h
 *
 *  ���ļ��������ӹ���
 *  �������˶������ӣ�����ʲô���͵ģ����ӵľ�����Ϣ��
 *  ��ͬģ�飬�������ӽ���ʱ������#sp_connect_add ��ӡ�������ʱ��ʹ�����е�KEYɾ��֮
 *
 *  Created on: 2013-11-19
 *      Author: lfx
 */

#ifndef SP_CONNECT_H_
#define SP_CONNECT_H_

typedef enum{
	SP_CON_ALL,
	SP_CON_JOVISION,
	SP_CON_RTSP,
	SP_CON_GB28181,
	SP_CON_PSIA,
	SP_CON_OTHER,
}SPConType_e;

typedef struct{

	//conType �� key��Ψһȷ��һ������
	SPConType_e conType;
	unsigned int key;

	char protocol[32];	//such as yst, onvif, 28181 and so on

	char addr[16];	//�����ߵ�IP��ַ
	char user[32];	//�����ߵĵ�½�ʻ�

	unsigned char param[128];	//�û�����
}SPConnection_t;

typedef struct{
	BOOL RTMP_EN; 				//rtmp ���ܿ���
	char url[64];				//rtmp url
}SPConnectionRtmp_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief ����һ������
 */
int sp_connect_add(SPConnection_t *connection);

/**
 *@�ؼ�ֵΪprotocol��key������ֵû��ϵ
 */
int sp_connect_del(SPConnection_t *connection);

/**
 *@�ؼ�ֵΪconType��key������ֵû��ϵ
 */
int sp_connect_breakoff(SPConnection_t *connection);

/**
 *@brief ��ȡ������
 */
int sp_connect_get_cnt(SPConType_e conType);

/**
 *@brief ����ָ�룬ָ��list�Ŀ�ͷ
 */
int sp_connect_seek_set();

SPConnection_t *sp_connect_get_next(SPConType_e conType);

/**
 * @brief ����rtmp
 * param chn ͨ������
 * param url rtmp�ṹ�壬�����Ƿ������Լ���������ַ
 */
int sp_connect_set_rtmp(int chn,SPConnectionRtmp_t *attr);

/**
 * @brief �ж��Ƿ�֧��ȫ��ͨЭ��
 * param void
 */
int sp_connect_extsdk_support(void);
#if 1
int sp_connect_init_semaphore();
int sp_connect_send_semaphore();
int sp_connect_recv_semaphore(int timeout);
int sp_connect_uninit_semaphore();

#endif

#ifdef __cplusplus
}
#endif


#endif /* SP_CONNECT_H_ */
