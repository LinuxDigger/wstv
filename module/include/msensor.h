
/*	mstream.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯��Ƶ��ش���
	������ʷ���svn�汾����־
*/

#ifndef __MSENSOR_H__
#define __MSENSOR_H__
#include "jv_sensor.h"

//sensor������
#define MAX_SENSOR	1
#define EFFECT_AWB				0
#define EFFECT_MIRROR			0x01
#define EFFECT_TURN				0x02
#define EFFECT_NOCOLOUR			0x03
#define EFFECT_LOW_FRAME        0x04
#define EFFECT_AGC              0x05    //�Զ�����
#define EFFECT_ABLC             0x06    //�Զ����ⲹ��

typedef enum{
	MSENSOR_DAYNIGHT_AUTO,  		//�Զ�
	MSENSOR_DAYNIGHT_ALWAYS_DAY,	//һֱ����
	MSENSOR_DAYNIGHT_ALWAYS_NIGHT,	//һֱҹ��
	MSENSOR_DAYNIGHT_TIMER,		//��ʱ����
}MSensorDaynightMode_e;	//��ҹģʽ
typedef enum{
	MSENSOR_DAYNIGHT_UART_NULL=0,  	//NULLģʽ����֧��UART
	MSENSOR_DAYNIGHT_UART_AUTO,  	//�Զ�
	MSENSOR_DAYNIGHT_UART_TODAY,	//TO����
	MSENSOR_DAYNIGHTUART_TONIGHT,	//TOҹ��
}MUARTDaynightMode_e;	//��ҹģʽ

typedef struct
{
	BOOL 			bOpenCutTest;	//��������
	unsigned int	nInterval;		//ÿ�μ��ʱ�䵥λs��MIN:0
	unsigned int	nCountTimes;			//�л�����
}cut_test_t;

typedef struct
{
	unsigned int	brightness;
	unsigned int	saturation;
    unsigned int    contrast;
	unsigned int    sharpness;
	unsigned int 	exposure;
	unsigned int	antifog;    //ȥ��ǿ��
    unsigned int    sw_cut;
    unsigned int    cut_rate;
	unsigned int    light;     //����ǿ��
	AutoExposure_t  ae;
	DRC_t drc;
    BOOL			bSupportWdr;		//�Ƿ�֧�ֿ�̬
    BOOL			bEnableWdr;		//�Ƿ�����̬
//    BOOL			bDigitalLDR;	 //�Ƿ�AE���ָ�Ӧ���棬����������light sensitive resistor
    BOOL			bDISOpen;			//�Ƿ������ַ���

    MSensorDaynightMode_e 	daynightMode;		//��ҹģʽ
    MUARTDaynightMode_e	  	uartdaynightMode;	//UART����ҹ����ģʽ
    JV_EXP_MODE_e 			exp_mode;				//�ع�ģʽ

    struct{
    	char hour;
    	char minute;
    }dayStart, dayEnd;

    unsigned int    effect_flag;    //sensorЧ������λ��bit0:awb	bit1:mirror		bit2:turn		bit3:nocloour   bit4:auto low frame 
                                    //bit5:auto gain control bit6:auto blacklight compensation
    unsigned int    ir_cut;          //ir_cut    �Ŀ���״̬������ҹ��ģʽʱ����ֹת��Ϊ��ɫģʽ�������в�������д�������ļ�
    Sence_e sence;				//Ӧ�ó��������ڡ������

	unsigned int	cutDelay;//CUT�л�ʱ���ӳ٣���λΪ�롣0���ӳ�
	JVRotate_e rotate; //��ת������ʵ������ģʽ
	Image_D    imageD; //ͼ�������ȼ��𣬼���Խ��Խ����������Խ��
	BOOL       AutoLowFrameEn;  //�Ƿ�ҹ���Զ���֡
	BOOL bSupportSl;  //�Ƿ�֧���ǹ⼶
	BOOL bEnableSl;  //�Ƿ����ǹ⼶

	cut_test_t stCutTest;
}msensor_attr_t;

/**
 *@brief ��ȡ��ǰ��Rotate״̬��
 *@note ���״̬���Ǹտ���ʱ��״̬����;��ʹͨ��#msensor_setparam �޸ģ���������ȡ����Ȼ��ԭ����ֵ��ֻ����������Ч
 */
JVRotate_e msensor_get_rotate();

/*******************�ֲ��ع�****************************/
#define RECT_MAX_CNT	4		//�����ľ���������
typedef struct _LOCAL_EXPOSURE
{
	BOOL	bLE_Enable;			//�Ƿ����ֲ��ع�
	BOOL	bLE_Reverse;		//����ѡ
	U32		nLE_Weight;			//�ع�Ȩֵ
	U32		nLE_RectNum;			//�ع�������������Ϊ10
	RECT	stLE_Rect[RECT_MAX_CNT];
}MLE_t;
/*******************�ֲ��ع�****************************/

/**
 *@brief ͨ��#rotate ������ת���λ��
 */
int msensor_rotate_calc(JVRotate_e rotate, int viw, int vih, RECT *rect);

/**
 *@brief ����sensorģ��Ĳ���
 *
 *@param pstAttr ����Ĳ���
 *
 */
void msensor_setparam(msensor_attr_t *pstAttr);

/**
 *@brief ��ȡsensorģ��Ĳ���
 *
 *@param pstAttr ����Ĳ���
 *
 */
void msensor_getparam(msensor_attr_t *pstAttr);
/**
 *@brief ��ȡsensorģ��Ĳ���
 *
 *@param pstAttr ����Ĳ���
 *
 */
void msensor_init(int sensorid);

/**
 *@brief ˢ��sensorʹ���ѱ���Ĳ���
 *
 *@param sensorid sensor�ţ�Ŀǰ�����Ϊ��sensor������0����
 *
 */
void msensor_flush(int sensorid);


/**
 * ��ȡ�Խ��Ľ���ֵ
 */
int msensor_get_focus_value(int sensorid);

/**
 * @brief ���þֲ��ع����
 *
 * @param
 */
int msensor_set_local_exposure(int sensorid,MLE_t *mle);
/*
 * @brief ��ȡ�ֲ��ع����
 */
int msensor_get_local_exposure(int sensorid,MLE_t *mle);
/**
 * �����Զ��ع�Ĭ��ֵ
 */
int msensor_exposure_set_default(int sensorid);

/**
 * ��ȡ���ַ���״̬
 * ����ֵ���������ַ��������ر�״̬��1Ϊ����0Ϊ�ر�
 */
int msensor_dis_get(int sensorid);
/*
 *�������ַ���״̬
 *bDISOpen ���ַ�������״̬0Ϊ�رգ�1Ϊ����
 *����ֵ��0�ɹ�-1ʧ��
 */
int msensor_dis_set(int snesorid,BOOL bDISOpen);

/*��ȡsensorҹ��״̬*/
BOOL msensor_mode_get();

int msensor_callback(int *param);

/* ���ð׹���Ƿ�ʹ�� */
void  msensor_set_whitelight_function(BOOL bEnable);
BOOL  msensor_get_whitelight_function();

/* �׹�Ʊ�����˸ʱ��֪ */
void msensor_set_alarm_light(BOOL bEnable);

/* ��ȡ��ǰȫ��ģʽ�µİ׹��״̬ */
BOOL msensor_get_whitelight_status();
#endif

