
/*	mtransmit.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯������غ���
	������ʷ���svn�汾����־
*/


#include "MRemoteCfg.h"
#include "maccount.h"

#ifndef __MTRANSMIT_H__
#define __MTRANSMIT_H__

//����������ģʽlk20140222
typedef enum{
	LAN_MODEL_DEFAULT,  		//����������ģʽ�������������٣�����ʱ
	LAN_MODEL_MAXNUM,			//��������������ģʽ��Ŀǰ�����������100��������ʱ
}LANModel_e;

/*�����������*/
typedef struct _PNETLINKINFO
{
	U32		nChannel;
	U32		nClientID;
	char	chClientIP[16];
	U32		nClientPort;
	U32		nType;
	BOOL	bChatting;
	ACCOUNT	stNetUser;
	struct _PNETLINKINFO *next;
} NETLINKINFO, *PNETLINKINFO;

typedef struct tagYST
{
	S8		strGroup[8];
	U32		nID;
	U32		nPort;
	U32		nStatus;
	U32		nYSTPeriod;

	BOOL	bTransmit[MAX_STREAM];

	LANModel_e	eLANModel;//����������ģʽlk20140222

	pthread_mutex_t mutex;	//���ڷֿ����ӵĻ�����
}YST, *PYST;


/*********************************************************************************************/

NETLINKINFO *__NETLINK_Get(U32 nClientID, NETLINKINFO *info);
NETLINKINFO *__NETLINK_GetByIndex(int clientIndex, NETLINKINFO *info);//������ѯ��������NULLʱ��ʾ�Ѿ�û��.clientIndex��0��ʼ



ACCOUNT *GetClientAccount(U32 nClientID, ACCOUNT *account);

//�ص���nClientID֮����������ӡ�
//��nClientIDΪ-1ʱ���ر���������
void mtransmit_disconnect_all_but(U32 nClientID);

VOID InitYSTID();
S32 InitYST();
S32 ReleaseYST();
S32 StartYSTCh(U32 nCh, BOOL bLanOnly, U32 nBufSize);
VOID StopYSTCh(U32 nCh);
void YstOnline();

BOOL StartMOServer(U32 nCh);
BOOL StopMOServer(U32 nCh);

VOID Transmit(S32 nChannel, VOID *pData, U32 nSize, U32 nType, unsigned long long timestamp);

VOID* SendInfo2Client(VOID *pArgs);

//��ȡ����ͨ����
YST* GetYSTParam(YST* pstYST);
//��������ͨ����
VOID SetYSTParam(YST* pstYST);

//������Եķ�ʽ���������ݡ�����ʧ�ܵ����
void MT_TRY_SendChatData(int nChannel,int nClientID,unsigned char uchType,unsigned char *pBuffer,int nSize);
//lck20121025
//NVR��IPC���й㲥ͨ��
//ָ�����õ��˾����һ�����ṹ��Ҳ���õ��˾����׷��
//����Ƕ��ʽNVRʱ�����ۺϿ���һ�£�����JSON��ʽ
typedef enum
{
	BC_SEARCH = 0x01,
	BC_GET,
	BC_SET,
	BC_NOPOWER,
	BC_GET_ERRORCODE,
	BC_SET_WIFI,
	BC_MAX,
}BC_CMD_E;

typedef struct tagBCPACKET
{
	U32		nCmd;
	S8		acGroup[8];
	U32		nYSTNO;
	U32		nPort;
	S8		acName[100];
	U32		nType;
	U32		nChannel;

	//��֤�õĹ���Ա�ʻ�
	char	acID[32];
	char	acPW[32];

	BOOL	bDHCP;
	U32		nlIP;
	U32		nlNM;
	U32		nlGW;
	U32		nlDNS;
	char	acMAC[20];

	U32		nErrorCode;
	int nPrivateSize;//�Զ�������ʵ�ʳ���
	char chPrivateInfo[500];//�Զ�����������
	char nickName[36];
}BCPACKET;

#endif

