/*
add by gyd 201311
��������Զ����Ƶ�طţ���DVR����ֲ����
*/
#include "jv_common.h"

#include "mplay_remote.h"
#include "mrecord.h"
#include "mstream.h"
#include "JVNSDKDef.h"
#include "Jmp4pkg.h"
#include "JvCDefines.h"
#include "JvServer.h"
#include "sctrl.h"
#include "mstorage.h"
#include "m_complayer.h"
#include "utl_common.h"


#define INVALID_CLIENTID		0

#define Common_GetMs		utl_get_ms

#define MP4_MAX_LIST_ITEM  320

typedef struct
{
	S32				ClientID;
	PLAYER_HANDLE	Handle;
	S32				nConnectionType;
	PlayerType_e	playType;
	PlayerStatus_e  playStatus;
	PlayerMode_e    playMode;
	VideoFileInfo_t videoFileInfo;
	S32				mp4Index;
	S32				mp4Count;
	mrecord_item_t  mp4Items[MP4_MAX_LIST_ITEM];	// ¼���б�¼��ʼʱ������stime��
}RemoteComPlayer_t;

//Զ�̻ط���������,NULL��ʾû�лط�
#define MAX_REMOTE_PLAY			2

#define CHECK_INDEX(x)			(((x) >= 0) && ((x) < MAX_REMOTE_PLAY))

RemoteComPlayer_t sRemotePlay[MAX_REMOTE_PLAY]={{0}};
RemotePlayer_t *sTutkRemotePlay[MAX_REMOTE_PLAY]={0};

//���дΪ25����ʵ�ʿ��ܲ�һ�������ֵ
//����������ʲ������ֵ���ᵼ�¿�����߿��˵��ٶ��쳣
#define I_FRAME_NGOP	25//50
//sRemotePlay�Ļ���
static pthread_mutex_t sMutexRemotePlay;    
static pthread_mutex_t sMutesTutkRemotePlay;  

//ipc��startcode�������ı�ʱ������no video����ʱֱ�����������ʵӦ�ü�һ����ƵԴ��start code�Ŷԣ���ʱ����
#define IPC_STARTCODE		0x1053564a
#define JVSNVR_STARTCODE	0x2053564A	//nvr��start code������ʱ�õ���ʹ��ǰ���豸(IPC)�Ļ��ֿز�������IPC��NVR
#define TRANSMIT_STREAM		1			//Զ����������


//���ݷֿ�id����Index
//�ɹ�����Index��ʧ�� -1
static S32 _remote_GetPlayerIndex(S32 clientId)
{
	int i;

	if (clientId == INVALID_CLIENTID)
	{
		printf("%s, Invalid Client ID: %d\n", __func__, clientId);
		return -1;
	}
	
	pthread_mutex_lock(&sMutexRemotePlay);
	for(i = 0; i < MAX_REMOTE_PLAY; i++)
	{
		if(sRemotePlay[i].ClientID == clientId)
		{
			pthread_mutex_unlock(&sMutexRemotePlay);
			return i;
		}
	}
	pthread_mutex_unlock(&sMutexRemotePlay);

	return -1;
}

//ע��ֿ�id
//�ɹ�����Index��ʧ�� -1
static S32 _remote_RegPlayerIndex(S32 clientId)
{
	int nIndex = -1;
	int i;

	if (clientId == INVALID_CLIENTID)
	{
		printf("%s, Invalid Client ID: %d\n", __func__, clientId);
		return -1;
	}

	nIndex = _remote_GetPlayerIndex(clientId);
	if (nIndex != -1)
	{
		printf("%s, Client ID %d already registered!\n", __func__, clientId);
		return nIndex;
	}

	pthread_mutex_lock(&sMutexRemotePlay);
	for(i = 0; i < MAX_REMOTE_PLAY; i++)
	{
		if(sRemotePlay[i].ClientID == INVALID_CLIENTID)
		{
			sRemotePlay[i].ClientID = clientId;
			pthread_mutex_unlock(&sMutexRemotePlay);
			return i;
		}
	}
	pthread_mutex_unlock(&sMutexRemotePlay);

	return -1;
}

//�Ƴ��ֿ�id
//�ɹ�����Index��ʧ�� -1
static S32 _remote_UnregPlayerIndex(S32 clientId)
{
	int nIndex = -1;
	int i;

	nIndex = _remote_GetPlayerIndex(clientId);
	if (nIndex == -1)
	{
		printf("%s, Client ID %d not registered!\n", __func__, clientId);
		return nIndex;
	}

	pthread_mutex_lock(&sMutexRemotePlay);
	for(i = 0; i < MAX_REMOTE_PLAY; i++)
	{
		if(sRemotePlay[i].ClientID == clientId)
		{
			memset(&sRemotePlay[i], 0, sizeof(sRemotePlay[i]));
			pthread_mutex_unlock(&sMutexRemotePlay);
			return i;
		}
	}
	pthread_mutex_unlock(&sMutexRemotePlay);

	return -1;
}

//����
int Remote_Player_Fast(S32 clientId)
{
	int nIndex = _remote_GetPlayerIndex(clientId);

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return -1;
	}

	return ComPlayer_ChgPlayState(sRemotePlay[nIndex].Handle, CPLAYER_CMD_FAST, NULL);
}

//����
int Remote_Player_Slow(S32 clientId)
{
	int nIndex = _remote_GetPlayerIndex(clientId);

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return -1;
	}

	return ComPlayer_ChgPlayState(sRemotePlay[nIndex].Handle, CPLAYER_CMD_SLOW, NULL);
}

//�����ٶȲ���
int Remote_Player_PlayNormal(S32 clientId)
{
	int nIndex = _remote_GetPlayerIndex(clientId);

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return -1;
	}

	return ComPlayer_ChgPlayState(sRemotePlay[nIndex].Handle, CPLAYER_CMD_PLAY, NULL);
}

//��ͣ
int Remote_Player_Pause(S32 clientId)
{
	int nIndex = _remote_GetPlayerIndex(clientId);

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return -1;
	}

	return ComPlayer_ChgPlayState(sRemotePlay[nIndex].Handle, CPLAYER_CMD_PAUSE, NULL);
}

//ȡ����ͣ
int Remote_Player_Resume(S32 clientId)
{
	int nIndex = _remote_GetPlayerIndex(clientId);

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return -1;
	}

	return ComPlayer_ChgPlayState(sRemotePlay[nIndex].Handle, CPLAYER_CMD_RESUME, NULL);
}

