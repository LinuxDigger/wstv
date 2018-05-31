#include "sctrl.h"
#include "mstream.h"
#include "jv_sensor.h"
#include "mipcinfo.h"

static SCTRLINFO SctrlInfo;

void SctrlSetParam(SCTRLINFO *info)
{
	memcpy((char *)&SctrlInfo, (char *)info, sizeof(SCTRLINFO));
}

SCTRLINFO *SctrlGetParam(SCTRLINFO *info)
{
	if (info)
	{
		memcpy((char *)info, (char *)&SctrlInfo, sizeof(SCTRLINFO));
	}
	return &SctrlInfo;
}

//��ȡ�ֻ������ͨ��
U32 SctrlGetMOChannel()
{
	U32 nCh=0;

	if (hwinfo.bHomeIPC)
	{
		switch(SctrlInfo.nPictureType)
		{
		case MOBILE_QUALITY_HIGH:
			nCh = 0;
			break;
		case MOBILE_QUALITY_MIDDLE:
			nCh = 0;
			break;
		case MOBILE_QUALITY_LOW:
			nCh = 1;
			break;
		default:
			break;
		}
	}
	
	return nCh;
}

void SctrlMakeHeader(U32 nChannel, JVS_FILE_HEADER_EX *exHeader)
{
	JVS_FILE_HEADER jHeader={0};
	mstream_attr_t attr;
	jv_audio_attr_t ai;

	jv_ai_get_attr(0, &ai);
	jHeader.audioType = ai.encType;
	mstream_get_running_param(nChannel, &attr);
	jHeader.nFlag			= STARTCODE;
	jHeader.nFrameWidth 	= attr.width;
	jHeader.nFrameHeight	= attr.height;
	jvstream_ability_t ability;
	jv_stream_get_ability(0, &ability);
	if (ability.maxFramerate == 30)
		jHeader.nVideoFormat	= 1;
	else
		jHeader.nVideoFormat	= 0;



	memset(exHeader, 0, sizeof(JVS_FILE_HEADER_EX));
	memcpy(exHeader->ucOldHeader, &jHeader, sizeof(JVS_FILE_HEADER));

	//�ṹ����Ϣ
	strncpy((char *)exHeader->ucHeader, "JFH",3);	//�ṹ��ʶ���룬����Ϊ��J','F','H'
	exHeader->ucVersion = 1;			//�ṹ��汾�ţ���ǰ�汾Ϊ1
	//�豸���
	exHeader->wDeviceType 			= JVS_DEVICETYPE_IPC;		//�豸����
	//��Ƶ����
	if(attr.vencType == JV_PT_H264)
		exHeader->wVideoCodecID 		= JVS_VIDEOCODECTYPE_H264;	//��Ƶ��������
	else if(attr.vencType == JV_PT_H265)
		exHeader->wVideoCodecID 		= JVS_VIDEOCODECTYPE_H265;	//��Ƶ��������
	if(attr.bAudioEn)
		exHeader->wVideoDataType 		= JVS_VIDEODATATYPE_VIDEOANDAUDIO;	//��������
	else
		exHeader->wVideoDataType 		= JVS_VIDEODATATYPE_VIDEO;			//��������
	exHeader->wVideoFormat 			= JVS_VIDEOFORMAT_PAL;				//��Ƶģʽ
	exHeader->wVideoWidth 			= attr.width;						//��Ƶ��
	exHeader->wVideoHeight 			= attr.height;						//��Ƶ��

	float cur, normal;
	if (jv_sensor_get_b_low_frame(0, &cur, &normal))
	{
		if (attr.framerate < cur)
			exHeader->wFrameRateNum = attr.framerate;
		else
			exHeader->wFrameRateNum = cur;
	}
	else
	{
		exHeader->wFrameRateNum = attr.framerate;				//֡�ʷ���
	}
	exHeader->wFrameRateDen 		= 1;						//֡�ʷ�ĸ
#if 1//SUPPORT_MP4
	//��Ƶ����
	unsigned int audioType;
	jv_audio_attr_t audioAttr;
	jv_ai_get_attr(0, &audioAttr);
	switch(audioAttr.encType)
	{
	default:
	case JV_AUDIO_ENC_PCM:
		audioType = JVS_AUDIOCODECTYPE_PCM;
		break;
	case JV_AUDIO_ENC_G711_A:
		audioType = JVS_AUDIOCODECTYPE_G711_alaw;
		break;
	case JV_AUDIO_ENC_G711_U:
		audioType = JVS_AUDIOCODECTYPE_G711_ulaw;
		break;
	}

	exHeader->wAudioCodecID 		= audioType;	//��Ƶ�����ʽ
	exHeader->wAudioSampleRate 		= 8000;						//��Ƶ������
	exHeader->wAudioChannels 		= 1;						//��Ƶ������
	exHeader->wAudioBits 			= 16;						//��Ƶ����λ��
	//¼�����
	exHeader->dwRecFileTotalFrames 	= 0;						//¼����֡��
	exHeader->wRecFileType =	JVS_RECFILETYPE_MP4;
	exHeader->ucGrpcVersion = 1;
#endif

	//printf("channel %d : wVideoWidth=%d, wVideoHeight=%d, wFrameRateNum=%d\n", nChannel, exHeader->wVideoWidth, exHeader->wVideoHeight, exHeader->wFrameRateNum);
}

