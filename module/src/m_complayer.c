/*****************************************************************************
�ļ�����    ��������ģ��: ����ָ���ļ�������״̬������Ƶ���ݵ�ͨ�ò�����
��������    ��2015-11-11
������      ������
�޸ļ�¼	��
    1. 2015-11-11:����

   ��ģ��ϸ��˵����
   1��һ���̸߳���һ��Handle�Ļط�
   2��ÿ��Handle��Ӧһ��ComPlayer_t�ṹ���߳�Ҳ������������Handle
 
 *****************************************************************************/
#include <math.h>
#include "jv_common.h"
#include "utl_common.h"
#include "m_mp4.h"

#include "m_complayer.h" 

#if SD_RECORD_SUPPORT

typedef struct ComPlayer
{
	struct ComPlayer*	pNext;

	// �ص�����
	void*				arg;
	FunSendMetaData		pSendMetaData;
	FunSendData			pSendData;
	FunEventCallback	pEventFunc;

	// �ļ���Ϣ
	S8					fname[MAX_FILENAME_LEN];
	BOOL				bLoadedInfo;
	MP4_READ_INFO		Mp4Info;
	void*				fp;

	// �����߳�
	pthread_t			ThreadId;
	BOOL				bRunning;

	// ����״̬
	CPlayState_e		PlayStatus;
	CPlayState_e		oldPlayStatus;
	float				PlaySpeed;
	pthread_mutex_t		sMutexFile;
	BOOL				bSeekFlag;
	BOOL				bMute;
	BOOL				bNeedRefresh;
	U32					nVFrameDecoded;
	U32					nAFrameDecoded;
}ComPlayer_t;


//��ǰ�������ݵ�ʱ�䣬��λms���Ǹ�����ֵ���͸����²��ԣ�678������;�߸�����,Ӧ����ֵԽ�����Խ�á�
#define CPLAYER_MS_AHEAD 		(8)
#define CPLAYER_MS_PROGRESS		(500)
#define CPLAYER_MS_TIMEOUT		(2000)			// ��ʱʱ������ֹԶ�̻ط�ʱ�Ͽ�����

#define FILE_FLAG_MP4			".mp4"

#define	pr_info(x...)			do{\
									printf("%s ", __FUNCTION__);\
									printf(x);\
								}while(0)

#define CPLAYER_CHECK_HDL(h)	do{\
									if (NULL == h)\
									{\
										pr_info("NULL handle!\n");\
										return RET_BADPARAM;\
									}\
								}while(0)

// ����ͷ���
static ComPlayer_t* 	s_Player	= NULL;


// �ڲ���������
static void		complayer_Add(ComPlayer_t* pPlayer);
static void		complayer_Del(ComPlayer_t* pPlayer);
static void		complayer_mp4(void* Param);
static void*	complayer_OpenReadMp4(ComPlayer_t* pPlayer);
static S32		complayer_FindIFrame(PLAYER_HANDLE Handle, U32 nCurFrame, BOOL bForward, BOOL bFindOppo);


// ����������
// fname			:Ҫ���ŵ��ļ�����NULLΪֻ�������������������ļ�
// pSendMetaData	:�ص�����������MetaData
// pSendData		:�ص���������������
// pSendData		:�ص��������¼�
// arg				:�ص�ʱ�ش�
// return			:��NULL �ɹ���NULL ʧ��
// *ע�⣺����֮�󲻻��������ţ���Ҫ����ComPlayer_ChgPlayState(Play)�ŻῪʼ����!!!
PLAYER_HANDLE ComPlayer_Create(const char *fname, FunSendMetaData pSendMetaData, FunSendData pSendData, FunEventCallback pEventFunc, void* arg)
{
	S32 ret = RET_SUCC;
	ComPlayer_t* pPlayer = NULL;
	// VideoFileInfo_t VideoFileInfo;

	if ((NULL == pSendMetaData) || (NULL == pSendData))
	{
		pr_info("Must have callback function!\n");
		return NULL;
	}

	pPlayer = (ComPlayer_t *)calloc(1, sizeof(ComPlayer_t));
	if (pPlayer == NULL)
	{
		pr_info("No momery!\n");
		return NULL;
	}

	pthread_mutex_init(&pPlayer->sMutexFile, NULL);

	pPlayer->pSendMetaData = pSendMetaData;
	pPlayer->pSendData = pSendData;
	pPlayer->pEventFunc = pEventFunc;
	pPlayer->arg = arg;

	if (fname != NULL)
	{
		if (ComPlayer_ChgPlayFile(pPlayer, fname, FALSE) != RET_SUCC)
		{
			pr_info("Open play file %s failed!\n", fname);
			free(pPlayer);
			return NULL;
		}
	}

	pPlayer->bRunning = TRUE;
	pPlayer->oldPlayStatus = CPLAYER_STATE_NOPLAY;	// �ǻط�״̬
	pPlayer->PlayStatus = CPLAYER_STATE_NOPLAY;		// �ǻط�״̬

	// �����߳�
	ret = pthread_create_normal(&pPlayer->ThreadId, NULL, (void *)complayer_mp4, (void *)pPlayer);

	if (ret != 0)
	{
		pr_info("Create thread Failed: %d,  %d,%d,%d\n", ret, EAGAIN, EINVAL, EPERM);
		free(pPlayer);
		return NULL;
	}

	complayer_Add(pPlayer);

	return pPlayer;
}