static int _searchFrameByTime(int nIndex, int nSeekTime)
{
	int i = 0;
	int stime, etime;
	int nTimePos, hour, min, sec;
	int timeOffset;
	int frameOffset = 0;
	char strFilePath[128]  = {0};
	struct tm tmPos;
	time_t tsp = (time_t)nSeekTime;
	localtime_r(&tsp, &tmPos);

	nTimePos = tmPos.tm_hour * 10000 + tmPos.tm_min * 100 + tmPos.tm_sec;
	stime = sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].stime;
	etime = sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].etime;

	float frameRate = 0;
	VideoFileInfo_t videoFileInfo;
	memset(&videoFileInfo, 0, sizeof(VideoFileInfo_t));

	printf("stime:%d etime:%d nTimePos:%d\n", stime, etime, nTimePos);
	if(stime <= nTimePos && nTimePos < etime)
	{
		/* one file */
		timeOffset = utl_time_range(UTL_TIME_HHMMSS, stime, nTimePos);
		if(0 != ComPlayer_GetFileInfo(sRemotePlay[nIndex].Handle, &videoFileInfo))
			return 0;
		if(videoFileInfo.nVFrameRateDen <= 0)
			return 0;
		frameRate = (float)videoFileInfo.nVFrameRateNum / (float)videoFileInfo.nVFrameRateDen;
		frameOffset = timeOffset * videoFileInfo.nVTotalFrame / utl_time_range(UTL_TIME_HHMMSS, stime, etime);
		printf("one file framerate:%d/%d=%f frameOffset:%d\n",
				videoFileInfo.nVFrameRateNum,
				videoFileInfo.nVFrameRateDen,
				frameRate, frameOffset);
	}
	else
	{
		/* change file */
		for(i = 0; i < sRemotePlay[nIndex].mp4Count; i++)
		{
			stime = sRemotePlay[nIndex].mp4Items[i].stime;
			etime = sRemotePlay[nIndex].mp4Items[i].etime;

			if((nTimePos >= stime && nTimePos < etime) 	// ָ��ʱ�������ĳ¼��֮��[stime, etime)
				|| (nTimePos == stime)					// ָ��ʱ���û����ĳ¼��֮�ڣ���ǡ�õ���ĳ¼��Ŀ�ʼʱ�䣨����ϵͳʱ�䱻�޸ĵ������
				|| (nTimePos < stime))					// ָ��ʱ���û��¼�񣬶�λ֮��ĵ�һ��¼��[0, stime)
			{
				mrecord_get_filename(&sRemotePlay[nIndex].mp4Items[i], strFilePath, sizeof(strFilePath));
				printf("fileName:%s stime:%d etime:%d\n", strFilePath, stime, etime);
				ComPlayer_ChgPlayFile(sRemotePlay[nIndex].Handle, strFilePath, TRUE);
				if(0 != ComPlayer_GetFileInfo(sRemotePlay[nIndex].Handle, &videoFileInfo))
					return 0;
				if(videoFileInfo.nVFrameRateDen <= 0)
					return 0;
				frameRate = (float)videoFileInfo.nVFrameRateNum / (float)videoFileInfo.nVFrameRateDen;
				memcpy(&sRemotePlay[nIndex].videoFileInfo, &videoFileInfo, sizeof(VideoFileInfo_t));
				sRemotePlay[nIndex].mp4Index = i;

				if(stime <= nTimePos && nTimePos < etime)
				{
					timeOffset = utl_time_range(UTL_TIME_HHMMSS, stime, nTimePos);
					frameOffset = timeOffset * videoFileInfo.nVTotalFrame / utl_time_range(UTL_TIME_HHMMSS, stime, etime);
					printf("other file framerate:%d/%d=%f frameOffset:%d\n",
							videoFileInfo.nVFrameRateNum,
							videoFileInfo.nVFrameRateDen, frameRate, frameOffset);
				}
				else
				{
					frameOffset = 0;
					printf("begin the nearest file play\n");
				}
				break;
			}
		}
		if (i == sRemotePlay[nIndex].mp4Count)
		{
			// û���ҵ�¼��������ֱ�ӷ���0����ͷ���ŵ�ǰ�ļ�
			frameOffset = 0;
			printf("not found match record file at %d!\n", nTimePos);
		}
	}
	return frameOffset;
}

//���ò���λ��
int Remote_Player_Seek(S32 clientId, U32 value)
{
	int frameOffset = 0;
	int Param[] = {-1, -10};
	int nIndex = _remote_GetPlayerIndex(clientId);

	switch(sRemotePlay[nIndex].playType)
	{
		case EN_PLAYER_TYPE_NORMAL:
			frameOffset = value;
			break;
		case EN_PLAYER_TYPE_TIMEPOINT:
			frameOffset = _searchFrameByTime(nIndex, value);
			break;
		default:
			break;
	}

	printf("seek frameOffset:%d\n", frameOffset);
	Param[0] = frameOffset;
	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return -1;
	}

	return ComPlayer_ChgPlayState(sRemotePlay[nIndex].Handle, CPLAYER_CMD_PLAY, Param);
}

static void *_remote_player_send_error(void *param)
{
	pthreadinfo_add((char *)__func__);
	RemotePlayer_t *player = (RemotePlayer_t *)param;

#ifdef YST_SVR_SUPPORT
	if(gp.bNeedYST)
	{
		JVN_SendPlay(player->clientId, JVN_RSP_PLAYE, player->nConnectionType, 0, NULL, 0, 0, 0, 0, 0, 0);
	}
#endif
	
	Remote_Player_Destroy(player->clientId);
	return NULL;
}

static void *_remote_player_send_over(int nIndex)
{
#ifdef YST_SVR_SUPPORT
	if(gp.bNeedYST)
	{
		JVN_SendPlay(sRemotePlay[nIndex].ClientID, JVN_RSP_PLAYOVER, sRemotePlay[nIndex].nConnectionType, 0, NULL, 0, 0, 0, 0, 0, 0);
	}
#endif
	printf("%s success.\n", __FUNCTION__);
	return NULL;
}

