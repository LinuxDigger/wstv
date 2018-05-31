/*
 * utl_audio.c
 *
 *  Created on: 2014��9��26��
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <errno.h>

#include "utl_audio.h"


typedef struct
{
	struct{
		char szRiffID[4];    // 'R','I','F','F'
		int  dwRiffSize;	//����һ����ַ��ʼ���ļ�β�����ֽ�������λ�ֽ��ں��棬
		char szRiffFormat[4]; // 'W','A','V','E'
	}RIFF_HEADER;

	struct{
		char szFmtID[4]; // 'f','m','t',' '
		int dwFmtSize;	//��һ�ṹ��Ĵ�С����ֵΪ16
		struct{
			short wFormatTag;	//Ϊ1ʱ��ʾ����PCM���룬����1ʱ��ʾ��ѹ���ı���
			short  wChannels;	//1Ϊ��������2Ϊ˫����
		    int dwSamplesPerSec;	//������
			int  dwAvgBytesPerSec;	//
			short wBlockAlign;
			short wBitsPerSample;	//Bits per sample
		}WAVE_FORMAT;
	}FMT_BLOCK;

} WAVE_FILE_HEADER;

typedef struct
{
	char szFactID[4]; // 'd','a','t','a'
	int dwFactSize;
}Block;


typedef struct{
	char fname[256];
	UtlAudioFileType_e type;
	FILE *fp;
	int audioStart; //��Ƶ��ʼ��λ��

	UtlAudioInfo_t info;
}UAParam_t;

/**
 *@brief �����Ƿ�Ϊwav��ʽ
 *
 *@return 1 if yes, 0 otherwise
 */
static int __utl_audio_try_parse_wav(UAParam_t *uaudio)
{
	WAVE_FILE_HEADER wh;

	fseek(uaudio->fp, 0, SEEK_SET);

	int len = fread(&wh, 1, sizeof(wh), uaudio->fp);
	if (len != sizeof(wh))
		return 0;
/*
	printf("fmtsize: %d, samplePerSec: %d, bitsPerSample: %d\n"
			, wh.FMT_BLOCK.dwFmtSize
			, wh.FMT_BLOCK.WAVE_FORMAT.dwSamplesPerSec
			, wh.FMT_BLOCK.WAVE_FORMAT.wBitsPerSample
			);
			*/
	uaudio->info.bitsPerSample = wh.FMT_BLOCK.WAVE_FORMAT.wBitsPerSample;
	uaudio->info.encType = wh.FMT_BLOCK.WAVE_FORMAT.wFormatTag;
	uaudio->info.samplerate = wh.FMT_BLOCK.WAVE_FORMAT.dwSamplesPerSec;
	/*
	char *S1="WAVE";
	char *S2="fmt";
	char *s3 ="fact";
	char *s4="data";

	 */

	if (memcmp(wh.RIFF_HEADER.szRiffID, "RIFF", 4) != 0
			|| memcmp(wh.RIFF_HEADER.szRiffFormat, "WAVE", 4) != 0
			|| memcmp(wh.FMT_BLOCK.szFmtID, "fmt ", 4) != 0
			)
	{
		printf("not wave file\n");
		return 0;
	}
	uaudio->audioStart += len;

	unsigned short addition;
	//����2�ֽڸ�����Ϣ
	if (wh.FMT_BLOCK.dwFmtSize == 18)
	{
		fread(&addition, 1, 2, uaudio->fp);
		uaudio->audioStart += 2;
	}

	//block
	Block block;
	while(1)
	{
		len = fread(&block, 1, sizeof(block), uaudio->fp);
		if (len != sizeof(block))
		{
			return 0;
		}
		uaudio->audioStart += len;
		if (memcmp(block.szFactID, "fact", 4) == 0)
		{
			fseek(uaudio->fp, block.dwFactSize, SEEK_CUR);
			uaudio->audioStart += len;
			continue;
		}
		else if (memcmp(block.szFactID, "data", 4) == 0)
		{
			uaudio->info.audioSize = block.dwFactSize;
			break;
		}
	}

	return 1;
}

/**
 *@brief ����Ƶ�ļ�
 *
 *@param fname ��Ƶ�ļ���
 *@param type ��Ƶ�ļ����͡��� #UTL_AUDIO_TYPE_UNKNOWNʱ�ڲ����Զ�ʶ��
 *
 *@return Handle of audio file
 */
UtlAudioHandle_t utl_audio_open(const char *fname, UtlAudioFileType_e type)
{
	UAParam_t *handle = malloc(sizeof(UAParam_t));

	memset(handle, 0, sizeof(UAParam_t));
	strncpy(handle->fname, fname, sizeof(handle->fname));
	handle->fp = fopen(fname, "rb");
	if (!handle->fp)
	{
		printf("utl_audio_fopen failed open file: %s, because: %s\n", fname, strerror(errno));
		free(handle);
		return NULL;
	}

	if (type == UTL_AUDIO_TYPE_WAV)
	{
		if (__utl_audio_try_parse_wav(handle))
		{
			return handle;
		}

	}

	//others
	if (__utl_audio_try_parse_wav(handle))
	{
		return handle;
	}

	return NULL;
}

/**
 *@brief �ر���Ƶ�ļ�
 *
 *@param handle Handle of audio file
 *
 *@return 0 if success
 */
int utl_audio_close(UtlAudioHandle_t handle)
{
	UAParam_t *uaudio = (UAParam_t *)handle;
	if (!handle)
	{
		printf("ERROR: %s, bad param\n", __func__);
		return -1;
	}

	fclose(uaudio->fp);
	memset(uaudio, 0, sizeof(UAParam_t));
	free(uaudio);

	return 0;
}

/**
 *@brief ��ȡ��Ƶ����
 */
int utl_audio_read(UtlAudioHandle_t handle, unsigned char *buffer, int len)
{
	UAParam_t *uaudio = (UAParam_t *)handle;
	if (!handle)
	{
		printf("ERROR: %s, bad param\n", __func__);
		return -1;
	}

	return fread(buffer, 1, len, uaudio->fp);
}
/**
 *@brief �ƶ���Ƶ�ļ��Ķ�ȡָ�롣ʹ�÷����ο� #fseek
 *
 *@brief �ر���Ƶ�ļ�
 *@param offset ƫ�ơ�
 *@param whence SEEK_CUR, SEEK_SET, SEEK_END
 *
 *@return 0 if success
 */
int utl_audio_seek(UtlAudioHandle_t handle, int offset, int whence)
{
	UAParam_t *uaudio = (UAParam_t *)handle;
	if (!handle)
	{
		printf("ERROR: %s, bad param\n", __func__);
		return -1;
	}
	int realoffset;
	if (whence == SEEK_SET)
	{
		realoffset = uaudio->audioStart+offset;
	}
	else
	{
		realoffset = offset;
	}

	return fseek(uaudio->fp, uaudio->audioStart+offset, whence);
}

/**
 *@brief ��ȡ��Ƶ�ļ���Ϣ
 *
 *@param handle Handle of audio file
 *@param info ��Ƶ��Ϣ
 *
 *@return 0 if success
 */
int utl_audio_get_fileinfo(UtlAudioHandle_t handle, UtlAudioInfo_t *info)
{
	UAParam_t *uaudio = (UAParam_t *)handle;
	if (!handle)
	{
		printf("utl_audio_get_fileinfo bad param\n");
		return -1;
	}

	memcpy(info, &uaudio->info, sizeof(UtlAudioInfo_t));

	return 0;
}


