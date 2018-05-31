/*
 * mivp.h
 *
 *  Created on: 2014-11-7
 *      Author: Administrator
 */

#ifndef MIVP_H_
#define MIVP_H_

#include "ivp.h"
#include "types_c.h"
#ifdef IVP_SUPPORT					//���ܷ���,��������set.sh��Ĭ�Ϲر�

#ifdef PLATFORM_hi3516D
#define IVP_RL_SUPPORT 		1		//�������ֺͰ��߼�� RL=Region Line
#define IVP_HIDE_SUPPORT 	1		//�ڵ�����
//#define IVP_TL_SUPPORT 1		//��ȡ����������TL=Take Left
//#define IVP_CDE_SUPPORT					1	//��Ⱥ�ܶ�
//#define IVP_OCL_SUPPORT                   1   //��Ա���
//#define IVP_HOVER_SUPPORT				1	//�ǻ����
//#define IVP_FM_SUPPORT			1		//�����ƶ�
//#define IVP_FIRE_SUPPORT			1	//�̻���
//#define IVP_HM_SUPPORT			1		//�ȶ�ͼ
//#define IVP_LPR_SUPPORT			1		//����ʶ��
//#define IVP_COUNT_SUPPORT 	1		//������ͳ��
#define IVP_SC_SUPPORT		1			//�������
#define IVP_VF_SUPPORT			1		//�齹���
#endif

#if (defined PLATFORM_hi3518EV200) ||  (defined PLATFORM_hi3516EV100)
//#define IVP_HIDE_SUPPORT 	1		//�ڵ�����
//#define IVP_RL_SUPPORT 		1		//�������ֺͰ��߼�� RL=Region Line
//#define IVP_HIDE_SUPPORT 	0		//�ڵ�����
//#define IVP_VF_SUPPORT			1		//�齹���
#endif

//#define IVP_RL_SUPPORT 		1		//�������ֺͰ��߼��
//#define IVP_COUNT_SUPPORT 	1		//������ͳ�ƣ���ͨ�������ע�͵���֧�ֵ����Ҫ��
//#define	IVP_HIDE_SUPPORT		1		//�ڵ�����
#endif
extern int bIVP_COUNT_SUPPORT;		//������ͳ�ƣ���������hwconfig.cfg�п��ƣ�Ĭ�Ϲر� ע���α����Ѿ�����ʹ�ã�Ĭ��Ϊ1��ʹ������ĺ궨����п���


#define MAX_POINT_NUM 10
typedef struct __MIVPREGION
{
	int						nCnt;						//��ĸ���>=2,=2ʱ�ǰ���
	IVP_POINT				stPoints[MAX_POINT_NUM];	//ͼ�εĵ������
	U32			 			nIvpCheckMode;				//���ߵķ�����
	U32 						eAlarmType;				//�����Ƿ񱻴���
	int 						staytimeout;					//������ʱʱ��
	int 						flickertimeout;				//OSD��˸��ʱʱ��
	BOOL					bAlarmed;
}MIVPREGION_t;

typedef enum{
	IVP_RL = 1,
	IVP_CDE,
	IVP_FM,
	IVP_HOVER,
}IVP_type_e;



typedef enum{
	MCOUNTOSD_POS_LEFT_TOP=0,
	MCOUNTOSD_POS_LEFT_BOTTOM,
	MCOUNTOSD_POS_RIGHT_TOP,
	MCOUNTOSD_POS_RIGHT_BOTTOM,
	MCOUNTOSD_POS_HIDE,
}mivpcountosd_pos_e;

#define MAX_IVP_REGION_NUM 4

#define	MAX_SEGMENT_CNT	80*10		//80��*10��
typedef struct _VR_REGION
{
	int 					cnt;
	ivpINTERVAL				stSegment[MAX_SEGMENT_CNT];
}IVPVRREGION_t;