// �ر�Զ�̻طŲ�����
// Handle			:������Handle
// return			:0�ɹ�
S32 ComPlayer_Destroy(PLAYER_HANDLE Handle, BOOL bBlock)
{
	ComPlayer_t* pPlayer = (ComPlayer_t*)Handle;

	CPLAYER_CHECK_HDL(Handle);

	if (pPlayer->bRunning)
	{
		pPlayer->bRunning = FALSE;
		pr_info("Destroy player, Thread Id: %d\n", (S32)pPlayer->ThreadId);
		if (bBlock)
		{
			pthread_join(pPlayer->ThreadId, NULL);
		}
		else
		{
			pthread_detach(pPlayer->ThreadId);
		}
	}

	return RET_SUCC;	
}

// ��ȡ�ļ���Ϣ
// Handle			:������Handle
// pFileInfo		:�ļ���Ϣ
// return			:0�ɹ�
S32 ComPlayer_GetFileInfo(PLAYER_HANDLE Handle, VideoFileInfo_t* pFileInfo)
{
	ComPlayer_t* pPlayer = (ComPlayer_t*)Handle;

	CPLAYER_CHECK_HDL(Handle);

	if (NULL == pFileInfo)
	{
		return RET_BADPARAM;
	}

	memset(pFileInfo, 0, sizeof(VideoFileInfo_t));

	if (!pPlayer->bLoadedInfo)
	{
		pr_info("File info %s still not loaded!\n", pPlayer->fname);
		return RET_ERR;
	}

	pr_info("w: %d, h: %d, fr: %f!\n", pPlayer->Mp4Info.iFrameWidth, pPlayer->Mp4Info.iFrameHeight, pPlayer->Mp4Info.dFrameRate);

	// ������뼶¼����ʱ��������������
	pFileInfo->msTime			= pPlayer->Mp4Info.iNumVideoSamples / pPlayer->Mp4Info.dFrameRate * 1000 + 0.5;

	// ��Ƶ��Ϣ
	pFileInfo->nVDataType		= pPlayer->Mp4Info.enMp4VideoType;
	pFileInfo->nVTotalFrame		= pPlayer->Mp4Info.iNumVideoSamples;
	pFileInfo->nVWidth			= pPlayer->Mp4Info.iFrameWidth;
	pFileInfo->nVHeight			= pPlayer->Mp4Info.iFrameHeight;
	pFileInfo->nVFrameRateNum	= (U32)(pPlayer->Mp4Info.dFrameRate * 1000);
	pFileInfo->nVFrameRateDen	= 1000;

	// ��Ƶ��Ϣ
	pFileInfo->nADataType		= pPlayer->Mp4Info.nMp4AudioType;
	pFileInfo->nASampleRate		= 8000;
	pFileInfo->nASampleBits		= 16;
	pFileInfo->nAChannels		= 1;

	return RET_SUCC;
}

