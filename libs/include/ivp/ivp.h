/***********************************************************
*ivp.h - Intelligent video parse head file
*
* Copyright(c) 2014~ 
*
*$Date: $ 
*$Revision: $
*
*-----------------------
*$Log: $
*
*
*01a, 14-10-13, Zhushuchao created
*
************************************************************/
#ifndef __IVP_H_
#define __IVP_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"
#include "zlist.h"

#include "../../../porting/hi3516D/include/hi_comm_video.h"

#define IVP_RULE_MAX 4

typedef enum
{
    e_IVP_MODE_DRAW_FRAME = 0x1,   //�������߻����
    e_IVP_MODE_FLUSH_FRAME = 0x2,  //��������ʱ���߻����������˸
    e_IVP_MODE_MARK_OBJECT = 0x4,  //��Ǳ�������
    e_IVP_MODE_MARK_SMOOTH = 0x8, //ƽ��ģʽ
    e_IVP_MODE_MARK_ALL = 0x10,      //���ȫ���˶�����
    e_IVP_MODE_COUNT_SHOW = 0x20, //��ʾץ��/����ͳ������ 
    e_IVP_MODE_COUNT_INOUT = 0x40, //����ͳ��
    e_IVP_MODE_PLATE_SNAP = 0x80,  //����ץ��ģʽ����������ȫ��������Ч��ֻ�ܻ�����
    e_IVP_MODE_PLATE_RECOGNIZING = 0x100, //����ʶ�� 
    e_IVP_MODE_FIRE_DETECT = 0x200, //������
    e_IVP_MODE_ABANDONED_OBJ_DETECTION = 0x400, //����������
    e_IVP_MODE_REMOVED_OBJ_DETECTION = 0x800, //������������
    e_IVP_MODE_CROWD_DENSITY_ESTIMATE = 0x1000, //��Ⱥ�ܶȹ���
    e_IVP_MODE_HOVER_DETECTION = 0x2000, //��Ա�ǻ����
    e_IVP_MODE_HEAT_MAP = 0x4000, //�ȶ�ͼ
    e_IVP_MODE_FAST_MOVE = 0x8000, //�����ƶ�
    e_IVP_MODE_VAR_RATE = 0x10000, //ռ���ʼ��ģʽ��������ڽϳ�ʱ��������ģʽͬʱ����
    e_IVP_MODE_SCENE_CHANGE = 0x40000,
    e_IVP_MODE_LPR = 0x1000000, //����ʶ�� 
}IVP_MODE;

typedef enum
{
    /*�������*/
    e_IVP_CHECK_MODE_AREA_IN = 0x1,  
    e_IVP_CHECK_MODE_AREA_OUT = 0x2, 
    e_IVP_CHECK_MODE_AREA = 0x3, 
    /*�ڵ����*/
    e_IVP_CHECK_MODE_HIDE = 0x4,
    /*�齹���*/
    e_IVP_CHECK_MODE_VIRTUAL_FOCUS = 0x8,
    /*���߼��*/
    e_IVP_CHECK_MODE_LINE_L2R = 0x10,  /*�������ҿ���߱���*/
    e_IVP_CHECK_MODE_LINE_R2L = 0x20,  /*�����������߱���*/  
    e_IVP_CHECK_MODE_LINE_LR = 0x30,   /*����˫�򱨾�*/
    e_IVP_CHECK_MODE_LINE_U2D = 0x40,
    e_IVP_CHECK_MODE_LINE_D2U = 0x80,
    e_IVP_CHECK_MODE_LINE_UD = 0xc0,
    /*����ץ��*/
    e_IVP_CHECK_MODE_CAR_SNAP = 0x100,    
    /*������ȡ���*/
    e_IVP_CHECK_MODE_ABANDONED_OBJ = 0x400, //����������
    e_IVP_CHECK_MODE_REMOVED_OBJ = 0x800, //������������
    /*��Ⱥ�ܶȹ���*/
    e_IVP_CHECK_MODE_CROWD_DENSITY = 0x1000, //��Ⱥ�ܶȹ���
    /*��Ա�ǻ����*/
    e_IVP_CHECK_MODE_HOVER = 0x2000, //��Ա�ǻ����
    e_IVP_CHECK_MODE_HEAT_MAP = 0x4000, //�ȶ�ͼ
    /*�����ƶ�*/
    e_IVP_CHECK_MODE_FAST_MOVE = 0x8000, //�����ƶ�
    e_IVP_CHECK_MODE_MOTION_RATE = 0x20000,  //�ƶ�Ŀ�����
    e_IVP_CHECK_MODE_SCENE_CHANGE = 0x40000,
}IVP_CHECK_MODE;

