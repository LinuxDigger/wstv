#ifndef __JV_COMMON_H__
#define __JV_COMMON_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <pthread.h>
#include <sys/poll.h>
#include <linux/input.h>
#include <signal.h>
#include <sys/stat.h>	//mkdir����ʹ��
#include <sys/vfs.h>	//���Ӳ�̿ռ�ʹ��
#include <dirent.h>		//����Ŀ¼ʹ��
#include <errno.h>		//�鿴������ʹ��
#include <sys/wait.h>	//����ӽ���״̬ʹ��
#include <sys/socket.h>	//������AF_INET��SOCK_DGRAM��
#include <netinet/in.h>	//���������纯��
#include <arpa/inet.h>	//������inet_ntoa�Ⱥ���
#include <sys/shm.h>	//�����ڴ�ʹ��
#include <sys/mount.h>	//mount
#include <sys/param.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <mqueue.h> 
#include <semaphore.h>
#include <mqueue.h>
#include <stdarg.h>

#include "defines.h"
#include "jv_product_def.h"

//�Ƿ���԰汾
#define IPCAMDEBUG		debugFlag
extern int debugFlag;


//V1.0.248��ʾ��һ��SVN�汾���еĵ�246���汾����Ϊ��1246�����ʾΪV1.1246
extern char main_version[16];
extern char ipc_version[32];
#define MAIN_VERSION	main_version
#define SUB_VERSION		".5600"
#define IPCAM_VERSION	ipc_version


//����������ļ����������ñ���õ������ִ������
#define JOVISION_CONFIG_FILE	"/etc/jovision.cfg"


#ifndef CONFIG_PATH
#define CONFIG_PATH "/etc/conf.d/jovision/"
#endif

//���ڱ���һЩ����Զ����ı�Ĳ�����Ŀ¼
#define CONFIG_FIXED_PATH "/etc/conf.d/fixed/"

//��Զ����ı���ļ�
#define CONFIG_FIXED_FILE CONFIG_FIXED_PATH"fixed"

#define CONFIG_HWCONFIG_FILE CONFIG_FIXED_PATH"hwconfig.cfg"

#define MAX_PATH			256


/**
 * errno of jovision. 
 * It should be added with JVERRBASE_XXX
 */
#define JVERR_NO						0
#define JVERR_UNKNOWN					-1
#define JVERR_DEVICE_BUSY				-2
#define JVERR_FUNC_NOT_SUPPORT		-3
#define JVERR_BADPARAM					-4
#define JVERR_NO_FREE_RESOURCE		-5
#define JVERR_NO_FREE_MEMORY			-6
#define JVERR_NO_ASSIGNED_RESOURCE	-7
#define JVERR_OPERATION_REFUSED		-8
#define JVERR_ALREADY_OPENED			-9
#define JVERR_ALREADY_CLOSED			-10
#define JVERR_TIMEOUT					-11
//#define MD_GRID_SET

/////////////////////////////////////���ܺ궨��///////////////////////////////////////
//���Դ�ӡ�ļ������к�,lck20100417
#ifndef Printf
#define Printf(fmt...)  \
do{\
	if(IPCAMDEBUG){	\
		printf("[%s]:%d ", __FILE__, __LINE__);\
		printf(fmt);} \
} while(0)
#endif

#define __FLAG__() do{printf("==FLAG== %s: %d\n", __FILE__, __LINE__);}while(0)

#ifndef CPrintf
#define CPrintf(fmt...)  \
do{\
	if(1){	\
		printf("\33[31m");\
		printf(fmt);} \
		printf("\33[0m ");\
} while(0)
#endif

#define CHECK_RET(express)\
		do{\
			S32 s32Ret = express;\
			if (0 != s32Ret)\
			{\
				printf("FUNC:%s,LINE:%d,failed! %x\n", __FUNCTION__, __LINE__, s32Ret);\
			}\
		}while(0)

