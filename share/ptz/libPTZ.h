#ifndef __LIBPTZ_H__
#define __LIBPTZ_H__
typedef enum PARITYBIT_ENUM
{
	PAR_NONE = 0,
	PAR_EVEN,
	PAR_ODD
}tParityBit_e;
typedef enum
{
	PTZ_DATAFLOW_NONE = 0,
	PTZ_DATAFLOW_HW,
	PTZ_DATAFLOW_SF

}tPtzDataFlow_e;
typedef struct tagPORTPARAMS
{
	int		nBaudRate;      // BAUDRATE_ENUM
	int		nCharSize;      // CHARSIZE_ENUM
	int		nStopBit;       // STOPBIT_ENUM
	tParityBit_e	nParityBit;     // PARITYBIT_ENUM
	tPtzDataFlow_e	nFlowCtl;       // Flow Control type(1/0)
} NC_PORTPARAMS, *PNC_PORTPARAMS;

typedef enum
{
	PTZ_PROTOCOL_HUIXUN = 1,
	PTZ_PROTOCOL_STD_PELCOP = 4,
	PTZ_PROTOCOL_YAAN = 28,
	PTZ_PROTOCOL_STD_PELCOD = 29,	
	PTZ_PROTOCOL_SHUER_PELCOD=30 ,
	PTZ_PROTOCOL_SELF=31 ,			//��άһ����Լ������Լ���ʱ��

	PTZ_PROTOCOL_MAX
}PtzProtocol_e;

typedef struct tagPTZinfo
{
	PtzProtocol_e nProtocol;
	int			  naddress;
	NC_PORTPARAMS nHwParams;
}tPTZhwinfo;

//����˵�� : �򿪴���
//����     : char *cFileName:������
//����ֵ   : �ļ�������,�������-1��ʾ��ʧ��
int DecoderOpenCom(const char *pFileName);

//����˵�� : �رմ���
//����     : int fd:���ڵ��ļ�������
//����ֵ   : 0:�ɹ�; -1:ʧ��
int DecoderCloseCom(int fd);

//����˵�� : ���ò�����
//����     : int fd:���ڵ��ļ�������
//           int nSpeed:������:B115200,B57600,B38400,B19200,B9600,B4800,B2400,B1200,B300
//����ֵ   : 0:�ɹ�; -1:ʧ��
int DecoderSetComBaudrate(int fd, int nSpeed);

//����˵�� : ���ô�������λ
//����     : int fd:���ڵ��ļ�������
//           int nDatabits:����λλ��:5,6,7,8
//           int nStopbits:ֹͣλλ��:1,2
//           int nParity:У��λ��ʽ:PAR_NONE,PAR_ODD,PAR_EVEN
//����ֵ   : 0:�ɹ�; -1:ʧ��
int DecoderSetComBits(int fd, int nDatabits, int nStopbits, int nParity);

//����˵�� : ���ô���
//����     : int fd:���ڵ��ļ�������
//           PNC_PORTPARAMS pParam:��������
//����ֵ   : 0:�ɹ�; -1:ʧ��
int DecoderSetCom(int fd, PNC_PORTPARAMS pParam);


////////////////////////////���ڷ��ͽ�������////////////////////////////
void DecoderSendCommand(int fd, char *pCmd, int nLen, int nBaudRate);
int DecoderReceiveDate(int fd, char *pCmd, int nLen, int nBaudRate);
void DecoderSendCommand_Ex(int fd, unsigned char *pCmd, int nLen, int nBaudRate);


//==========================����============================================================
void DecoderReset(int fd, int nAddress, int nProtocol, int nBaudRate);

//==========================�ϡ��¡����ҡ��Զ�==================================================
void DecoderLeftStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);
void DecoderLeftStop(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderRightStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);
void DecoderRightStop(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderUpStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);
void DecoderUpStop(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderDownStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);
void DecoderDownStop(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAutoStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);
void DecoderAutoStop(int fd, int nAddress, int nProtocol, int nBaudRate);

//�෽λ�ƶ�
//bLeft Ϊ��ʱ���ƣ�Ϊ�������ƣ�leftSpeedΪ0ʱ���ƶ�
//bUp Ϊ�������ƣ�Ϊ��ʱ���ƣ�upSpeedΪ0ʱ���ƶ�
void DecoderPanTiltStart(int fd, int nAddress, int nProtocol, int nBaudRate, int bLeft, int bUp, int leftSpeed, int upSpeed);
void DecoderPanTiltStop(int fd, int nAddress, int nProtocol, int nBaudRate);