typedef enum
{
    e_IPV_ALARM_TYPE_CLEAR = 0,
    e_IPV_ALARM_TYPE_IN = 0x1,
    e_IPV_ALARM_TYPE_OUT = 0x2,
    e_IPV_ALARM_TYPE_HIDE = 0x4,
    e_IPV_ALARM_TYPE_VIRTUAL_FOCUS = 0x8,
    e_IPV_ALARM_TYPE_L2R = 0x10,
    e_IPV_ALARM_TYPE_R2L = 0x20,
    e_IPV_ALARM_TYPE_LR = 0x30,
    e_IPV_ALARM_TYPE_U2D = 0x40,
    e_IPV_ALARM_TYPE_D2U = 0x80,
    e_IPV_ALARM_TYPE_UD = 0xc0,
    e_IPV_ALARM_TYPE_COUNT = 0x100,
    e_IPV_ALARM_TYPE_FIRE = 0x200, //����/������, areaIndex=0Ϊ���棬areaIndex=1Ϊ����
    e_IPV_ALARM_TYPE_ABANDONED_OBJ = 0x400, //����������
    e_IPV_ALARM_TYPE_REMOVED_OBJ = 0x800, //������������
    e_IPV_ALARM_TYPE_CROWD_DENSITY = 0x1000, //��Ⱥ�ܶȹ���
    e_IPV_ALARM_TYPE_HOVER = 0x2000, //��Ա�ǻ����
    e_IPV_ALARM_TYPE_OCL = 0x3000, //��Ա����
    e_IPV_ALARM_TYPE_HEAT_MAP = 0x4000, //�ȶ�ͼ
    e_IPV_ALARM_TYPE_FAST_MOVE = 0x8000, //�����ƶ�
    e_IVP_ALARM_TYPE_SCENE_CHANGE = 0x40000,
}IVP_ALARM_TYPE;

typedef enum {
    e_IVP_RES_16_9 = 1,
    e_IVP_RES_4_3,
    e_IVP_RES_BUTT,
} IVP_RES_TYPE;

typedef enum
{
    e_IVP_ABANDON_CTL_TYPE_ALARM_TIME = 0,  // ����/��ȡ���屨��ʱ��
    e_IVP_ABANDON_CTL_TYPE_SENSITIVITY = 1,  //������ȡ���������,������Ϊ0-5,Ĭ��Ϊ3
}IVP_ABANDON_CTL_TYPE;

typedef struct
{
    int x;
    int y;
}IVP_POINT;

typedef struct
{
    int x;
    int y;
    int x2;
    int y2;
}IVP_RECT;

typedef struct
{
    _UINT8 st; /*Ϊ1��λ������һ�����򴥷�����*/
    _UINT8 mode; /*IVP_ALARM_TYPE���*/
    _UINT8 count;  
    _UINT8 count1;
    _UINT8 count2;
    _UINT8 count3;
    _UINT8 id;
    _UINT8 type; /*IVP_ALARM_TYPE for trace area out*/
    _INT16 xm;
    _INT16 ym;
    _UINT8 st0;
    _INT8 lst;
    _UINT16 stayTime;
    _UINT32 np;
    int x;
    int y;
    int x2;
    int y2;
    IVP_RECT dr;
    void * v;
    //void *prev;
}IVP_AREA;

typedef struct
{
    _UINT32 u32PhyAddr[3];
    void  *pVirAddr[3];
    _UINT32 u32Stride[3];
    void *res; //�ⲿ���룬�ͷ�ʱ����
}IVP_FRAME;

/*ռ�������������ݽṹ��ռ������ʹ��Z_LIST�洢��ÿ��Z_NODE �� data ָ��һ�����ݽṹ*/
/*������������ͷ����������ߴ��Ϊ3:1
  ����ʱԴͼ����320*240���õ���������ΧΪ106*80
*/
typedef struct
{
  int row;
  int start;
  int end;
}ivpINTERVAL;
/*�����ص�
    areaIndex=������������
    type>0��������type=0�������
    areasΪ��������IVP_AREA�б�

    ����ͳ��ʱ,
        areaInddex = ��������<<16|������
        type = e_IPV_ALARM_TYPE_COUNT
        areas = null
*/
typedef void (*ON_IVP_ALARM)(void *ivp, int areaIndex, _UINT32 type, Z_LIST *areas);