#define DEBUG_LEVEL 1

#define Debug(level, fmt...) \
	do{ \
		if (DEBUG_LEVEL >= level) {\
		printf("[%s]:%d ", __FILE__, __LINE__);\
		printf(fmt);} \
	} while(0)


#define XDEBUG(express,name) CHECK_RET(express)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array)   (sizeof(array) / sizeof(array[0]))
#endif


#define STRNCPY(dst, src, n)	snprintf(dst, n, "%s", src)

//��val��Ϊmin,max��Χ�ڵ�ֵ
#define VALIDVALUE(val, min, max)		(val<min)?min:((val>max)?max:val)

//�жϵ�ǰʱ���Ƿ���ʱ�����,ʱ��ֵΪ����ĵ�N��
#define isInTimeRange(nTimeNow, nBeginTime, nEndTime) \
    ((nBeginTime < nEndTime && (nTimeNow >= nBeginTime && nTimeNow <= nEndTime)) ||\
     (nBeginTime >= nEndTime && (nTimeNow >= nBeginTime || nTimeNow <= nEndTime)) )

//���룬align������2��N�η���
//���϶��룬���磺JV_ALIGN_CEILING(5,4) = 8
#define JV_ALIGN_CEILING(x,align)     ( ((x) + ((align) - 1) ) & ( ~((align) - 1) ) )
//���¶��룬���磺JV_ALIGN_FLOOR(5,4) = 4
#define JV_ALIGN_FLOOR(x,align)       (  (x) & (~((align) - 1) ) )


#define jv_strncpy(d, s, n) \
	do{\
		strncpy(d, s, n); \
		((unsigned char *)(d))[n-1] = '\0'; \
	}while(0)

#define jv_ystNum_parse(str,nDeviceInfo,ystID) \
	do{\
		char YSTG[4]; \
		memcpy(YSTG,&nDeviceInfo,4); \
		if(YSTG[1] == 0x0) \
			sprintf(str, "%c%d",YSTG[0], ystID); \
		else if(YSTG[2] == 0x0) \
			sprintf(str, "%c%c%d",YSTG[0],YSTG[1], ystID); \
		else if(YSTG[3] == 0x0) \
			sprintf(str, "%c%c%c%d",YSTG[0],YSTG[1],YSTG[2], ystID); \
		else \
			sprintf(str, "%c%c%c%c%d",YSTG[0],YSTG[1],YSTG[2],YSTG[3], ystID); \
	}while(0)
	 
///////////////////////////////////////���Ͷ���///////////////////////////////////////
typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int		U32;

typedef char			S8;
typedef short				S16;
typedef int					S32;

typedef unsigned long long	U64;
typedef long long			S64;

#ifdef OK
#undef OK
#endif
#define OK 0

#ifdef ERROR
#undef ERROR
#endif
#define ERROR -1

#define SUCCESS				0
#define FAILURE				(-1)

///////////////////////////////////////��������///////////////////////////////////////

#define HWINFO_STREAM_CNT hwinfo.streamCnt	//ʵ����������

#define	MAX_STREAM			3	///< ����·���ֻ����
#define MAX_MDRGN_NUM		4		//�ƶ�����������Motion Detect
#define	MAX_MDCHN_NUM		1

#define	STARTCODE			0x1153564A		//֡startcode

///////////////////////////////////////���ݽṹ///////////////////////////////////////
//��ά¼���ļ��ļ�ͷ
typedef struct tagJVS_FILE_HEADER
{
	U32	nFlag;				//¼���ļ���ʶ
	U32	nFrameWidth;		//ͼ����
	U32	nFrameHeight;		//ͼ��߶�
	U32	nTotalFrames;		//��֡��
	U32	nVideoFormat;		//��Ƶ��ʽ:0,PAL 1,NTSC
	U32	bThrowFrame_CARD;	//�Ƿ��֡��4�ֽ�
	U32	u32IIndexAddr;		//�Ƿ��֡��4�ֽ�
	//U32 u32Reserved;		//����,4�ֽ�
	U32 audioType;//jv_audio_enc_type_e
}JVS_FILE_HEADER,*PJVS_FILE_HEADER;