// �趨/��������ļ�
// Handle			:������Handle
// fname			:�ļ���(����·��)
// return			:0�ɹ�
S32 ComPlayer_ChgPlayFile(PLAYER_HANDLE Handle, const char* fname, BOOL bSendMetaData)
{
	ComPlayer_t* pPlayer = (ComPlayer_t*)Handle;
	VideoFileInfo_t VideoFileInfo;
	BOOL bFileChanged = FALSE;			// �ж��Ƿ���Ҫ������Ƶ��Ϣ

	CPLAYER_CHECK_HDL(Handle);

	pr_info("Filename: %s\n", fname);
	if (!fname)
	{
		pr_info("Bad param, fname: %s!\n", fname);
		return RET_BADPARAM;
	}

	//ֻ�ط�mp4�ļ�
	if (strcmp(&fname[strlen(fname) - strlen(FILE_FLAG_MP4)], FILE_FLAG_MP4) != 0)
	{
		pr_info("Only support mp4 file!\n");
		return RET_BADPARAM;
	}
	
	if (access(fname, F_OK) != 0)
	{
		pr_info("File %s not exist!\n", fname);
		return RET_BADPARAM;
	}

	pthread_mutex_lock(&pPlayer->sMutexFile);

	// �����һ�ļ���Ϣ
	if (strlen(pPlayer->fname) > 0)
	{
		MP4_Close_Read(pPlayer->fp, &pPlayer->Mp4Info);	
		pPlayer->fp = NULL;
		pPlayer->bLoadedInfo = FALSE;
		bFileChanged = TRUE;			// ֮ǰ�Ѿ����ļ��ڲ��ţ�����Ϊ�ļ����
	}

	strncpy(pPlayer->fname, fname, MAX_FILENAME_LEN);

	if (NULL == complayer_OpenReadMp4(pPlayer))
	{
		pr_info("Read file info err\n");
		pthread_mutex_unlock(&pPlayer->sMutexFile);
		return RET_ERR;
	}

	// �Ѽ����ļ���Ϣ
	pPlayer->bLoadedInfo = TRUE;
	pPlayer->nAFrameDecoded = 0;
	pPlayer->nVFrameDecoded = 0;
	if (pPlayer->PlayStatus != CPLAYER_STATE_NOPLAY)
	{
		pPlayer->oldPlayStatus = pPlayer->PlayStatus;
	}
	pPlayer->PlayStatus = CPLAYER_STATE_NOPLAY;

	pthread_mutex_unlock(&pPlayer->sMutexFile);

	if (bSendMetaData && bFileChanged)
	{
		memset(&VideoFileInfo, 0, sizeof(VideoFileInfo));
		// ��ȡ�ļ���Ϣ
		ComPlayer_GetFileInfo(pPlayer, &VideoFileInfo);

		if (pPlayer->pSendMetaData != NULL)
		{
			pPlayer->pSendMetaData(pPlayer, &VideoFileInfo, pPlayer->arg);
		}
	}

	return RET_SUCC;
}

// �������״̬
// Handle			:������Handle
// cmd				:����ָ��
// Param			:������Ŀǰ��������λ֡�źͲ����ٶȵȼ�
// return			:�ɹ�����0��ʧ�ܷ��ظ�ֵ
S32 ComPlayer_ChgPlayState(PLAYER_HANDLE Handle, CPLAYER_CMD_e cmd, void* Param)
{
	ComPlayer_t* pPlayer = (ComPlayer_t*)Handle;
	S32 *pParam = (S32 *)Param;
	S32 nReqSeekFrame = (pParam != NULL) ? pParam[0] : -1;
	S32 nReqSpeedRank = (pParam != NULL) ? pParam[1] : -10;		// -4 ~ 4Ϊ��Чֵ
	S32 nSeekIFrame = -1;

	CPLAYER_CHECK_HDL(Handle);

	pthread_mutex_lock(&pPlayer->sMutexFile);

	switch (cmd)
	{
	case CPLAYER_CMD_PLAY:
		pPlayer->PlayStatus = CPLAYER_STATE_PLAY;
		pPlayer->PlaySpeed = 1.0;
		pPlayer->bNeedRefresh = TRUE;
		if (nReqSeekFrame > 0)
		{
			nSeekIFrame = complayer_FindIFrame(Handle, nReqSeekFrame, TRUE, TRUE);
			if (nSeekIFrame > 0)
			{
				pPlayer->nVFrameDecoded = nSeekIFrame - 1;
			}
			else
			{
				pthread_mutex_unlock(&pPlayer->sMutexFile);
				return RET_OPERATION_REFUSED;
			}
		}
		break;
	case CPLAYER_CMD_PAUSE:
		if (pPlayer->PlayStatus != CPLAYER_STATE_PAUSE)
		{
			pPlayer->oldPlayStatus = pPlayer->PlayStatus;
			pPlayer->PlayStatus = CPLAYER_STATE_PAUSE;
		}
		break;
	case CPLAYER_CMD_RESUME:
		if (CPLAYER_STATE_PAUSE == pPlayer->PlayStatus)
		{
			pPlayer->PlayStatus = pPlayer->oldPlayStatus;
		}
		else if (CPLAYER_STATE_NOPLAY == pPlayer->PlayStatus)
		{
			if (pPlayer->oldPlayStatus == CPLAYER_STATE_NOPLAY)
			{
				pr_info("Player old status is no play, change to play!!!\n");
				pPlayer->oldPlayStatus = CPLAYER_STATE_PLAY;
			}
			pPlayer->PlayStatus = pPlayer->oldPlayStatus;
			pPlayer->bNeedRefresh = TRUE;
		}
		break;
	case CPLAYER_CMD_SEEK:
		pPlayer->oldPlayStatus = pPlayer->PlayStatus;
		pPlayer->PlayStatus = CPLAYER_STATE_STEP;
		pPlayer->bSeekFlag = TRUE;
		if (nReqSeekFrame >= 0)
		{
			nSeekIFrame = complayer_FindIFrame(Handle, nReqSeekFrame, TRUE, TRUE);
			if (nSeekIFrame > 0)
			{
				pPlayer->nVFrameDecoded = nSeekIFrame - 1;
				pPlayer->bNeedRefresh = TRUE;
			}
			else
			{
				pthread_mutex_unlock(&pPlayer->sMutexFile);
				return RET_OPERATION_REFUSED;
			}
		}
		break;
	case CPLAYER_CMD_FAST:
		pPlayer->PlayStatus = CPLAYER_STATE_SPEED;
		pPlayer->PlaySpeed *= 2;
		if (pPlayer->PlaySpeed  > 4.0)
		{
			pPlayer->PlaySpeed  = 4.0;
		}
		break;
	case CPLAYER_CMD_SLOW:
		pPlayer->PlayStatus = CPLAYER_STATE_SPEED;
		pPlayer->PlaySpeed /= 2;
		if (pPlayer->PlaySpeed  < 0.25)
		{
			pPlayer->PlaySpeed  = 0.25;
		}
		break;
	case CPLAYER_CMD_SPEED:
		pPlayer->PlayStatus = CPLAYER_STATE_SPEED;
		if ((nReqSpeedRank >= -4) && (nReqSpeedRank <= 4))
		{
			pPlayer->PlaySpeed = 1.0 * pow(2, nReqSpeedRank);
		}
		if (nReqSeekFrame >= 0)
		{
			nSeekIFrame = complayer_FindIFrame(Handle, nReqSeekFrame, TRUE, TRUE);
			if (nSeekIFrame > 0)
			{
				pPlayer->nVFrameDecoded = nSeekIFrame - 1;
				pPlayer->bNeedRefresh = TRUE;
			}
			else
			{
				pthread_mutex_unlock(&pPlayer->sMutexFile);
				return RET_OPERATION_REFUSED;
			}
		}
		break;
	case CPLAYER_CMD_MUTE:
		pPlayer->bMute = TRUE;
		break;
	case CPLAYER_CMD_UNMUTE:
		pPlayer->bMute = FALSE;
		break;
	default:
		break;
	}

	// ���/���ŵ�1.0����ʱ�Զ��л�����ͨ����״̬
	if (CPLAYER_STATE_SPEED == pPlayer->PlayStatus)
	{
		if (((pPlayer->PlaySpeed - 1.0) > -1e-6) && ((pPlayer->PlaySpeed - 1.0) < 1e-6))
		{
			pPlayer->PlaySpeed = 1.0;
			pPlayer->PlayStatus = CPLAYER_STATE_PLAY;
		}
	}

	// ͬ����Ƶ֡��
	//pPlayer->nAFrameDecoded = (pPlayer->nVFrameDecoded * 25) / pPlayer->Mp4Info.dFrameRate;
	pPlayer->nAFrameDecoded = MP4_GetSyncAudioFrame(pPlayer->fp, &pPlayer->Mp4Info, (pPlayer->nVFrameDecoded > 0) ? pPlayer->nVFrameDecoded : 1);
	if (pPlayer->nAFrameDecoded > 0)
	{
		--pPlayer->nAFrameDecoded;
	}

	pthread_mutex_unlock(&pPlayer->sMutexFile);

	return RET_SUCC;	
}

