
/**
 *@file jv_sensor.h file about sensor
 * define the interface of sensor.
 *@author vito
 */
#ifndef _JV_SENSOR_H_
#define _JV_SENSOR_H_
#include "jv_common.h"
#define WEIGHT_ZONE_ROW 15
#define WEIGHT_ZONE_COLUMN 17
typedef enum{
	SENCE_INDOOR,
	SENCE_OUTDOOR,
	SENCE_DEFAULT,
	SENCE_MODE1,
	SENCE_MAX
}Sence_e;

typedef enum{
	IMAGE_TYPE_STD,  //��׼
	IMAGE_TYPE_HIGH_CONTRAST,//ͨ͸���
	IMAGE_TYPE_LOW_CONTRAST, //�ͶԱȶȣ���ͷ��
	IMAGE_TYPE_HIGH_SHARPNESS,//�������
	IMAGE_TYPE_MAX,
}Image_Type;



typedef enum{
	DAYNIGHT_AUTO,  		//�Զ�
	DAYNIGHT_ALWAYS_DAY,	//һֱ����
	DAYNIGHT_ALWAYS_NIGHT,	//һֱҹ��
	DAYNIGHT_TIMER,		//��ʱ����
}DaynightMode_e;	//��ҹģʽ


typedef struct {
	int light;
	int voltage;
}light_v;

typedef struct {
	int adcRValue;
	int adcLValue;
	int adcHValue;
}adc_value_v;



typedef struct {
	unsigned int  ExpThrd;
	unsigned int  LumThrd;
	BOOL  bLow;
	//unsigned int lowFramerate;  
}light_ae_node;

typedef struct {
	unsigned int  ExpThrd;
	unsigned int  LumThrd;
	BOOL  bLow_12;	//����12֡ʹ��
	BOOL  blow_5; //����5֡ʹ��
}light_ae_node_n;


typedef struct {
	BOOL   bLow[2];
	unsigned int LowRatio[2];
	unsigned int ExpLtoDThrd[2];//������������֡��ֵ
	unsigned int ExpDtoLThrd[2];//�Ӱ������Ľ�֡��ֵ
	unsigned int ExpLine[3]; //����״̬�µ�����ع�����
	
}frame_ctrl_node_n;

typedef struct {
	unsigned int  LumThrd;//ֻ��lum�������л�ʱ��
	unsigned int  ISOThrd;
}light_ctrl_node_n;



typedef enum{
	D_LOW,
	D_NORMAL,
	D_HIGH,
	D_MAX,
}Image_D;

typedef enum
{
	FRM_RATIO_FULL,
	FRM_RATIO_HALF,
	FRM_RATIO_10,
	FRM_RATIO_8,
}FRM_RATIO;

typedef enum
{
    JV_EXP_AUTO_MODE,     //�Զ��ع�
    JV_EXP_HIGH_LIGHT_MODE, //ǿ������
    JV_EXP_LOW_LIGHT_MODE, //���ⲹ��
    JV_EXP_MAX_MODE
}JV_EXP_MODE_e;

/*
 * �����ع�ģʽ
 * param mode �ع�ģʽ
 * return 0 �ɹ���<0ʧ��
 */
int jv_sensor_set_exp_mode(int sensorid,JV_EXP_MODE_e mode);

typedef enum
{
    JV_ANTIFLICK_CLOSE,	//�ر�
    JV_ANTIFLICK_50Hz,	//50Hz
    JV_ANTIFLICK_60Hz,	//60Hz
    JV_ANTIFLICK_MAX
}JV_ANTIFLICK_e;

//���ù�Ƶ����ȫ�ֱ���
int jv_sensor_set_antiflick(int sensorid,JV_ANTIFLICK_e mode);

typedef struct _JV_LOCAL_EXPOSURE
{
	U8 u8Weight[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN];
}JV_SENSOR_LE_t;
typedef void (*restore_sensor)(int id);
/**
 *@brief open sensor device
 *@param no
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int sensor_ircut_init(void);

/**
 *@brief close sensor device
 *@param no
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int sensor_ircut_deinit(void);

/**
 *@brief ����Ϊҹ��ģʽ��������һЩ���˺ڰף�CUT֮�����ɫ�Ĺ���
 */