typedef struct	__ALARM_OUT
{
	BOOL 			bStarting;					//�Ƿ����ڱ���,���ڿͻ��˱�����һֱ��������������Ƿ�����ͻ��˷��ͱ���

	U32 			nDelay;						//������ʱ��С�ڴ�ʱ��Ķ�α���ֻ����һ���ʼ����ͻ��˲��ܴ�����

	BOOL 		bEnableRecord;				//�Ƿ�������¼��
	BOOL 		bOutAlarm1;					//�Ƿ������ 1·�������
	U32 			bOutClient;					//�Ƿ�������ͻ��˱���
	U32 			bOutEMail;					//�Ƿ����ʼ�����
	BOOL 		bOutVMS;					//�Ƿ�����VMS������
	BOOL		bOutSound;					//���û��������
}Alarm_Out_t;
typedef struct 
{
	//����

	int					nPoints;							//��ĸ���==2,������
	IVP_POINT			stPoints[2];						//ͼ�εĵ������
	U32 					eAlarmType;				//�����Ƿ񱻴���
	int 					staytimeout;					//������ʱʱ��
	int 					flickertimeout;				//OSD��˸��ʱʱ��
	BOOL				bAlarmed;
}MIVP_CL_t, *PMIVP_CL_t;


typedef struct _RL
{
	//���߼�����������
	BOOL 			bEnable;							//�Ƿ���������Ƶ����
	
	BOOL			bHKEnable;							//����nvr�Խ�ר��
	BOOL			bCLEnable;							//����nvr�Խ�ר�ã�Խ����� cross line	ʹ��reg0
	BOOL			bRInvEnable;							//����nvr�Խ�ר�ã������������ region invasion ʹ��reg1
	BOOL			bRInEnable;							//����nvr�Խ�ר�ã������������	region in ʹ��reg2
	BOOL			bROutEnable;							//����nvr�Խ�ר�ã��뿪������� region out ʹ��reg3
	
	U32 			nRgnCnt;							//�����������
	MIVPREGION_t 	stRegion[MAX_IVP_REGION_NUM];		//��������

	MIVP_CL_t		stClimb;
	//���ߺ��������ֵĸ߼�����
	BOOL 			bDrawFrame;							//�������߻����
	BOOL 			bFlushFrame;						//��������ʱ���߻����������˸
	BOOL 			bMarkObject;						//��Ǳ�������--v2
	BOOL 			bMarkAll;							//���ȫ���˶�����

	U32 			nAlpha;								//����͸����  ����ȡ�������ˣ��������ظ�ʽ�� 20151105
	U32 			nSen;								//������
	U32 			nThreshold;							//��ֵ
	U32 			nStayTime;							//ͣ��ʱ��

	Alarm_Out_t		stAlarmOutRL;						//�������
}MIVP_RL_t, *PMIVP_RL_t;


typedef struct _COUNT
{
	//����ͳ��
	BOOL				bOpenCount;						//��������ͳ�ƹ���--v2
	BOOL				bShowCount;						//��ʾ����ͳ��--v2
	mivpcountosd_pos_e	eCountOSDPos;					//λ��
	U32					nCountOSDColor;					//������ɫ
	U32					nCountSaveDays;					//��������
	U32					nTimeIntervalReport;				//�ϱ�Ѹ��ʿƽ̨���ʱ��
	int					nPoints;							//��ĸ���>=2,=2ʱ�ǰ���
	IVP_POINT			stPoints[2];						//ͼ�εĵ������
	U32 					nCheckMode;					//���ģʽA2B B2A
	//ͼ���������
	BOOL 			bDrawFrame;							//�������߻����
	BOOL 			bFlushFrame;						//��������ʱ���߻����������˸
	BOOL 			bMarkObject;						//��Ǳ�������--v2
	BOOL 			bMarkAll;							//���ȫ���˶�����

	BOOL			nLinePosY;
}MIVP_Count_t, *PMIVP_Count_t;

typedef struct _VR
{
	BOOL 			bVREnable;						//�Ƿ���ռ���ʱ���
	int				nVRThreshold;					//ռ���ʱ�����ֵ
	int 			nSen;							//������%
	int 			nCircleTime;					//������ڣ���λs��Ĭ��3600s//��δ�ṩ���û�
	int 			nVariationRate;					//ռ����
	BOOL			bFinished;
	IVPVRREGION_t	stVrRegion;						//ռ���ʼ�ⷶΧ��
}MIVP_VR_t, *PMIVP_VR_t;

