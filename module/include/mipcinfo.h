#ifndef _MIPCINFO_H_
#define _MIPCINFO_H_


/*
Locale Description // Short String //  Hex Value //  Decimal Value
Chinese-China // zh-cn // 0x0804 // 2052
Chinese-Taiwan // zh-tw // 0x0404 // 1028
English-United States // en-us // 0x0409 // 1033


*/

//#define LANGUAGE_CHINESE	0x0804
//#define LANGUAGE_ENGLISH	0x0409

//����
#define LANGUAGE_CN			0
#define LANGUAGE_EN			1
#define LANGUAGE_MAX		2
#define LANGUAGE_DEFAULT	LANGUAGE_CN
#define MAX_TUTK_UID_NUM	21
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	REBOOT_TIMER_NEVER,
	REBOOT_TIMER_EVERYDAY,
	REBOOT_TIMER_EVERYSUNDAY,
	REBOOT_TIMER_EVERYMOUNDAY,
	REBOOT_TIMER_EVERYTUNESDAY,
	REBOOT_TIMER_EVERYWEDNESDAY,
	REBOOT_TIMER_EVERYTHURSDAY,
	REBOOT_TIMER_EVERYFRIDAY,
	REBOOT_TIMER_EVERYSATURDAY,
}RebootTimer_e;

typedef enum {
	LED_CONTROL_AUTO,		// ���� ��ҹ��: ����ҹ��: ��
	LED_CONTROL_ON,			// ����
	LED_CONTROL_OFF,		// ����
}LedControl_e;

typedef struct
{
	char type[32];
	char product[32];
	char version[20];
	char acDevName[32];
	char nickName[36];
	unsigned int sn;
	unsigned int ystID;
	
	//GUID(4),SN(1),DATE(1),GROUP(1),CARDTYPE(1),MODEL(1),ENCYR_VER(1),YSTNUM(1),DEV_VER(1),USER(1)
	U32	nDeviceInfo[13];		
	int nLanguage;
   // int nPosition;
    
	char date[32];//format yyyy-MM-dd hh:mm:ss
	BOOL bSntp;	//�Զ�����Уʱ
	char ntpServer[64];//�Զ���ʱ�����������
	int sntpInterval;//�Զ�����Уʱ���ʱ�䣬��λΪСʱ
	int tz;	//ʱ��  -12 ~ 13
	int bDST; //Daylight Saving Time ����ʱ

	RebootTimer_e rebootDay;
	int rebootHour; //ȡֵ0��23��ʾ0�㵽23��
	LedControl_e	LedControl;	// LED���ƹ���
	BOOL bRestriction; //TRUE:���Ʋ��ֹ���;FALSE:ȡ������.
	BOOL bAlarmService;	//�Ƿ�����Ѹ��ʿ��������,TRUE:����,FALSE:������
	char portUsed[512];
	char osdText[8][64];
	unsigned int osdX;
	unsigned int osdY;
	unsigned int endX;
	unsigned int endY;
	unsigned int osdSize;
	unsigned int osdPosition;
	unsigned int osdAlignment;
	BOOL needFlushOsd;
	char lcmsServer[64];
	char tutkid[MAX_TUTK_UID_NUM];

	BOOL	bWebServer;
	U32		nWebPort;
}ipcinfo_t;

int ipcinfo_init(void);
int ipcinfo_get_port_used();

/** 
 *@brief ��ȡIPCINFO
 *@param param ��� IPCINFO�Ĳ���
 *
 *@return paramΪNULLʱ������һ���ڲ���ָ��
 *		param����NULLʱ������param��ֵ
 */
ipcinfo_t *ipcinfo_get_param(ipcinfo_t *param);

/**
 *@brief ��ȡ����оƬ�е��ͺ�
 */
unsigned int ipcinfo_get_model_code();

/** 
 *@brief ����IPCINFO
 *@param param ���� IPCINFO�Ĳ���
 *
 *@note param->date�������ã�����ʱ����Ҫ����ĺ���
 *@return 0 if success
 */
int ipcinfo_set_param(ipcinfo_t *param);

/** 
 *@brief ����ʱ��
 *@param date Ҫ���õ�ʱ�䣬���ʽΪYYYY-MM-DD hh:mm:ss
 *
 *@return 0 if success
 */
int ipcinfo_set_time(char *date);

int ipcinfo_set_time_ex(time_t *newtime);

/**
 *@brief ����ʱ��
 *@param timeZone ʱ�� ��Ϊ������Ϊ������Χ -12 ~ 13
 */
int ipcinfo_timezone_set(int timeZone);

/**
 *@brief ����ʱ��
 *@param timeZone ʱ�� ��Ϊ������Ϊ������Χ -12 ~ 13
 */
int ipcinfo_timezone_get();

IPCType_e ipcinfo_get_type();

int ipcinfo_get_type2();

int multiosd_process(ipcinfo_t *param);

/**
 *@brief �Ƿ��Ѿ�У��ʱ
 *
 */
BOOL ipcinfo_TimeCalibrated();

/**
 *@brief ���ò����ļ�д��
 *@param bString �Ƿ�Ϊ�ַ���
 *@param key
 *@param value
 *
 */

#define ipcinfo_set_value(key,value)\
	do{\
		ipcinfo_t attr;\
		ipcinfo_get_param(&attr);\
		attr.key = value;\
		ipcinfo_set_param(&attr);\
	}while(0)
	
#define ipcinfo_set_str(key,value)\
	do{\
		ipcinfo_t attr;\
		ipcinfo_get_param(&attr);\
		strcpy((char *)attr.key, (char *)value);	\
		ipcinfo_set_param(&attr);\
	}while(0)
			
	
#define ipcinfo_set_buf(key,value)\
	do{\
		ipcinfo_t attr;\
		ipcinfo_get_param(&attr);\
		memcpy(attr.key , value, sizeof(attr.key));\
		ipcinfo_set_param(&attr);\
	}while(0)
#ifdef __cplusplus
		}
#endif


#endif

