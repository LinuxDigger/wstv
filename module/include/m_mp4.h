#ifndef _M_MP4_H_
#define _M_MP4_H_

#include "Jmp4pkg.h"
#include "jv_common.h"

#define _FOPEN_BUF_SIZE (64*1024)

typedef struct
{
	unsigned int 		iDataStart;					//��ȡû��װ��ϵ��ļ���Ҫ�õ���
	
	BOOL			bNormal;						// TRUE, ��ʾ����ɷ�װ,FALSE, ��װ���ڽ�����
	
	BOOL				bHasVideo;					// �Ƿ�����Ƶ
	BOOL				bHasAudio;					// �Ƿ�����Ƶ

	unsigned int		iFrameWidth;				// ��Ƶ���
	unsigned int		iFrameHeight;				// ��Ƶ�߶�
	double				dFrameRate;					// ֡��
	unsigned int		iNumVideoSamples;			// VideoSample����
	unsigned int		iNumAudioSamples;			// AudioSample����
	MP4_AVCC			avcC;						// mp4 sps\pps

	char				szVideoMediaDataName[8];	// ��Ƶ�������� "avc1"
	char				szAudioMediaDataName[8];	// ��Ƶ�������� "samr" "alaw" "ulaw"	
	AUDIO_TYPE_E		nMp4AudioType;
	VIDEO_TYPE_E		enMp4VideoType;
}MP4_READ_INFO;


//��MP4���ļ�
//strFile			:�ļ���
//pInfo 			:�ļ�����Ϣ
//return NULL ʧ�ܣ�>0 �ɹ�
void *MP4_Open_Read(char *strFile, MP4_READ_INFO *pInfo);

//�ر�MP4���ļ�
void MP4_Close_Read(void *handle, MP4_READ_INFO *pInfo);

//��ȡMP4�ļ�I֡
//handle  MP4�ļ��򿪵ľ��
//pInfo   �ļ���Ϣ
//pPack   ���h264��amr֡���ݽṹ�� 
//bForword  ��ʾ���ҷ���, TRUE��ǰ(����), FALSE, ���(����) 
//return    FALSEʧ��  TRUE �ɹ�
BOOL MP4_ReadIFrame(void *handle, MP4_READ_INFO *pInfo, PAV_UNPKT pPack, BOOL bForword);

//��ȡMP4�ļ�һ֡����
//handle  MP4�ļ��򿪵ľ��
//pInfo   �ļ���Ϣ
//pPack   ���h264��amr֡���ݽṹ�� 
//return    ���ڽ��з�װ�� ����0��ʾ�����ݿɶ��������ʾ�����ݶ���������ɷ�װ�� FALSEʧ��  TRUE �ɹ�
BOOL MP4_ReadOneFrame(void *handle, MP4_READ_INFO *pInfo, PAV_UNPKT pPack);

//��ȡ��iFrameVideoͬ������Ƶ֡��
int MP4_GetSyncAudioFrame(void *handle, MP4_READ_INFO *pInfo, int iFrameVideo);

#endif

