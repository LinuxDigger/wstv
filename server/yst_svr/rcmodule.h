/**
 *@file rcmodule.h �ֿ����ģ��ͨѶ����չ���͵Ĳ���
 *@author Liu Fengxiang
 */
#ifndef _RCMODULE_H_
#define _RCMODULE_H_

//��չ���ͣ�����ָ���ĸ�ģ��ȥ����,lck20120206
#define RC_EX_FIRMUP		0x01
#define RC_EX_NETWORK		0x02
#define RC_EX_STORAGE		0x03
#define RC_EX_ACCOUNT		0x04
#define RC_EX_PRIVACY		0x05
#define RC_EX_MD		    0x06
#define RC_EX_ALARM		    0x07
#define RC_EX_SENSOR		0x08
#define RC_EX_PTZ			0x09
#define RC_EX_AUDIO			0x0a
#define RC_EX_ALARMIN		0x0b
#define RC_EX_REGISTER 		0x0c
#define RC_EX_EXPOSURE		0x0d
#define RC_EX_QRCODE		0x0e
#define RC_EX_IVP			0x0f
#define RC_EX_DOORALARM		0x10
#define RC_EX_PTZUPDATE		0x11
#define RC_EX_COMTRANS		0x12
#define RC_EX_CHAT			0x13
#define RC_EX_ALARMOUT		0x14
#define RC_EX_NICKNAME		0x15
#define RC_EX_CLOUD			0x16
#define RC_EX_STREAM		0x17
#define RC_EX_PLAY_REMOTE   0x18
#define RC_EX_HEARTBEAT		0x19
#define RC_EX_RESTRICTION	0x20
#define RC_EX_REQ_IDR		0x21

#define RC_EX_DEBUG			0x24


typedef struct tagEXTEND
{
	U32	nType;
	U32	nParam1;
	U32	nParam2;
	U32	nParam3;
	U8	acData[RC_DATA_SIZE-16];
} EXTEND, *PEXTEND;

//
#define   COMM_NO_PERMISSION	0X0405

//------------------��������-------------------------------------
#define EX_ADSL_ON		0x01	//����ADSL��Ϣ
#define EX_ADSL_OFF		0x02	//�Ͽ�ADSL��Ϣ
#define EX_WIFI_AP		0x03	//��ȡAP��Ϣ���������ֻ�ʹ���Լ����ֻ��汾
#define EX_WIFI_ON		0x04	//����wifi
#define EX_WIFI_OFF		0x05	//�Ͽ�wifi
#define EX_NETWORK_OK	0x06	//���óɹ�
#define EX_UPDATE_ETH	0x07	//�޸�eth������Ϣ
#define EX_NW_REFRESH	0x08	//ˢ�µ�ǰ������Ϣ
#define EX_NW_SUBMIT	0x09	//ˢ�µ�ǰ������Ϣ
#define EX_WIFI_CON 	0x0a
#define EX_WIFI_AP_CONFIG 0x0b
#define EX_START_AP		0x0c	//����AP
#define EX_START_STA	0x0d	//����STA

#define EX_WIFI_AP_DLL	0x0e	//DLL���ֻ��ͻ����г�ͻ������һ���������

#define EX_WIFI_SHOW_RSSID 0x0f //��������豸wifi�ź�����ʱ�������������

typedef struct 
{
    char eth_acip[16];
    char eth_acdns[16];
    char eth_acgw[16];
    char eth_acnm[16];

    BOOL dir_link;
    BOOL dhcp;
	BOOL bReboot;
	BOOL bModify;

	U32 nWIFI_mode;
	U32 nAP;
	char	acID[32];
	char	acPW[64];
    U32 wifiAuth;
    U32 wifiEncryp;

	U32	nActived;
}NETWORKINFO;
extern NETWORKINFO  stNetworkInfo;

typedef struct
{
    char wifiId[32];
    char wifiPwd[64];
    int wifiAuth;
    int wifiEncryp;
    U8 wifiIndex;
    U8 wifiChannel;
    U16 wifiRate;
}WIFI_INFO;


U32 build_network_param(char *pData);

//Զ��������Ϣ������
VOID NetworkProc(REMOTECFG *remotecfg);

//�����Ƶ���ò���,lck20120221
U32 build_stream_param(char *pData);


//------------------�ƶ����-------------------------------------
#define EX_MD_REFRESH			0x01
#define EX_MD_SUBMIT			0x02
#define EX_MD_UPDATE			0x03