// ��ȡ����״̬
// Handle			:������Handle
// pState			:����״̬
// return			:�ɹ�����0��ʧ�ܷ��ظ�ֵ
S32 ComPlayer_GetPlayStatus(PLAYER_HANDLE Handle, CPlayerStatus_t* pState)
{
	ComPlayer_t* pPlayer = (ComPlayer_t*)Handle;

	CPLAYER_CHECK_HDL(Handle);
	if (NULL == pState)
	{
		pr_info("Invalid pState\n");
		return RET_BADPARAM;		
	}

#if 0
	switch (pPlayer->PlayStatus)
	{
	case EN_PLAYER_PLAY:		pState->PlayState = CPLAYER_STATE_PLAY;			break;
	case EN_PLAYER_PAUSE:		pState->PlayState = CPLAYER_STATE_PAUSE;		break;
	case EN_PLAYER_STEP:		pState->PlayState = CPLAYER_STATE_STEP;			break;
	case EN_PLAYER_SPEED:		pState->PlayState = CPLAYER_STATE_SPEED;		break;
	default:					pState->PlayState = CPLAYER_STATE_MAX;			break;
	}
#endif

	STRNCPY(pState->fname, pPlayer->fname, sizeof(pPlayer->fname));
	pState->PlaySpeed = pPlayer->PlaySpeed;
	pState->nTotalFrame = pPlayer->Mp4Info.iNumVideoSamples;
	pState->nCurFrame = pPlayer->nVFrameDecoded;
	pState->bMute = pPlayer->bMute;

	return RET_SUCC;
}

