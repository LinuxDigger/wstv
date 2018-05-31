
/*	mrecord.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯¼����غ���
	������ʷ���svn�汾����־
*/

#ifndef __MRECORD_H__
#define __MRECORD_H__

#include "jv_common.h"

#define	FILE_FLAG		".mp4"

#define MAX_REC_TASK	2
#define RECORD_CHN		record_chn	//¼��ͨ��
#define RECORD_CLOUD_CHN	1

extern int record_chn;

//¼������ö��
typedef enum tagRECTYPE
{
	REC_STOP	= 0,	//¼����ֹͣ
	REC_NORMAL	= 'N',	//����¼�� 78
	REC_DISCON  = 'D',	//������ʱ¼��. IPC��������ʱ¼��
	REC_TIME	= 'T',	//��ʱ¼�� 84
	REC_MOTION	= 'M',	//�ƶ����¼�� 77
	REC_ALARM	= 'A',	//����¼�� 65
	REC_ONE_MIN	= 'O',	//һ����¼��79
	REC_CHFRAME	= 'C',  //��֡¼��
	REC_PUSH	= 'P',	//����¼��
	REC_IVP	= 'I',		//���ܷ���¼��

} RECTYPE;

typedef enum 
{
	ALARM_TYPE_NONE,
	ALARM_TYPE_MOTION,
	ALARM_TYPE_ALARM,
	ALARM_TYPE_IVP,
	ALARM_TYPE_IVP_VR,
	ALARM_TYPE_IVP_HIDE,
	ALARM_TYPE_IVP_LEFT,
	ALARM_TYPE_IVP_REMOVED,
	ALARM_TYPE_BABYCRY,
	ALARM_TYPE_PIR,
	ALARM_TYPE_WIRELESS,
	ALARM_TYPE_BUTT,
}alarm_type_e;

typedef enum
{
	RECORD_MODE_STOP = 0,		// ֹͣ¼��
	RECORD_MODE_NORMAL,			// ��ͨ¼��
	RECORD_MODE_ALARM,			// ����¼��
	RECORD_MODE_CHFRAME,		// ��֡¼��
	RECORD_MODE_TIMING,			// ��ʱ¼��
	RECORD_MODE_DISCONN,		// ������¼��

	RECORD_MODE_BUTT
}RecordMode_e;

// ¼���������ͣ���ֵԽ�����ȼ�Խ��
typedef enum
{
	RECORD_REQ_NONE,			// ������
	RECORD_REQ_ALARM,			// ����¼��
	RECORD_REQ_PUSH,			// ¼������
	RECORD_REQ_ONEMIN,			// һ����¼��
}REC_REQ;

typedef void (*FuncRecFinish)();

typedef struct
{
	REC_REQ				ReqType;
	union
	{
		int				nDuration;
		alarm_type_e	AlarmType;
	};
	FuncRecFinish		pCallback;
}REC_REQ_PARAM;


/**
 *@brief ¼����ص����ԡ�
 *
 *
 */
typedef struct
{
	BOOL		bEnable;			///< �Ƿ������ֹ�¼��
	unsigned int	file_length;			///< ¼���ļ��ĵ����ĳ��ȣ�����Ϊ��λ

	///��ʱ���
	BOOL		timing_enable;		///< �Ƿ����ö�ʱ¼��

	BOOL discon_enable; ///< �Ƿ�����������¼��
	BOOL alarm_enable; ///< �Ƿ����ñ���¼��

	/**
	 * ��ʱ¼�ƵĿ�ʼʱ��ͽ���ʱ�䣬��λΪ�룬��ȡֵ��0 ~ 24*60*60 ֮��
	 * ��timing_stop > timing_startʱ����ʱ��Χ���ǵ����timing_start��timing_stop
	 * ��timing_stop < timing_startʱ����ʱ��ΧΪ�����timing_start���ڶ����timing_stop
	 * 
	 */
	unsigned int	timing_start;		
	unsigned int	timing_stop;

	BOOL		disconnected;		///< ������ʱ¼��
	BOOL		detecting;			///< �ƶ����¼��
	BOOL		alarming;			///< �ⲿ����¼��
	BOOL		ivping;			///< ���ܷ���¼��
	unsigned int alarm_pre_record;		///<Ԥ¼�Ƶ�ʱ�䣬���Ϊ10��
	unsigned int alarm_duration;		///<����������¼�Ƶĳ���ʱ��
	BOOL		chFrame_enable;			//��֡¼���ʹ�ܿ���
	unsigned int chFrameSec;				//��֡¼����
}mrecord_attr_t;

