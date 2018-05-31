/*
 * maudio.h
 *
 *  Created on: 2014��9��28��
 *      Author: Administrator
 */

#ifndef MAUDIO_H_
#define MAUDIO_H_

#include <jv_common.h>
#include <jv_ai.h>

#define VOICE_BASE_PATH 		"/home/voice/"

#define VOICE_WIFI_SET_FAIL		VOICE_BASE_PATH"wifi_fail.711"
#define VOICE_WIFI_SET_SUCC		VOICE_BASE_PATH"wifi_ok.711"
#define VOICE_NOT_FIND_NET		VOICE_BASE_PATH"notfindnet.711"
#define VOICE_REV_NET_SETTING	VOICE_BASE_PATH"revNetSetting.711"
#define VOICE_ENABLE			VOICE_BASE_PATH"voiceStart.711"
#define VOICE_RESET 			VOICE_BASE_PATH"reset.711"
#define VOICD_WAITSET			VOICE_BASE_PATH"waitset.711"
#define VOICE_NEW_WAITSET		VOICE_BASE_PATH"waitforsetting.pcm"
#define VOICE_ALARMING			VOICE_BASE_PATH"alarming.711"
#define VOICE_STARTPROTECT	VOICE_BASE_PATH"startprotect.711"
#define VOICE_CLOSEPROTECT	VOICE_BASE_PATH"closeprotect.711"
#define VOICE_CHATSTARTTIP			VOICE_BASE_PATH"chattip1.711"
#define VOICE_CHATCLOSETIP			VOICE_BASE_PATH"chattip2.711"
#define VOICE_START_RECORD			VOICE_BASE_PATH"startrecord.711"
#define VOICE_STOP_RECORD			VOICE_BASE_PATH"stoprecord.711"
#define VOICE_CHAT_RING			VOICE_BASE_PATH"chat_ring.711"
#define VOICE_CHAT_HANGUP			VOICE_BASE_PATH"chat_hangup.711"
#define VOICE_CHAT_REFUSE			VOICE_BASE_PATH"chat_refuse.711"
#define VOICE_CHAT_NO_ANSWER			VOICE_BASE_PATH"chat_no_answer.711"


/**
 *@brief 
 *��ʼ������ʼ���߳�
 */
int maudio_init();
/**
*@brief �����������߳���,��ֹ��Ƶ�������
*@param aiChannelId:AI channel ID, if set -1, no AI channel is stopped;
*@param aoChannelId:AO channel ID, if set -1, no AO channel is stopped;
*/
int maudio_deinit(int aiChannelId, int aoChannelId);

void maudio_selFile_byLanguage(char *fname);

/**
*@brief ����711U��Ƶ�ļ�
*@param fname:��Ƶ�ļ���
*@param bPlayNow:�Ƿ���������
*@param bBlock:�Ƿ�����
*/
int maudio_speaker(const char *fname,BOOL bTransByLan, BOOL bPlayNow, BOOL bBlock);

int maudio_speaker_stop();

BOOL maudio_speaker_GetEndingFlag();

/******************************
��ȡ��Ƶ�ļ����ݷ��͵�ao��������������ʽ������pcm����g711
******************************/
void maudio_readfiletoao(char* FilePath);

void maudio_setDefault_gain(jv_audio_attr_t* ai_attr);

void maudio_resetAIAO_mode(int mode);

#endif /* MAUDIO_H_ */