typedef void (*ON_IVP_RATE)(void *ivp, double rate, struct timeval tv);

/* Generic Callback Function */
typedef void (*ON_IVP)(void *ivp, void * parg);

/*����ʶ��ص�����*/
/**
 * ON_IVP_LPR ivp lpr callback type
 * @param ivp [in] ivp handle
 * @param parg [in] ���趨�Ļص����ͽ���
 */
typedef void (*ON_IVP_LPR)(void *ivp, const void * parg);

/*
��̨����״̬��ȡ�ص�
    ������ʱ����1,���򷵻�0
*/
typedef int (* IVP_PTZ_CALLBK)(int chn);

/*��ͼ�ص�,����nvr����Ҫ�ⲿ���ߡ�����豸
    �߿�����ΪivpGetImageParseSize��ȡ�ķֱ��ʣ��ⲿ��Ҫת��Ϊʵ����ʾ�ֱ���
    IVP_DRAW_RECT_CALLBK:��2�����߿�ľ��ο�
    IVP_DRAW_LINE_CALLBK:��width�߿��ֱ��
    clr ΪRGB4444��ʽ, ��4λΪ͸����,clr = 0ʱ���
    IVP_DRAW_RECT_CALLBK����
       chn = ivpStart�����chn
       mode = 0ʵ�Ŀ�mode = 1���߿�
       rt = �������ϡ���������
       clr = RGB4444��ʽ��ɫ
    IVP_DRAW_LINE_CALLBK����
       chn = ivpStart�����chn
       x1,y1,x2,y2 = ��������
       clr = RGB4444��ʽ��ɫ
       width = �߿�
    ����ֵ0=�ɹ�����0Ϊ������
    ����ת���㷨���������Ϊ����x1,y1��ת��������Ϊx,y��
        int ivpWidth, ivpHeight, x, y;
        ivpGetImageParseSize(ivp, &ivpWidth, &ivpHeight);
        x = x1*ʵ�ʷֱ��ʿ��/ivpWidth;
        y = y1*ʵ�ʷֱ��ʸ߶�/ivpHeight;
*/
typedef int (*IVP_DRAW_RECT_CALLBK)(int chn, int mode, IVP_RECT *rt, _UINT16 clr);
typedef int (*IVP_DRAW_LINE_CALLBK)(int chn, int x1, int y1,  int x2,  int y2, _UINT16 clr, int width);

/*��ʼ��
    mode = ΪIVP_MODE�����
    chn = ΪҪ����viͨ��
    pixelfmt = PIXEL_FORMAT_RGB_4444����PIXEL_FORMAT_RGB_1555
    ʧ�ܷ���NULL���ɹ����ؾ��*/
void *ivpStart(_UINT32 mode, int chn, int pixelfmt);

/*
ivpStart2��Ҫ�Ļص�
*/
/*��ȡyuv���ݻص�
chn >= 0��ȡ��Ӧͨ�����ݣ�chn<0�ͷ��ѻ�ȡ������
frame = yuv���ݲ���ָ�룬���ⲿ���
timeout = ���볬ʱʱ�䣬-1Ϊ����
�ɹ�����0��ʧ�ܷ��ش�����*/
typedef int (*IVP_GET_FRAME_CALLBK)(int chn, IVP_FRAME *frame, int timeout); 
/*����/�ͷ������ڴ�ص���ע���ͨ���ص�������ͷ��ڴ�(*phyAddr��Ϊ��Ϊ�ͷ�)��
    �ɹ�����0�����phyAddr��virAddr��mapAddr 
    ʧ�ܷ��ط�0
*/
typedef int (*IVP_MEM_CALLBK)(_UINT32 size, void **phyAddr, void **virAddr, void **mapAddr); 

/*��ʼ������2�����ڴ��ⲿ��ȡyuv���ݵ����(����nvr)
    mode = ΪIVP_MODE�����
    chn = ΪҪ����ͨ��
    width,height=����ͼ����
    getFrame = ��ȡyuv֡�ص�
    memCallBk = MMZ�ڴ�����ص�
    ʧ�ܷ���NULL���ɹ����ؾ��*/
void *ivpStart2(_UINT32 mode, int chn, int width, int height, IVP_GET_FRAME_CALLBK getFrame, IVP_MEM_CALLBK memCallBk);

/*�ͷ�*/
int ivpRelease(void *ivp);

/*ע�ᱨ���ص�*/
int ivpRegisterCallbk(void *ivp, ON_IVP_ALARM callbk);