static S32 _remote_ReleasePlayer(S32 clientId)
{
	int ret = -1;
	int nIndex = _remote_GetPlayerIndex(clientId);

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d, client id: %d\n", __func__, nIndex, clientId);
		return ret;
	}

	if(sRemotePlay[nIndex].playStatus != EN_PLAYER_NO)
	{
		sRemotePlay[nIndex].playStatus = EN_PLAYER_NO;
		ComPlayer_Destroy(sRemotePlay[nIndex].Handle, TRUE);

		_remote_UnregPlayerIndex(clientId);
		printf("%s, client:%d index:%d\n", __func__, clientId, nIndex);
	}
	else
	{
		printf("%s, client:%d index:%d is not started!\n", __func__, clientId, nIndex);
		return ret;
	}

	return (ret = 0);
}

// �ļ���Ϣ
static S32 __OnSendMetaData(const PLAYER_HANDLE Handle, const VideoFileInfo_t* pMetaData, void* arg)
{
	JVS_FILE_HEADER_EX jHeaderEx;
	JVS_FILE_HEADER	jHeader = {0};
	int nIndex = (int)arg;

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return -1;
	}

	//�ϸ�ʽ
	jHeader.nFlag			= JVSNVR_STARTCODE;
	jHeader.nVideoFormat	= JVS_VIDEOFORMAT_PAL;
	jHeader.nTotalFrames	= pMetaData->nVTotalFrame;
	jHeader.nFrameWidth 	= pMetaData->nVWidth;
	jHeader.nFrameHeight 	= pMetaData->nVHeight;
	jHeader.u32IIndexAddr   = IPC_STARTCODE;

	//�¸�ʽ
	memset(&jHeaderEx, 0, sizeof(JVS_FILE_HEADER_EX));
	memcpy(jHeaderEx.ucOldHeader, &jHeader, sizeof(JVS_FILE_HEADER));

	//�ṹ����Ϣ
	strcpy((char *)jHeaderEx.ucHeader, "JFH");	//�ṹ��ʶ���룬����Ϊ��J','F','H'
	jHeaderEx.ucVersion = 1;			//�ṹ��汾�ţ���ǰ�汾Ϊ1
	//�豸���
	jHeaderEx.wDeviceType 			= JVS_DEVICETYPE_NVR;		//�豸����
	//��Ƶ����
	jHeaderEx.wVideoCodecID 		= (pMetaData->nVDataType == VIDEO_TYPE_H265) 
										? JVS_VIDEOCODECTYPE_H265 : JVS_VIDEOCODECTYPE_H264;			//��Ƶ��������
	jHeaderEx.wVideoDataType 		= (pMetaData->nADataType != AUDIO_TYPE_UNKNOWN) 
										? JVS_VIDEODATATYPE_VIDEOANDAUDIO : JVS_VIDEODATATYPE_VIDEO;	//��������
	jHeaderEx.wVideoFormat 			= JVS_VIDEOFORMAT_PAL;			//��Ƶģʽ
	jHeaderEx.wVideoWidth 			= pMetaData->nVWidth;			//��Ƶ��
	jHeaderEx.wVideoHeight 			= pMetaData->nVHeight;			//��Ƶ��
	jHeaderEx.wFrameRateNum 		= pMetaData->nVFrameRateNum;	//֡�ʷ���
	jHeaderEx.wFrameRateDen 		= pMetaData->nVFrameRateDen;	//֡�ʷ�ĸ

	//��Ƶ����
	jHeaderEx.wAudioCodecID 		= (pMetaData->nADataType == AUDIO_TYPE_G711_U) ? JVS_AUDIOCODECTYPE_G711_ulaw : 
										(pMetaData->nADataType == AUDIO_TYPE_PCM) ? JVS_AUDIOCODECTYPE_PCM : 
										JVS_AUDIOCODECTYPE_G711_alaw;									//��Ƶ�����ʽ
	jHeaderEx.wAudioSampleRate 		= 8000;						//��Ƶ������
	jHeaderEx.wAudioChannels 		= 1;						//��Ƶ������
	jHeaderEx.wAudioBits 			= 16;						//��Ƶ����λ��
	//¼�����
	jHeaderEx.dwRecFileTotalFrames 	= pMetaData->nVTotalFrame;				//¼����֡��
	jHeaderEx.wRecFileType 			= JVS_RECFILETYPE_MP4;		//¼������

	//�·ֿ�(���ݰ忨��dvrԶ�̻ط�)��
	U8	pBuffer[sizeof(JVS_FILE_HEADER_EX) + 8] = {0};
	memcpy(pBuffer+2, &jHeaderEx, sizeof(JVS_FILE_HEADER_EX));

#ifdef YST_SVR_SUPPORT
	if(gp.bNeedYST)
	{
		JVN_SendPlay(sRemotePlay[nIndex].ClientID, JVN_RSP_PLAYDATA, sRemotePlay[nIndex].nConnectionType, JVN_DATA_O, pBuffer, sizeof(JVS_FILE_HEADER_EX)+2, 
							pMetaData->nVWidth, pMetaData->nVHeight, pMetaData->nVTotalFrame, 0, 0);
	}
#endif
	
	printf("%s, Send meta data success.\n", __func__);

	return 0;
}

// ��Ƶ/��Ƶ��
static S32 __OnSendData(const PLAYER_HANDLE Handle, CPLAYER_TYPE_e nType, const void* pData, S32 nSize, S32 nSeq, S32 nPts, S32 nDts, void* arg)
{
	int nIndex = (int)arg;
	int nTargetType = 0;
	S32 timeStamp = 0;
	float frameRate = 0;

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return -1;
	}