//��ά¼���ļ�֡ͷ
typedef struct tagJVS_FRAME_HEADER
{
	U32	nStartCode;
	U32	nFrameType:4;	//�͵�ַ
	U32	nFrameLens:20;
	U32	nTickCount:8;	//�ߵ�ַ
	//HI_U64	u64PTS;		//ʱ���
}JVS_FRAME_HEADER, *PJVS_FRAME_HEADER;

typedef struct tagRECT
{
	U32		x;
	U32		y;
	U32		w;
	U32		h;
}RECT, *PRECT;

//��ά���Ź�
typedef struct tagJDOG
{
    int 	nCount;			//��������
    int 	nInitOK;		//JDVR�Ƿ�ɹ���ʼ��
    int 	nQuit;			//�Ƿ������˳�
}JDOG, *PJDOG;

typedef struct 
{
	BOOL running;
	pthread_t thread;
	int iMqHandle;
	sem_t sem;
	pthread_mutex_t mutex;
}jv_thread_group_t;

#define jv_assert(x,express) do{\
		if (!(x))\
		{\
			printf("%s:%d ==>jv_assert failed\n", __FILE__, __LINE__);\
			express;\
		}\
	}while(0)

		
//��Ƶ֡���ͣ���NvcAudioEnc_eһ��
typedef enum{
	AUDIO_TYPE_UNKNOWN,
	AUDIO_TYPE_PCM,
	AUDIO_TYPE_G711_A,
	AUDIO_TYPE_G711_U,
	AUDIO_TYPE_AAC,
	AUDIO_TYPE_G726_40
}AUDIO_TYPE_E;

typedef enum{
	VIDEO_TYPE_UNKNOWN=0,
	VIDEO_TYPE_H264=1,
	VIDEO_TYPE_H265,
}VIDEO_TYPE_E;
	
extern int MAX_FRAME_RATE;

extern int VI_WIDTH;
extern int VI_HEIGHT;

//��������ֵ
#define RET_SUCC					0
#define RET_ERR						-1
#define RET_DEVICE_BUSY				-2
#define RET_FUNC_NOT_SUPPORT		-3
#define RET_BADPARAM				-4
#define RET_NO_FREE_RESOURCE		-5
#define RET_NO_FREE_MEMORY			-6
#define RET_NO_ASSIGNED_RESOURCE	-7
#define RET_OPERATION_REFUSED		-8

#define ENCRYPT_100W 0x5010
#define ENCRYPT_130W 0x5013
#define ENCRYPT_200W 0x5020
#define ENCRYPT_300W 0x5030
#define ENCRYPT_500W 0x5050

typedef enum{
	SENSOR_UNKNOWN,
	SENSOR_OV9712      ,
	SENSOR_OV9732      ,
	SENSOR_AR0130      ,
	SENSOR_AR0330      ,
	SENSOR_OV2710      ,
	SENSOR_OV9750      ,
	SENSOR_IMX178      ,
	SENSOR_OV4689      ,    
	SENSOR_AR0230      ,
	SENSOR_IMX290      ,
	SENSOR_IMX123      ,
	SENSOR_OV5658      ,
	SENSOR_IMX225      ,
	SENSOR_BG0701      ,
	SENSOR_BT601       ,	//������оģ��sensor�ͺ�
	SENSOR_AR0237	   ,   //AR0237MIPI
	SENSOR_AR0237DC	   ,   //AR0237DC
	SENSOR_OV9750m	   ,  //mipi�ӿڵ�ov9750
	SENSOR_SC2045      ,
	SENSOR_MN34227     ,
	SENSOR_SC2135      ,
	SENSOR_OV2735	   ,
	SENSOR_IMX291      ,//50
	SENSOR_SC2235,
	SENSOR_MAX
}SensorType_e;