#ifndef YST_SVR_SUPPORT
void MT_TRY_SendChatData(int nChannel,int nClientID,unsigned char uchType,unsigned char *pBuffer,int nSize)
{
	
}
#endif

//ˢ��web����״̬
int webserver_flush()
{
	utl_system("killall thttpd");
	ipcinfo_t info;
	ipcinfo_get_param(&info);
	if (info.bWebServer)
	{
		utl_system("/progs/bin/thttpd -d /progs/html/ -c \"/cgi-bin/*|/onvif/*\" -u root -T utf-8&");
	}
	return 0;
}

char *SctrlGetKeyValue(const char *pBuffer, const char *key, char *valueBuf, int maxValueBuf)
{
	int i = 0;
	char *ptmpItem;
	char *pValue = NULL;  //gyd
	int klen = strlen(key);
	while (pBuffer && *pBuffer)     //gyd
	{
		if (strncmp(pBuffer, key, klen) == 0)
		{
			if (pBuffer[klen] == '=')
			{
				int i = 0;
				pBuffer += klen+1;
				while(*pBuffer && *pBuffer != ';' && i < maxValueBuf)
					valueBuf[i++] = *pBuffer++;
				valueBuf[i] = '\0';
//				printf("find key: %s, value: %s\n", key, valueBuf);
				return valueBuf;
			}
		}
		pBuffer = strchr(pBuffer, ';');
		if (pBuffer && pBuffer[0])
			pBuffer++;
	}
	return NULL;
}

int SctrlGetStreamInfo(S32 nChannel, SctrlStreamInfo* Info)
{
	if (!Info)		return -1;

	mstream_attr_t attr;
	jv_audio_attr_t ai;

	mstream_get_running_param(nChannel, &attr);
	jv_ai_get_attr(0, &ai);

	Info->width = attr.width;
	Info->height = attr.height;
	Info->framerate = attr.framerate;
	Info->vencType = attr.vencType;
	Info->audioEn = attr.bAudioEn;
	Info->aencType = ai.encType;

	float cur, normal;
	if (jv_sensor_get_b_low_frame(0, &cur, &normal))
	{
		if (cur < Info->framerate)
			Info->framerate = cur;
	}

	return 0;
}

BOOL SctrlCheckStreamInfoChanged(const SctrlStreamInfo* a, const SctrlStreamInfo* b)
{
	if (
		(a->width != b->width)
		|| (a->height != b->height)
		|| (a->framerate != b->framerate)
		|| (a->vencType != b->vencType)
		|| (a->audioEn != b->audioEn)
		|| (a->aencType != b->aencType)
		)
	{
		return TRUE;
	}

	return FALSE;
}

