#include "mrecord.h"

#include "m_mp4.h"

//��MP4���ļ�
//strFile			:�ļ���
//pInfo 			:�ļ�����Ϣ
//return NULL ʧ�ܣ�>0 �ɹ�
void *MP4_Open_Read(char *strFile, MP4_READ_INFO *pInfo)
{
    #if SD_RECORD_SUPPORT
	//MP4_CHECK mp4Check={0};
	//int ret;
	void *handle;
	char strIndex[128];

	memset(pInfo, 0, sizeof(MP4_READ_INFO));
	
	//��ȡ�����ļ���
	MP4_GetIndexFile(strFile, strIndex);
	//�����رյ�mp4�ļ�
	MP4_INFO stMp4Info;
	handle = JP_OpenUnpkg(strFile, &stMp4Info, _FOPEN_BUF_SIZE);

	if(handle)
	{
		pInfo->iDataStart = 0;							//0��ʾ��װ��ϵ��ļ���>0��ʾ���ڷ�װ���ļ�
		pInfo->bNormal = 1;//mp4Check.bNormal;		
		pInfo->iFrameWidth = stMp4Info.iFrameWidth;		// ��Ƶ���
		pInfo->iFrameHeight = stMp4Info.iFrameHeight;	// ��Ƶ�߶�
		pInfo->dFrameRate = stMp4Info.dFrameRate;		// ֡��
		//memcpy(&pInfo->avcC, &stMp4Info.avcC, sizeof(MP4_AVCC));			// avcc
		pInfo->iNumVideoSamples = stMp4Info.iNumVideoSamples;			// VideoSample����
		pInfo->iNumAudioSamples = stMp4Info.iNumAudioSamples;			// AudioSample����
		strcpy(pInfo->szVideoMediaDataName, stMp4Info.szVideoMediaDataName);	// ��Ƶ�������� "avc1"
		strcpy(pInfo->szAudioMediaDataName, stMp4Info.szAudioMediaDataName);	// ��Ƶ�������� "samr" "alaw" "ulaw"	

		if(pInfo->iNumVideoSamples)						// �Ƿ�����Ƶ
			pInfo->bHasVideo = 1;			
		if(pInfo->iNumAudioSamples)						// �Ƿ�����Ƶ
			pInfo->bHasAudio = 1;
	}
	else
	{
		JP_MP4_INFO jp_mp4Info;
		handle = JP_OpenFile(strFile, 0, strIndex, &jp_mp4Info, _FOPEN_BUF_SIZE);
		
		if(handle)
		{
			pInfo->iDataStart = 0;//mp4Check.iDataStart;		//0��ʾ��װ��ϵ��ļ���>0��ʾ���ڷ�װ���ļ�
			pInfo->bNormal = 0;//mp4Check.bNormal;		
			pInfo->bHasVideo = jp_mp4Info.bHasVideo;		// �Ƿ�����Ƶ
			pInfo->bHasAudio = jp_mp4Info.bHasAudio;		// �Ƿ�����Ƶ
			pInfo->iFrameWidth = jp_mp4Info.iFrameWidth;	// ��Ƶ���
			pInfo->iFrameHeight = jp_mp4Info.iFrameHeight;	// ��Ƶ�߶�
			pInfo->dFrameRate = jp_mp4Info.dFrameRate;		// ֡��
			pInfo->iNumVideoSamples = jp_mp4Info.iNumVideoSamples;
			pInfo->iNumAudioSamples = jp_mp4Info.iNumAudioSamples;
			strcpy(pInfo->szVideoMediaDataName, jp_mp4Info.szVideoMediaDataName);	// ��Ƶ�������� "avc1"
			strcpy(pInfo->szAudioMediaDataName, jp_mp4Info.szAudioMediaDataName);	// ��Ƶ�������� "samr" "alaw" "ulaw"	
			//memcpy(&pInfo->avcC, &jp_mp4Info.avcC, sizeof(MP4_AVCC));// avcc
		}
	}

	//��Ƶ����
	if(strcmp(pInfo->szVideoMediaDataName, "avc1") == 0)
	{
		pInfo->enMp4VideoType = VIDEO_TYPE_H264;
	}
	else if(strcmp(pInfo->szVideoMediaDataName, "hev1") == 0 || strcmp(pInfo->szVideoMediaDataName, "hvc1") == 0)
	{
		pInfo->enMp4VideoType = VIDEO_TYPE_H265;
	}
	else
	{
		pInfo->enMp4VideoType = VIDEO_TYPE_UNKNOWN;
	}

	//��Ƶ����
	if(strcmp(pInfo->szAudioMediaDataName, "alaw") == 0)
	{
		pInfo->nMp4AudioType = AUDIO_TYPE_G711_A;
	}
	else if(strcmp(pInfo->szAudioMediaDataName, "ulaw") == 0)
	{
		pInfo->nMp4AudioType = AUDIO_TYPE_G711_U;
	}
	else
	{
		pInfo->nMp4AudioType = AUDIO_TYPE_UNKNOWN;
	}

	/*printf("MP4_Open_Read:%s, ret %x	Video=%d	Audio=%d\n",strFile,(U32)handle, pInfo->enMp4VideoType, pInfo->nMp4AudioType);*/
	//printf("video:%s,audio:%s\n",pInfo->szVideoMediaDataName, pInfo->szAudioMediaDataName);
	/*printf("iDataStart=%d,w%d h%d fr%f v_nr%d a_nr%d\n",pInfo->iDataStart,pInfo->iFrameWidth,pInfo->iFrameHeight,pInfo->dFrameRate,pInfo->iNumVideoSamples,pInfo->iNumAudioSamples);*/
	return handle;
    #else
    return 0;
    #endif
}