#ifdef YST_SVR_SUPPORT
	if(gp.bNeedYST)
	{
		switch (nType)
		{
		case CPLAYER_TYPE_I:			nTargetType = JVN_DATA_I;			break;
		case CPLAYER_TYPE_P:			nTargetType = JVN_DATA_P;			break;
		case CPLAYER_TYPE_B:			nTargetType = JVN_DATA_B;			break;
		case CPLAYER_TYPE_ULAW:
		case CPLAYER_TYPE_ALAW:			nTargetType = JVN_DATA_A;			break;
		default:						nTargetType = -1;					break;
		}
		switch(sRemotePlay[nIndex].playType)
		{
			case EN_PLAYER_TYPE_NORMAL:
				timeStamp = nPts;
				break;
			case EN_PLAYER_TYPE_TIMEPOINT:
				{
					if((nTargetType != JVN_DATA_I && nTargetType != JVN_DATA_P) ||
							sRemotePlay[nIndex].videoFileInfo.nVFrameRateDen <= 0 ||
							sRemotePlay[nIndex].videoFileInfo.nVTotalFrame <= 0)
					{
						timeStamp = nPts;
						break;
					}
					frameRate = (float)sRemotePlay[nIndex].videoFileInfo.nVFrameRateNum / (float)sRemotePlay[nIndex].videoFileInfo.nVFrameRateDen;
					if(frameRate <= 0)
					{
						timeStamp = nPts;
						break;
					}

					int secPos = 0;
					if(sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].type == REC_CHFRAME)
					{
						secPos = utl_time_range(UTL_TIME_HHMMSS,
								sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].stime,
								sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].etime);
						secPos = (float)secPos / (float)sRemotePlay[nIndex].videoFileInfo.nVTotalFrame * (float)nSeq;
					}
					else
					{
						secPos = (float)nSeq / frameRate;
					}

					timeStamp = utl_time_modify(UTL_TIME_HHMMSS,
							sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].stime, secPos);

					/* ��ֹ¼��ʱ�䳬�����������ֻ�ʱ������ʾ���� */
					if(timeStamp < sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].stime)
						timeStamp = 235959;

					Printf("stime:%d etime:%d nSeq:%d total:%d secpos:%d timestamp:%d nTargetType:%d\n",
							sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].stime,
							sRemotePlay[nIndex].mp4Items[sRemotePlay[nIndex].mp4Index].etime,
							nSeq, sRemotePlay[nIndex].videoFileInfo.nVTotalFrame,
							secPos, timeStamp, nTargetType);
				}
				break;
			default:
				break;
		}

		if (nTargetType != -1)
			JVN_SendPlay(sRemotePlay[nIndex].ClientID, JVN_RSP_PLAYDATA, sRemotePlay[nIndex].nConnectionType, nTargetType, (uint8_t*)pData, nSize, 0, 0, 0, (U64)timeStamp, 0);
	}
#endif

	return 0;
}

// �¼��ص���������ɵ��¼�
static void __OnEventCallback(const PLAYER_HANDLE Handle, CPlayEvent_e nEvent, S32 Param1, S32 Param2, void* arg)
{
	int nIndex = (int)arg;
	char strFilePath[128]  = {0};

	if (!CHECK_INDEX(nIndex))
	{
		printf("%s, Invalid index: %d\n", __func__, nIndex);
		return;
	}

	switch (nEvent)
	{
	case CPLAYER_EVENT_END_OK:
	case CPLAYER_EVENT_END_ERR:
		printf("%s, play finished %s\n", __func__, (nEvent == CPLAYER_EVENT_END_OK) ? "normally" : "abnormally");
		switch (sRemotePlay[nIndex].playMode)
		{
			case EN_PLAYER_MODE_ONE:
				_remote_player_send_over(nIndex);
				_remote_ReleasePlayer(sRemotePlay[nIndex].ClientID);
				break;
			case EN_PLAYER_MODE_SEQUENCE:
				if(CPLAYER_EVENT_END_ERR == nEvent ||
						sRemotePlay[nIndex].mp4Index + 1 >= sRemotePlay[nIndex].mp4Count)
				{
					printf("remote play over!\n");
					_remote_player_send_over(nIndex);
					_remote_ReleasePlayer(sRemotePlay[nIndex].ClientID);
				}
				else
				{
					printf("remote play next file!\n");
					mrecord_get_filename(&sRemotePlay[nIndex].mp4Items[++sRemotePlay[nIndex].mp4Index],
							strFilePath, sizeof(strFilePath));
					ComPlayer_ChgPlayFile(sRemotePlay[nIndex].Handle, strFilePath, TRUE);

					if(0 == ComPlayer_GetFileInfo(sRemotePlay[nIndex].Handle,
								&sRemotePlay[nIndex].videoFileInfo))
					{
						ComPlayer_ChgPlayState(sRemotePlay[nIndex].Handle, CPLAYER_CMD_PLAY, NULL);
					}
				}
				break;
			default:
				break;
		}
		break;
	case CPLAYER_EVENT_PROGRESS:
		/*printf("current remote video play progress: %f%%\n", ceil((double)Param2 / (double)Param1 * 100));*/
		printf("current remote video play progress: %d%%\n", Param2 * 100 / (Param1 == 0 ? 1 : Param1));
		break;
	default:
		break;
	}
}

static int _checkTimePoint(int nSeekTime, int index, char strPath[], int *offset)
{
	int ret = -1;
	int i = 0;
	int mp4Count = 0;
	int nTimePos; /* ��ʽ:235959 */

	int stime, etime, hour, min, sec;
	
	struct tm tmPos;
	char chPathTime[16]  = {0};
	char strFilePath[128]  = {0};
	time_t tsp = (time_t)nSeekTime;
	localtime_r(&tsp, &tmPos);
	sprintf(chPathTime, "%4.4d%2.2d%2.2d", tmPos.tm_year + 1900, tmPos.tm_mon + 1, tmPos.tm_mday);
	printf("get chPathTime %d : %s \n", nSeekTime, chPathTime);

	nTimePos = tmPos.tm_hour * 10000 + tmPos.tm_min * 100 + tmPos.tm_sec;

	mp4Count = mrecord_search_file(atoi(chPathTime), 0, atoi(chPathTime), 235959,
			sRemotePlay[index].mp4Items, MP4_MAX_LIST_ITEM);

	printf("mp4 file count:%d\n", mp4Count);
	if(mp4Count == 0)
		return ret;

	mrecord_attr_t record;
	mrecord_get_param(0, &record);

	sRemotePlay[index].mp4Count = mp4Count;
	for(i = 0; i < mp4Count; i++)
	{
		stime = sRemotePlay[index].mp4Items[i].stime;
		etime = sRemotePlay[index].mp4Items[i].etime;

		if(sRemotePlay[index].mp4Items[i].type == REC_NORMAL ||
				sRemotePlay[index].mp4Items[i].type == REC_ALARM)
		{
			if(utl_time_range(UTL_TIME_HHMMSS, stime, nTimePos)  > (record.file_length + 300))
				continue;
		}
		if(stime < nTimePos && nTimePos < etime)
		{
			mrecord_get_filename(&sRemotePlay[index].mp4Items[i], strFilePath, sizeof(strFilePath));
			printf("fileName:%s stime:%d etime:%d\n", strFilePath, stime, etime);
			memcpy(strPath, strFilePath, strlen(strFilePath));
			sRemotePlay[index].mp4Index = i;
			ret = 0;
			break;
		}
	}

	/* û���ҵ��ļ�,Ĭ�ϲ��ŵ�һ�� */
	if(i == mp4Count)
	{
		mrecord_get_filename(&sRemotePlay[index].mp4Items[0], strFilePath, sizeof(strFilePath));
		printf("fileName:%s stime:%d etime:%d\n", strFilePath,
				sRemotePlay[index].mp4Items[0].stime,
				sRemotePlay[index].mp4Items[0].etime);
		memcpy(strPath, strFilePath, strlen(strFilePath));
		sRemotePlay[index].mp4Index = 0;
		ret = 0;
	}

	return ret;
}

