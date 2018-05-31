#ifndef _3518_ISP_H
#define _3518_ISP_H

#include"jv_common.h"
#include"hi_comm_isp.h"

#define ADJ_SCENE	0x80040026
#define IN_DOOR		0x01
#define OUT_DOOR	0x02
#define NIGHT_MODE  0x03      //һ�����ó�����ģʽ-- ���ñ��Ͷ�Ϊ0������AE�㷨��DGain������AE Target����AE ����Ȩ�ء�
#define DAY_MODE    0x04      //���óɰ���ģʽ����Ҫ�ָ�brightness��ֵ��
#define CHECK_NIGHT   0X05      //WDR sensor ����ģʽ
#define CHECK_DAY     0x06      //WDR sensor ��������û�������������
#define DEFAULT       0x07      //Ĭ��
#define MODE1         0x08      //���


#define ADJ_BRIGHTNESS  0x80040015
#define ADJ_SATURATION  0x80040016
#define ADJ_CONTRAST    0x80040012
#define ADJ_COLOURMODE  0x80040019
#define ADJ_SHARPNESS   0x80040024
#define SFHDR_SW        0x80040040  //������ر����HDR  ���������öԱȶȣ��ٿ���HDR �ָ�ʱ��ҲҪ�ָ��Աȶ�
#define HDR_ON          0x01
#define HDR_OFF         0x02

#define MIRROR_TURN     0x80040014
#define PFI_SW          0x80040041 //����������Ƶ����  ��ʱISP���͵�25֡  ��ʱ���»ָ���30֡ ��������ʹ��
#define PFI_OFF         0x01       //��isp_ioctl(0,PFI_SW, PFI_ON);
#define PFI_ON          0x02       //��isp_ioctl(0,PFI_SW, PFI_OFF);

#define LOW_FRAME       0x80040043 //��֡

#define GET_ID          0x80040042 //��ȡsensor ID

#define WDR_SW   0x80040027 //����WDR
typedef struct I2C_DATA_S

{

	unsigned char	dev_addr; 

	unsigned int 	reg_addr; 

	unsigned int 	addr_byte_num; 

	unsigned int 	data; 

  	unsigned int 	data_byte_num;

  	

}I2C_DATA_S ;

/*
	Ϊ�˼��sensor�½������ṹ��
*/
typedef enum {
	I2C,
	SPI
}	Comm_Type_e;

typedef struct {
	unsigned int device_addr;
	unsigned int reg_addr;
	unsigned int reg_width;
	unsigned int data_width;
}	I2c_Attr_e;

typedef struct {
	SensorType_e  SensorType;
	char * Sensor_Name;
	/*ͬһ��sensor�ڲ�ͬ�Ľӿ�(dvp,mipi)�����ڲ�ͬ�Ļ�����(���� ����Ĳ�ͬ��ͷ��)�����������Ϊ��ͬsensor����*/
	char * devName;
	HI_BOOL Check_Flag;
	Comm_Type_e	  CommType;
	union {
		int spi_id_reg;
		I2c_Attr_e I2c_Attr;  
	};
	int Id_Reg_Value;
}	Check_Sensor_s;


/**
 *@brief operate HISI ISP
 *@param fd reserved;cmd operate cmd; value operate value
 *@retval 0 if success
 *@retval <0 if failed. JV_ERR_XXX
 */
 /*
˵��:
1.ʹ�ô˺��������˺ڰ�ģʽ֮�󣬱��ͶȻ�����Ϊ0����ת��Ϊ��ɫģʽʱ����Ҫ���»ָ����Ͷȡ�
2.ʹ�ô˺���������HDR֮���ڹر�HDR�ĵط�����Ҫ���»ָ��Աȶ�ֵ��
3.ʹ��һ������Ϊҹ��ģʽʱ���������������öԱȶȡ�����ֵ����ת��Ϊ����ģʽʱ����Ҫ���»ָ��Աȶȡ�����ֵ��
4.ʹ��GET_ID���� ��ȡ��ǰ��sensor ID �Ӷ�����sensor,������jv_common ��ʼ��ʱѡ����ȷ�ĺ���
*/
int isp_ioctl(int fd,int cmd,unsigned long value);

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/*
//gyd add for  isp control   20130415
*/
/*
AE ctrl
*/

