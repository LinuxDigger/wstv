#ifndef _JV_AI_H_
#define _JV_AI_H_
#include <jv_common.h>
#define JV_MAX_AUDIO_FRAME_LEN		(10*1024)


typedef enum
{
    JV_AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8K samplerate*/
    JV_AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025K samplerate*/
    JV_AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16K samplerate*/
    JV_AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050K samplerate*/
    JV_AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24K samplerate*/
    JV_AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32K samplerate*/
    JV_AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1K samplerate*/
    JV_AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48K samplerate*/
    JV_AUDIO_SAMPLE_RATE_BUTT,
} jv_audio_sample_rate_e;

typedef enum 
{
    JV_AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    JV_AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width*/
    JV_AUDIO_BIT_WIDTH_32  = 2,   /* 32bit width*/
    JV_AUDIO_BIT_WIDTH_BUTT,
} jv_audio_bit_width_e;

typedef enum
{
	JV_AUDIO_ENC_PCM,
	JV_AUDIO_ENC_G711_A,
	JV_AUDIO_ENC_G711_U,
	JV_AUDIO_ENC_G726_16K, // not support
	JV_AUDIO_ENC_G726_24K, // not support
	JV_AUDIO_ENC_G726_32K, // not support
	JV_AUDIO_ENC_G726_40K,
	JV_AUDIO_ENC_ADPCM,
	JV_AUDIO_ENC_AMR,
	JV_AUDIO_ENC_AAC,
}jv_audio_enc_type_e;

typedef enum
{
	JV_SPEAKER_OWER_NONE,
	JV_SPEAKER_OWER_ALARMING,
	JV_SPEAKER_OWER_CHAT,
	JV_SPEAKER_OWER_VOICE,
}jv_speaker_ower_e;

jv_speaker_ower_e speakerowerStatus;

typedef struct
{
	jv_audio_sample_rate_e sampleRate;
	jv_audio_bit_width_e bitWidth;
	jv_audio_enc_type_e encType;
	//��ԭ�нṹ�Ļ����ϣ����Ͽͻ���Ҫ�Ĺ���
	//BOOL echo;  			//�������� (1:enabled; 0:disabled)
	int level;  			//�������� (0-100)
	int soundfile_level;    //���������ļ�����
	int micGain;		  	//��˷�����Microphone gain(0-100)	����������ʹ��
	int micGainTalk;		//��˷����棬ȫ˫���Խ�ʹ��
}jv_audio_attr_t;

typedef struct
{
	unsigned char aData[JV_MAX_AUDIO_FRAME_LEN];
	U64 u64TimeStamp;                /*audio frame timestamp*/
	U32 u32Seq;                      /*audio frame seq*/
	U32 u32Len;                      /*data lenth per channel in frame*/
} jv_audio_frame_t;

#define BUF_SZ (320*2*10+1)
typedef struct
{
    unsigned char buf[BUF_SZ];
    unsigned int readPos;
    unsigned int writePos;
    //unsigned int size;
}BUF_RING;

/**
 *@brief ����Ƶ�ɼ�
 *
 *@param channelid ��Ƶͨ����
 *@param attr ��Ƶͨ�������ò���
 *
 *@return 0 �ɹ�
 *
 */
int jv_ai_start(int channelid, jv_audio_attr_t *attr);

/**
 *@brief ��ȡ��Ƶ�������
 *
 *@param channelid ��Ƶͨ����
 *@param attr ��Ƶͨ�������ò���
 *
 *@return 0 �ɹ�
 *
 */
int jv_ai_get_attr(int channelid, jv_audio_attr_t *attr);

/**
 *@brief ������Ƶ����
 *
 *@param channelid ��Ƶͨ����
 *@param attr ��Ƶͨ�������ò���
 *
 *@return 0 �ɹ�
 *
 */
int jv_ai_set_attr(int channelid, jv_audio_attr_t *attr);

/**
 *@brief �ر���Ƶ�ɼ�
 *
 *@param channelid ��Ƶͨ����
 *
 *@return 0 �ɹ�
 *
 */
int jv_ai_stop(int channelid);

/**
 *@brief ��ȡһ֡��Ƶ����
 *
 *@param channelid ��Ƶͨ����
 *@param frame ��Ƶ֡
 *
 *@return 0 ��ȷ�Ļ�ȡ�������ݣ� -1��������
 *
 *@note ����IPC��˵��ֻ��һ·��Ƶ����ȴ�ж�·��Ƶ��
 * Ϊ��Ӧ�����������������ÿ��ͨ�������Ի�ȡ��һ������
 *
 */
int jv_ai_get_frame(int channelid, jv_audio_frame_t *frame);

typedef void (*jv_ai_pcm_callback_t)(int channelid, jv_audio_frame_t *frame);

/**
 *@brief ע��ص��������ṩ��Ƶ��PCM֡���ϲ�Ӧ��
 *
 *@param channelid ��Ƶͨ����
 *@param callback_ptr �ص�����ָ��
 */
int jv_ai_set_pcm_callback( int channelid, jv_ai_pcm_callback_t callback_ptr);

 /*
  * ���������Խ�״̬��ָ�������Ƿ��ڶԽ�״̬
  */
void jv_ai_setChatStatus(BOOL bChat);
BOOL jv_ai_GetChatStatus();

int jv_ai_RestartAec(int chan,BOOL bSwitch);
int jv_ai_RestartAnr(int channel,BOOL bSwitch);

int jv_ai_SetMicgain(int gainval);
int jv_ai_SetMicMute(int mute);

#endif