typedef struct _DETECT
{
	BOOL			bDetectEn;					//�Ƿ����ƶ�Ŀ�����
}MIVP_DETECT_t,*PMIVP_DETECT_t;

typedef struct _HIDE
{
	BOOL			bHideEnable;					//�Ƿ����ڵ�����
	int 			nThreshold;
	Alarm_Out_t		stHideAlarmOut;					//�����������
}MIVP_HIDE_t,*PMIVP_HIDE_t;

typedef struct _TakeLeft
{
	BOOL			bTLEnable;						//�Ƿ�����Ʒ������ȡ����

	BOOL			bHKEnable;						//�Ƿ��Ǻ���nvr����
	BOOL			bLEnable;						//haik �Ƿ�����Ʒ��������		left
	BOOL			bTEnable;						//haik �Ƿ�����Ʒ��ȡ����		take

	int				nTLMode;						//���ģʽ	0Left 1Take
	int				nTLAlarmDuration;				//��������ʱ��
	int				nTLSuspectTime;					//�����ж�ʱ��
	int				nTLSen;							//������

	int				nTLRgnCnt;
	MIVPREGION_t 	stTLRegion[MAX_IVP_REGION_NUM];		//��������

	Alarm_Out_t		stTLAlarmOut;					//�����������
}MIVP_TL_t,*PMIVP_TL_t;

typedef struct __MIVPRECT
{
	int						nCnt;						//�궨�����
	IVP_POINT				stPoints[4];	//�궨������������������
}MIVPRECT_t;


typedef struct _CROWD_DENSITY
{
	BOOL			bEnable;				//�Ƿ�����Ա�ܶȼ��
	int				nCDERate;				//��Ա�ܶ�ֵ
	U32 			nRgnCnt;						//�����������
	BOOL 			bDrawFrame;						//��������
	MIVPRECT_t 		stRegion[MAX_IVP_REGION_NUM];	//��������֧��
	int				nCDEThreshold;				//��Ա�ۼ�������ֵ
	Alarm_Out_t		stCDEAlarmOut;				//�����������
}MIVP_CDE_t,*PMIVP_CDE_t;

typedef struct _CRL
{
	BOOL			bEnable;						//�Ƿ�����Ա���
	int				nRgnCnt;	
    int             nExist;                      //�������ִ���Ա
    int             nLimit;                      //�����������Ա
	BOOL 			bDrawFrame;						//��������
	MIVPRECT_t 	    stRegion;					//��������
	Alarm_Out_t	    stOCLAlarmOut;					//�����������
}MIVP_OCL_t,*PMIVP_OCL_t;

typedef struct _FM	//�����ƶ��ṹ��
{
	BOOL 			bEnable;							//�Ƿ��������ƶ����
	U32 			nRgnCnt;							//�����������
	MIVPREGION_t 	stRegion[MAX_IVP_REGION_NUM];		//��������

	//�߼�����
	BOOL 			bDrawFrame;							//�������߻����
	BOOL 			bFlushFrame;						//��������ʱ���߻����������˸
	BOOL 			bMarkObject;						//��Ǳ�������--v2
	BOOL 			bMarkAll;							//���ȫ���˶�����

	U32 			nAlpha;								//����͸����  ����ȡ�������ˣ��������ظ�ʽ�� 20151105
	U32 			nSen;								//������
	U32 			nThreshold;							//��ֵ
	U32 			nStayTime;							//ͣ��ʱ��
	U32				nSpeedLevel;						//�ٶȵȼ�

	Alarm_Out_t		stAlarmOutRL;						//�������
}MIVP_FM_t, *PMIVP_FM_t;

typedef struct _HOVER		//�ǻ��ṹ��
{
	BOOL			bEnable;					//�Ƿ����ǻ�����
	Alarm_Out_t		stAlarmOut;					//�����������
}MIVP_HOVER_t,*PMIVP_HOVER_t;