/**
 * @brief �����Ͷ���IRCUT�л����ж�ģʽ
 */
typedef enum
{
	IRCUT_SW_BY_GPIO=0,		//Ŀǰǹ��ģ��Ĵ�ͳ��ʽ����GPIO����л��ٽ��
	IRCUT_SW_BY_AE,  		//�佹һ�����Ĭ����AE�л�IRCUT
	IRCUT_SW_BY_UART,  		//�佹һ���,ͨ�����ڽ���IRCUT�л��ź�
	IRCUT_SW_BY_ADC0,		//ADCͨ��0��ǹ������̨�������ͨ��ADC�ɼ���������ѹ����ʵ��IRCUT�л�
	IRCUT_SW_BY_ADC1,		//ADCͨ��1
	IRCUT_SW_BY_ADC2,		//ADCͨ��2
	IRCUT_SW_BY_ISP ,      //��Ӳ��ϻ��ߴ������
}ENUM_IRSW_MODE;

typedef enum{
	JVSENSOR_ROTATE_NONE = 0,
	JVSENSOR_ROTATE_90   = 1,
	JVSENSOR_ROTATE_180  = 2,
	JVSENSOR_ROTATE_270  = 3,
	JVSENSOR_ROTATE_MAX
}JVRotate_e;

typedef enum{
	IPCTYPE_JOVISION, //��ά��
	IPCTYPE_SW, //��ά��
	IPCTYPE_JOVISION_GONGCHENGJI,//��ά���̻�
	IPCTYPE_IPDOME,//IPC������
	IPCTYPE_MAX
}IPCType_e;

typedef struct{
	JVRotate_e rotate;
	BOOL bEnableWdr;
	BOOL bSupportWdr;
	IPCType_e ipcType;
	U32 ipcGroup;
}JVCommonParam_t;

typedef enum
{
	ALARM_DISKERROR=0,
	ALARM_DISKFULL,
	ALARM_DISCONN,
	ALARM_UPGRADE,
	ALARM_GPIN,
	ALARM_VIDEOLOSE=5,		// ��Ƶ��ʧ
	ALARM_HIDEDETECT,		// ��Ƶ�ڵ�
	ALARM_MOTIONDETECT,		// �ƶ����
	ALARM_POWER_OFF,
	ALARM_MANUALALARM,
	ALARM_GPS=10,
	ALARM_DOOR,				// 433����
	ALARM_IVP,
	ALARM_BABYCRY,
	ALARM_PIR = 15,			// PIR
	ALARM_ONE_MIN = 17,		// һ����¼�����ͱ���
	ALARM_SHOT_PUSH,		// ��������
	ALARM_REC_PUSH,			// ¼������
	ALARM_CHAT_START=20, 	//������Ƶ�Խ�
	ALARM_CHAT_STOP,		//�ر���Ƶ�Խ�
	ALARM_CAT_HIDEDETECT=22,		// ��Ƶ�ڵ�,è�۹���
	ALARM_TOTAL
}ALARM_TYPEs;

typedef enum
{
	ALARM_OBSS_MOTIONDETECT = '0',
	ALARM_OBSS_ELECEYE = '1',
	ALARM_OBSS_THIRD = '2'
	
}ALARM_OBSS_TYPES;

enum
{
	AUDIO_MODE_ONE_WAY	= 1 << 0,		// ����
	AUDIO_MODE_TWO_WAY	= 1 << 1,		// ˫��
	AUDIO_MODE_NO_WAY	= 1 << 2,		// ����Ƶ^_^
};

