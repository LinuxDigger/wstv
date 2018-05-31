
/*	mdetect.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯�ƶ������ش���
	������ʷ���svn�汾����־
*/

#ifndef _MDETECT_H_
#define _MDETECT_H_

/**
 *@brief ��Ҫ����ʱ�Ļص�����
 * �ú�����Ҫ����֪ͨ�ֿأ��Ƿ��о�������
 *@param channelid ����������ͨ����
 *@param bAlarmOn �����������߹ر�
 *
 */
typedef void (*alarming_callback_t)(int channelid, BOOL bAlarmOn);
#define MAX_REGION_ROW 32
typedef struct tagMD
{
	BOOL	bEnable;			//�Ƿ����ƶ����
	U32		nSensitivity;		//������
	U32		nThreshold;			//�ƶ������ֵ
	U32		nRectNum;			//�ƶ����������������Ϊ4��0��ʾȫ������
	RECT	stRect[MAX_MDRGN_NUM];
	BOOL 	bEnableRecord;		//�Ƿ�������¼��

	U32		nDelay;
	U32		nStart;
	BOOL	bOutClient;			//�Ƿ������ֿ�
	BOOL	bOutEMail;			//�Ƿ����ʼ�
	BOOL	bOutVMS;			//�Ƿ�����VMS������
	BOOL	bBuzzing;			//�Ƿ����������

	U32		nRegion[MAX_REGION_ROW];		//�ƶ���ⷶΧ��
	int		nRow;				//����
	int 	nColumn;			//����
}MD, *PMD;

/**
 *@brief ��ʼ��
 *@return 0
 *
 */
int mdetect_init(void);

/**
 *@brief ����
 *@return 0
 *
 */
int mdetect_deinit(void);


/**
 *@brief ��ʱ���úͽ���
 *@return 0
 *
 */
void mdetect_enable();

void mdetect_disable();

/**
 *@brief ���ñ����ص�����
 *
 *
 */
int mdetect_set_callback(alarming_callback_t callback);

/**
 *@brief ���ò���
 *@param channelid Ƶ����
 *@param md Ҫ���õ����Խṹ��
 *@note �������ȷ���������Ե�ֵ������#mdetect_get_param��ȡԭ����ֵ
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mdetect_set_param(int channelid, MD *md);

/**
 *@brief ��ȡ����
 *@param channelid Ƶ����
 *@param md Ҫ���õ����Խṹ��
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mdetect_get_param(int channelid, MD *md);

/**
 *@brief ʹ������Ч
 *	��#mdetect_set_param֮������ı���ʹ��״̬�����ñ�����
 *@param channelid Ƶ����
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mdetect_flush(int channelid);

/**
 *@brief ��ʼ�ƶ����
 *@param channelid Ƶ����
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mdetect_start(int channelid);

/**
 *@brief ֹͣ�ƶ����
 *@param channelid Ƶ����
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mdetect_stop(int channelid);

/**
 *@brief �Ƿ����ƶ���ⱨ��
 *@param channelid Ƶ����
 *@return Ϊ�棬���б�������������û�С�
 *
 *@note #MD::nDelay ��ֵ�������˱�����೤ʱ���ڣ��Ǵ��ڱ�����
 *
 */
BOOL mdetect_b_alarming(int channelid);

extern int md_timer[MAX_STREAM];
BOOL _mdetect_timer_callback(int tid, void *param);

#endif

