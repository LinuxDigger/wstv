#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <jv_ao.h>
#include <jv_ai.h>
#include <utl_ifconfig.h>
#include <utl_audio.h>
#include "maudio.h"
#include "mioctrl.h"
#include "mipcinfo.h"
#include "SYSFuncs.h"

#if 0
#define AUDIO_DEBUG printf
#else
#define AUDIO_DEBUG
#endif

static jv_thread_group_t audio_group;
static BOOL bAudioEnding = FALSE;
static BOOL bPreRunning = FALSE;

pthread_mutex_t soundmode_mutex;

static void _speak_process(void *param)
{
	char *fname=(char *)param;
	int ret=0;
	int nFrameCnt = 0;
	int minPlayTime = 0;		// ��С����ʱ�䣬ms

	pthreadinfo_add((char *)__func__);
	jv_ao_mute(0);//open speaker
	int fd = open(fname,O_RDONLY);
	if(fd < 0)
	{
		jv_ao_mute(1);
		printf("open audio file failed!\n");
		return;
	}

	int len = 0;
	jv_audio_frame_t frame;
	static BOOL bFlag = FALSE;
	jv_audio_attr_t aoAttr;
	jv_ai_get_attr(0, &aoAttr);
	jv_audio_enc_type_e encType;
	
	//check audio enc
	if(!bFlag && (strstr(fname,"pcm") == NULL))
	{
		encType = aoAttr.encType;
		if (aoAttr.encType != JV_AUDIO_ENC_G711_A)
		{
			aoAttr.encType = JV_AUDIO_ENC_G711_A;
			jv_ao_stop(0);
			jv_ao_start(0, &aoAttr);
		}

		bFlag = TRUE;
	}
	int Datalen = 0;
	Datalen = (strstr(fname,"pcm") == NULL) ? 320 : 882;

	while(audio_group.running)
	{
		len = read(fd,frame.aData,Datalen);
		if(len <= 0)
		{
			break;
		}
		frame.u32Len = len;
		++nFrameCnt;

		ret = jv_ao_send_frame(0, &frame);
		if(ret < 0)
		{
			break;
		}
	}
	close(fd);

	minPlayTime = nFrameCnt * ((strstr(fname, "pcm") == NULL) ? 40 : 10);
	/*printf("read finish, frame: %d, min play time: %d\n", nFrameCnt, minPlayTime);*/

#if (defined PLATFORM_hi3518EV200) ||  (defined PLATFORM_hi3516EV100) // 18EV200/16EV100 SDK�б������Ҫ���⴦��
	//adec��Ҫ��֪һ����Ƶ�������
	jv_audio_attr_t ao_attr;
	jv_ao_get_attr(0, &ao_attr);
	if (ao_attr.encType != JV_AUDIO_ENC_PCM)
	{
		jv_ao_adec_end();
	}
#endif

	/*�ȴ���Ƶ���������ɻ��߳�ʱ*/
	int getAOStatusCnt = 0;
	jv_ao_status aoStatus;

#if (defined PLATFORM_hi3518EV200) ||  (defined PLATFORM_hi3516EV100) // 18EV200/16EV100 SDK�б������Ҫ���⴦��
	// �ȴ����ſ�ʼ
	do
	{
		usleep(10 * 1000);
		jv_ao_get_status(0, &aoStatus);
		getAOStatusCnt++;
		if (getAOStatusCnt > 80)
			break;
	}
	while (aoStatus.aoBufBusyNum == 0);
#endif

	// sleep��̲���ʱ�䣬ȷ����Ƶ�ܲ�������
	usleep(minPlayTime * 1000);

	// �ȴ����Ž���
	getAOStatusCnt = 0;
	do
	{
		usleep(100 * 1000);
		jv_ao_get_status(0, &aoStatus);
		getAOStatusCnt++;
		if (getAOStatusCnt > 100)
		{
			AUDIO_DEBUG("jv_ao_get_status timeout\n");
			break;
		}
	}
	while (aoStatus.aoBufBusyNum != 0);

	if (speakerowerStatus != JV_SPEAKER_OWER_ALARMING)	//aoAttr.encType != encType && 
	{
		jv_ai_get_attr(0, &aoAttr);

		jv_ao_stop(0);
		jv_ao_start(0, &aoAttr);
		bFlag = FALSE;
	}

	//�������������������ã���Ҫ�رչ���
	if(speakerowerStatus <= JV_SPEAKER_OWER_ALARMING || speakerowerStatus == JV_SPEAKER_OWER_VOICE)
		jv_ao_mute(1);//close speaker

	bAudioEnding = TRUE;
	bPreRunning  = FALSE;
	
	audio_group.running = FALSE;
	
	AUDIO_DEBUG("_speak_process exit\n");
}