typedef struct
{
	char	type;		// RECTYPE
	char	part;		// �������
	int		date;		// 20170101
	int		fileTime;	// 235959
	int		stime;		// 235959
	int		etime;		// 235959
}mrecord_item_t;

/**
 *@brief ��ʼ��
 *
 *@return 0
 */
int mrecord_init(void);

/**
 *@brief ����
 *
 *@return 0
 */
int mrecord_deinit(void);

/**
 *@brief ���ò���
 *@param channelid ͨ����
 *@param attr ����ָ��
 *
 *@return 0 �����Ǵ����
 */
int mrecord_set_param(int channelid, mrecord_attr_t *attr);

/**
 *@brief ��ȡ����
 *@param channelid ͨ����
 *@param attr ����ָ��
 *
 *@return 0 �����Ǵ����
 */
int mrecord_get_param(int channelid, mrecord_attr_t *attr);

/**
 *@brief ǿ��ֹͣ¼��
 *
 *	������Ҫ��Ե��ֱ��ʷ����仯ʱ������¼����
 *@param channelid ͨ����
 *
 *@return 0 �����Ǵ����
 */
int mrecord_stop(int channelid);

void mrecord_onestop_sound();


/**
 *@brief ˢ��ĳͨ�������ã�ʹ֮��Ч��
 *
 *	������Ҫ��Զ�ʱ��
 *@param channelid ͨ����
 *
 *@return 0 �����Ǵ����
 */
int mrecord_flush(int channelid);

/**
 *@brief ����һ֡����
 *@param channelid ͨ����
 *@param buffer �������ڵ��ڴ�ָ��
 *@param len ���ݳ���
 *@param frametype �������ͣ��ο�#JVS_TYPE_P ��
 *
 */
int mrecord_write(int channelid, unsigned char *buffer, int len, unsigned int frametype, unsigned long long timestamp);

/**
 *@brief �����ⲿ�����������ƶ���ⱨ��
 *@param channelid ͨ����
 *@param type ��������
 *@param param ���Ӳ���
 *@return 0 ���ߴ����
 *
 */
int mrecord_alarming(int channelid, alarm_type_e type, void *param);

/**
 *@brief ��ȡ����������¼���ļ���ͼƬ����
 *@param[IN]  type
 *@param[OUT] alarmInfo ������Ϣ����������������¼���ļ���ͼƬ����
 *@return 0 ���ߴ����return -1 û��SD��
 */
int mrecord_alarm_get_attachfile(RECTYPE type, void *alarmInfo);

const char* mrecord_get_now_recfile();

int mrecord_one_min_rec(int channelid);

/*
 * �ɵ��ƴ洢����Ҫ�����ͷ�Ԥ¼��ռ�õ���Դ���ƴ洢ʹ�õģ�
 * Ŀǰֻ�����ƴ洢�豸�������ļ�������ʱ�������������֮ǰ���ͷ���Դ��ֹ����
 */
void mrecord_cloud_destroy(int channelid);

/*
�ƴ洢�ϴ��ڶ�����
*/
void mrecord_cloud_write(int channelid, unsigned char *buffer, int len, unsigned int frametype, unsigned long long timeStamp);

int mrecord_get_recmode(int* nChFrameSec);

int mrecord_set_recmode(int Type, int nChFrameSec);


/*
 * ����¼��������ƣ����ڴ�������¼���������ʱ¼�������籨��¼��һ��¼������¼���
 */
int mrecord_request_start_record(int channelid, const REC_REQ_PARAM* ReqParam);

int mrecord_request_stop_record(int channelid, REC_REQ Request);


void MP4_GetIndexFile(char *strFile, char *strIndex);
/**
 *@brief ���ò����ļ�д��
 *
 */
#define mrecord_set(channelid, key,value)\
do{\
	mrecord_attr_t attr;\
	mrecord_get_param(channelid, &attr);\
	attr.key = value;\
	mrecord_set_param(channelid, &attr);\
}while(0)

void mrecord_pre_reinit();

int mrecord_search_file(int startdate, int starttime, int enddate, int endtime, mrecord_item_t* pResult, int maxcnt);

const char* mrecord_get_filename(const mrecord_item_t* pResult, char* name, int len);

#endif

