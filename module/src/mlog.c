

#include "jv_common.h"
#include <utl_filecfg.h>
#include "mipcinfo.h"
#include "mlog.h"

//��ȡlog��־�ļ�������������򴴽����������ڴ���ļ���д���һ��"creat"��¼,����ɹ���������־�ڴ��ַ�����򷵻�0
LOG stLog;
LOG * mlog_init(char *strLogFile)
{
	stLog.fd=open(strLogFile, O_RDWR|O_CREAT, 0777);
	if(stLog.fd < 0)
	{
		printf("Load logs failed\n");
		return NULL;
	}

	if(lseek(stLog.fd, 0, SEEK_END))	//����log�ļ�
	{
		lseek(stLog.fd, 0, SEEK_SET);
		read(stLog.fd, &stLog.head, sizeof(LOG_HEAD));
		read(stLog.fd, stLog.item, sizeof(stLog.item));
		Printf("Load log file\n");
	}
	else				//�������ڴ��й����һ����¼����д��log�ļ�
	{
		Printf("New log file created.\n");
		stLog.head.nVersion		= LOG_VERSION;
		stLog.head.nRecordTotal	= 1;
		stLog.head.nItemSize	= LOG_RECORD_LEN;
		stLog.head.nCurRecord	= 0;
		stLog.head.nSavedRecord	= 0;
		write(stLog.fd, &stLog.head, sizeof(LOG_HEAD));

		//�����ļ�ʱд���һ����־
		stLog.item[0].nTime = time(NULL);
		sprintf(stLog.item[0].strLog, "%s", mlog_translate("No Log File, Create It."));
		write(stLog.fd, &stLog.item[0], sizeof(LOG_ITEM));
		Printf("Create-");
	}
	Printf("Ver=%x, Total=%d, nCur=%d, nSaved=%d\n", stLog.head.nVersion,
		stLog.head.nRecordTotal, stLog.head.nCurRecord, stLog.head.nSavedRecord);

	pthread_mutex_init(&stLog.mutexLog, NULL);

	return &stLog;
}

VOID mlog_deinit()
{
	if(stLog.fd>0)
	{
		close(stLog.fd);
	}
	pthread_mutex_destroy(&stLog.mutexLog);
}
static BOOL s_enable = TRUE;
void mlog_enable(BOOL bEnable)
{
	s_enable = bEnable;
}

//���ڴ��м�¼��־��ptr�ǽ�Ҫ��¼����־
VOID mlog_write(char *strLog, ...)
{
	va_list list;
	U32 nIndex, nSaved;

	if (!s_enable)
		return ;
	pthread_mutex_lock(&stLog.mutexLog);
	strLog = mlog_translate(strLog);

	va_start(list, strLog);

	//�������1000����־���ص��׵�ַ�����ǽϾõ���־
	if(++stLog.head.nCurRecord >= LOG_MAX_RECORD)
	{
		stLog.head.nCurRecord = 0;
	}
	nIndex	= stLog.head.nCurRecord;
	stLog.item[nIndex].nTime = time(NULL);
	vsnprintf(stLog.item[nIndex].strLog, sizeof(stLog.item[nIndex].strLog), strLog, list);
	va_end(list);

	//��־�������ۼӣ����LOG_MAX_RECORD��
	if(++stLog.head.nRecordTotal >=  LOG_MAX_RECORD)
	{
		stLog.head.nRecordTotal=LOG_MAX_RECORD;
	}

	nSaved	= stLog.head.nSavedRecord;
	if(nIndex%LOGS_NEED_SAVE == 0)
	{
		stLog.head.nSavedRecord=nIndex;
		//д����־�ļ�ͷ��Ϣ,lck20120112
		lseek(stLog.fd, 0, SEEK_SET);
		write(stLog.fd, &stLog.head, sizeof(LOG_HEAD));
		Printf("nTotal=%d\n", stLog.head.nRecordTotal);
		//�����Ҫ�������־�ﵽ�ļ�β������Ҫ���ǽϾõ���־
		if(nIndex < nSaved)
		{
			//д����־
			write(stLog.fd, stLog.item, sizeof(stLog.item));
			Printf("Save logs\n");
		}
		else	//��Ҫ�������־����Ҫ�����ļ�ͷ������־
		{
			//д������δ�������־��¼
			lseek(stLog.fd, sizeof(LOG_HEAD)+(sizeof(LOG_ITEM)*(nSaved+1)), SEEK_SET);
			write(stLog.fd, &stLog.item[nIndex], sizeof(LOG_ITEM)*(nIndex-nSaved));
			Printf("Save items %s\n", stLog.item[nIndex].strLog);
		}
	}

	//д����־���ӡ������Ϣ,lck20120112
	Printf("Ver=%x, Total=%d, nCur=%d, nSaved=%d\n", stLog.head.nVersion,
		stLog.head.nRecordTotal, stLog.head.nCurRecord, stLog.head.nSavedRecord);

	pthread_mutex_unlock(&stLog.mutexLog);
}

/**
 *@brief ��ȡ��ǰ���Զ�Ӧ���ַ���
 *
 *@param pStr Ӣ���ַ���
 *
 *@return ���ؽ�Ӣ�ķ������ַ���
 *
 */
char* mlog_translate(char *pStr)
{
	char *ret;

	if (ipcinfo_get_param(NULL)->nLanguage == LANGUAGE_CN)
	{
		ret = utl_fcfg_get_value("/progs/language/zh-cn.lan", pStr);
		if (ret)
			return ret;
	}
	return pStr;
}