//����Զ�̻طŲ�����
//clientId			:�ֿ�id
//fname 			:Ҫ���ŵ��ļ���
//nConnectionType	:��������
//return 0 �ɹ���<0 ʧ��
int Remote_Player_Create(S32 clientId, int nConnectionType, PlayerType_e playType, PlayerMode_e playMode, void *param)
{
#if SD_RECORD_SUPPORT
	int nIndex = -1;
	int timeOffset = 0;
	int cParam[] = {-1, -10};
	char strPath[128]    = {0};
	PLAYER_HANDLE Handle = NULL;

	printf("Remote_Player_create, client:%d, type:%d playType:%d playMode:%d\n",
			clientId, nConnectionType, playType, playMode);

	nIndex = _remote_GetPlayerIndex(clientId);
	if (nIndex != -1)
	{
		printf("ERROR: client:%d already playing\n", clientId);
		goto play_err;
	}

	nIndex = _remote_RegPlayerIndex(clientId);
	if (nIndex == -1)
	{
		printf("ERROR: client:%d reg failed, no free resource\n", clientId);
		goto play_err;
	}
	
	switch(playType)
	{
		case EN_PLAYER_TYPE_NORMAL:
			{
				memcpy(strPath, (char *)param, strlen((char *)param));
			}
			break;
		case EN_PLAYER_TYPE_TIMEPOINT:
			{
				int timePoint = (int)param;
				if(0 != _checkTimePoint(timePoint, nIndex, strPath, &timeOffset))
				{
					printf("ERROR: client:%d check name and time failed!\n", clientId);
					goto play_err;
				}
			}
			break;
		default:
			break;
	}

	Handle = ComPlayer_Create(strPath, __OnSendMetaData, __OnSendData, __OnEventCallback, (void*)nIndex);
	if (Handle == NULL)
	{
		printf("ERROR: client:%d already playing\n", clientId);
		_remote_UnregPlayerIndex(clientId);
		goto play_err;
	}

	VideoFileInfo_t videoFileInfo;
	memset(&videoFileInfo, 0, sizeof(VideoFileInfo_t));
	if(0 != ComPlayer_GetFileInfo(Handle, &videoFileInfo))
	{
		printf("ERROR: ComPlayer_GetFileInfo failed!\n");
		_remote_UnregPlayerIndex(clientId);
		goto play_err;
	}
	if(videoFileInfo.nVFrameRateDen <= 0)
	{
		printf("ERROR: videoFileInfo.nVFrameRateDen <= 0!\n");
		_remote_UnregPlayerIndex(clientId);
		goto play_err;
	}

	memcpy(&sRemotePlay[nIndex].videoFileInfo, &videoFileInfo, sizeof(VideoFileInfo_t));
	printf("nVTotalFrame:%d nVFrameRateNum:%d nVFrameRateDen:%d \n",
			sRemotePlay[nIndex].videoFileInfo.nVTotalFrame,
			sRemotePlay[nIndex].videoFileInfo.nVFrameRateNum,
			sRemotePlay[nIndex].videoFileInfo.nVFrameRateDen);
	cParam[0] = (float)sRemotePlay[nIndex].videoFileInfo.nVFrameRateNum / (float)sRemotePlay[nIndex].videoFileInfo.nVFrameRateDen * (float)timeOffset;

	// ������ʼ����
	ComPlayer_ChgPlayState(Handle, CPLAYER_CMD_PLAY, cParam);

	sRemotePlay[nIndex].Handle = Handle;
	sRemotePlay[nIndex].nConnectionType = nConnectionType;
	sRemotePlay[nIndex].playMode = playMode;
	sRemotePlay[nIndex].playType = playType;
	sRemotePlay[nIndex].playStatus = EN_PLAYER_PLAY;

	return 0;

#endif

play_err:
	do {
		pthread_t pid;
		static RemotePlayer_t failPlayer;
		memset(&failPlayer, 0, sizeof(RemotePlayer_t));
		failPlayer.clientId = clientId;
		failPlayer.nConnectionType = nConnectionType;
		pthread_create_detached(&pid, NULL, (void *)_remote_player_send_error, (void *)&failPlayer);
	} while(0);
	return -1;
}

//�ر�Զ�̻طŲ�����
int Remote_Player_Destroy(S32 clientId)
{
	if (_remote_ReleasePlayer(clientId) < 0)
	{
		return -1;
	}

	printf("%s, client: %d\n", __func__, clientId);
	return 0;
}


//��MP4�ļ���Ϣ
//����-1  �ļ�����   0�ɹ�
int _remote_ReadFileInfo_mp4(RemotePlayer_t *player)
{
	//���ļ���Ϣ
	player->fp = MP4_Open_Read(player->fname, &player->mp4Info);
	if (player->fp == NULL)
	{
		printf("ERROR: Failed Open file: %s, because: %s\n", player->fname, strerror(errno));

		return -1;
	}

	printf("%d %d %d\n",player->nWidth, player->nHeight, player->frameRate);
	if(player->mp4Info.iFrameWidth==0 || player->mp4Info.iFrameHeight==0 || (U32)player->frameRate==0)
	{
		//printf("ERROR: Failed Open file: %s, because: %s\n", player->fname, strerror(errno));
		if(player->fp!=NULL)
		{
			MP4_Close_Read(player->fp, &player->mp4Info);
			player->fp = NULL;
		}
		return -1;
	}

	player->nWidth = player->mp4Info.iFrameWidth;
	player->nHeight = player->mp4Info.iFrameHeight;
	player->frameRate = (U32)player->mp4Info.dFrameRate;
	player->nTimeSlice = 1000 / player->frameRate;
	player->setPosInFrame = 0;
	return 0;
}