static void* complayer_OpenReadMp4(ComPlayer_t* pPlayer)
{
	//���ļ���Ϣ
	pPlayer->fp = MP4_Open_Read(pPlayer->fname, &pPlayer->Mp4Info);
	if (NULL == pPlayer->fp)
	{
		pr_info("Failed open file: %s, Info: %s\n", pPlayer->fname, strerror(errno));
		return NULL;
	}

	if((0 == pPlayer->Mp4Info.iFrameWidth) || (0 == pPlayer->Mp4Info.iFrameHeight)
		|| (0 == (U32)(pPlayer->Mp4Info.dFrameRate * 1000)))
	{
		MP4_Close_Read(pPlayer->fp, &pPlayer->Mp4Info);
		pr_info("File %s info error, Width: %d, Height: %d, FrameRate: %f\n", pPlayer->fname, 
				pPlayer->Mp4Info.iFrameWidth, pPlayer->Mp4Info.iFrameHeight, pPlayer->Mp4Info.dFrameRate);
		return NULL;
	}

	return pPlayer->fp;
}

static void complayer_Add(ComPlayer_t* pPlayer)
{
	ComPlayer_t* pItr = s_Player;

	if (NULL == s_Player)
	{
		s_Player = pPlayer;
		return;
	}

	while (pItr->pNext != NULL)
	{
		pItr = pItr->pNext;
	}

	pItr->pNext = pPlayer;
}

static void complayer_Del(ComPlayer_t* pPlayer)
{
	ComPlayer_t* pItr = s_Player;

	if (pPlayer == s_Player)
	{
		s_Player = s_Player->pNext;
		free(pPlayer);
		return;
	}

	while ((pItr != NULL) && (pItr->pNext != NULL))
	{
		if (pItr->pNext == pPlayer)
		{
			pItr->pNext = pPlayer->pNext;
			free(pPlayer);
		}
		pItr = pItr->pNext;
	}
}

static void complayer_SendVideoFrame(ComPlayer_t* pPlayer, AV_UNPKT* pvpack)
{
	pPlayer->nVFrameDecoded = pvpack->iSampleId;

	if ((pvpack->bKeyFrame) && (pPlayer->pEventFunc != NULL))
	{
		pthread_mutex_unlock(&pPlayer->sMutexFile);
		pPlayer->pEventFunc(pPlayer, CPLAYER_EVENT_I_FRAME, 
							pPlayer->Mp4Info.iNumVideoSamples, pPlayer->nVFrameDecoded, pPlayer->arg);
		pthread_mutex_lock(&pPlayer->sMutexFile);
	}

	if (pPlayer->bSeekFlag)
	{
		pPlayer->bSeekFlag = FALSE;
		if (pPlayer->PlayStatus != CPLAYER_STATE_PAUSE)
		{
			pPlayer->oldPlayStatus = pPlayer->PlayStatus;
			pPlayer->PlayStatus = CPLAYER_STATE_PAUSE;
		}
	}

	if (pPlayer->pSendData != NULL)
	{
		pthread_mutex_unlock(&pPlayer->sMutexFile);
		pvpack->iSampleTime = utl_get_ms();		// �ط���ʵ��ʱ����ʱ��������ⲿ�ְ�ʱ������ŵĲ������޷��������
		pPlayer->pSendData(pPlayer, (pvpack->bKeyFrame ? CPLAYER_TYPE_I : CPLAYER_TYPE_P),
							pvpack->pData, pvpack->iSize, pvpack->iSampleId, pvpack->iSampleTime, pvpack->iSampleTime, pPlayer->arg);
		pthread_mutex_lock(&pPlayer->sMutexFile);
	}
}

static void complayer_SendAudioFrame(ComPlayer_t* pPlayer, AV_UNPKT* papack)
{
	pPlayer->nAFrameDecoded = papack->iSampleId;

	if (pPlayer->pSendData != NULL)
	{
		pthread_mutex_unlock(&pPlayer->sMutexFile);
		papack->iSampleTime = utl_get_ms();		// �ط���ʵ��ʱ����ʱ��������ⲿ�ְ�ʱ������ŵĲ������޷��������
		pPlayer->pSendData(pPlayer, 
							((pPlayer->Mp4Info.nMp4AudioType == AUDIO_TYPE_G711_U) ? CPLAYER_TYPE_ULAW : 
							(pPlayer->Mp4Info.nMp4AudioType == AUDIO_TYPE_AAC) ? CPLAYER_TYPE_AAC : 
							(pPlayer->Mp4Info.nMp4AudioType == AUDIO_TYPE_G711_A) ? CPLAYER_TYPE_ALAW : CPLAYER_TYPE_ULAW), 
							papack->pData, papack->iSize, papack->iSampleId, papack->iSampleTime, papack->iSampleTime, pPlayer->arg);
		pthread_mutex_lock(&pPlayer->sMutexFile);
	}
}