int jv_sensor_set_nightmode(BOOL bNight);

/**
 *@brief ��ʱ���jv_sensor_b_nightmode�������˵ư��ϵ�ҹ��ģʽ
 */
BOOL jv_sensor_b_night(void);
/**
 *@brief AE���ָ�Ӧ�������������light sensitive resistor
 *@brief ��jv_sensor_b_night��������һ�����ж��Ƿ�ҹģʽ
 *@retval 0�� day 1:night
 */
BOOL jv_sensor_b_night_use_ae(void);


/**
 *@brief set brightness
 *@param brightness value 0~255
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_brightness(int sensorid, int nValue);

/**
 *@brief set antifog
 *@param antifog value 0~255
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_antifog(int sensorid, int nValue);

/**
 *@brief set saturation
 *@param saturation value 0~255
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
 int jv_sensor_light(int sensorid, int nValue);
/**
 *@brief set saturation
 *@param saturation value 0~255
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */

int jv_sensor_saturation(int sensorid, int nValue);

/**
 *@brief set contrast
 *@param contrast value 0~255
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_contrast(int sensorid, int nValue);

/**
 *@brief ����ָ��sensor�����
 *
 *@param sensorid sensor�ţ�Ŀǰ�����Ϊ��sensor������0����
 *@param sharpness ���ֵ
 *
 */
int jv_sensor_sharpness(int sensorid, int sharpness);

int jv_sensor_hue(int sensorid, int value);

/**
 *@brief set chroma
 *@param chroma value
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_chroma(int sensorid, int nValue);

/**
 *@brief set wdr
 *@param bEnable Enable or not wide dynamic range
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_wdr(int sensorid, int bEnable);

/**
 *@brief set mirror
 *@param sensorid
 *@param mirror value
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_mirror_turn(int sensorid, BOOL bMirror, BOOL bTurn);

/**
 *@brief set awb
 *@param sensorid
 *@param awb value
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_awb(int sensorid, int nValue);
/**
 *@brief set nocolour
 *@param sensorid
 *@param nocolour value
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_nocolour(int sensorid, BOOL bNoColor);

/**
 *@brief Get Reference Value for Focus
 *@param sensorid
 *@return Reference Value for Focus
 */
int jv_sensor_get_focus_value(int sensorid);
/**
 *@brief �����Ƿ�֡�� ��������ȡ���ֻ��ҹ��ʹ��
 *@param sensorid
 *@return Reference Value for Focus
 */
int jv_sensor_low_frame(int sensorid, int bEnable);

/**
 *@brief ��ȡ��֡״̬
 *
 *@param sensorid id
 *@param current OUT ��֡��ĵ�ǰ֡��
 *@param normal OUT ����֡��
 *
 *@return 1 ��ʾ��֡  0 ��ʾδ��֡
 */
BOOL jv_sensor_get_b_low_frame(int sensorid, float *current, float *normal);

/**
 *@brief set sence
 *@param sensorid
 *@param sence sence for use
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_sensor_set_sence(int sensorid, Sence_e sence);


//�Ƿ����Զ���֡����
int jv_sensor_auto_lowframe_enable(int sensorid, int bEnable);



/**
 *@brief set definition
 *@param sensorid
 *@param level for use
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */

int jv_sensor_set_definition(int sensorid, Image_D level);

struct offset_s {
		int RG_Offset;
		int BG_Offset;
		int Exp;
};