void maudio_setDefault_gain(jv_audio_attr_t* ai_attr)
{
	ai_attr->level = 0x0a;
	ai_attr->soundfile_level = 0x0c;
	ai_attr->micGain = 0x0A;		//40db
	ai_attr->micGainTalk = 0x05;	//30db

	return;	
}

/**
*@brief 
*��ʼ������ʼ���߳���,����AI��Ƶ�������ʹ����Ƶ���룬
*����AO��Ƶ�������ʹ����Ƶ���
*/
int maudio_init()
{
	jv_audio_attr_t ai_attr;
	jv_audio_attr_t ao_attr;
	jv_ai_get_attr(0, &ai_attr);

	//��ע�⣬��Ƶ�������Ϳ�����SYSFuncs.c�б��޸�
	ai_attr.encType = JV_AUDIO_ENC_G711_A;
	ai_attr.bitWidth = JV_AUDIO_BIT_WIDTH_16;
	ai_attr.sampleRate = JV_AUDIO_SAMPLE_RATE_8000;
	ao_attr.encType = JV_AUDIO_ENC_G711_A;
	ao_attr.bitWidth = JV_AUDIO_BIT_WIDTH_16;
	ao_attr.sampleRate = JV_AUDIO_SAMPLE_RATE_8000;
	
	maudio_setDefault_gain(&ai_attr);

	printf("Start AI and AO\n");
	memcpy(&ao_attr, &ai_attr, sizeof(jv_audio_attr_t));
	
	jv_ai_start(0, &ai_attr);
	jv_ao_start(0, &ao_attr);
	
	pthread_mutex_init(&audio_group.mutex, NULL);
	pthread_mutex_init(&soundmode_mutex, NULL);
	audio_group.running = FALSE;
	AUDIO_DEBUG("maudio_init ok\n");

	return 0;
}
/**
*@brief �����������߳���,��ֹ��Ƶ�������
*/
int maudio_deinit(int aiChannelId, int aoChannelId)
{
	if(aiChannelId != -1)
		jv_ai_stop(aiChannelId);
	if(aoChannelId != -1)
		jv_ao_stop(aoChannelId);
	pthread_mutex_destroy(&audio_group.mutex);
	AUDIO_DEBUG("maudio_deinit ok\n");
	return 0;
}

/**
*@brief �������Թ�����ʾ���ļ���
*/

char*	voice_lan[LANGUAGE_MAX] = {"cn","en"};

void maudio_selFile_byLanguage(char *fname)
{
	char tempname1[128] = {0};
	char tempname2[128] = {0};
	
	char* pos = strrchr(fname,'/');

	strcpy(tempname2,pos+1);
	
	strncpy(tempname1,fname,pos-fname+1);

	ImportantCfg_t im;
	memset(&im, 0, sizeof(ImportantCfg_t));
	
	ReadImportantInfo(&im);

	sprintf(fname,"%s%s/%s",tempname1,voice_lan[im.nLanguage],tempname2);
}

/**
*@brief ����PCM��Ƶ�ļ�
*/
int maudio_speaker(const char *fname, BOOL bTransByLan,BOOL bPlayNow, BOOL bBlock)
{
	pthread_mutex_lock(&audio_group.mutex);
	jv_audio_attr_t ao_attr;
	jv_ao_get_attr(0, &ao_attr);
	static char tempFname[128] = {0};
	if(NULL == fname)
	{
		pthread_mutex_unlock(&audio_group.mutex);
		return -1;
	}
	strcpy(tempFname,fname);

	if(bTransByLan)
	{	
		maudio_selFile_byLanguage(tempFname);	
	}

	if(access(tempFname, 0) != 0)
	{
		pthread_mutex_unlock(&audio_group.mutex);
		return -1;
	}
	if(audio_group.running)
	{
//	 	if(bPlayNow)
	 	{
			AUDIO_DEBUG("stop maudio_speaker\n");
			audio_group.running = FALSE;
	 	}
		pthread_join(audio_group.thread, NULL);
	}
	
	if(ao_attr.soundfile_level != -1)
	{
		jv_ao_ctrl(ao_attr.soundfile_level);
	}
	AUDIO_DEBUG("start maudio_speaker\n");
	audio_group.running = TRUE;
	bAudioEnding = FALSE;
	pthread_create_normal(&audio_group.thread, NULL, (void *)_speak_process, (void*)tempFname);
	if (bBlock == FALSE)
	{
		pthread_detach(audio_group.thread);
	}
	else
	{
		pthread_join(audio_group.thread, NULL);
	}
	pthread_mutex_unlock(&audio_group.mutex);

	if(ao_attr.soundfile_level != -1)
	{
		jv_ao_ctrl(ao_attr.level);
	}
	return 0;
}