static void complayer_mp4(void* Param)
{
	ComPlayer_t* pPlayer = (ComPlayer_t*)Param;
	U64 msNow, msLast;						// ��ǰʱ��
	AV_UNPKT apack, vpack, pack;			// MP4���ݰ�
	unsigned char* vpredata, *apredata;
	int vpresize = 32 * 1024;
	int apresize = 5 * 1024;
	BOOL bNeedA = FALSE, bNeedV = FALSE;
	double	timestamp = 0;
	S32 ret = RET_SUCC;
	U64 msLastVFrame = 0, msLastAFrame = 0;	// ���������ʱ��
	U64 msLastProg = 0;						// ����������
	uint64_t countTest = 0;

	pthreadinfo_add(__func__);

	// �ȴ���ʼ����
	while (CPLAYER_STATE_NOPLAY == pPlayer->PlayStatus)
	{
		usleep(50 * 1000);
	}

	pr_info("start play file: %s!\n", pPlayer->fname);

	// �ȷ���MetaData
	if ((strlen(pPlayer->fname) != 0) && (pPlayer->bLoadedInfo))
	{
		VideoFileInfo_t VideoFileInfo;

		// ��ȡ�ļ���Ϣ
		ComPlayer_GetFileInfo(pPlayer, &VideoFileInfo);

		if (pPlayer->pSendMetaData != NULL)
		{
			pPlayer->pSendMetaData(pPlayer, &VideoFileInfo, pPlayer->arg);
		}
	}

	msLast = utl_get_ms();		// ��ȡ��ǰʱ��
	msLastProg = utl_get_ms();

	memset(&pack, 0, sizeof(pack));
	memset(&apack, 0, sizeof(apack));
	memset(&vpack, 0, sizeof(vpack));

	// Ԥ��������Ƶ�ڴ棬�����ظ�malloc/free�������ڴ���Ƭ����
	vpredata = (unsigned char*)malloc(vpresize);
	apredata = (unsigned char*)malloc(apresize);

	while (pPlayer->bRunning)
	{
		utl_sleep_ms(1);
		countTest++;

		pthread_mutex_lock(&pPlayer->sMutexFile);
		if (pPlayer->bNeedRefresh)
		{
			bNeedV = TRUE;
			bNeedA = TRUE;
		}
		
		// ����Ƶ֡
		if (bNeedV)
		{
			memset(&pack, 0, sizeof(pack));
			pack.iType = JVS_UPKT_VIDEO;
			pack.iSampleId = pPlayer->nVFrameDecoded + 1;

			if (pPlayer->PlaySpeed > 2.0)
			{
				// 2�������ϣ�������ֻ��I֡
				ret = MP4_ReadIFrame(pPlayer->fp, &pPlayer->Mp4Info, &pack, TRUE);
				//pr_info("MP4_ReadIFrame, ret: %d, Frame: %d, Size: %d\n", ret, pack.iSampleId, pack.iSize);
			}
			else
			{
				ret = MP4_ReadOneFrame(pPlayer->fp, &pPlayer->Mp4Info, &pack);

				if (pPlayer->bSeekFlag)
				{
					pr_info("MP4_ReadOneFrame, ret: %d, Frame: %d, bKeyFrame: %d, Size: %d\n", ret, pack.iSampleId, pack.bKeyFrame, pack.iSize);
				}
				//pr_info("MP4_ReadOneVideoFrame, ret: %d, Frame: %d, Size: %d\n", ret, pack.iSampleId, pack.iSize);
			}

			if (vpack.pData != vpredata)
			{
				free(vpack.pData);
			}
			memset(&vpack, 0, sizeof(vpack));
			if ((1 == ret) && (pack.iSize > 0))
			{
				bNeedV = FALSE;

				vpack = pack;
				vpack.pData = vpredata;			// ����ʹ��Ԥ�����ڴ棬����Ƶ��malloc
				if (pack.iSize > vpresize)
				{
					// pr_info("malloc video, size: %d\n", pack.iSize);
					vpack.pData = (unsigned char*)malloc(pack.iSize);
				}
				memcpy(vpack.pData, pack.pData, pack.iSize);

				// ���û��ʱ���������֡�ʼ���ʱ�������ֹ��ʱ����ļ��޷���������
				if (vpack.iSampleTime == 0)
				{
					vpack.iSampleTime = vpack.iSampleId * 1000 / pPlayer->Mp4Info.dFrameRate;
				}
			}
			else
			{
				// �ļ���ȡ������ʧ��
				pPlayer->oldPlayStatus = pPlayer->PlayStatus;
				pPlayer->PlayStatus = CPLAYER_STATE_NOPLAY;
				MP4_Close_Read(pPlayer->fp, &pPlayer->Mp4Info);	
				pPlayer->fp = NULL;
				bNeedV = FALSE;
				bNeedA = FALSE;
				pr_info("Total frame: %d, Decoded frame: %d\n", pPlayer->Mp4Info.iNumVideoSamples, pPlayer->nVFrameDecoded);
				
				if (pPlayer->pEventFunc != NULL)
				{
					CPlayEvent_e nEvent = (pPlayer->nVFrameDecoded < pPlayer->Mp4Info.iNumVideoSamples) 
												? CPLAYER_EVENT_END_ERR : CPLAYER_EVENT_END_OK;

					pthread_mutex_unlock(&pPlayer->sMutexFile);
					pPlayer->pEventFunc(pPlayer, nEvent, 0, 0, pPlayer->arg);
					/*pthread_mutex_lock(&pPlayer->sMutexFile);*/
					continue;
				}
			}
		}

		// ����Ƶ֡
		if (bNeedA)
		{
			memset(&pack, 0, sizeof(pack));
			pack.iType = JVS_UPKT_AUDIO;
			pack.iSampleId = pPlayer->nAFrameDecoded + 1;

			ret = MP4_ReadOneFrame(pPlayer->fp, &pPlayer->Mp4Info, &pack);
			if (apack.pData != apredata)
			{
				free(apack.pData);
			}
			memset(&apack, 0, sizeof(apack));
			if ((1 == ret) && (pack.iSize > 0))
			{
				bNeedA = FALSE;

				apack = pack;
				apack.pData = apredata;			// ����ʹ��Ԥ�����ڴ棬����Ƶ��malloc
				if (pack.iSize > apresize)
				{
					// pr_info("malloc audio, size: %d\n", pack.iSize);
					apack.pData = (unsigned char*)malloc(pack.iSize);
				}
				memcpy(apack.pData, pack.pData, pack.iSize);

				// ���û��ʱ���������֡�ʼ���ʱ�������ֹ��ʱ����ļ��޷���������
				if (apack.iSampleTime == 0)
				{
					apack.iSampleTime = apack.iSampleId * pPlayer->Mp4Info.iNumVideoSamples * 1000 
										/ pPlayer->Mp4Info.dFrameRate / pPlayer->Mp4Info.iNumAudioSamples;
				}
			}
			else
			{
				// ��ȡʧ��ʱҲ����ȡ������һֱ����ĳ������֡
				++pPlayer->nAFrameDecoded;
			}
		}

		if (pPlayer->bNeedRefresh)
		{
			timestamp = vpack.iSampleTime;

			// ��Ƶ��Sizeһ������0(������Ϊ��Ƶ���������)���˴�ֻ���ж���Ƶ�Ƿ�ɹ�����
			if ((apack.iSize > 0) && (vpack.iSampleTime >= apack.iSampleTime))
			{
				timestamp = apack.iSampleTime;
			}

			/*printf("VFrameDecoded: %d, AFrameDecoded: %d, vpack.iSize: %d, apack.iSize: %d, vpack.iSampleTime: %lld, apack.iSampleTime: %lld, timestamp: %f\n",*/
						/*pPlayer->nVFrameDecoded, pPlayer->nAFrameDecoded, vpack.iSize, apack.iSize, vpack.iSampleTime, apack.iSampleTime, timestamp);*/
			pPlayer->bNeedRefresh = FALSE;
		}

		msNow = utl_get_ms();		// ��ȡ��ǰʱ��

		// �ȷ�ʱ�����С��֡
		if (vpack.iSampleTime >= apack.iSampleTime)
		{
			//��Ƶ֡����
			if ((!pPlayer->bMute) && (CPLAYER_STATE_PLAY == pPlayer->PlayStatus)
				&& (apack.iSize > 0) && (timestamp + CPLAYER_MS_AHEAD >= apack.iSampleTime))
			{
				complayer_SendAudioFrame(pPlayer, &apack);
				msLastAFrame = msNow;
				bNeedA = TRUE;
			}

			// ��Ƶ֡����
			if ((CPLAYER_STATE_PAUSE != pPlayer->PlayStatus) && (CPLAYER_STATE_NOPLAY != pPlayer->PlayStatus)
				&& (vpack.iSize > 0) && (timestamp + CPLAYER_MS_AHEAD >= vpack.iSampleTime))
			{
				complayer_SendVideoFrame(pPlayer, &vpack);
				msLastVFrame = msNow;
				bNeedV = TRUE;
			}
		}
		else
		{
			// ��Ƶ֡����
			if ((CPLAYER_STATE_PAUSE != pPlayer->PlayStatus) && (CPLAYER_STATE_NOPLAY != pPlayer->PlayStatus)
				&& (vpack.iSize > 0) && (timestamp + CPLAYER_MS_AHEAD >= vpack.iSampleTime))
			{
				complayer_SendVideoFrame(pPlayer, &vpack);
				msLastVFrame = msNow;
				bNeedV = TRUE;
			}

			//��Ƶ֡����
			if ((!pPlayer->bMute) && (CPLAYER_STATE_PLAY == pPlayer->PlayStatus)
				&& (apack.iSize > 0) && (timestamp + CPLAYER_MS_AHEAD >= apack.iSampleTime))
			{
				complayer_SendAudioFrame(pPlayer, &apack);
				msLastAFrame = msNow;
				bNeedA = TRUE;
			}
		}

		if (msNow + CPLAYER_MS_AHEAD >= msLastVFrame + CPLAYER_MS_TIMEOUT)
		{
			// CPLAYER_MS_TIMEOUTʱ���������ݣ����ͱ�������֡
			if (pPlayer->pEventFunc != NULL)
			{
				pthread_mutex_unlock(&pPlayer->sMutexFile);
				pPlayer->pEventFunc(pPlayer, CPLAYER_EVENT_KEEP, 0, 0, pPlayer->arg);
				pthread_mutex_lock(&pPlayer->sMutexFile);
			}
			msLastVFrame = msNow;
		}

		if (msNow + CPLAYER_MS_AHEAD >= msLastProg + CPLAYER_MS_PROGRESS)
		{
			CPlayEvent_e enEvent = CPLAYER_EVENT_PROGRESS;

			if (CPLAYER_STATE_NOPLAY == pPlayer->PlayStatus)
			{
				enEvent = CPLAYER_EVENT_WAITPLAY;
			}
			else if (CPLAYER_STATE_PAUSE == pPlayer->PlayStatus)
			{
				enEvent = CPLAYER_EVENT_KEEP;
			}
			else if (CPLAYER_STATE_STEP == pPlayer->PlayStatus)
			{
				enEvent = CPLAYER_EVENT_KEEP;
			}

			if (pPlayer->pEventFunc != NULL)
			{
				pthread_mutex_unlock(&pPlayer->sMutexFile);
				pPlayer->pEventFunc(pPlayer, enEvent, 
									pPlayer->Mp4Info.iNumVideoSamples, pPlayer->nVFrameDecoded, pPlayer->arg);
				pthread_mutex_lock(&pPlayer->sMutexFile);
			}
			msLastProg = msNow;
		}

		msNow = utl_get_ms();		// Ϊ��֤����׼ȷ���ٴλ�ȡ��ǰʱ��
		if ((CPLAYER_STATE_PAUSE != pPlayer->PlayStatus) && (CPLAYER_STATE_NOPLAY != pPlayer->PlayStatus))
		{
			timestamp += (msNow - msLast) * pPlayer->PlaySpeed;
		}
		msLast = msNow;
		pthread_mutex_unlock(&pPlayer->sMutexFile);
	}

	if (vpack.pData != NULL && vpack.pData != vpredata)
	{
		free(vpack.pData);
	}
	if (apack.pData != NULL && apack.pData != apredata)
	{
		free(apack.pData);
	}
	if (vpredata)		free(vpredata);
	if (apredata)		free(apredata);

	if (pPlayer->fp != NULL)
	{
		MP4_Close_Read(pPlayer->fp, &pPlayer->Mp4Info);	
		pPlayer->fp = NULL;
	}

	pthread_mutex_destroy(&pPlayer->sMutexFile);

	complayer_Del(pPlayer);

	pr_info("Thread exit, fname: %s\n", pPlayer->fname);
}