/*ע����̨״̬��ȡ�ص�,����̨���豸����Ҫע��*/
int ivpRegisterPtzCallbk(void * ivp, IVP_PTZ_CALLBK callbk);

/*ע�ửͼ�ص�*/
int ivpRegisterDrawCallbk(void * ivp, IVP_DRAW_RECT_CALLBK cbRect, IVP_DRAW_LINE_CALLBK cbLine);

/*���ü��������0~100,ֵԽСԽ����,ȱʡΪ6*/
int ivpSetSensitivity(void *ivp, int sensitivity);

/*���÷�����ͣ���೤ʱ�䱨��,��λ��,ȱʡΪ0*/
int ivpSetStaytime(void *ivp, int time);
/*���÷����ڱ����೤ʱ���ֹͣ����,��λ100ms,ȱʡΪ20*/
int ivpSetAlarmtime(void *ivp, int time);
/*�����˶������ֵ,ȱʡ40*/
int ivpSetThreshold(void *ivp, int value);

/*����/���ñ�ʶ*/
int ivpEnMode(void *ivp, IVP_MODE value, int en);

/*��λ��������*/
int ivpReset(void *ivp);

/*
��ͣ/������������������̨ʱ���ɵ��ô˽ӿ���ͣ��ָ���������
    mode = 0 ����;
    mode = 1 ֹͣ;
*/
int ivpPause(void *ivp, int mode);

/*
���ü�����
   index = ��������,
   ponitNum = �������(���߼��Ϊ2,������>2),
   points = �����б�,
   orgW/orgH = ��������ݵ�ͼ����
*/
int ivpAddRule(void *ivp,  int index, IVP_CHECK_MODE mode,  int pointNum, IVP_POINT *points, int orgW, int orgH);

/*
���ð���ҹ��ģʽ
 mode = 0; ����
 mode = 1;ҹ��
*/
int ivpSetDayNightMode(void *ivp, int mode);

/*����һ֡���ݣ�nvr��ʹ�ã���ʼ����ɺ󣬷�����Ҫ������ͨ����������H264����
   buf=֡������
   len=֡��
   ts=ʱ��
   ����0��ʾ�ɹ�����0Ϊ�������
*/
int ivpSendFream(void *ivp, _UINT8 *buf, int len, _UINT32 ts);

/*��ȡivpStartʱ�����chn*/
int ivpGetChn(void *ivp);

/*��ȡ����ͼ��ֱ��ʣ������ⲿ����ʱ��������ת����
   width = ������ָ��
   height = �߶����ָ��
   ����0��ʾ�ɹ�*/
int ivpGetImageParseSize(void *ivp, int *width, int *height);

/*��������ȡ��������
    param: type IVP_ABANDON_CTL_TYPE
    value: IVP_ABANDON_CTL_TYPE ��Ӧ����ֵ*/
int ivpAbandonDctl(void *ivp, int type, int value);

/*ע��ռ���ʱ仯�����ص�*/
int ivpRegVRAlarmCB(void *ivp, ON_IVP_RATE callback);
/*ע��ռ���ʱ仯������ɻص�*/
int ivpRegVRFinishCB(void *ivp, ON_IVP_RATE callback);
/*����ռ���ʱ仯������ֵ*/
int ivpSetVRBias(void *ivp, double bias);
/*����ռ���ʼ�����ڣ���λ:s*/
int ivpSetVRCircleT(void *ivp, int sec);
/*����ռ���ʼ���׼ֵ�������ڣ���λ:s*/
int ivpSetVRBaseUpT(void *ivp, int sec);
/*��ȡռ���ʱ仯ֵ*/
int ivpGetVR(void *ivp,double * vr,struct timeval *tv);
/*�ⲿ����ռ���ʷ����㷨ִ��*/
int ivpVRExternTriger(void *ivp);
/*����ռ���ʷ�����׼ֵ*/
int ivpUpdateVRef(void *ivp);
/*��ȡռ�÷�Χ����*/
Z_LIST *ivpGetVRContour(void *ivp);
/*ע����Ⱥ�ۼ������ص�*/
int ivpRegDensityEstimateAlarmCB(void *ivp, ON_IVP_RATE callback);
/*ע����Ⱥ�ܶȼ�����ɻص�*/
int ivpRegDensityEstimateFinishCB(void *ivp, ON_IVP_RATE callback);
/*������Ⱥ�ܶȱ�����ֵ*/
int ivpSetDensityEstimateAlarmBias(void *ivp, double value);
/*�����Ⱥ�ܶȹ���Ȩ�ر궨���ο�*/
int ivpAddDensityEstimateRect(void *ivp, IVP_RECT *rect, int rect_num, int orgW, int orgH);