typedef struct _FIRE		//�̻𱨾��ṹ��
{
	BOOL			bEnable;					//�Ƿ����̻𱨾�
	int				sensitivity;					//������
	Alarm_Out_t		stAlarmOut;					//�����������
}MIVP_FIRE_t,*PMIVP_FIRE_t;

typedef struct _VF		//�齹���ṹ��
{
	BOOL			bEnable;					//�Ƿ����齹���
	int 			nThreshold;
	Alarm_Out_t		stAlarmOut;					//�����������
}MIVP_VF_t,*PMIVP_VF_t;

typedef struct _SC		//��������ṹ��
{
	BOOL			bEnable;					//�Ƿ����������
	int				nThreshold;					//�����ӳ�ƫ�Ʊ�����ֵ,[0-100]���洦�����Ҫ��ֵ
	int 			duration;					//��������ʱ��
	Alarm_Out_t		stAlarmOut;					//�����������
}MIVP_SC_t,*PMIVP_SC_t;


typedef struct _HM		//�ȶ�ͼ�ṹ��
{
	BOOL			bEnable;					//�Ƿ����ȶ�ͼ
	int 			upCycle;					//�������ڣ��ص�
	MMLImage		image;
}MIVP_HM_t,*PMIVP_HM_t;

typedef struct _LPR		//����ʶ��ṹ��
{
	BOOL			bEnable;					//�Ƿ�������ʶ��
	char 			lp_num[8];					//���ƺ�
	IVP_LPR_PARAM_WORK_MODE work_mode;			//����ģʽ
	U32 			nRgnCnt;							
	MIVPRECT_t		stRegion;						//����,ֻ�õ�һ����
	IVP_LPR_PARAM_DIRECTION def_dir;			//���������໹���Ҳ�
	IVP_LPR_PARAM_DISPLAY_TYPE display;			//�Ƿ���ʾ�������
}MIVP_LPR_t,*PMIVP_LPR_t;


typedef struct __MIVP
{
	//�ڲ�ʹ�ñ���
	BOOL				bNeedRestart;				//�Ƿ���Ҫ����

	//�������ֺͰ��߼��
	MIVP_RL_t			st_rl_attr;
	//���߼���,���ڰ���
	//����ͳ��
	MIVP_Count_t		st_count_attr;

	//ռ���ʱ���
	MIVP_VR_t			st_vr_attr;

	//�ڵ�����
	MIVP_HIDE_t			st_hide_attr;				//�ڵ�����

	MIVP_TL_t			st_tl_attr;					//��ȡ��������

	MIVP_CDE_t			st_cde_attr;				//��Ա�ܶȹ���

    MIVP_OCL_t          st_ocl_attr;                //ӵ�����

	MIVP_FM_t			st_fm_attr;					//�����ƶ����

	MIVP_HOVER_t		st_hover_attr;				//�ǻ�����

	MIVP_FIRE_t			st_fire_attr;				//�̻𱨾�����

	MIVP_VF_t			st_vf_attr;					//�齹���

	MIVP_SC_t			st_sc_attr;					//�������

	MIVP_HM_t			st_hm_attr;					//�ȶ�ͼ

	MIVP_LPR_t			st_lpr_attr;					//����ʶ��
	//�ƶ�Ŀ�����
	MIVP_DETECT_t		st_detect_attr;
	//����ץ�ģ���δ���
	BOOL				bPlateSnap;					//����ץ��ģʽ����������ȫ��������Ч��ֻ�ܻ�����--v2

	char				sSnapRes[16];
}MIVP_t, *PMIVP_t;

typedef void* IVPHandle;

/**
 *@brief ��Ҫ����ʱ�Ļص�����
 * �ú�����Ҫ����֪ͨ�ֿأ��Ƿ��о�������
 *@param channelid ����������ͨ����
 *@param bAlarmOn �����������߹ر�
 *
 */
typedef void (*alarming_ivp_callback_t)(int channelid, BOOL bAlarmOn);

