/****************************************************************************
�ļ�����    ��������ģ��: ����ָ���ļ�������״̬�����Ƶ���ݵ�ͨ�ò�����
��������    ��2015-11-11
������      ������
�޸ļ�¼	��
    1. 2015-11-11:����
*****************************************************************************/

#ifndef _M_COM_PLAYER_H
#define _M_COM_PLAYER_H

#include "jv_common.h"

#if (SD_RECORD_SUPPORT)

#define	MAX_FILENAME_LEN		(256)


typedef void*	PLAYER_HANDLE;

typedef enum
{
	CPLAYER_CMD_PLAY,		// ����
	CPLAYER_CMD_PAUSE,		// ��ͣ
	CPLAYER_CMD_RESUME,		// ����
	CPLAYER_CMD_SEEK,		// ��λ
	CPLAYER_CMD_FAST,		// ���
	CPLAYER_CMD_SLOW,		// ����
	CPLAYER_CMD_SPEED,		// ָ���ٶȲ���
	CPLAYER_CMD_MUTE,		// ����
	CPLAYER_CMD_UNMUTE,		// ȡ������
	CPLAYER_CMD_MAX
}CPLAYER_CMD_e;

typedef enum
{
	CPLAYER_TYPE_I,
	CPLAYER_TYPE_B,
	CPLAYER_TYPE_P,
	CPLAYER_TYPE_ALAW,
	CPLAYER_TYPE_ULAW,
	CPLAYER_TYPE_AAC,
	CPLAYER_TYPE_MAX
}CPLAYER_TYPE_e;

typedef enum
{
	CPLAYER_STATE_NOPLAY,	// �ǻط�״̬
	CPLAYER_STATE_PLAY,		// ����
	CPLAYER_STATE_PAUSE,	// ��ͣ
	CPLAYER_STATE_STEP,		// ��֡
	CPLAYER_STATE_SPEED,	// ���������
	CPLAYER_STATE_MAX
}CPlayState_e;

typedef enum
{
	CPLAYER_EVENT_KEEP,		// �������ӣ�Զ�̻ط�ʱ��ֹ�Ͽ�����
	CPLAYER_EVENT_END_OK,	// �����������
	CPLAYER_EVENT_END_ERR,	// �쳣�������
	CPLAYER_EVENT_I_FRAME,	// ����I֡
	CPLAYER_EVENT_PROGRESS,	// ���ȸ����¼�
	CPLAYER_EVENT_WAITPLAY,	// �ȴ���ʼ����
	CPLAYER_EVENT_MAX
}CPlayEvent_e;

typedef struct
{
	U32				msTime;						// ¼����ʱ��(ms)
	// ��Ƶ��Ϣ
	VIDEO_TYPE_E	nVDataType; 				// ��Ƶ�����ʽ
	S32 			nVTotalFrame;				// ��֡��
	S32 			nVWidth;					// ���
	S32 			nVHeight;					// �߶�
	S32 			nVFrameRateNum; 			// ֡�ʷ���
	S32 			nVFrameRateDen; 			// ֡�ʷ�ĸ
	// ��Ƶ��Ϣ
	AUDIO_TYPE_E	nADataType; 				// ��Ƶ���ݸ�ʽ
	S32 			nASampleRate;				// ������
	S32 			nASampleBits;				// ����λ��
	S32 			nAChannels; 				// ������
}VideoFileInfo_t;

typedef struct
{
	S8				fname[MAX_FILENAME_LEN];
	CPlayState_e	PlayState;
	float			PlaySpeed;
	U32				nTotalFrame;
	U32				nCurFrame;
	BOOL			bMute;
}CPlayerStatus_t;

// �ļ���Ϣ
typedef S32	(*FunSendMetaData)(const PLAYER_HANDLE Handle, const VideoFileInfo_t* pMetaData, void* arg);

// ��Ƶ/��Ƶ��
typedef S32 (*FunSendData)(const PLAYER_HANDLE Handle, CPLAYER_TYPE_e nType, const void* pData, S32 nSize, S32 nSeq, S32 nPts, S32 nDts, void* arg);

// �¼��ص���������ɵ��¼�
typedef void (*FunEventCallback)(const PLAYER_HANDLE Handle, CPlayEvent_e nEvent, S32 Param1, S32 Param2, void* arg);


// ����������
// fname			:Ҫ���ŵ��ļ�����NULLΪֻ�������������������ļ�
// pSendMetaData	:�ص�����������MetaData
// pSendData		:�ص���������������
// pSendData		:�ص��������¼�
// arg				:�ص�ʱ�ش�
// return			:��NULL �ɹ���NULL ʧ��
PLAYER_HANDLE ComPlayer_Create(const char *fname, FunSendMetaData pSendMetaData, FunSendData pSendData, FunEventCallback pEventFunc, void* arg);

// �ر�Զ�̻طŲ�����
// Handle			:������Handle
// return			:0�ɹ�
S32 ComPlayer_Destroy(PLAYER_HANDLE Handle, BOOL bBlock);

// ��ȡ�ļ���Ϣ
// Handle			:������Handle
// pFileInfo		:�ļ���Ϣ
// return			:0�ɹ�
S32 ComPlayer_GetFileInfo(PLAYER_HANDLE Handle, VideoFileInfo_t* pFileInfo);

// �趨/��������ļ�
// Handle			:������Handle
// fname			:�ļ���(����·��)
// bSendMetaData	:�Ƿ���Ҫ����MetaData
// return			:0�ɹ�
S32 ComPlayer_ChgPlayFile(PLAYER_HANDLE Handle, const char* fname, BOOL bSendMetaData);

// �������״̬
// Handle			:������Handle
// cmd				:����ָ��
// Param			:������Ŀǰ��������λ֡�źͲ����ٶȵȼ�
// return			:�ɹ�����0��ʧ�ܷ��ظ�ֵ
S32 ComPlayer_ChgPlayState(PLAYER_HANDLE Handle, CPLAYER_CMD_e cmd, void* Param);

// ��ȡ����״̬
// Handle			:������Handle
// pState			:����״̬
// return			:�ɹ�����0��ʧ�ܷ��ظ�ֵ
S32 ComPlayer_GetPlayStatus(PLAYER_HANDLE Handle, CPlayerStatus_t* pState);

#endif

#endif

