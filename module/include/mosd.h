


/*	mosd.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯OSD��ʾ��صĹ��ܣ�ģ����ֻ��������ֵ,��ֱ���������û�����,lck20120110
	������ʷ���svn�汾����־
*/

#ifndef __MOSD_H__
#define __MOSD_H__

#define	RGN_WIDTH		240
#define	RGN_HEIGHT		60
#define FONT_LINE_PAD	4
#define FONT_HEIGHT		osdstatus.flheader->YSize
#define	TEXT_LINE_NUM	8
typedef enum{
	MCHNOSD_POS_LEFT_TOP=0,
	MCHNOSD_POS_LEFT_BOTTOM,
	MCHNOSD_POS_RIGHT_TOP,
	MCHNOSD_POS_RIGHT_BOTTOM,
	MCHNOSD_POS_HIDE,
}mchnosd_pos_e;
typedef enum{
	MULTIOSD_POS_LEFT_TOP=0,
	MULTIOSD_POS_LEFT_BOTTOM,
	MULTIOSD_POS_RIGHT_TOP,
	MULTIOSD_POS_RIGHT_BOTTOM,
	MULTIOSD_POS_HIDE,
}multiosd_pos_e;
typedef enum{
	MULTIOSD_LEFT=0,
	MULTIOSD_RIGHT,
}multiosd_align_e;

/**
 *@brief ʱ���ʽ����
 *
 */
//typedef enum{
//	OSD_TM_FMT_MMDDYYYY,	///< ��ʽΪ05/07/2010
//	OSD_TM_FMT_YYYYMMDD,	///<��ʽΪ2010-05-07
//	OSD_TM_FMT_MAX
//}mosd_time_format_e;

/**
 *@brief channel osd status
 *
 */
typedef struct 
{
	BOOL				bShowOSD;			///< �Ƿ���ͨ������ʾOSD
	//mosd_time_format_e	tm_fmt;				///< ʱ���ʽ
	char					timeFormat[32];		///< ʱ���ʽ YYYY-MM-DD hh:mm:ss ���������
	mchnosd_pos_e		position;			///< OSD��λ�� 0, ���ϣ�1�����£�2�����ϣ�3������
	mchnosd_pos_e		timePos;			///< OSD��λ�ã�ʱ�䲿�� 0, ���ϣ�1�����£�2�����ϣ�3������
	char					channelName[32];	///<ͨ������
	BOOL osdbInvColEn;		//�Ƿ�ɫlk20131218
	BOOL bLargeOSD;			//�Ƿ��ó���OSD
}mchnosd_attr;

/**
 *@brief channel osd section for GQ 
 *
 */
typedef struct 
{
	BOOL bEnable;
	unsigned int backColor;
	unsigned int fontColor;
	int posX;
	int posY;
	int text_line_num;
	char text[TEXT_LINE_NUM][256];
}mchnosd_section_attr;

/**
 *@brief ��ʼ��
 *
 */
int mchnosd_init(void);

/**
 *@brief ����
 *
 *
 */
int mchnosd_deinit(void);

/**
 *@brief ��ʱ�䣨������ת����ָ����ʽ���ַ���
 *
 *@param timeFormat ���ڸ�ʽ���磺YYYY-MM-DD hh:mm:ss��Y�����꣬M��������������
 * 				˳�����⣬���磬Ҳ����YYYY��MM��DD��hh:mm:ss
 *@param nsecond ������time(NULL)�ķ���ֵ
 *@param str ���ڱ�������ʱ����ڴ�ռ�
 *
 *@return str�ĵ�ַ
 *
 */
char *mchnosd_time2str(char *timeFormat, time_t nsecond, char *str);

/**
 *@brief ����OSD�Ĳ���
 *
 *@param channelid ͨ����
 *@param attr Ҫ���õ�����
 *
 *@return 0 �ɹ�
 *
 */
int mchnosd_set_param(int channelid, mchnosd_attr *attr);

/**
 *@brief ��ȡOSD�Ĳ���
 *
 *@param channelid ͨ����
 *@param attr ���ڴ洢Ҫ��ȡ�����Ե�ָ��
 *
 *@return 0 �ɹ�
 *
 */
int mchnosd_get_param(int channelid, mchnosd_attr *attr);


/**
 *@brief ֹͣOSD
 *
 *@param channelid ͨ����
 *
 *@return 0 �ɹ�
 *
 */
int mchnosd_stop(int channelid);

/**
 *@brief ˢ�£�ʹ������Ч
 *
 *@param channelid ͨ����
 *
 *@return 0 �ɹ�
 *
 */
int mchnosd_flush(int channelid);

/**
 *@brief ����ͨ���ŵ�����
 *
 *@param channelid ͨ���ţ�-1��ʾ����ͨ��
 *
 *@return 0 �ɹ�
 *
 */