int Remote_ReadFileInfo_MP4(RemotePlayer_t *player)
{
	return _remote_ReadFileInfo_mp4(player);
}

BOOL Remote_Read_Frame_MP4(void *handle, MP4_READ_INFO *pInfo, PAV_UNPKT pPack)
{
	return MP4_ReadOneFrame(handle, pInfo, pPack);
}

#if 0
//Զ�̻ط�ʱ�����ֿط���¼���ļ���Ϣ
void _remote_SendInfo(RemotePlayer_t *player)
{
	JVS_FILE_HEADER_EX jHeaderEx;
	JVS_FILE_HEADER	jHeader = {0};
	int nW = player->nWidth;
	int nH = player->nHeight;
	int nFrameRate = player->frameRate;
	int nTotalFrame;
	int nHeadFormat = JVS_VIDEOFORMAT_PAL;

	if(player->bMp4)
		nTotalFrame = player->mp4Info.iNumVideoSamples;
	else
		nTotalFrame = player->header.nTotalFrames;

	//�ϸ�ʽ
	jHeader.nFlag			= JVSNVR_STARTCODE;
	jHeader.nVideoFormat	= nHeadFormat;
	jHeader.nTotalFrames	= nTotalFrame;
	jHeader.nFrameWidth 	= nW;
	jHeader.nFrameHeight 	= nH;
	jHeader.u32IIndexAddr   = IPC_STARTCODE;

	//�¸�ʽ
	int nHeadDataType;
	if(player->bMp4)
		nHeadDataType = player->mp4Info.bHasAudio ? JVS_VIDEODATATYPE_VIDEOANDAUDIO : JVS_VIDEODATATYPE_VIDEO;
	else
		nHeadDataType = JVS_VIDEODATATYPE_VIDEO;
	
	memset(&jHeaderEx, 0, sizeof(JVS_FILE_HEADER_EX));
	memcpy(jHeaderEx.ucOldHeader, &jHeader, sizeof(JVS_FILE_HEADER));

	//�ṹ����Ϣ
	strcpy((char *)jHeaderEx.ucHeader, "JFH");	//�ṹ��ʶ���룬����Ϊ��J','F','H'
	jHeaderEx.ucVersion = 1;			//�ṹ��汾�ţ���ǰ�汾Ϊ1
	//�豸���
	jHeaderEx.wDeviceType 			= JVS_DEVICETYPE_NVR;		//�豸����
	//��Ƶ����
	jHeaderEx.wVideoCodecID 		= JVS_VIDEOCODECTYPE_H264;	//��Ƶ��������
	jHeaderEx.wVideoDataType 		= nHeadDataType;			//��������
	jHeaderEx.wVideoFormat 			= nHeadFormat;				//��Ƶģʽ
	jHeaderEx.wVideoWidth 			= nW;						//��Ƶ��
	jHeaderEx.wVideoHeight 			= nH;						//��Ƶ��
	jHeaderEx.wFrameRateNum 		= nFrameRate;				//֡�ʷ���
	jHeaderEx.wFrameRateDen 		= 1;						//֡�ʷ�ĸ

	if(player->bMp4)
	{
		//��Ƶ����
		jHeaderEx.wAudioCodecID 		= JVS_AUDIOCODECTYPE_G711_ulaw;	//��Ƶ�����ʽ
		jHeaderEx.wAudioSampleRate 		= 8000;						//��Ƶ������
		jHeaderEx.wAudioChannels 		= 1;						//��Ƶ������
		jHeaderEx.wAudioBits 			= 16;						//��Ƶ����λ��
		//¼�����
		jHeaderEx.dwRecFileTotalFrames 	= nTotalFrame;				//¼����֡��
		jHeaderEx.wRecFileType 			= JVS_RECFILETYPE_MP4;		//¼������
	}
	else
	{
		//��Ƶ����
		jHeaderEx.wAudioCodecID 		= JVS_AUDIOCODECTYPE_PCM;	//��Ƶ�����ʽ
		jHeaderEx.wAudioSampleRate 		= 8000;						//��Ƶ������
		jHeaderEx.wAudioChannels 		= 1;						//��Ƶ������
		jHeaderEx.wAudioBits 			= 8;						//��Ƶ����λ��
		//¼�����
		jHeaderEx.dwRecFileTotalFrames 	= nTotalFrame;				//¼����֡��
		jHeaderEx.wRecFileType 			= JVS_RECFILETYPE_SV5;		//¼������
	}

	//�·ֿ�(���ݰ忨��dvrԶ�̻ط�)��
	U8	pBuffer[sizeof(JVS_FILE_HEADER_EX) + 8] = {0};
	memcpy(pBuffer+2, &jHeaderEx, sizeof(JVS_FILE_HEADER_EX));

#ifdef YST_SVR_SUPPORT
	if(gp.bNeedYST)
	{
		JVN_SendPlay(player->clientId, JVN_RSP_PLAYDATA, player->nConnectionType, JVN_DATA_O, pBuffer, sizeof(JVS_FILE_HEADER_EX)+2, player->nWidth, player->nHeight, nTotalFrame, 0, 0);
	}
#endif
}
//ʱ����طž������
static void _remote_player_time_mp4(void *param)
{
//��ǰ�������ݵ�ʱ�䣬��λms���Ǹ�����ֵ���͸����²��ԣ�678������;�߸�����,Ӧ����ֵԽ�����Խ�á�
#define _MS_AHEAD (8)	

	int ret;
	RemotePlayer_t *player = (RemotePlayer_t *)param;
	int IFrameAwayCnt = 0;	//��֡��ţ�����I֡�ľ���
	AV_UNPKT pack;
	BOOL bPlayVideo;
	int bType;	//��Ƶ֡����
	U64 msNow;			//��ǰʱ��
	int	msLast = (int)player->msLast;
	//THREAD_REGISTER();

	//��O֡
	_remote_SendInfo(player);

	while(player->running)
	{
		Common_GetMs(&msNow);
		//printf("now:%d, want:%d\n",(U32)msNow, (U32)player->msWant);
		//������ĳ��λ�ã���Ҫ��λ
		if (player->setPosInFrame)
		{
			int frame = player->setPosInFrame;
			pack.iType = JVS_UPKT_VIDEO;
			pack.iSampleId = frame;
			MP4_ReadIFrame(player->fp, &player->mp4Info, &pack, TRUE);

			//printf("set pos,Iframe at:%d\n", pack.iSampleId);
			player->frameDecoded = pack.iSampleId-1;
			player->frameDecodedA = (player->frameDecoded * 25) / player->frameRate;//������Ƶ֡��

			player->setPosInFrame = 0;
		}

		//�Ѿ����ﲥ�ŵ�ʱ��
		if(msNow + _MS_AHEAD >= player->msWant)
		{
			//��ȡ����
			if (player->skipflag == VDEC_SPEED_SKIP_ONLY_I)
			{
				//��ȡI֡
				pack.iType = JVS_UPKT_VIDEO;
				pack.iSampleId = player->frameDecoded + 1;
				ret = MP4_ReadIFrame(player->fp, &player->mp4Info, &pack, TRUE);
				if(pack.iSize)
				{
					player->frameDecoded = pack.iSampleId;
					player->frameDecodedA = (player->frameDecoded * 25) / player->frameRate;
				}
			}
			else
			{
				//��ȡ��Ƶ֡
				pack.iType = JVS_UPKT_VIDEO;
				pack.iSampleId = player->frameDecoded + 1;

				ret = MP4_ReadOneFrame(player->fp, &player->mp4Info, &pack);
				if(pack.iSize)
					player->frameDecoded++;
			}

			if(!pack.iSize)
			{
				player->bFileOver = TRUE;

				break;	//�˳�����
			}

			//�ж��Ƿ�Ҫ������
			bPlayVideo = TRUE;
			{
				if (pack.bKeyFrame)
				{
					IFrameAwayCnt = 0;
					bType = JVN_DATA_I;
				}
				else
				{
					IFrameAwayCnt++;
					bType = JVN_DATA_P;
				}

				//����֡
				if (player->skipflag > VDEC_SPEED_SKIP_ONLY_I)	//����VDEC_SPEED_SKIP_ONLY_I������֡
				{
					if (IFrameAwayCnt & (player->skipflag-1))
					{
						//continue;
						bPlayVideo = FALSE;
					}
				}
			}

			//������Ƶ֡
			if(bPlayVideo&&ret)
			{
				//��ȡʱ��
				player->msLast = msNow;
	            if(player->msWant != 0xffffffffffffffffULL);
				    player->msWant = player->msLast + player->nTimeSlice;

				//����
#ifdef YST_SVR_SUPPORT
				if(gp.bNeedYST)
				{
					JVN_SendPlay(player->clientId, JVN_RSP_PLAYDATA, player->nConnectionType, bType, pack.pData, pack.iSize, 0, 0, 0, pack.iSampleTime / 1000 + msLast, 0);
				}
#endif
			}
		}
		
		//������Ƶ֡
		//�ж��Ƿ���Ҫ����Ƶ�������Ѿ����ŵ���Ƶ֡������Ӧ�ò��ŵ���Ƶ֡����(��Ƶ�̶���25֡)
		if(player->playStatus == EN_PLAYER_PLAY && player->mp4Info.iNumAudioSamples > 0)
		{
			if(msNow + _MS_AHEAD >= player->msWantA)
			{
				//printf("want%d,cur%d\n", cnt, player->frameDecodedA);
				pack.iType = JVS_UPKT_AUDIO;
				pack.iSampleId = player->frameDecodedA + 1;
				ret = MP4_ReadOneFrame(player->fp, &player->mp4Info, &pack);
				if (ret && pack.iSize)
				{
					player->frameDecodedA++;

					//����
#ifdef YST_SVR_SUPPORT
					if(gp.bNeedYST)
					{
						JVN_SendPlay(player->clientId, JVN_RSP_PLAYDATA, player->nConnectionType, JVN_DATA_A, pack.pData, pack.iSize, 0, 0, 0, pack.iSampleTime, 0);
					}
#endif
				}
				player->msWantA = msNow + 40;
			}
		}

		Common_SleepMs(1);
	}
	if (player->fp)
	{
		MP4_Close_Read(player->fp, &player->mp4Info);
		player->fp = NULL;
	}
	
	//�ֿ��������ʱ�������ֿط���JVN_RSP_PLAYOVER���Լ�������ϲŷ���
	if(!player->reqEnd)
	{
#ifdef YST_SVR_SUPPORT
		if(gp.bNeedYST)
		{
			JVN_SendPlay(player->clientId, JVN_RSP_PLAYOVER, player->nConnectionType, 0, NULL, 0, 0, 0, 0, 0, 0);
		}
#endif
	}

	printf("play over\n");
	int i;
	
	return ;
}

