

#ifndef __MLOG_H__
#define __MLOG_H__

#define LOG_VERSION			0x10    //�汾��
#define LOG_MAX_RECORD		1000    //����¼
#define LOG_RECORD_LEN		64      //LOG_ITEM��С
#define LOG_STR_LEN			59      //��¼����
#define LOGS_NEED_SAVE		1		//

typedef struct
{
	unsigned int nVersion;			//�汾��
	unsigned int nRecordTotal;		//��¼����
	unsigned int nItemSize;			//��¼����
	unsigned int nCurRecord;		//��ǰ��¼
	int nSavedRecord;		//�Ѿ�����ļ�¼
	char nReserve[16];		//����
}LOG_HEAD;

typedef struct
{
	unsigned int	nTime;					//ʱ��
	char	strLog[LOG_STR_LEN+1];	//�¼�
}LOG_ITEM;

typedef struct
{
	LOG_HEAD		head;					//��־ͷ
	LOG_ITEM		item[LOG_MAX_RECORD];	//��־����
	int				fd;

	pthread_mutex_t mutexLog;				//������
}LOG;

extern LOG stLog;

LOG *mlog_init(char *strLogFile);
void mlog_deinit();

/**
 *@ʹ�ܻ��߽�ֹLOG
 *
 *
 */
void mlog_enable(BOOL bEnable);

void mlog_write(char *strLog, ...);

/**
 *@brief ��ȡ��ǰ���Զ�Ӧ���ַ���
 *
 *@param pStr Ӣ���ַ���
 *
 *@return ���ؽ�Ӣ�ķ������ַ���
 *
 */
extern char* mlog_translate(char *pStr);

#endif