typedef struct 
{
//�ع�����
    ISP_OP_TYPE_E type;

//�ֶ��ع�����
    HI_S32 s32AGain; 
    HI_S32 s32DGain; 
    HI_U32 u32ExpLine; 
    HI_U32 u32ISPDGain;
    HI_BOOL bManualExpLineEnable; 
    HI_BOOL bManualAGainEnable; 
    HI_BOOL bManualDGainEnable; 
    HI_BOOL bManualISPDGainEnable;

//�Զ��ع�����
    ISP_AE_MODE_E enAEMode;		
    HI_U16 u16ExpTimeMax;       
    HI_U16 u16ExpTimeMin;       
    HI_U16 u16DGainMax;        
    HI_U16 u16DGainMin;         
    HI_U16 u16AGainMax;			
    HI_U16 u16AGainMin;         
    HI_U8  u8ExpStep;			
    HI_S16 s16ExpTolerance;		
    HI_U8  u8ExpCompensation;	
    //ISP_AE_FRAME_END_UPDATE_E  enFrameEndUpdateMode;
    HI_BOOL bByPassAE;
    HI_U8 u8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN]; 

  

    
}EXPOSURE_DATA;

 

int isp_exposure_set(EXPOSURE_DATA * pdata);

int isp_exposure_get(EXPOSURE_DATA * pdata);



/*
WB ctrl
*/
typedef struct
{
      //wb mode
      ISP_OP_TYPE_E type;

      //manual mode
      HI_U16 u16Rgain;      
      HI_U16 u16Ggain;      
      HI_U16 u16Bgain;

    //auto mode
    HI_U8 u8RGStrength;        
    HI_U8 u8BGStrength;        
    HI_U8 u8ZoneSel;          
    HI_U8 u8HighColorTemp;     
    HI_U8 u8LowColorTemp;     
    HI_U8 u8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN]; 

      
     
    
}WB_DATA;



int isp_wb_set(WB_DATA *pdata);

int isp_wb_get(WB_DATA *pdata);


int isp_wdr_init();

/*
gamma ctrl
*/
typedef struct
{
    ISP_GAMMA_ATTR_S bEnable; 
    ISP_GAMMA_CURVE_TYPE_E enGammaCurve; 
    HI_U16 u16Gamma[257]; 
    HI_U16 u16Gamma_FE[257];


    
    
}GAMMA_DATA;


int isp_gamma_set(GAMMA_DATA *pdata);

int isp_gamma_get(GAMMA_DATA *pdata);


/*
sharpen ctrl
*/
typedef struct 
{
    HI_BOOL bEnable;
	HI_BOOL bManualEnable;
	HI_U8 u8StrengthTarget;   
	HI_U8 u8StrengthMin;      
    HI_U8 u8SharpenAltD[8]; 
    HI_U8 u8SharpenAltUd[8]; 

   
}SHARPEN_DATA;


int isp_sharpen_set(SHARPEN_DATA *pdata);

int isp_sharpen_get(SHARPEN_DATA *pdata);


/*CCM ctrl*/

typedef struct 
{
    HI_BOOL bSatManualEnable; 
    HI_U8 u8SatTarget; 
    HI_U8 au8Sat[16]; 

    HI_U32 pu32Value;
    
    HI_U16 u16HighColorTemp; 
    HI_U16 au16HighCCM[9]; 
    HI_U16 u16MidColorTemp; 
    HI_U16 au16MidCCM[9]; 
    HI_U16 u16LowColorTemp; 
    HI_U16 au16LowCCM[9]; 

    

    
}CCM_DATA;


int isp_ccm_set(CCM_DATA * pdata);

int isp_ccm_get(CCM_DATA * pdata);
    
int  jv_sensor_get_ircut_staus();


/**
 *@brief  ����sensor��׼֡Ƶ
 *@param  fps:���õĻ�׼֡Ƶ
 *@retval 0 if success
 *@retval <0 if failed. 
 */
int isp_set_std_fps(int fps);



int isp_get_std_fps();



/**
 *@brief ��ȡ��ǰ��sensor֡Ƶ
 *@param void
 *@retval  ��ȡ��֡Ƶ
 */
float isp_get_fps();

/**
 *@brief ��֡�ӿ�
 *@param ratio ��֡����
 *@retval 0 if success
 *@retval <0 if failed. 
 */
int isp_set_low_fps(int ratio);


/**
 *@brief �ж��Ƿ�֡
 *@param fd reserved;cmd operate cmd; value operate value
 *@retval TRUE  :��֡
 *@retval FALSE:δ��֡. 
 */
BOOL isp_check_low_fps();

#endif