/**
 *@brief �ѽṹ��ת��Ϊ�ַ���
 *@param pData ����ת�����ַ������ڴ��ַ
 *@return ת�����ַ����ĳ���
 *
 */
U32 build_mdetect_param(int channelid, char *pData);

//Զ�����ô���
VOID MDProc(REMOTECFG *cfg);


//------------------�����ڵ�-------------------------------------
#define EX_COVERRGN_REFRESH		0x01
#define EX_COVERRGN_SUBMIT		0x02
#define EX_COVERRGN_UPDATE		0x03

//��������ڵ�����
U32 build_privacy_param(char *pData);

//Զ�����ô���
VOID PrivacyProc(REMOTECFG *cfg);//xian U8 *pData


//------------------sensor����-----------------------------------
#define EX_SENSOR_REFRESH		0x01
#define EX_SENSOR_SUBMIT		0x02
#define EX_SENSOR_CANCEL		0x03
#define EX_SENSOR_SAVE			0x04
#define EX_SENSOR_GETPARAM	0x05

//���sensor����
U32 build_sensor_param(char *pData);

//Զ�����ô���
VOID SensorProc(REMOTECFG *cfg);

#define EX_PTZ_PRESET_ADD	0x01	//���Ԥ�õ�
#define EX_PTZ_PRESET_DEL	0x02	//ɾ��
#define EX_PTZ_PRESET_CALL	0x03	//����Ԥ�õ�

#define EX_PTZ_PATROL_ADD	0x04
#define EX_PTZ_PATROL_DEL	0x05
#define EX_PTZ_PATROL_START	0x06	//��ʼѲ��
#define EX_PTZ_PATROL_STOP		0x07	//����Ѳ��

#define EX_PTZ_TRAIL_REC_START	0x08
#define EX_PTZ_TRAIL_REC_STOP	0x09
#define EX_PTZ_TRAIL_START		0x0A	//��ʼ�켣
#define EX_PTZ_TRAIL_STOP		0x0B	//ֹͣ�켣

#define EX_PTZ_GUARD_START	0x0C	//��������
#define EX_PTZ_GUARD_STOP		0x0D	//ֹͣ����

#define EX_PTZ_SCAN_LEFT		0x0E	//��ɨ��߽�
#define EX_PTZ_SCAN_RIGHT		0x0F	//��ɨ�ұ߽�
#define EX_PTZ_SCAN_START		0x10	//��ɨ��ʼ
#define EX_PTZ_SCAN_STOP		0x11	//����ɨ��
#define EX_PTZ_WAVE_SCAN_START	0x12	//����ɨ��
#define EX_PTZ_COM_SETUP			0x13	//��̨Э�鲨���ʵȲ�������
#define EX_PTZ_PRESETS_PATROL_START			0x14	//һ���������Ԥ�õ㲢��ʼѲ��
#define EX_PTZ_PRESETS_PATROL_STOP			0x15	//һ������ɾ��Ԥ�õ㲢ֹͣѲ��
#define EX_PTZ_BOOT_ITEM				0x16	//IPC������ִ�еĶ������ޣ�Ѳ������ɨ
#define EX_AF_CALIBRATION				0x17	//һ���autofocusУ��
#define EX_PTZ_PRESET_SET		0x18	//ֱ������Ԥ�Ƶ㣬���Բ������
#define EX_PTZ232_COM_SETUP			0x19	//��оЭ�鲨���ʵȲ�������
#define EX_PTZ_PRESET_CLEAR		0x1A	//ֱ�����Ԥ�õ�
#define EX_PTZ_SCHEDULE_SAVE	0x1B	//���涨ʱ�ƻ�
#define EX_PTZ_PATROL_SETSPEED	0x1C	//����Ѳ���ٶ�
#define EX_PTZ_GUARD_SAVE		0x1D	//����������Ϣ
#define EX_PTZ_ZOOM_ZONE		0x1E	//3D��λ
#define EX_AF_CALIBRATION_INFAUTO				0x20	//һ���autofocus��INFΪ��׼ִ���Զ�У׼
#define EX_AF_CALIBRATION_FILTERSET				0x21	//hisi��AF�˲�����������
#define EX_AF_CALIBRATION_CHECKLENS				0x22			//��ͷ�ϼ�,2m
#define EX_AF_CALIBRATION_CHECKRESULT				0x23		//У׼��ļ���
#define EX_AF_CALIBRATION_CHECKINFRESULT			0x24	 //У׼��INF�ļ���
#define EX_AF_CALIBRATION_SEL_CURVE					0x25	 //ѡ���������

#define EX_PTZ_SCAN_UP				0x2A	//ɨ���ϱ߽�
#define EX_PTZ_SCAN_DOWN			0x2B	//ɨ���±߽�
#define EX_PTZ_VERT_SCAN_START		0x2C	//��ֱɨ��
#define EX_PTZ_RANDOM_SCAN_START	0x2D	//���ɨ��
#define EX_PTZ_FRAME_SCAN_START		0x2E	//֡ɨ��
#define EX_PTZ_GETSPEED			0x2F		//��ȡ��̨�ٶ�
//-----------------Ԥ�õ����-----------------------------------
void PTZProc(REMOTECFG *remotecfg);

//------------------�洢����-------------------------------------
//�洢����ָ��,lck20120306
#define EX_STORAGE_REFRESH	0x01
#define EX_STORAGE_REC		0x02
#define EX_STORAGE_OK		0x04
#define EX_STORAGE_ERR		0x05
#define EX_STORAGE_FORMAT	0x06
#define EX_STORAGE_SWITCH   0x07
#define EX_STORAGE_GETRECMODE 0x08

U32 build_storage_param(char *pData);

//MStorageģ�飬Զ��ָ���,lck20120306
VOID StorageProc(REMOTECFG *remotecfg);


//------------------�˻�����-------------------------------------
/**
 *@brief �ʻ�����ָ��
 *
 */
#define EX_ACCOUNT_OK		0x01
#define EX_ACCOUNT_ERR		0x02
#define EX_ACCOUNT_REFRESH	0x03
#define EX_ACCOUNT_ADD		0x04
#define EX_ACCOUNT_DEL		0x05
#define EX_ACCOUNT_MODIFY	0x06

/**
 *@brief ���û���Ϣ��ָ����ʽ�ŵ��ַ�����
 *@param pData �����洢�û���Ϣ��BUFFER
 *@return �û���Ϣ�ĳ���
 *
 */
U32 build_account_param(char *pData);

/**
 *@brief Զ�����ô���
 *@param pData Ҫ����������
 *@param nSize Ҫ�������ݵĳ���
 */
VOID AccountProc(REMOTECFG *remotecfg);


//------------------�澯����-------------------------------------
#define EX_MAIL_TEST	0x01	//����ADSL��Ϣ
#define EX_ALARM_SUBMIT	0x02
#define EX_SCHEDULE_SUBMIT	0x03
#define EX_ALARM_REFRESH  0X04

U32 build_alarm_param(char *pData);
VOID MailProc(REMOTECFG *remotecfg);

//------------------��������-------------------------------------
#define EX_MALARMIN_SUBMIT			0x01

VOID MAlarmInProc(REMOTECFG *cfg);

//------------------�Ŵű���-------------------------------------
#define EX_MDOORALARM_SUBMIT			0x01

VOID MDoorAlarmProc(REMOTECFG *cfg);

//------------------����ͨע��-----------------------------------
#define EX_REGISTER_SUCCESS 0x01	//ע��ɹ�
#define EX_REGISTER_FAILE	0x02	//ע��ʧ��
#define EX_REGISTER_ONLINE	0x03	//��������ע��
VOID MRegisterProc(REMOTECFG *remotecfg);

//------------------��ά��--------------------------------------
#define MAX_SIZE 3*64 * 64 + 128*1024-16
#define EX_QRCODE_YST			0x04
#define EX_QRCODE_AND			0x05
#define EX_QRCODE_IOS			0x06
#define EX_QRCODE_EX			0x07
VOID QRCodeProc(REMOTECFG *remotecfg);

//-----------------����ͨ����----------------------------------
#define EX_NICKNAME_GET				0x08
#define EX_NICKNAME_SET				0x09
#define EX_NICKNAME_DEL				0x0a
#define EX_NICKNAME_GETOK			0x0b
#define EX_NICKNAME_GETFAIL			0x0c
#define EX_NICKNAME_SETOK			0x0d
#define EX_NICKNAME_SETFAIL			0x0e
#define EX_NICKNAME_DELOK			0x0f
#define EX_NICKNAME_DELFAIL			0x10
#define EX_NICKNAME_GETFAIL_INVALID	0x11
#define EX_NICKNAME_GETFAIL_OFFLINE			0x12
#define EX_NICKNAME_GETFAIL_INPROGRESS			0x13
#define EX_NICKNAME_SETFAIL_INVALID	0x14
#define EX_NICKNAME_SETFAIL_OFFLINE			0x15
#define EX_NICKNAME_SETFAIL_INPROGRESS			0x16
#define EX_NICKNAME_DELFAIL_INVALID			0x17
#define EX_NICKNAME_DELFAIL_OFFLINE			0x18
#define EX_NICKNAME_DELFAIL_INPROGRESS			0x19

