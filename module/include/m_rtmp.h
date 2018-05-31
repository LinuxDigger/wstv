/******************************************************************************

  Copyright (c) 2015 Jovision Technology Co., Ltd. All rights reserved.

  File Name     : m_rtmp.h
  Version       : 
  Author        : Wang Tao
  Created       : 2015-10-27

  Description   : RTMP portings for Net Alarm, call procedure: 
  Rtmp_SetURL()->Rtmp_Start()->Rtmp_SetMetadata()->Rtmp_SendData()->Rtmp_Stop()

  History       : 
  1.Date        : 2015-10-27
    Author      : Wang Tao
    Modification: Created file
******************************************************************************/
#ifndef _M_RTMP_H
#define _M_RTMP_H

#include "jv_common.h"
#include "JvMediaClient.h"

#define NET_ALARM_MAX_PLAY		3 //Զ�̻ط�·��


#define RTMP_INVALID_HDL		(-1)
#define RTMP_LIVE_HDL(ch, st)	(((ch) * MAX_STREAM) + (st))
#define RTMP_PLAY_HDL(n)		((MAX_STREAM) + (n))


typedef	S32						RTMP_HDL;

// RTMP����״̬
typedef enum
{
	RTMP_DISCONNECTED,			// δ����
	RTMP_CONNECTING,			// ������(�ȴ��ص�)
	RTMP_CONNECTED,				// ������
	RTMP_WAITDISCONNECT			// �ȴ��Ͽ�(�����в��ܶϿ�����Ҫ�����ӳɹ����ٶϿ�)
}RTMP_STATE_e;

typedef enum
{
	RTMP_EVENT_GETMETA,			// ��ȡMetaData
	RTMP_EVENT_DISCON			// ��ý��Ͽ�
}RTMP_EVENT_e;

typedef	void					(*RtmpCallback)(RTMP_EVENT_e eEvent, void* pParam, void* arg);


S32 Rtmp_Init();

S32 Rtmp_Deinit();

// ������ý���URL
S32 Rtmp_SetURL(RTMP_HDL nHandle, const S8 *pURL);


// ��ʼ��ý������
S32 Rtmp_Start(S32 nHandle);

S32 Rtmp_RegCallback(S32 nHandle, RtmpCallback pReqMetaData, void* arg);

/*����RTMP������Ϣ*/
S32 Rtmp_UpdateMetadata(S32 nHandle);

S32 Rtmp_SendMetaData(RTMP_HDL nHandle, JMC_Metadata_t* pData);

S32 Rtmp_ConvDataType(S32 nType, U16 AudioCodec);

/*����RTMP��������*/
S32 Rtmp_SendData(RTMP_HDL nHandle, S32 nType, S8 *pData, S32 nSize, S32 nPts, S32 nDts);

S32 Rtmp_GetStatus(RTMP_HDL nHandle);

S32 Rtmp_Stop(RTMP_HDL nHandle);

#endif /* _M_RTMP_H */