int mivp_init();

/**
 *@brief ��ʼ��
 *@return 0
 *
 */
int mivp_start(int chn);

/**
 *@brief ����
 *@return 0
 *
 */
int mivp_stop(int chn);

int mivp_count_reset(int chn);
int mivp_count_get(int channelid, MIVP_t *mivp);
int mivp_count_set(int channelid, MIVP_t *mivp);
int mivp_count_in_get();
int mivp_count_out_get();
typedef struct {
	unsigned short clear; 	//��ɫ
	unsigned short text;	//���ɫ
	unsigned short border;	//���
}IVPCountColor_t;
unsigned short mivp_count_show_color_get(IVPCountColor_t *color);

/**
 *@brief ���ñ����ص�����,֪ͨ�ֿص�
 *
 */
int mivp_set_callback(alarming_ivp_callback_t callback);

/**
 *@brief ���ò���
 *@param channelid Ƶ����
 *@param mivp Ҫ���õ����Խṹ��
 *@note �������ȷ���������Ե�ֵ������#mivp_get_param��ȡԭ����ֵ
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mivp_set_param(int channelid, MIVP_t *mivp);

/**
 *@brief ��ȡ����
 *@param channelid Ƶ����
 *@param mivp Ҫ���õ����Խṹ��
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mivp_get_param(int channelid, MIVP_t *mivp);

/*
 * @brief ���ú������ģʽ
 * bNightMode 1ҹ��ģʽ��0����ģʽ
 */
int mivp_set_day_night_mode(int bNightMode);

/**
 *@brief ʹ������Ч
 *	��#mivp_set_param֮�󣬵��ñ�����
 *@param channelid Ƶ����
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int mivp_restart(int chn);

/*
 * @brief ���ý���ģʽ
 * @param chn ͨ����
 * @param value ģʽ
 * @param en 1������0����
 */
int mivp_mode_en(int chn,IVP_MODE value, int en);

/*���ü��������0~1000,ֵԽСԽ����,ȱʡΪ1*/
int mivp_SetSensitivity(int chn, int sensitivity);

/*���÷�����ͣ���೤ʱ�䱨��,��λ��,ȱʡΪ0*/
int mivp_SetStaytime(int chn, int time);

/*�����˶������ֵ*/
int mivp_SetThreshold(int chn, int value);

/**
 *@brief ��ͣ/���� ���ܷ���
 *@param channelid Ƶ����
 *@param mode 0��ͣ��1����
 *@return 0 �ɹ���-1 ʧ��
 */
int mivp_pause(int channelid, int mode);

//�Ƿ�֧�����ܷ���(�ܿ���)
int mivp_bsupport();
//�Ƿ�֧�ְ��ߺ���������(�������ֿ���)
int mivp_rl_bsupport();
//�Ƿ�֧�����ܷ�������ͳ��(����ͳ�ƿ���)
int mivp_count_bsupport();