/* �ȶ�ͼ �������� */
typedef enum {
    e_IVP_HEATMAP_CTL_TYPE_TIME = 0, // �ȶ�ͼ�ص�ʱ����, ��λ �� s
    e_IVP_HEATMAP_CTL_TYPE_BUTT,
} IVP_HEATMAP_CTL_TYPE;

/* �ȶ�ͼ ���� */
int ivpHeatMapctl(void *ivp, int type, int value);

/*ע���ȶ�ͼ�ص�*/
int ivpRegHeatMapCb(void *ivp, ON_IVP cb);
/* End of �ȶ�ͼ �������� */

/* �����ƶ� �������� */
typedef enum {
    /* �����ƶ��ٶȵȼ� 0 - 5,
       �ȼ�Ϊ n ʱ, ����С�� 6 - n ����򴩹��������Ұ��Ŀ�괥������ */
    e_IVP_FAST_MOVE_CTL_SPEED_LEVEL = 0,
    e_IVP_FAST_MOVE_CTL_TYPE_BUTT,
} IVP_FAST_MOVE_CTL_TYPE;

/* �����ƶ� ���� */
int ivpFastMovectl(void *ivp, int type, int value);
/* End of �����ƶ� �������� */

/* motion rate ���� */
/**
 * ivpRegMotionRateCb �ƶ�Ŀ����ʵ�ע��ص�����
 * @param ivp [in] ivp handle pointer
 * @param onMotionRate [in] callback function of motion rate
 * @return errorcode
 */
int ivpRegMotionRateCb(void *ivp, ON_IVP_RATE onMotionRate);
/* End of motion rate �������� */

/* ������� �������� */
/* �����ӳ�ƫ�Ʊ�����ֵ����Чֵ[0.0125,0.0625] */
int ivpSceneChangeSetThreshold(void *ivp, float threshold);
/* ���ñ�������ʱ������λΪ�� */
int ivpSceneChangeSetDuration(void *ivp, int duration);
/* End of ������� �������� */

/* �̻��� �������� */
/* �����̻��������ȣ���Ч��Χ(0-100],ֵԽСԽ����, ȱʡΪ30 */
int ivpFireSmokeSetSensitivity(void *ivp, int sensitivity);
/* End of �̻��� �������� */

/*�����ڵ����������0~100,ֵԽСԽ����,ȱʡΪ6*/
int ivpSetHideSensitivity(void *ivp, int sensitivity);

/*�����齹���������0~100,ֵԽСԽ����,ȱʡΪ6*/
int ivpSetFocusSensitivity(void *ivp, int sensitivity);

/*���ó���ʶ���������*/
typedef enum {
    e_IVP_LPR_PARAM_WORK_MODE = 0,
    e_IVP_LPR_PARAM_ROI,
    e_IVP_LPR_PARAM_DIRECTION,
    e_IVP_LPR_PARAM_DISPLAY_ROI,
    e_IVP_LPR_PARAM_TRIGGER_TIMEOUT,
    e_IVP_LPR_PARAM_DEFAULT_PROVINCE,
} IVP_LPR_PARAM_TYPE;

/*����ʶ����ģʽ*/
typedef enum {
    e_IVP_LPR_WORK_MODE_AUTO = 0,
    e_IVP_LPR_WORK_MODE_OUTSIDE_TRIGGER,
    e_IVP_LPR_WORK_MODE_INSIDE_TRIGGER,
} IVP_LPR_PARAM_WORK_MODE;

/* Ŀǰ���� width = 1680, height = 288 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} IVP_LPR_RECT;

/* ������б���� */
typedef enum {
    e_IVP_LPR_DIR_CCW = 0,
    e_IVP_LPR_DIR_CW = 1,
} IVP_LPR_PARAM_DIRECTION;

/* �Ƿ���ʾLPR ROI */
typedef enum {
    e_IVP_LPR_HIDE_ROI = 0,
    e_IVP_LPR_SHOW_ROI = 1,
} IVP_LPR_PARAM_DISPLAY_TYPE;