#define RED "\033[0;31m"
#define END "\033[0;39m"

static S32 complayer_FindIFrame(PLAYER_HANDLE Handle, U32 nCurFrame, BOOL bForward, BOOL bFindOppo)
{
	S32 ret = -1;
	ComPlayer_t* pPlayer = (ComPlayer_t*)Handle;
	AV_UNPKT pack;		// MP4���ݰ�	

	CPLAYER_CHECK_HDL(Handle);

	pack.iType = JVS_UPKT_VIDEO;
	pack.iSampleId = nCurFrame;

	ret = MP4_ReadIFrame(pPlayer->fp, &pPlayer->Mp4Info, &pack, bForward);

	if (ret)
	{
		pr_info("Find key frame forward, nCurFrame: %d, bForward: %d, KeyFrame: %d\n", nCurFrame, bForward, pack.iSampleId);
		return pack.iSampleId;
	}
	else
	{
		if (bFindOppo)
		{
			ret = MP4_ReadIFrame(pPlayer->fp, &pPlayer->Mp4Info, &pack, !bForward);
			if (ret)
			{
				pr_info("Find key frame backward, nCurFrame: %d, bForward: %d, KeyFrame: %d\n", nCurFrame, !bForward, pack.iSampleId);
				return pack.iSampleId;
			}
			else
			{
				pr_info(RED"Not find key frame on both side, nCurFrame: %d, KeyFrame: %d\n"END, nCurFrame, -1);
				return -1;
			}
		}
		else
		{
			pr_info(RED"Not find key frame, nCurFrame: %d, bForward: %d, KeyFrame: %d\n"END, nCurFrame, bForward, pack.iSampleId);
			return -1;
		}
	}
}

#endif //#if SD_RECORD_SUPPORT