//==========================�䱶���佹����Ȧ======================================================
//�䱶
void DecoderZoomInStart(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderZoomInStop(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderZoomOutStart(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderZoomOutStop(int fd, int nAddress, int nProtocol, int nBaudRate);

//�佹
void DecoderFocusNearStart(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderFocusNearStop(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderFocusFarStart(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderFocusFarStop(int fd, int nAddress, int nProtocol, int nBaudRate);

//��Ȧ
void DecoderIrisOpenStart(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderIrisOpenStop(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderIrisCloseStart(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderIrisCloseStop(int fd, int nAddress, int nProtocol, int nBaudRate);

//==========================����==================================================================
void DecoderAUX1On(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAUX1Off(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAUX2On(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAUX2Off(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAUX3On(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAUX3Off(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAUX4On(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAUX4Off(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderAUXNOn(int fd, int nAddress, int nProtocol, int nBaudRate, int n);
void DecoderAUXNOff(int fd, int nAddress, int nProtocol, int nBaudRate, int n);

//==========================��չ==================================================================
void DecoderSetLeftLimitPosition(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderSetRightLimitPosition(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderSetUpLimitPosition(int fd, int nAddress, int nProtocol, int nBaudRate);
void DecoderSetDownLimitPosition(int fd, int nAddress, int nProtocol, int nBaudRate);

void DecoderSetLimitScanSpeed(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan, int nSpeed);

void DecoderLimitScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan);

//������ɨ
void DecoderLimitScanStop(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan);

//��ʼ����ɨ��
void DecoderWaveScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);

//��������ɨ��
void DecoderWaveScanStop(int fd, int nAddress, int nProtocol, int nBaudRate);

//��ʼ��ֱɨ��
void DecoderVertScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);
//��ʼ���ɨ��
void DecoderRandomScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);
//��ʼ֡ɨ��
void DecoderFrameScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed);

//����Ԥ��λ
void DecoderSetPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nNumber);
void DecoderClearPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nNumber);
void DecoderClearAllPreset(int fd, int nAddress, int nProtocol, int nBaudRate);
//����Ԥ��λ
void DecoderLocatePreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nNumber, int nPatrolSpeed);

//��ʼԤ��λѲ��
void DecoderStartPatrol(int fd, int nAddress, int nProtocol, int nBaudRate);
//ֹͣԤ��λѲ��
void DecoderStopPatrol(int fd, int nAddress, int nProtocol, int nBaudRate);
//��ʼ�켣��¼
void DecoderSetScanOnPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan);
//�����켣��¼
void DecoderSetScanOffPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan);
//��ʼ�켣Ѳ��
void DecoderLocateScanPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan);
//ֹͣ�켣Ѳ��
void DecoderStopScanPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan);

//==========================ͬ��DVR��Ѳ���ӿ�========================================
//��ʼ����Ѳ��
void DecoderSetPatrolOn(int fd, int nAddress, int nProtocol, int nBaudRate);
//��������Ѳ��
void DecoderSetPatrolOff(int fd, int nAddress, int nProtocol, int nBaudRate);
//���һ��Ѳ����
void DecoderAddPatrol(int fd, int nAddress, int nProtocol, int nBaudRate, int nPreset, int nSpeed, int nStayTime);
//��ʼѲ��
void DecoderStartHWPatrol(int fd, int nAddress, int nProtocol, int nBaudRate);
//����Ѳ��
void DecoderStopHWPatrol(int fd, int nAddress, int nProtocol, int nBaudRate);
//�����ʸ�ʽת��
int Ptz_nToBaudrate(int nBaud);
//��ĳһ������Ļ�м�Ŵ���ʾ
//x,y  ����Ļ�ֳ�64x64������x,y�ֱ�����Ÿ��Է������ֵ
//zoom �Ŵ�ı�������ֵΪʵ�ʱ���x16
void DecoderZoomPosition(int fd, int nAddress, int nProtocol, int nBaudRate, int x, int y, int zoom);
extern void jv_cam_focus_far();
extern void jv_cam_stop();
extern void jv_cam_zoomin();
extern void jv_cam_zoomout();
extern void jv_cam_focus_near();
extern void jv_cam_get_zoomfocuspos( int *argv);
extern void jv_cam_get_zoompos( int *argv);
extern void jv_cam_get_focuspos( int *argv);
extern void jv_cam_zoomfocus_direct(unsigned int *argv);
extern void jv_cam_zoom_direct(unsigned int *argv);
extern void jv_cam_focus_direct(unsigned int *argv);

//���3D��λ
void DecoderZoomZone(int fd, int nAddress, int nProtocol, int nBaudRate, int x, int y, 
	int w, int h, int width, int height,int zoom);

//�ƶ����ٹ���
void DecoderTraceObj(int fd, int nAddress, int nProtocol, int nBaudRate,int x, int y, 
	int zoom, int focus, int cmd);


#endif