int mchnosd_set_name(int channelid, char *strName);

/**
 *@brief �޸�osd�Ƿ�ɫ
 *
 *@param channelid ͨ���ţ�-1��ʾ����ͨ��
 *
 *@return 0 �ɹ�
 *
 */
int mchnosd_set_be_invcol(int channelid,BOOL bInvColEn);

/**
 *@brief ����ͨ���ŵ�����
 *
 *@param channelid ͨ���ţ�-1��ʾ����ͨ��
 *
 *@return 0 �ɹ�
 *
 */
int mchnosd_set_time_format(int channelid, char *timeFormat);

//�����Ƿ���ʾ�����ο�����������֮��
void mchnosd_display_focus_reference_value(BOOL bDisplay);

//���õ���ģʽҪ��ʾ������
void mchnosd_debug_mode(BOOL bDebug, char *display);

typedef struct{
	multiosd_pos_e attrPos;
	multiosd_align_e attrAlign;
	int multiOsdHandle;
}multiosd_info_t;

typedef struct{
	int x;	//pos
	int y;	//pos
	int endx;
	int endy;
	int rows;	//����
	int columns;	//��������ÿ�е�����
	int mainFontSize;
	int subFontSize;
	char text[TEXT_LINE_NUM][256];
	int text_w[3][TEXT_LINE_NUM];
	int max_w[3];
}mchnosd_region_t;

/*
 *@brief �����û��Զ�����ʾ����
 *
 *@param channelid ͨ���ţ�-1��ʾ����ͨ��
 *@param param �����С�Ȳ���
 */
int mchnosd_region_create(int channelid, mchnosd_region_t *region);

int mchosd_region_stop();

/*
 *@brief ���ö�����ʾ��Ϣ
 *
*/
int multiple_set_param(int mposition,int malignment);

/*
 *@brief �����û��Զ�����ʾ����
 *
 *@param handle region�ľ��,  #mchnosd_region_create �ķ���ֵ
 */
int mchnosd_region_destroy(int handle);

/**
 *@brief
 *
 *@param handle region�ľ���� #mchnosd_region_create �ķ���ֵ
 *@param draw
 */
int mchnosd_region_draw(int handle, mchnosd_section_attr *draw);
/**
*���ܷ�����Ⱥ�ܶ���ʾ
**/
int mchnosd_ivp_cde_draw(int rate);
/**
*���ܷ���ˢ��
**/
int __chnosd_ivpcde_flush(int channelid);

/**
 *@brief
 *@param ��ȡ�Ƿ���OSDʱ��ֿ�ģʽ
 *
 */
BOOL mchnosd_get_seperate();

//����ΪԤ�õ�ͱ䱶��ʾ������Ϣ
typedef struct
{
	int osdhandle[MAX_STREAM];
	int fontsize[MAX_STREAM];

	pthread_mutex_t mutex;
	pthread_t thread;
	BOOL bNeedRefresh;  //ˢ�±�־
	BOOL bRunning;
	BOOL bZoomCalling;  //�䱶���ñ�־
	BOOL bPresCalling;  //Ԥ�õ���ñ�־
	int preset;         //Ԥ�õ��
	char content[24];   //��ʾ��������
	BOOL bOSDHide;      //���ر�־(��OSD�ֶ�����"����"��10S�Զ�����)
	BOOL bNoMove;       //�����ƶ���־������10S�Զ�����
	BOOL bNoChange;     //��ʾ�����ޱ仯��־������10S�Զ�����
	
} ptzosdstatus_t;

ptzosdstatus_t *mosd_get_ptzstatus();

int mchnosd_draw_text(int channelid, int osdHandle, int px, int py, char *text, int fontsize);
/****************************************************************************************************************/
/*
 * ����ץͼosd�������С
 */
int mosd_set_snap_flush(int fontsize);
int mchnosd_snap_add_txt(int id,char *str);
int mchnosd_snap_del_txt(int id);
/****************************************************************************************************************/

/****************************************************************************************************************/
/*
 * ��̨�˵�
 */
#define VSC_OSD_CNT 3  //��̨�˵���ʾ3������
#define OSD_MAX_ROW 12	//���12��
#define OSD_MAX_COL 26	//1��֧��12�����ĺ��֣�ʵ��ֻ��ʾ11��

typedef struct
{
	BOOL newPage;	//�µ�һҳ
	int change[2];	//��ҳ���ı���к�:1�л�2��
	char text[OSD_MAX_ROW][OSD_MAX_COL]; //�����ַ���
}mvisca_osd_attr;

int mvisca_osd_copy(mvisca_osd_attr *from, int len);	//�ӽ��ջ�������������ʾ������
int mvisca_osd_build(int channelid);					//������̨�˵�����
int mvisca_osd_flush(int channelid);					//�޸�����ˢ�²˵�����
/****************************************************************************************************************/

#endif

