/*
 * sp_rec.h
 *
 *  Created on: 2013-11-1
 *      Author: lfx
 */

#ifndef SP_REC_H_
#define SP_REC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LEN 1024*1024


/* ֡���Ͷ���*/
typedef enum
{
	RTP_PACK_TYPE_FRAME_P=0,
	RTP_PACK_TYPE_FRAME_I,
	RTP_PACK_TYPE_FRAME_B,
	RTP_PACK_TYPE_FRAME_AUDIO,
	RTP_PACK_TYPE_FRAME_JEPG,
	RTP_PACK_TYPE_FRAME_MOTION,
	RTP_PACK_TYPE_FRAME_NUM,
	RTP_PACK_TYPE_FRAME_NULL=0xff,/*������������*/
}RtpPackType_e;

/**
 *@brief ��ʼ¼��
 *
 *@param channelid ͨ����
 */
int sp_rec_start(int channelid);
/**
 *@brief ��ͣ¼��
 *
 *@param channelid ͨ����
 */
 int sp_rec_pause(int channelid);

/**
 *@brief ����¼��
 *
 *@param channelid ͨ����
 */
int sp_rec_stop(int channelid);

/**
 *@brief ����Ƿ�����¼��
 */
int sp_rec_b_on(int channelid);



/**
 * ֡�ṹ
 */
typedef struct
{
	unsigned char buf[MAX_LEN];	//֡����
	int len;
	int framerate;				//֡��
	int width;					//֡��
	int height;					//֡��
	RtpPackType_e type;			//֡����
}FrameInfo_t;

typedef enum{
	SP_REC_TYPE_MOTION,	//�ƶ����¼��
	SP_REC_TYPE_TIME,	//��ʱ¼��
	SP_REC_TYPE_ALARM,	//����¼��
	SP_REC_TYPE_MANUAL,	//�ֶ�¼��
	SP_REC_TYPE_ALL,	//ȫ��һֱ¼��
	SP_REC_TYPE_STOP,	//ֹͣ¼��
	SP_REC_TYPE_DISCONNECT,
}SPRecType_e;

/**
 * ¼���ļ��ṹ
 */
typedef struct _REC_Search
{
	char fname[128];			//�ļ�·��
	long start_time;	//¼���ļ���ʼʱ�䣬1970������������
	long end_time;		//¼���ļ�����ʱ��
	int secrecy;				//��������/0Ϊ������/1Ϊ����
	SPRecType_e type;				//¼���������
	struct _REC_Search *next;
}REC_Search;

typedef void * REC_HANDLE;		//���

/**
 * @brief ����¼�񲥷ž��
 *
 * @param channelid ͨ����
 * @param rec_start_time Ҫ���ŵ�¼��ʼʱ��,��1970�굽��ʼ���ŵ���
 * @param rec_end_time Ҫ���ŵ�¼�����ʱ�䣬��1970�굽�������ŵ���
 *
 * @return REC_HANDLE ����¼�񲥷ž��
 */
REC_HANDLE  sp_rec_create(int channelid, long start_time, long end_time);

/**
 *@brief ����rec���
 */
int sp_rec_destroy(REC_HANDLE handle);

/**
 *@brief �ָ�����¼��
 *
 *@param channelid ͨ����
 */
int sp_rec_resume(int channelid);

/**
 * @brief �����ļ�
 *
 * @param channelid ͨ����
 * @param rec_start_time Ҫ������¼��ʼʱ��,��1970�굽��ʼ���ŵ���
 * @param rec_end_time Ҫ������¼�����ʱ�䣬��1970�굽�������ŵ���
 *
 * @return ����������
 *
 * @note �����������Ҫ����#sp_rec_search_release�ͷ���Դ
 *
 */
REC_Search *sp_rec_search(int channelid, long start_time, long end_time);

/**
 * @brief ɾ��������Ϣ
 *
 * @param list ���������#sp_rec_search �Ľ��
 *
 * @return ����������
 *
 */
int sp_rec_search_release(REC_Search *list);
/**
 * @brief ������Ϣ����
 *
 * @param list ���������#sp_rec_search �Ľ��
 *
 * @return ��������������
 *
 */
 int sp_rec_search_cnt(REC_Search *list);

/**
 * @brief ��λ��Ƶ����λ��
 */
int sp_rec_seek(REC_HANDLE handle, int play_range_begin);

/**
 * @brief ��ȡ¼���ļ�
 *
 * @param handle ¼�񲥷ž��
 * @param buf ����֡�ṹ
 *
 * @return 0
 */
int sp_rec_read(REC_HANDLE handle, FrameInfo_t *buf);

/**
 * @brief ����¼��ģʽ
 *
 * @param channelid ͨ����
 * @param rectype ¼��ģʽ
 *
 * @return 0 �����Ǵ����
 */
int sp_rec_set_mode(int channelid, SPRecType_e rectype);
/**
 * @brief ���¼��ģʽ
 *
 * @param channelid ͨ����
 *
 * @return ¼��ģʽ
 */
SPRecType_e sp_rec_get_mode(int channelid);

/**
 * @brief ����¼���ļ�����
 *
 * @param channelid ͨ����
 * @param ¼���ļ�������ȵ�λ����
 *
 * @return 0 �����Ǵ����
 */
unsigned int sp_rec_set_length(int channelid, unsigned int len);
/**
 * @brief ���¼���ļ����ʱ��
 *
 * @param channelid ͨ����
 *
 * @return ¼���ļ����ʱ�䳤��
 */
unsigned int sp_rec_get_length(int channelid);

#ifdef __cplusplus
}
#endif

#endif /* SP_REC_H_ */