VOID YSTNickNameProc(REMOTECFG *remotecfg);
//------------------sensor����Ȥ�����Ż�����������Ȥ������룬�;ֲ��ع�-----------------------------
#define EX_ROI_REFRESH		0x01
#define EX_ROI_SUBMIT		0x02
#define EX_ROI_UPDATE		0x03
#define ROI_MAX_RECT_CNT		4
typedef struct
{
	BOOL bROIReverse;
//	BOOL bROIExposureEN;
//	BOOL bROIQPEN;
	U32  nWeightQP;
	U32  nWeightExposure;
	RECT roiRect[ROI_MAX_RECT_CNT];
}roiInf;

VOID ROIProc(REMOTECFG *cfg);

char *GetKeyValue(const char *pBuffer, const char *key, char *valueBuf, int maxValueBuf);
U32 GetIntValueWithDefault(char *pBuffer, char *pItem, U32 defVal);

//----------------���ܷ���----------------------------------------------------------------------
#define EX_IVP_REFRESH		0x01
#define EX_IVP_SUBMIT		0x02
#define EX_IVP_UPDATE		0x03
#define EX_IVP_COUNT		0x04
#define EX_IVP_COUNT_RESET	0x05
#define EX_IVP_VR_REFRESH	0x06
#define EX_IVP_VR_UPDATE	0x07
#define EX_IVP_VR_SUBMIT	0x08
#define EX_IVP_VR_REF		0x09
#define EX_IVP_VR_TRIGER	0x10

#define EX_IVP_HIDE_SUBMIT	0x11
#define EX_IVP_TL_SUBMIT	0x12
#define EX_IVP_DETECT_SUBMIT	0x13

VOID IVPProc(REMOTECFG *remotecfg);

//----------------�Խ����ֹ���-------------------------------------------------------------------
#define EX_CHAT_REFRESH 0x01
#define EX_CHAT_SUBMIT  0x02

VOID ChatProc(REMOTECFG* cfg);

//----------------����������ֹ���----------------------------------------------------------------
#define EX_ALARMOUT_REFRESH 0x01
#define EX_ALARMOUT_SUBMIT  0X02

VOID AlarmOutProc(REMOTECFG* cfg);

//---------------�ƴ洢���ֹ���----------------------------------------------------------------------
#define EX_CLOUD_REFRESH 0x01
#define EX_CLOUD_SUBMIT  0x02

VOID CloudProc(REMOTECFG* cfg);

//-------------��Ƶ��Ϣ--------------------------------------------------------------------------
#define EX_STREAM_REFRESH 0x01
#define EX_STREAM_SUBMIT  0x02

VOID StreamProc(REMOTECFG *cfg);

//-------------Զ��¼��طţ�ʱ�����ʽʵ�������ӽӿ�-------------------------
#define EX_REPLAY_GETLIST	0x01
#define EX_REPLAY_START   0x02

VOID PlayRemoteProc(REMOTECFG* cfg);

//----------------���Ʋ��ֹ���----------------------------------------------------------------------
#define EX_RESTRICTION_GET	0x01//��ȡ������Ϣ����
#define EX_RESTRICTION_SET	0x02//��������
#define EX_RESTRICTION_GETOK	0x03//δ����
#define EX_RESTRICTION_SETOK	0x04//����ɹ�
#define EX_RESTRICTION_GETFAILED	0x05//��ȡ������Ϣʧ��
#define EX_RESTRICTION_SETFAILED	0x06//����ʧ��
#define EX_RESTRICTION_REP	0x07//�Ѿ�����
#define EX_RESTRICTION_REV	0x08//�ָ�����
#define EX_RESTRICTION_REVOK	0x09//�ָ��ɹ�
#define EX_RESTRICTION_REVFAILED	0x0A//�ָ�ʧ��
VOID RestrictionProc(REMOTECFG *remotecfg);

VOID ReqIDRProc(REMOTECFG *cfg);

//------------------DebugProc----------------------
#define EX_DEBUG_TELNET			0x01
#define EX_DEBUG_TELNET_OFF		0x02
#define EX_DEBUG_REDIRECTPRINT	0x20
#define EX_DEBUG_RECOVERPRINT	0x21
#define EX_DEBUG_GETLOG			0x22
VOID DebugProc(REMOTECFG *remotecfg);

#endif