typedef struct
{
	struct{
		BOOL ptzBsupport;//�Ƿ�֧��PTZ
		int ptzprotocol;
		int ptzbaudrate;
	};
	BOOL wdrBsupport;	//�Ƿ�֧�ֿ�̬
	BOOL rotateBSupport; //�Ƿ�֧����ת
	char product[32]; // /etc/jovision.cfg�б���ģ�����������productֵ
	char type[32];
	unsigned int encryptCode;
	SensorType_e sensor;
	IPCType_e ipcType;

	//�������
	struct{
		int streamCnt;	//���������ԭ������3·��������Щ������ֻ��2·
	};
	BOOL bHomeIPC; //add for homeipc
	BOOL bSoftPTZ;	//�Ƿ�֧������̨
	BOOL bSupport3DLocate;	//�Ƿ�֧��3D��λ
	BOOL bEMMC;
	HW_TYPE_e HwType;	// Ӳ��ƽ̨
	char devName[32];	//IPC�豸����
	BOOL remoteAudioMode;	// AUDIO_MODE_XXX
	BOOL bNewVoiceDec;		//�Ƿ�ʹ���µ���������
	BOOL bXWNewServer;		//�Ƿ�ʹ���µ�СάAPP������
	struct{
		BOOL bMirror;	//�Ƿ���ˮƽ��ת
		BOOL bFlip; //�Ƿ��Ǵ�ֱ��ת
	};
	BOOL bSupportLDC;		//�Ƿ�֧�ֻ���У������
	BOOL bSupportXWCloud;	//�Ƿ�֧��Сά�ƴ洢
	BOOL bSupportAESens;	//�Ƿ�֧������������ȵ���
	BOOL bSupportVoiceConf;	//�Ƿ�֧����������
	BOOL bSupportPatrol;	//�Ƿ�֧����̨Ԥ�õ��Ѳ��
	BOOL bInternational;	//�Ƿ�ΪСά���ʰ�
	BOOL bCloudSee;			//�Ƿ�Ϊcloudsee��
	BOOL bSupportAVBR;		//�Ƿ�֧��AVBR
	BOOL bSupportSmtVBR;	//�Ƿ�֧������VBR���ھ�ֹ����ʱ����ͼ����������������
	BOOL bSupportMCU433;	//�Ƿ�֧�ֵ�Ƭ��433����
	BOOL bVgsDrawLine;		//�Ƿ�ʹ��VGS���ߣ����軭�ߣ���VBʹ����Ҫ��Ӧ����
	BOOL bSupportMVA;		//�Ƿ�֧��Сά���ܻ�
	BOOL bSupportReginLine;		//�Ƿ�֧���������ֺͰ���
	BOOL bSupportLineCount;		//�Ƿ�֧�ֹ��߼���
	BOOL bSupportHideDetect;	//�Ƿ�֧���ڵ����
	BOOL bSupportClimbDetect;	//�Ƿ�֧���������
	BOOL bSupportRedWhiteCtrl;  //�Ƿ�֧��ȫ��ģʽ����
	struct
	{
		ENUM_IRSW_MODE	ir_sw_mode;				//IR�л���ģʽ
		unsigned char 	ir_sensitive;			//IR�л���������,��AE�жϻ���ADC�ж�ʱһ�����á�
		int				ir_power_holdtime;		//IRͨ�籣��ʱ�䣬��λ:us
	};
}hwinfo_t;

extern hwinfo_t hwinfo;

// ��GPARAM����ŵ�jv_common.h�У�������porting�з���gp
typedef struct tm		TM;
typedef struct
{
	U32		nYTSpeed;			// ��̨ת�٣�0~255�����Ը���255������Ҫ����Ч��
	U32		nYTCheckTimes;		// ��̨�Լ�Ȧ����1: ת��Ȧ��2: תһȦ
	U32		nInterval;			// ��̨�Լ���ʱ��
	BOOL	bYTOriginReset;		// ��̨�Լ���Ƿ������1: ������0: ������
	U32		nYTLRSteps;			// ��̨�����ܲ���
	U32		nYTUDSteps;			// ��̨�����ܲ���
	U32		nYTLREndStep;		// ��̨���һ���λ��
	U32		nYTUDEndStep;		// ��̨���»���λ��
}FactoryTestCfg;

