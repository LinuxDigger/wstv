/*
 * utl_audio.h
 *
 *  Created on: 2014��9��26��
 *      Author: Administrator
 */

#ifndef UTL_AUDIO_H_
#define UTL_AUDIO_H_

#include <stdio.h>

typedef enum{
	UTL_AUDIO_TYPE_UNKNOWN,
	UTL_AUDIO_TYPE_WAV,

}UtlAudioFileType_e;

typedef enum{
	UTL_AUDIO_ENC_PCM,
	UTL_AUDIO_ENC_G711_A,
	UTL_AUDIO_ENC_G711_U,
	UTL_AUDIO_ENC_G726_16K, // not support
	UTL_AUDIO_ENC_G726_24K, // not support
	UTL_AUDIO_ENC_G726_32K, // not support
	UTL_AUDIO_ENC_G726_40K,
	UTL_AUDIO_ENC_ADPCM,
}UtlAudioEncType_e;

typedef void * UtlAudioHandle_t;

typedef struct{
	UtlAudioEncType_e encType;
	int samplerate; //�����ʣ�8000��16000��32000��������
	int bitsPerSample; //bits per sample 8, 16
	int audioSize; //��Ƶ����ռ���ֽ���
}UtlAudioInfo_t;

/**
 *@brief ����Ƶ�ļ�
 *
 *@param fname ��Ƶ�ļ���
 *@param type ��Ƶ�ļ����͡��� #UTL_AUDIO_TYPE_UNKNOWNʱ�ڲ����Զ�ʶ��
 *
 *@return Handle of audio file
 */
UtlAudioHandle_t utl_audio_open(const char *fname, UtlAudioFileType_e type);

/**
 *@brief �ر���Ƶ�ļ�
 *
 *@param handle Handle of audio file
 *
 *@return 0 if success
 */
int utl_audio_close(UtlAudioHandle_t handle);

/**
 *@brief ��ȡ��Ƶ����
 */
int utl_audio_read(UtlAudioHandle_t handle, unsigned char *buffer, int len);

/**
 *@brief �ƶ���Ƶ�ļ��Ķ�ȡָ�롣ʹ�÷����ο� #fseek
 *
 *@brief �ر���Ƶ�ļ�
 *@param offset ƫ�ơ�
 *@param whence SEEK_CUR, SEEK_SET, SEEK_END
 *
 *@return 0 if success
 */
int utl_audio_seek(UtlAudioHandle_t handle, int offset, int whence);

/**
 *@brief ��ȡ��Ƶ�ļ���Ϣ
 *
 *@param handle Handle of audio file
 *@param info ��Ƶ��Ϣ
 *
 *@return 0 if success
 */
int utl_audio_get_fileinfo(UtlAudioHandle_t handle, UtlAudioInfo_t *info);

#endif /* UTL_AUDIO_H_ */
