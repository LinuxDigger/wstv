
/*	SYSFuncs.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯ϵͳ��ش���
	������ʷ���svn�汾����־
*/

#ifndef __SYSFUNCS_H__
#define __SYSFUNCS_H__
#include "jv_common.h"


#define CONFIG_VER	1

typedef struct{
	int nLanguage;
	unsigned int nTimeFormat;
	unsigned int ystID;
}ImportantCfg_t;

#define IMPORTANT_CFG_FILE	CONFIG_PATH"/important.cfg"	//��Ҫ�����ļ������������ԡ�����ͨ�ŵȻָ�ϵͳʱ����ʧ����Ϣ
#define DEFAULT_FORCE_FILE	CONFIG_PATH"/default/force.cfg"	//ָʾ�ļ������ڸ��ļ�����ǿ��ʹ��system.cfg
#define DEFAULT_CFG_FILE	CONFIG_PATH"/default/system.cfg"	//Ĭ���ļ����ָ�ϵͳʱ��ʹ�ñ��ļ�����ʼ������
#define RESTRICTION_FILE	CONFIG_PATH"bRestriction.cfg"	//���ƹ����Ƿ�򿪣�����ļ����ڱ�ʾ��

U32 ParseParam(char *pParam, int nMaxLen, char *pBuffer);

int ReadImportantInfo(ImportantCfg_t* im);

VOID ReadConfigInfo();
VOID WriteConfigInfo();

char *sysfunc_get_key_value(char *data, char *key, char *value, int maxlen);

/**
 *@brief ��һ��Buffer�л�ȡָ��Key��Ӧ��Valueֵ
 */
char *SYSFuncs_GetKeyValue(const char *pBuffer, const char *key, char *valueBuf, int maxValueBuf);

/**
 *@brief ��ȡָ���Ĳ���
 */
char *SYSFuncs_GetValue(const char *fname, const char *key, char *value, int maxValueLen);

/**
 *@brief ����ϵͳ
 */
int SYSFuncs_reboot(void);

/**
 *@brief �ָ�ϵͳĬ��ֵ
 */
int SYSFuncs_factory_default(int bHardReset);

/**
 *@brief �ָ�ϵͳĬ��ֵ������Ҫ�����豸
 */
int SYSFuncs_factory_default_without_reboot();

int __CalcBitrate(int w, int h, int framerate, int vencType);

#endif




//svn test