int maudio_speaker_stop()
{
	if(audio_group.running)
	{
		audio_group.running = FALSE;

		bPreRunning = TRUE;
		
		pthread_join(audio_group.thread, NULL);

	}

	return 0;
}

BOOL maudio_speaker_GetEndingFlag()
{
	if(!bPreRunning)
		return TRUE;
	
	return bAudioEnding;
}

void maudio_readfiletoao(char* FilePath)
{
	//return;				//�����ļ���δ׼���ã���ʱ�Ȳ���������
	jv_audio_frame_t frame;

	int fd = -1;
	char chBuf[640] = {0};
	int len = 0;
	
	fd = open(FilePath,O_RDONLY);
	if(fd < 0)	
	{	 
		printf("open file %s failed!\n",FilePath);
		return;
	}

	jv_audio_attr_t ao_attr;
	jv_ao_get_attr(0, &ao_attr);
	if(ao_attr.soundfile_level != -1)
	{
		jv_ao_ctrl(ao_attr.soundfile_level);
	}

	while(1)
	{
		len = read(fd,chBuf, 320);
		if(len <= 0)
		{			
			break;
		}
		
		memcpy(frame.aData, chBuf, 320);
		frame.u32Len = 320;
		//���Ž��յ�����Ƶ֡
		jv_ao_send_frame(0,&frame);			
	}

	close(fd);

	/*�ȴ���Ƶ���������ɻ��߳�ʱ*/
	int getAOStatusCnt = 0;
	jv_ao_status aoStatus;
	jv_ao_get_status(0, &aoStatus);
	while(aoStatus.aoBufBusyNum > 1)
	{
		jv_ao_get_status(0, &aoStatus);
		getAOStatusCnt++;
		if (getAOStatusCnt > 100)
		{
			AUDIO_DEBUG("jv_ao_get_status timeout\n");
			break;
		}
		usleep(100*1000);
	}

	if(ao_attr.soundfile_level != -1)
	{
		jv_ao_ctrl(ao_attr.level);
	}
}

/*******************************
*��������AI ��AO,��Ϊ�������ø��������ŵı��뷽ʽ�Ͳ����ʲ�һ��
*mode: 1 (��������)  2 (��������)
********************************/
void maudio_resetAIAO_mode(int mode)
{
	if (!hwinfo.bSupportVoiceConf)
		return;

	pthread_mutex_lock(&soundmode_mutex);
	static int curMode = 0;
	if(curMode == mode)
	{
		pthread_mutex_unlock(&soundmode_mutex);
		return;
	}

	jv_ai_stop(0);
	jv_ao_stop(0);

	jv_audio_sample_rate_e wave_hz = JV_AUDIO_SAMPLE_RATE_24000;
#ifdef SOUND_WAVE_DEC_SUPPORT
	if(hwinfo.bNewVoiceDec)
		wave_hz = JV_AUDIO_SAMPLE_RATE_44100;
#endif
	
	jv_audio_attr_t aiAttr;
	jv_ai_get_attr(0,&aiAttr);
	if(1 == mode)
		aiAttr.encType = JV_AUDIO_ENC_PCM;
	else
		aiAttr.encType = JV_AUDIO_ENC_G711_A;
	aiAttr.bitWidth = JV_AUDIO_BIT_WIDTH_16;
	if(1 == mode)
		aiAttr.sampleRate = wave_hz;
	else
		aiAttr.sampleRate = JV_AUDIO_SAMPLE_RATE_8000;
	jv_ai_start(0, &aiAttr);
	
	jv_audio_attr_t aoAttr;
	jv_ai_get_attr(0,&aoAttr);
	if(1 == mode)
		aoAttr.encType = JV_AUDIO_ENC_PCM;
	else
		aoAttr.encType = JV_AUDIO_ENC_G711_A;
	aoAttr.bitWidth = JV_AUDIO_BIT_WIDTH_16;

	if(1 == mode)
		aoAttr.sampleRate = wave_hz;
	else
		aoAttr.sampleRate = JV_AUDIO_SAMPLE_RATE_8000;
	jv_ao_start(0, &aoAttr);

	curMode = mode;
    pthread_mutex_unlock(&soundmode_mutex);

}