/*
 * ivplprctl ����ʶ���������
 * @param ivp [in] ivp ���
 * @param type [in] ��������
 *      e_IVP_LPR_PARAM_WORK_MODE,��Ӧ parg ���� IVP_LPR_PARAM_WORK_MODE *
 *      e_IVP_LPR_PARAM_ROI,��Ӧ parg ���� IVP_LPR_RECT *
 *      e_IVP_LPR_PARAM_DIRECTION,��Ӧ parg ���� IVP_LPR_PARAM_DIRECTION *
 *      e_IVP_LPR_PARAM_DISPLAY_ROI,��Ӧ parg ���� IVP_LPR_PARAM_DISPLAY_TYPE *
 *      e_IVP_LPR_PARAM_TRIGGER_TIMEOUT,��Ӧ parg ���� int *, ��λ ms
 *      e_IVP_LPR_PARAM_DEFAULT_PROVINCE,��Ӧ parg ���� char *
 * @param parg [in] ����ָ��
 * @return status ���óɹ�״̬
 */

int ivplprctl(void * ivp, IVP_LPR_PARAM_TYPE type, void * parg);

/* LPR �ص� YUV420 ͼ��ṹ */
typedef struct {
    int width;
    int height;
    void* yuv[3];
} IVP_LPR_YUV420;

/* LPR default �ص������ṹ */
typedef struct {
    const char * lp;
    IVP_LPR_YUV420 * org_yuv;
    IVP_LPR_RECT * lp_location;
    VIDEO_FRAME_INFO_S * org_frame;
} IVP_LPR_CB_DEFAULT_PARAM;
 
/*���ó��ƻص������Ƿ�Я����Ӧ��������*/
/**
 * IVP_LPR_CB_PARAM_TYPE ����LPR�ص�����Я������������,�ص������в��������������ɴ�ȷ��
 *  e_IVP_LPR_CB_PARAM_RESULT ����ʶ����,����Ϊchar*
 *  e_IVP_LPR_CB_PARAM_ORG_IMAGE ����ʶ������Ӧԭͼ,����ΪIVP_LPR_YUV420*
 *  e_IVP_LPR_CB_PARAM_RESULT ����ʶ������Ӧ��������,����ΪIVP_LPR_RECT*
 */
typedef enum {
    e_IVP_LPR_CB_PARAM_RESULT = 0,
    e_IVP_LPR_CB_PARAM_ORG_IMAGE,
    e_IVP_LPR_CB_PARAM_LOCATION,
    e_IVP_LPR_CB_PARAM_ORG_FRAME,
} IVP_LPR_CB_PARAM_TYPE;

/*���ó��ƻص���������*/
/**
 * IVP_LPR_CB_TYPE ����LPR�ص���������,�ص������в���������ʽ�ɴ�ȷ��
 *  e_IVP_LPR_CB_DEFAULT �ص�Ϊ����ʶ����,����Ϊchar*,�Լ���ivplprsetcbparam���趨������
 */
typedef enum {
    e_IVP_LPR_CB_DEFAULT = 0,
} IVP_LPR_CB_TYPE;

/**
 * ivplprregcb ����ʶ��ص�ע��
 * @param ivp [in] ivp ���
 * @param type [in] LPR �ص��������� IVP_LPR_CB_TYPE
 * @param cb [in] �ص�����ָ��
 * @return status ���óɹ�״̬
 */
int ivplprregcb(void * ivp, IVP_LPR_CB_TYPE type, void * cb);

/**
 * ivplprcbenparam ����ʶ��ص�Я����������ʹ��
 * @param ivp [in] ivp ���
 * @param type [in] LPR �ص��������� IVP_LPR_CB_TYPE
 * @param param [in] LPR �ص��������� IVP_LPR_CB_PARAM_TYPE
 * @return status ���óɹ�״̬
 */
int ivplprcbenparam(void * ivp, IVP_LPR_CB_TYPE type, IVP_LPR_CB_PARAM_TYPE param);

/**
 * ivplprcbdisparam ����ʶ��ص�Я����������ȥʹ��
 * @param ivp [in] ivp ���
 * @param type [in] LPR �ص��������� IVP_LPR_CB_TYPE
 * @param param [in] LPR �ص��������� IVP_LPR_CB_PARAM_TYPE
 * @return status ���óɹ�״̬
 */
int ivplprcbdisparam(void * ivp, IVP_LPR_CB_TYPE type, IVP_LPR_CB_PARAM_TYPE param);

/**
 * ivpLPRTrigger ����ʶ���ⲿ�����ӿ�
 * @param ivp [in] ivp ���
 * @return status �����ɹ�״̬
 */
int ivplprtrigger(void * ivp);



#ifdef __cplusplus
}
#endif
#endif

