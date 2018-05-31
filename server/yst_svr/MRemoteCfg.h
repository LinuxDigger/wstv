
/*	MRemoteCfg.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯����PC�������Լ�������Ϣ����غ���
	������ʷ���svn�汾����־
*/

#ifndef __MREMOTECFG_H__
#define __MREMOTECFG_H__
#include "jv_common.h"

#define	RC_PORT				6666
#define RC_ITEM_NUM			MAX_STREAM*64		//ÿ��ͨ�����64������
#define RC_ITEM_LEN			64					//���������󳤶�64�ֽ�
#define RC_DATA_SIZE		RC_ITEM_NUM * RC_ITEM_LEN + 128*1024

//Զ�̿���ָ��
#define RC_DISCOVER			0x01
#define RC_GETPARAM			0x02
#define RC_SETPARAM			0x03
#define RC_VERITY			0x04
#define RC_LOADDLG			0x05
#define RC_EXTEND			0x06
#define RC_USERLIST			0x07
#define RC_PRODUCTREG		0X08
#define RC_GETSYSTIME		0x09
#define RC_SETSYSTIME		0x0a
#define RC_DEVRESTORE		0x0b
#define RC_SETPARAMOK		0x0c
#define RC_DVRBUSY			0X0d
#define RC_GETDEVLOG		0x0e
#define RC_DISCOVER_CSST	0x0f
#define RC_WEB_PROXY		0x0f//����WEBҳ��
//#define RC_JSPARAM			0x10	//json��ʽ������ģʽ
#define RC_GPIN_ADD 		0x10
#define RC_GPIN_SET 		0x11
#define RC_GPIN_SELECT		0x12
#define RC_GPIN_DEL 		0x13
#define RC_BOARDEXT_CHECK	0x14
#define RC_GET_WIFI_CFG_TYPE	0x17
#define RC_GPIN_BIND_PTZPRE 0x18

typedef struct tagPACKET
{
	U32	nPacketType:5;			//�������ͣ�֧��32������
	U32	nPacketCount:8;			//�����������֧��256����
	U32	nPacketID:8;			//�����
	U32	nPacketLen:11;			//���ĳ��ȣ�������󳤶Ȳ�����2048�ֽڣ���(������2048��)
	U8	acData[RC_DATA_SIZE];
} PACKET, *PPACKET; 

//�˽ṹ��ΪPACKET�ṹ���EXTEND�ṹ��ĺϲ�,lck20121105
typedef struct tagPACKETEX
{
	//��packet�ṹ���ǰ���ֽ�
	int	nType:5;
	int	nCount:8;
	int	nID:8;
	int	nLen:11;

	//extend
	int	nTypeEx;
	int	nParam1;
	int	nParam2;
	int	nParam3;
	char acData[RC_DATA_SIZE];
} PACKETEX, *PPACKETEX;

//���ֻ�ͨ��ʹ�õ����ݰ�,lck20121107
#define PACKET_O_SIZE			1024
#define PACKET_O_STARTCODE		0xFFFFFFFF
typedef struct
{
	int nStartCode;		//�̶�ΪPACKET_O_STARTCODE
	int nDataType;		//����
	int reserved;		//�����ֶ�
	int nLength;		//data�ַ�������
	char acData[PACKET_O_SIZE];
}PACKET_O;

//nDataType����:
#define RC_GET_MOPIC	0x01		//���ֻ����ͻ������� data:"Enabled=0;nPictureType=0"
#define RC_SET_MOPIC	0x02		//�ֻ��������ػ������� data:"nPictureType=0"

//����:
typedef enum
{
	PIC_QUALITY_HIGH = 0,	//��
	PIC_QUALITY_MID,			//��
	PIC_QUALITY_LOW,		  //��
}E_PicQuality;


//Զ���������õĽṹ
typedef struct tagREMOTECFG
{
	U32			nSockID;
	BOOL		bRunning;
	BOOL		bInThread;
	pthread_mutex_t mutex;	//���ڷֿ����õĻ�����

	//Զ������״̬
	BOOL		bSetting;
	S32			nClientID;
	U32			nCmd;
	S32			nCh;
	U32			nSize;

	//Զ����������
	PACKET		stPacket;
}REMOTECFG;

// ����REMOTECFG������ֽ�����nSizeΪstPacket���������ֽ���
// ������������32�ֽ�
#define REMOTE_CFG_SIZE(nSize)		(sizeof(REMOTECFG) - sizeof(PACKET) + (nSize) + 32)


extern REMOTECFG stRemoteCfg;

//dll�����Լ��궨��
enum
{
	IPCAM_MAIN		=0,
	IPCAM_SYSTEM	=1,
	IPCAM_STREAM	=2,
	IPCAM_STORAGE	=3,
	IPCAM_ACCOUNT	=4,
	IPCAM_NETWORK	=5,
	IPCAM_LOGINERR	=6,
	IPCAM_PTZ		=7,
	IPCAM_IVP		=8,
	IPCAM_ALARM    = 9,
	IPCAM_MAX
};

#include <mrecord.h>

void remotecfg_send_self_data(int channelid,char *data,int nSize);

//֪ͨ�ֿعرջ�򿪱���
void remotecfg_alarm_on(int channelid, BOOL bOn, alarm_type_e type);

/**
 *@brief ��ʼ��
 *
 */
int remotecfg_init(void);

/**
 *@brief ����
 *
 */
int remotecfg_deinit(void);

void remote_start_chat(int channelid, int clientid);

//channelidֵΪ-1ʱ��������STOP��Ϣ��ȥ����Ϊ�Ǵ��������ӶϿ�
void remote_stop_chat(int channelid, int clientid);

//���ͶԽ�����
void remote_send_chatdata(int channelid, char *buffer, int len);

/**
 *@brief ������Ϣ
 *@param Ҫ���͵���Ϣ��ָ��
 *@note cfgָ����ڴ棬�Ƕ�̬����ģ��������Ҫ�ͷ�
 *
 */
int remotecfg_sendmsg(REMOTECFG *cfg);

VOID SetDVRParam(REMOTECFG *cfg, char *pData);

/**
 *@brief ����param��ָ��Զ����������
 *@param Ҫ���������ָ��
 *@note paramָ����ڴ棬�Ƕ�̬����ģ��������Ҫ�ͷ�
 *
 */
void* remotecfg_proc(void *param);

int remotecfg_sendmsg(REMOTECFG *cfg);

void remotecfg_alarmin_callback(int channelid, BOOL bAlarmOn);

void remotecfg_mdetect_callback(int channelid, BOOL bAlarmOn);

void remotecfg_mbabycry_callback(int channelid, BOOL bAlarmOn);

void remotecfg_mpir_callback(int channelid, BOOL bAlarmOn);

void remotecfg_mivp_callback(int channelid, BOOL bAlarmOn);

void remotecfg_mivp_vr_callback(int channelid, BOOL bAlarmOn);

void remotecfg_mivp_hide_callback(int channelid, BOOL bAlarmOn);

void remotecfg_mivp_left_callback(int channelid, BOOL bAlarmOn);

void remotecfg_mivp_removed_callback(int channelid, BOOL bAlarmOn);

void DoorAlarmInsert(signed char result, signed char index);
void DoorAlarmSend(unsigned char* name, int arg);
void DoorAlarmStop();

#endif