typedef struct tagGPARAM
{
	//���߳�
	BOOL	bRunning;
	time_t	ttNow;
	TM		*ptmNow;

	//�Ƿ���ʾOSD
	//BOOL	bShowOSD;
	//����ʱ���ʽ
	U32		nTimeFormat;

	//�Ի���汾��
	U32		nIPCamCfg;

	//�˳���־
	U32		nExit;

	// ��������־
	BOOL	bFactoryFlag;
	char	FactoryFlagPath[256];
	FactoryTestCfg TestCfg;

	struct{
		BOOL bTalking;
		int	nClientID;			// ����ͨ\��ý�巽ʽ��ID
		int nLocalChannel;
	} ChatInfo;

	//�Ƿ���������ͨ
	BOOL bNeedYST;
	//�Ƿ�����Zrtsp
	BOOL bNeedZrtsp;
	//�Ƿ�����web
	BOOL bNeedWeb;
	//�Ƿ�����onvif
	BOOL bNeedOnvif;
}GPARAM, *PGPRAM;

extern GPARAM gp;

typedef struct{
	int group;
	int bit;
	int onlv;
}GpioValue_t;

typedef struct{
	GpioValue_t resetkey; //Reset����
	GpioValue_t statusledR; //״ָ̬ʾ��R
	GpioValue_t statusledG; //״ָ̬ʾ��G
	GpioValue_t statusledB; //״ָ̬ʾ��B
	GpioValue_t cutcheck; //ҹ�Ӽ��
	GpioValue_t cutday; //�а���
	GpioValue_t cutnight; //������
	GpioValue_t redlight; //����ƿ���
	GpioValue_t alarmin1; //��������1
	GpioValue_t alarmin2; //��������2
	GpioValue_t alarmout1; //�������1
	GpioValue_t audioOutMute;
	GpioValue_t wifisetkey;		//wifi���ü�
	GpioValue_t cutcheck_ext; //����ҹ�Ӽ��
	GpioValue_t redlight_ext; //������������
	GpioValue_t ledlight_in;	//led�ư������
	GpioValue_t muxlight_in;	//����ư������
	GpioValue_t sensorreset; //sensor��λ
	GpioValue_t pir; // PIR���
	GpioValue_t oneminrec;		//һ����¼�񰴼�
	GpioValue_t whitelight;		//�׹�LED��
}higpio_values_t;

extern higpio_values_t higpios;

/**
 *@brief ƽ̨��ص��ܳ�ʼ��
 *
 */
int jv_common_init(JVCommonParam_t *param);

/**
 *@brief ƽ̨��صĽ���
 *
 */
int jv_common_deinit(void);

// ��ȫ�˳�ƽ̨�������˳�ý��ҵ��󣬳����˳�ƽ̨
void jv_common_deinit_platform(void);


/*
 *@brief DEBUG�õ�yst��
 */
unsigned int jv_yst(unsigned int *a,unsigned int b);

/*
 *@brief ʹ��VI��mirror��flip����
 */
 void VI_Mirror_Flip(BOOL bMirror, BOOL bFlip);

 //�����ú���������߳���Ϣ
extern void pthreadinfo_add(const char *threadName);
 
#define IsSystemFail(ret) (-1 == (ret) || 0 == WIFEXITED((ret)) || 0 != WEXITSTATUS((ret)))
extern int utl_system_ex(char* strCmd, char* strResult, int nSize);
extern int utl_system(char *cmd, ...);

#define LINUX_THREAD_STACK_SIZE (512*1024)

extern int pthread_create_detached(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg);
extern int pthread_create_normal(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg);
extern int pthread_create_normal_priority(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg);

#endif