typedef struct  {
	
	///////////////////////////此两者平台相关，为白平衡分区的行列数
	unsigned char Max_Row;
	unsigned char Max_Col;
	///////////////////////////平台相关,统计信息相对�?56的精度，
	unsigned char Statistical_Accuracy;
	
	///////////////////////////红外下的R/G B/G
	int	Ratio_Rg_Ir;
	int	Ratio_Bg_Ir;
	
	/////////////////////////////一般自然光下R/G,B/G的取值范�?
	unsigned short Ratio_Rg_Natural_Light_Max;
	unsigned short Ratio_Rg_Natural_Light_Min;
	unsigned short Ratio_Bg_Natural_Light_Max;
	unsigned short Ratio_Bg_Natural_Light_Min;
	
	//////////////////////////////一般自然光下R/G,B/G的经典�?
	unsigned short Ratio_Rg_Natural_Light_Typcial;	
	unsigned short Ratio_Bg_Natural_Light_Typical;
	
	//////////////////////////////A光源下R/G,B/G的取�?
	unsigned short Ratio_Rg_A_Light;	
	unsigned short Ratio_Bg_A_Light;
	
	///////////////////////////////A光源下红白cut片的透过率的比�?
	unsigned char Cut_Ratio_A_Light;
	//////////////////////////////经典的自然光下红白片透过率的比�?
	unsigned char Cut_Ratio_Natural_Light_Typical;
	//////////////////////////////经典比例下绿色向亮度的转化比�?
	unsigned char Ratio_G_To_L;
	
	///////////////////////////合理计算点的取值空�?
	unsigned int R_Limit_Value_Max;
	unsigned int G_Limit_Value_Max;
	unsigned int B_Limit_Value_Max; 
	unsigned int R_Limit_Value_Min; 
	unsigned int G_Limit_Value_Min; 
	unsigned int B_Limit_Value_Min; 
	
	///////////////////////////大小场景下R/G,B/G的偏�?
	struct offset_s * BR_Gain_Offset; 
	int BR_Gain_Offset_Num;
} Soft_Light_Sensitive_t;


/**
 *@brief reload isp_helper thread
 */
void isp_helper_reload(void);

typedef struct{
	//ISP_AE_MODE_E  enAEMode;
	BOOL bAEME;//�Ƿ��Զ��ع�
	BOOL bByPassAE;
	BOOL bLowFrameMode; //��֡��ʽ����ʱ����
	unsigned int exposureMax; //����ع�ʱ�䡣�ع�ʱ��Ϊ �� 1/exposureMax �룬ȡֵ 3 - 100000
	unsigned int exposureMin;
	unsigned short u16DGainMax;
	unsigned short u16DGainMin;
	unsigned short u16AGainMax;
	unsigned short u16AGainMin;
    unsigned int u32ISPDGainMax;      /*RW,  the ISPDgain's  max value, Range : [0x400, 0xFFFFFFFF]*/
    unsigned int u32SystemGainMax;    /*RW, the maximum gain of the system, Range: [0x400, 0xFFFFFFFF],it's related to specific sensor*/
//	unsigned char u8ExpStep;
//	unsigned short s16ExpTolerance;
//	HI_U8  u8ExpCompensation;
	//ISP_AE_FRAME_END_UPDATE_E enFrameEndUpdateMode��
//	HI_U8 u8Weight[WEIGHT_ZONE_ROW][WEIGHT_ZONE_COLUMN];
	/*�ֶ��ع����*/
	int s32AGain;			/*RW,  sensor analog gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
	int s32DGain;			/*RW,  sensor digital gain (unit: times), Range: [0x0, 0xFF],it's related to the specific sensor */
	unsigned int u32ISPDGain; 		/*RW,  sensor digital gain (unit: times), Range: [0x0, 0xFF],it's related to the specific isp	*/
	unsigned int u32ExpLine;			/*RW,  sensor exposure time (unit: line ), Range: [0x0, 0xFFFF],it's related to the specific sensor */

	BOOL bManualExpLineEnable;
	BOOL bManualAGainEnable;
	BOOL bManualDGainEnable;
	BOOL bManualISPGainEnable;
}AutoExposure_t;

/**
 *@brief �����Զ��ع�Ĳ���
 */
int jv_sensor_ae_set(AutoExposure_t *ae);