int mivp_detect_bsupport();
int mivp_detect_start(int chn);
/****************************************************�ڵ�����*****************************************/
int mivp_hide_bsupport();
int mivp_hide_start(int chn);
int mivp_hide_flush(int chn);
int mivp_hide_stop(int chn);
int mivp_hide_set_callback(alarming_ivp_callback_t callback);
int mivp_hide_get_param(int chn, MIVP_HIDE_t *attr);
int mivp_hide_set_param(int chn, MIVP_HIDE_t *attr);
/****************************************************�ڵ�����*****************************************/
/*********************************************************��Ⱥ�ܶȹ���****************************************************************/
int mivp_cde_bsupport();				//�Ƿ�֧��
int mivp_cde_correct(int chn);			//�궨�����ýӿ�
int mivp_cde_start(int chn);			//���ܿ����ӿ�
int mivp_cde_stop(int chn);				//���ܹرսӿ�
int mivp_cde_flash(int chn);			//������̬���ýӿ�
int mivp_cde_get_rate();
int mivp_cde_set_callback(alarming_ivp_callback_t callback);		//�����ص��ӿ�
/*********************************************************��Ⱥ�ܶȹ���**************************************************************/
/*********************************************************��Ա���**************************************************************/
int mivp_ocl_bsupport();               //��Ա���
int mivp_ocl_start(int chn);
int mivp_ocl_stop(int chn);
int mivp_ocl_flush(int chn);
int mivp_ocl_set_callback(alarming_ivp_callback_t callback);
/*********************************************************��Ա���**************************************************************/
/*********************************************************�����ƶ����**************************************************************/
int mivp_fm_bsupport();					//�Ƿ�֧��
int mivp_fm_start(int chn);				//���ܿ����ӿ�
int mivp_fm_stop(int chn);				//���ܹرսӿ�
int mivp_fm_flash(int chn);				//������̬���ýӿ�
int mivp_fm_set_callback(alarming_ivp_callback_t callback);			//�����ص��ӿ�
/*********************************************************�����ƶ����**************************************************************/
/****************************************************�ǻ�����*****************************************/
int mivp_hover_bsupport();				//�Ƿ�֧��
int mivp_hover_start(int chn);			//���ܿ����ӿ�
int mivp_hover_stop(int chn);			//���ܹرսӿ�
int mivp_hover_set_callback(alarming_ivp_callback_t callback);		//�����ص��ӿ�
/****************************************************�ǻ�����*****************************************/
/****************************************************�̻𱨾�*****************************************/
int mivp_fire_bsupport();				//�Ƿ�֧��
int mivp_fire_start(int chn);			//���ܿ����ӿ�
int mivp_fire_flash(int chn);			//����ˢ��
int mivp_fire_stop(int chn);			//���ܹرսӿ�
int mivp_fire_set_callback(alarming_ivp_callback_t callback);		//�����ص��ӿ�
/****************************************************�̻𱨾�*****************************************/
/****************************************************�齹���*****************************************/
int mivp_vf_bsupport();
int mivp_vf_start(int chn);
int mivp_vf_flush(int chn);
int mivp_vf_stop(int chn);
int mivp_vf_set_callback(alarming_ivp_callback_t callback);
/****************************************************�齹���*****************************************/
/****************************************************�ȶ�ͼ*****************************************/
int mivp_hm_bsupport();
int mivp_hm_start(int chn);
int mivp_hm_stop(int chn);
/****************************************************�ȶ�ͼ*****************************************/
/****************************************************����ʶ��*****************************************/
int mivp_lpr_bsupport();
int mivp_lpr_start(int chn);
int mivp_lpr_stop(int chn);
int mivp_lpr_flush(int chn);
/****************************************************����ʶ��*****************************************/

/****************************************************��ȡ��������*****************************************/
int mivp_tl_bsupport();
int mivp_tl_start(int chn);
int mivp_tl_stop(int chn);
int mivp_tl_set_callback(alarming_ivp_callback_t callback);
int mivp_tl_get_param(int chn, MIVP_TL_t *attr);
int mivp_tl_set_param(int chn, MIVP_TL_t *attr);
int mivp_tl_flush();
int mivp_left_set_callback(alarming_ivp_callback_t callback);
int mivp_removed_set_callback(alarming_ivp_callback_t callback);
/****************************************************��ȡ��������*****************************************/
/****************************************************�����������*****************************************/
int mivp_sc_bsupport();						//�Ƿ�֧��
int mivp_sc_start(int chn);					//���ܿ���
int mivp_sc_flush(int chn);						//ˢ��
int mivp_sc_stop(int chn);					//���ܹر�
int mivp_sc_set_callback(alarming_ivp_callback_t callback);	//�����ص�
/****************************************************�����������*****************************************/




/**
 *@brief ��ȡ������ָ�뷽ʽʡ�ڴ�
 *@param channelid Ƶ����
 */
PMIVP_t  mivp_get_info(int channelid);


/*���ñ��������೤ʱ�䱨��,��λ��,ȱʡΪ20*/
int mivp_SetAlarmtime(int chn, int time);

int mivp_climb_bsupport(void);

#endif /* MIVP_H_ */