//�ر�Զ�̻طŲ����������ʱ����ط�
int Remote_Player_Time_Destroy(S32 clientId)
{
	RemotePlayer_t *player = _remote_FindPlayer(clientId);
	if(!player)
		return -1;
	
	player->running = FALSE;
	//player->reqEnd = TRUE;

	//�Է��̻߳�û���˳�
	usleep(50*1000);
	return 0;	
}

static int Remote_Player_Time_Create(RemotePlayer_t *player, char* fname,int starttime,int timeoffset)
{
#if SD_RECORD_SUPPORT
	int ret=-1;
	int len,i,lenFlag;

	Printf("client:%d, name:%s, type:%d, starttime:%d, timeoffset:%d\n", 
		player->clientId, fname, player->nConnectionType, starttime, timeoffset);
	if (!fname)
	{
		printf("ERROR: Bad Param, fname: %s\n", fname);
		return -1;
	}

	//ֻ�ط�mp4�ļ�
	len = strlen(fname);
	lenFlag = strlen(FILE_FLAG);
	if(strcmp(&fname[len-lenFlag], FILE_FLAG) != 0)
	{
		printf("only support mp4 file\n");
		return -1;
	}

	if(access(fname, F_OK) != 0)
	{
		printf("mp4 file is not exist\n");
		return -1;
	}

	
	//��ʼ��player
	player->speed = 1.0;
	player->running = TRUE;
	player->frameRate = 25;//������һ����ʼֵ
	strcpy(player->fname, fname);
	
	player->playStatus = EN_PLAYER_PLAY;
	player->thread = 0;
	player->bMp4 = TRUE;
	player->bFileOver = FALSE;
	player->frameDecoded = 0;
	player->frameDecodedA = 0;
	player->bFileOver = FALSE;
	//player->I_IndexList = NULL;	//���ã�д0
	//player->I_Cnt = 0;			//���ã�д0
	
	player->msLast = starttime;
	
	ret = _remote_ReadFileInfo_mp4(player);
	if(ret < 0)
	{
		printf("read file info err\n");
		return -1;
	}

	player->setPosInFrame = timeoffset * player->frameRate;

	_remote_player_time_mp4((void *)player);
#endif
	return 0;
}

