/*
 * sp_log.h
 *
 *  Created on: 2014-12-01
 *      Author: Qin Lichao
 */

#ifndef SP_LOG_H_
#define SP_LOG_H_

#define SPLOG_VERSION			0x10    //�汾��
#define SPLOG_MAX_RECORD		1000    //����¼
#define SPLOG_RECORD_LEN		64      //LOG_ITEM��С
#define SPLOG_STR_LEN			59      //��¼����
#define SPLOGS_NEED_SAVE		1		//

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	unsigned int nVersion;			//�汾��
	unsigned int nRecordTotal;		//��¼����
	unsigned int nItemSize;			//��¼����
	unsigned int nCurRecord;		//��ǰ��¼
	int nSavedRecord;		//�Ѿ�����ļ�¼
	char nReserve[16];		//����
}SPLOG_HEAD;

typedef struct
{
	unsigned int	nTime;					//ʱ��
	char	strLog[SPLOG_STR_LEN+1];	//�¼�
}SPLOG_ITEM;

typedef struct
{
	SPLOG_HEAD		head;					//��־ͷ
	SPLOG_ITEM		item[SPLOG_MAX_RECORD];	//��־����
	int				fd;
}SPLOG;


/**
 * @brief ��ȡ��־
 *
 * @param date ��־����
 * @return 0�ɹ� -1ʧ��
 */
int sp_log_get(SPLOG *log);

/**
 * @brief �����־
 *
 * @return 0�ɹ� -1ʧ��
 */
int sp_log_clear(void);

#ifdef __cplusplus
}
#endif


#endif /* SP_LOG_H_ */