/**
 *@brief ��ȡ�Զ��ع�Ĳ���
 */
int jv_sensor_ae_get(AutoExposure_t *ae);

typedef struct _DRC_t
{
    BOOL bDRCEnable;
    //BOOL bDRCManualEnable;        
    //unsigned int  u32StrengthTarget;  /*RW,  Range: [0, 0xFF]. It is not the final strength used by ISP. 
    //                                 * It will be clipped to reasonable value when image is noisy. */
    //unsigned int  u32SlopeMax;        /*RW,  Range: [0, 0xFF]. Not recommended to change. */
    //unsigned int  u32SlopeMin;        /*RW,  Range: [0, 0xFF]. Not recommended to change. */
    //unsigned int  u32WhiteLevel;      /*RW,  Range: [0, 0xFFF]. Not recommended to change. */
    //unsigned int  u32BlackLevel;      /*RW,  Range: [0, 0xFFF]. Not recommended to change. */
    //unsigned int  u32VarianceSpace;     /*RW,  Range: [0, 0xF]. Not recommended to change*/
    //unsigned int  u32VarianceIntensity; /*RW,  Range: [0, 0xF].Not recommended to change*/
} DRC_t;

typedef struct _JV_EXP_CHECK_THRESH_S
{
	unsigned int ToDayExpThresh; //������л��������ٽ�ֵ
	unsigned int ToDayAEDiffTresh;//�л�������AE�仯��ֵ
	
	unsigned int  ToNightExpThresh; //������л������ٽ�?
	unsigned char ToNightLumThresh;
	

	unsigned int DayFullFrameExpThresh; //ȫ֡ģʽ����ķ�ֵ
	unsigned int DayHalfFrameExpThresh; //��ȫ֡ģʽ����ķ�ֵ
	unsigned int DayLowFrameExpThresh_0; //ȫ��ģʽ�½�֡��exp�Լ�frm,��һ�������罵����ȫ֡
	unsigned int DayLowFrameExpThresh_1; //ȫ��ģʽ�½�֡��exp�Լ�frm,�ڶ��������罵��1/4ȫ֡
	unsigned char DayLowFrameRate_0;
	unsigned int OutDoorExpLineThresh;
	unsigned int InDoorExpLineThresh;
	unsigned int DayLowFrameRatio;       //���ñ�����ʽ���н�֡���޸����ڽ�֡����VI֡Ƶ����ȷ
	unsigned int DayLowFrameRatio_1;       //���ñ�����ʽ���н�֡���޸����ڽ�֡����VI֡Ƶ����ȷ

}JV_EXP_CHECK_THRESH_S;




typedef enum{
	SENSOR_STATE_UNKNOWN,
	SENSOR_STATE_DAY_FULL_FRAMRATE,  		//25֡Ƶ��ɫ
	SENSOR_STATE_DAY_HALF_FRAMRATE,  		//12֡Ƶ��ɫ
	SENSOR_STATE_DAY_QUARTER_FRAMRATE,  	//6֡Ƶ��ɫ
	SENSOR_STATE_NIGHT,		                //25֡�ڰ�ͼ��
	//SENSOR_STATE_NIGHT_FULL_FRAMRATE,        //12֡�ڰ�ͼ��
}JV_SENSOR_STATE;	



typedef enum{
	AE_UNKNOWN_MODE,
	AE_25_FULLCOLOR,  		//25֡Ƶ��ɫ
	AE_12_FULLCOLOR,  		//12֡Ƶ��ɫ
	AE_12_NO_COLOR,        //12֡�ڰ�ͼ��
}AE_COLOR_FRM_MODE;	//��ҹģʽ

typedef int (*jv_msensor_callback_t)(int *param);

int jv_msensor_set_callback(jv_msensor_callback_t callback);
int jv_msensor_set_getadc_callback(jv_msensor_callback_t callback);
/**
 *@brief ����DRC�Ĳ���
 */
int jv_sensor_drc_set(DRC_t *drc);

/**
 *@brief ��ȡDRC�Ĳ���
 */