static void* _remote_player_time_pthread(void* param)
{
	int ret = 0;
	int i = 0;
	int indexPlayer = 0;
	RemotePlayer_t *player = NULL;
	char strT[7]={0};
	char strPath[128]={0};
	char strFolder[10]={0};
	U8	TypePlay;		//��������¼���ļ�����
	U32 ChnPlay; 		//��������¼���ļ�ͨ��

	struct tm tm1;
	time_t time1;

	char chPathTime[16] = {0};
	char chFilePath[64] = {0};
	char chStartTime[32] = {0};
	
	REPLAY_INFO* replay_info = (REPLAY_INFO*)param;

	int clientId = replay_info->clientId;
	int connectType = replay_info->connectType;
	int play_time = replay_info->play_time;

	pthreadinfo_add((char *)__func__);

	//gmtime����ȥ��ʱ��
	localtime_r((time_t*)(&play_time),&tm1);
	sprintf(chPathTime,"%4.4d%2.2d%2.2d",tm1.tm_year + 1900, tm1.tm_mon + 1,tm1.tm_mday);
	Printf("get pathtime : %s \n",chPathTime);
	
	sprintf(strPath, "%s", mstorage_get_cur_recpath(NULL, 0));
	if(access(strPath, F_OK))
	{
		return NULL;
	}
	memset(strFolder, 0, 10);
	strncat(strFolder, chPathTime, 8);
	strcat(strPath, strFolder);
	
	if(access(strPath, F_OK))
	{
		Printf("Path:%s can't access\n", strPath);
		return NULL;
	}

	player = _remote_FindPlayer(clientId);
	if(player != NULL)
	{
		printf("ERROR: client:%d already playing\n", clientId);
		goto play_err;
	}
	
	player = (RemotePlayer_t *)malloc(sizeof(RemotePlayer_t));
	if(player == NULL)
	{
		printf("ERROR: no momery\n");
		goto play_err;
	}

	//׼����player���ٷ���ȫ������
	pthread_mutex_lock(&sMutexRemotePlay);
	for(i=0; i<MAX_REMOTE_PLAY; i++)
	{
		if(sRemotePlay[i] == NULL)
		{
			sRemotePlay[i] = player;
			indexPlayer = i;
			break;
		}
	}
	pthread_mutex_unlock(&sMutexRemotePlay);
	if(i >= MAX_REMOTE_PLAY)
	{
		printf("no more player\n");
		free(player);
		goto play_err;
	}
	
	memset(player, 0, sizeof(RemotePlayer_t));
	player->clientId = clientId;
	player->nConnectionType = connectType;
	player->running = TRUE;
	
	DIR *pDir	= opendir(strPath);
	struct dirent *pDirent	= NULL;
	while((pDirent=readdir(pDir)) != NULL && player->running == TRUE)
	{
		//�����������������ͺ�ͨ��
		if(!strcmp(FILE_FLAG, pDirent->d_name+strlen(pDirent->d_name)-strlen(FILE_FLAG)))
		{
			sscanf(pDirent->d_name, "%c%2d%6s", &TypePlay, &ChnPlay, strT);
			sprintf(chStartTime,"%s%s",chPathTime,strT);
			sscanf(chStartTime, "%4d%2d%2d%2d%2d%2d",      
			      &tm1.tm_year,   
			      &tm1.tm_mon,   
			      &tm1.tm_mday,   
			      &tm1.tm_hour,   
			      &tm1.tm_min,  
			      &tm1.tm_sec);  

			tm1.tm_year -= 1900;  
			tm1.tm_mon --;  
			tm1.tm_isdst=-1;  
			time1 = mktime(&tm1);
	
			sprintf(chFilePath,"%s/%s",strPath,pDirent->d_name);
			struct stat statbuf;
			stat(chFilePath,&statbuf);

			ret = 0;
			//printf("remote play time: %d,%d,%d,%d\n\n",
			//	(int)time1, (int)play_time,(int)statbuf.st_mtime,player->running);
			if (time1 <= play_time && play_time < statbuf.st_mtime)
			{
				ret = Remote_Player_Time_Create(player,chFilePath,time1,play_time-time1);
			}
			else if(play_time < statbuf.st_mtime)
			{
				ret = Remote_Player_Time_Create(player,chFilePath,time1,0);
			}
			if (ret != 0)
			{
				free(player);
				closedir(pDir);
				sRemotePlay[indexPlayer] = NULL;
				goto play_err;
			}
		}
	}

	pthread_mutex_lock(&sMutexRemotePlay);
	for(i=0; i<MAX_REMOTE_PLAY; i++)
	{
		if(sRemotePlay[i] == player)
		{
			free(player);
			sRemotePlay[i] = NULL;
			break;
		}
	}
	pthread_mutex_unlock(&sMutexRemotePlay);

	closedir(pDir);

	return NULL;

play_err:
	do {
		RemotePlayer_t failPlayer;
		memset(&failPlayer, 0, sizeof(RemotePlayer_t));
		failPlayer.clientId = clientId;
		failPlayer.nConnectionType = connectType;
		_remote_player_send_error((void *)&failPlayer);
	} while(0);
	
	return NULL;
	
}

void Remote_Player_Time(S32 clientId, int nConnectionType, unsigned int play_time,unsigned long long base_time)
{
	int i = 0;

	pthread_t pid;

	static REPLAY_INFO replay_info;
	
	replay_info.clientId = clientId;
	replay_info.connectType= nConnectionType;
	replay_info.play_time = play_time;
	replay_info.base_time = base_time;
	if(0 != pthread_create(&pid,NULL,_remote_player_time_pthread,(void*)&replay_info))
	{
		printf("create replay thread failed!!!! \n\n");
		return;
	}
	pthread_detach(pid);
}
#endif