//�ر�MP4���ļ�
void MP4_Close_Read(void *handle, MP4_READ_INFO *pInfo)
{
 #if SD_RECORD_SUPPORT
	if(!pInfo->bNormal)
	{
		JP_CloseFile(handle);
	}
	else
	{
		JP_CloseUnpkg(handle);
	}
#endif
}

//��ȡMP4�ļ�I֡
//handle  MP4�ļ��򿪵ľ��
//pInfo   �ļ���Ϣ
//pPack   ���h264��amr֡���ݽṹ�� 
//bForword  ��ʾ���ҷ���, TRUE��ǰ(����), FALSE, ���(����) 
//return    FALSEʧ��  TRUE �ɹ�
BOOL MP4_ReadIFrame(void *handle, MP4_READ_INFO *pInfo, PAV_UNPKT pPack, BOOL bForword)
{
	//ʹ��mp4��Ķ��ؼ�֡�ӿ�
	BOOL ret = 0;
#if SD_RECORD_SUPPORT
	int nIFrame=0;
	pPack->iType = JVS_UPKT_VIDEO;
	pPack->iSize = 0;

	if(pPack->iSampleId < 0)
		return FALSE;
	
	if(!pInfo->bNormal)
	{
		nIFrame = JP_ReadKeyFrame(handle, pPack->iSampleId, bForword);
		if(nIFrame >= 0)
		{
			pPack->iSampleId = nIFrame;
			ret = JP_ReadFile(handle, pPack);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		nIFrame = JP_UnpkgKeyFrame(handle, pPack->iSampleId, bForword);
		if(nIFrame >= 0)
		{
			pPack->iSampleId = nIFrame;
			ret = JP_UnpkgOneFrame(handle, pPack);
		}
		else
		{
			return FALSE;
		}
	}
#endif
	return ret;
}
//��ȡMP4�ļ�һ֡����
//handle  MP4�ļ��򿪵ľ��
//pInfo   �ļ���Ϣ
//pPack   ���h264��amr֡���ݽṹ�� 
//return    ���ڽ��з�װ�� ����0��ʾ�����ݿɶ��������ʾ�����ݶ���������ɷ�װ�� FALSEʧ��  TRUE �ɹ�
BOOL MP4_ReadOneFrame(void *handle, MP4_READ_INFO *pInfo, PAV_UNPKT pPack)
{
	BOOL ret;
    #if SD_RECORD_SUPPORT
	pPack->iSize = 0;
	if(!pInfo->bNormal)
	{
		ret = JP_ReadFile(handle, pPack);
	}
	else
	{
		ret = JP_UnpkgOneFrame(handle, pPack);
	}

	//if(pPack->iType == JVS_UPKT_AUDIO)
	//	printf("r ret%d,type%d,len%d, %x %x %x %x\n", ret, pPack->iType, pPack->iSize, pPack->pData[0], pPack->pData[1], pPack->pData[2], pPack->pData[3]);
#else
	ret = FALSE;
    #endif
    return ret;
}

//��ȡ��iFrameVideoͬ������Ƶ֡��
int MP4_GetSyncAudioFrame(void *handle, MP4_READ_INFO *pInfo, int iFrameVideo)
{
	#if SD_RECORD_SUPPORT
    int iFrameAudio=0;
	if(pInfo->bNormal)
	{
        iFrameAudio = JP_PkgGetAudioSampleId(handle, iFrameVideo, NULL, NULL);
	}
    else
    {
        iFrameAudio = JP_JdxGetAudioSampleId(handle, iFrameVideo, NULL, NULL);
    }
    return iFrameAudio;
	#endif
	return 0;
}