int jv_sensor_drc_get(DRC_t *drc);

/**
 *@brief change ir cut to night or day
 */
void jv_sensor_set_ircut(BOOL bNight);

/*
 * brief ���þֲ��ع�
 * sensorid 0
 * jv_sensor_le �ֲ��ع�Ȩֵ��
 */
int jv_sensor_set_local_exposure(int sensorid,JV_SENSOR_LE_t *jv_sensor_le);
/*
 * brief ��ȡ�ֲ��ع�
 * sensorid 0
 * jv_sensor_le �ֲ��ع�Ȩֵ��
 */
int jv_sensor_get_local_exposure(int sensorid,JV_SENSOR_LE_t *jv_sensor_le);

/*
 * ��ȡ���ַ�������״̬
 * ����0û������1����
 */
int jv_sensor_get_dis(int sensorid);
/*
 * �������ַ���
 * sensorid 0
 * disopen 1:�򿪣�0�ر�
 */
int jv_sensor_set_dis(int sensorid,BOOL disopen);


//��Ժ�����ʱ�����⽫sensor֡Ƶ����һ���

int jv_sensor_frame_trip(int framerate);

/**
 *@brief set the iris/aperture  ���ڹ�Ȧ��С
 *@param sensorid
 *@param size up or down
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
int jv_iris_adjust(int sensorid, BOOL SIZEUP);

/*
 * ��ȡ��ת����
 * jvRotate ��ת����ָ��
 * ����ֵ  0�ɹ���-1ʧ��
 */
int jv_sensor_rotate_get(JVRotate_e *jvRotate);

/*
 * ������ת����
 * jvRotate  ��ת����
 * ����ֵ  0�ɹ���-1ʧ��
 */
int jv_sensor_rotate_set(JVRotate_e jvRotate);

/*
 * ���ߵײ��û������Ƿ�Ϊһֱ��ɫģʽ
 * ����ֵ  0�ɹ���-1ʧ��
 */

int jv_sensor_set_daynight_mode(int sensorid, int mode);


int jv_sensor_base_framerate_init(int rf);

int jv_sensor_set_framerate(int frameRate);

int jv_sensor_set_framerate_v2(int frameRate,BOOL bLow);


int jv_sensor_set_vi_framerate(int frm);

/*
 *@brief  ����VI���½ӿڣ�������½������ʹ�øýӿ�
 *@parms frameRate -���õ�VI֡Ƶ
 *@retval����ֵ  0�ɹ���-1ʧ��
 */
int jv_sensor_set_fps(int frameRate);

/*
 *@brief  �����ع���Բ���
 *
 */
int jv_sensor_set_exp_params(int sensorid,int value);

int jv_sensor_set_denoise(int sensorid,int value);
int jv_sensor_set_defini(int sensorid,int value);
int jv_sensor_set_desmear(int sensorid,int value);
int jv_sensor_set_fluency(int sensorid,int value);
/*
*@brief		��ȡadcʵʱֵ����cutֵ��������
*
*/
int jv_adc_get_value(adc_value_v* value);

BOOL jv_sensor_get_outindoor();

typedef enum
{
    IRCUT_DAY,
    IRCUT_NIGHT,
}IRCUT_STATUS;

int jv_sensor_get_adc_high_val();
int jv_sensor_get_adc_low_val();
int jv_sensor_get_adc_val();

BOOL jv_sensor_b_daynight_usingAE();

int jv_sensor_set_max_vi_20fps(BOOL Enable);

int jv_get_vi_maxframrate();

/* ���ð׹���Ƿ�ʹ�� */
void  jv_sensor_set_whitelight_function(BOOL bEnable);
BOOL  jv_sensor_get_whitelight_function();

/* �׹�Ʊ�����˸ʱ��֪ */
void jv_sensor_set_alarm_light(BOOL bEnable);

/* ��ȡ��ǰȫ��ģʽ�µİ׹��״̬ */
BOOL jv_sensor_get_whitelight_status();
#endif

