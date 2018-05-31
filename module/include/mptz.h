#ifndef __M_PTZ_H__
#define __M_PTZ_H__


#define MAX_PATROL_NOD	256
#define MAX_PRESET_CT	256	//Ԥ�õ�������

#include "libPTZ.h"
#include "jv_common.h"


#define PTZ_SPEED_NOR	255

//��̨�ṹ��
typedef struct _PATROL_NOD
{
    U32 nPreset;      //Ԥ�õ������±�
    U32 nStayTime;    //ͣ��ʱ��
}PATROL_NOD, *PPATROL_NOD;

//��̨�ṹ��
typedef struct _PTZ
{
    U32          nAddr;                      //��ַ
    U32          nProtocol;                  //Э��
    U32          nBaudRate;                  //������,����λ,ֹͣλ��
    NC_PORTPARAMS nHwParams;					//����Ӳ��������Ϣ
    BOOL            bLeftRightSwitch;           //���һ���
    BOOL            bUpDownSwitch;              //���»���
    BOOL            bIrisZoomSwitch;            //��Ȧ�䱶����
    BOOL            bFocusZoomSwitch;           //�佹�䱶����
    BOOL            bZoomSwitch;                //�䱶����
//    BOOL		    bPtzAuto;                   //�Ƿ����Զ�״̬
//    BOOL		    bStartPatrol;               //�Ƿ�����Ѳ��״̬
//    BOOL		    nCurPatrolNod;              //��ǰ��Ѳ����
//    time_t          tmCurPatrolNod;             //��ǰѲ��ʱ��
//    U32			nPresetCt;						//Ԥ�õ����
//    U32			pPreset[MAX_PRESET_CT];			//Ԥ�õ�����
//    U32          nPatrolSize;                //Ѳ��������Ԥ�õ���
//    PATROL_NOD      aPatrol[MAX_PATROL_NOD];    //Ѳ��
	U32		scanSpeed;	//��ɨ�ٶ�
}PTZ, *PPTZ;

//3D��λ�ṹ��
typedef struct _ZONE
{
	int x;		//�ͻ���Ȧ���������� x����
	int y;		//�ͻ���Ȧ���������� y����
	int w;		//�ͻ���Ȧ�������
	int h;		//�ͻ���Ȧ�������
}ZONE_INFO, *PZONE_INFO;

//�ƶ����ٽṹ��
typedef struct _TRACEOBJ
{
	int x;		//x���경��
	int y;		//y���경��
	int zoom;	//zoom����
	int focus;	//focus����
}TRACE_INFO, *PTRACE_INFO;

typedef enum
{
	GUARD_NO,
	GUARD_PRESET,
	GUARD_PATROL,
	GUARD_TRAIL,
	GUARD_SCAN,
}PTZ_GUARD_TYPE_E;

//������Ϣ
typedef struct
{
	U32 guardTime;		//����ʱ�䡣0��ʾ�����ر�
	PTZ_GUARD_TYPE_E guardType;
	union
	{
		U32 nRreset;
		U32 nPatrol;;
		U32 nTrail;
		U32 nScan;
	};
}PTZ_GUARD_T;

//Ԥ�õ���Ϣ
typedef struct
{
    U32			nPresetCt;						//Ԥ�õ����
    U32			pPreset[MAX_PRESET_CT];			//Ԥ�õ�����
    char			name[MAX_PRESET_CT][32];		//Ԥ�õ�����
//    int 		lenzoompos[MAX_PRESET_CT];
//	int 		lenfocuspos[MAX_PRESET_CT];
}PTZ_PRESET_INFO;

//Ѳ����Ϣ
typedef struct
{
    U32         nPatrolSize;                //Ѳ��������Ԥ�õ���
    U32         nPatrolSpeed;               //Ѳ���ٶ�
    PATROL_NOD  aPatrol[MAX_PATROL_NOD];    //Ѳ��
}PTZ_PATROL_INFO;

//��̨��ʱ����
typedef struct
{
	U32 		schedule[2];		//��ʱ����
	struct{
    	char hour;
    	char minute;
    }schTimeStart[2], schTimeEnd[2];	//��ʱ����ʱ���
	BOOL bSchEn[2];						//ʹ�ܶ�ʱ����1/2
}PTZ_SCHEDULE_INFO;

//typedef struct
//{
//	BOOL bGuardonStart;		//�����ػ�
//	int guardTimeCnt;		//�����ʱ
//}PTZ_SCHGUARD_STATUS;

typedef struct
{
	//BOOL sch_runflag[2];
	int sch_LastSch;
	int sch_LastItem;	//��ʱ�����־����������ʼ��ֹͣ�ж�
}PTZ_SCHEDULE_STATUS;

PTZ *PTZ_GetInfo(void);

PTZ_PRESET_INFO *PTZ_GetPreset(void);

PTZ_PATROL_INFO *PTZ_GetPatrol(void);

PTZ_GUARD_T *PTZ_GetGuard(void);

PTZ_SCHEDULE_INFO *PTZ_GetSchedule(void);	//��ȡ��ʱ������Ϣ

S32 PTZ_Init();
S32 PTZ_Release();

/**
 * ����Ԥ�õ�
 * @param nCh	ͨ���ţ�Ĭ�� 0
 * @param nPreset Ԥ�õ��
 */
void inline PtzSetPreset(U32 nCh, S32 nPreset);
S32 PTZ_AddPreset(U32 nCh, U32 nPreset, char *name);
S32 PTZ_DelPreset(U32 nCh, U32 nPreset);
S32 PTZ_StartPatrol(U32 nCh, U32 nPatrol);	//����Ѳ���� 1��2
S32 PTZ_StopPatrol(U32 nCh);
S32 Ptz_Guard_Pause(U32 nCh);

/////////////////////////////////////��̨����///////////////////////////////////
S32 inline PTZ_Auto(U32 nCh, U32 nSpeed);
//��ʼ����
void inline PtzUpStart(U32 nCh, S32 nSpeed);
//ֹͣ����
void inline PtzUpStop(U32 nCh);
//��ʼ����
void inline PtzDownStart(U32 nCh, S32 nSpeed);
//ֹͣ����
void inline PtzDownStop(U32 nCh);
//��ʼ����
void inline PtzLeftStart(U32 nCh, S32 nSpeed);
//ֹͣ����
void inline PtzLeftStop(U32 nCh);
//��ʼ����
void inline PtzRightStart(U32 nCh, S32 nSpeed);
//ֹͣ����
void inline PtzRightStop(U32 nCh);

//�෽λ�ƶ�
//bLeft Ϊ��ʱ���ƣ�Ϊ�������ƣ�leftSpeedΪ0ʱ���ƶ�
//bUp Ϊ�������ƣ�Ϊ��ʱ���ƣ�upSpeedΪ0ʱ���ƶ�
void PtzPanTiltStart(U32 nCh, BOOL bLeft, BOOL bUp, int leftSpeed, int upSpeed);
void PtzPanTiltStop(U32 nCh);


//��ʼ��Ȧ+
void inline PtzIrisOpenStart(U32 nCh);
//ֹͣ��Ȧ+
void inline PtzIrisOpenStop(U32 nCh);
//��ʼ��Ȧ-
void inline PtzIrisCloseStart(U32 nCh);
//ֹͣ��Ȧ-
void inline PtzIrisCloseStop(U32 nCh);
//��ʼ�佹+
void inline PtzFocusNearStart(U32 nCh);
//ֹͣ�佹+
void inline PtzFocusNearStop(U32 nCh);
//��ʼ�佹-
void inline PtzFocusFarStart(U32 nCh);
//ֹͣ�佹-
void inline PtzFocusFarStop(U32 nCh);
//��ʼ�䱶+
void inline PtzZoomOutStart(U32 nCh);
//ֹͣ�䱶+
void inline PtzZoomOutStop(U32 nCh);
//��ʼ�䱶-
void inline PtzZoomInStart(U32 nCh);
//ֹͣ�䱶-
void inline PtzZoomInStop(U32 nCh);
//ת��Ԥ�õ�
void inline PtzLocatePreset(U32 nCh, S32 nPreset);
//�Զ�
void inline PtzAutoStart(U32 nCh ,S32 nSpeed);
//ֹͣ
void inline PtzAutoStop(U32 nCh);

S32 AddPatrolNod(PTZ_PATROL_INFO *pPatrol, U32 nPreset, U32 nStayTime);
S32 DelPatrolNod(PTZ_PATROL_INFO *pPatrol, U32 nPreset);
S32 ModifyPatrolNod(PTZ_PATROL_INFO *pPatrol, U32 nOldPreset, U32 nNewPreset, U32 nNewStayTime);


//��ʼ¼�ƹ켣
S32 PtzTrailStartRec(U32 nCh, U32 nTrail);

//ֹͣ¼�ƹ켣
S32 PtzTrailStopRec(U32 nCh, U32 nTrail);

//��ʼ�켣
S32 PtzTrailStart(U32 nCh, U32 nTrail);

//ֹͣ�켣
S32 PtzTrailStop(U32 nCh, U32 nTrail);

//���û���ֹͣ����
S32 PtzGuardSet(U32 nCh, PTZ_GUARD_T *guard);

//������ɨ��߽�
S32 PtzLimitScanLeft(U32 nCh);

//������ɨ�ұ߽�
S32 PtzLimitScanRight(U32 nCh);

//������ɨ�ٶ�
S32 PtzLimitScanSpeed(U32 nCh, int nScan, int nSpeed);

//��ʼ��ɨ
S32 PtzLimitScanStart(U32 nCh, int nScan);

//ֹͣ��ɨ
S32 PtzLimitScanStop(U32 nCh, int nScan);

//��ʼ����ɨ��
S32 PtzWaveScanStart(U32 nCh, int nSpeed);

//��������ɨ��
S32 PtzWaveScanStop(U32 nCh, int nSpeed);

//������ɨ�ϱ߽�
S32 PtzLimitScanUp(U32 nCh);

//������ɨ�ϱ߽�
S32 PtzLimitScanDown(U32 nCh);

//��ʼ��ֱɨ��
S32 PtzVertScanStart(U32 nCh, int nSpeed);

//��ʼ���ɨ��
S32 PtzRandomScanStart(U32 nCh, int nSpeed);

//��ʼ֡ɨ��
S32 PtzFrameScanStart(U32 nCh, int nSpeed);

//����˵�� : �Ѳ�����ת���ַ���
//����     : HI_U32 nBaudrate:������
//����ֵ   : COMBO������ ; -1:ʧ��
char *Ptz_BaudrateToStr(U32 nBaudrate);

//����˵�� : �ַ���ת���ɲ�����
//����     : HI_U32 nIndex :COMBO������
//����ֵ   : ������ ; -1:ʧ��
U32 Ptz_StrToBaudrate(char *cStr);

//����˵�� : ���ò�����ʱ,�Ѳ�����ת����COMBO��������
//����     : U32 nBaudrate:������
//����ֵ   : COMBO������ ; -1:ʧ��
S32 Ptz_BaudrateToIndex(U32 nBaudrate);

//����˵�� : ���ò�����ʱ,��COMBO��������ת���ɲ�����
//����     : U32 nIndex :COMBO������
//����ֵ   : ������ ; -1:ʧ��
S32 Ptz_IndexToBaudrate(U32 nIndex);

//����˵�� : ����Э��ʱ,��Э��ת����COMBO��������
//����     : U32 nProtocol:libPTZ.a�е�Э����
//����ֵ   : COMBO������ ; -1:ʧ��
S32 Ptz_ProtocolToIndex(U32 nProtocol);

//����˵�� : ����Э��ʱ,��COMBO��������ת����libPTZ.a�е�Э����
//����     : U32 nIndex :COMBO������
//����ֵ   : Э���� ; -1:ʧ��
S32 Ptz_IndexToProtocol(U32 nIndex);

//����N ��ʼ
//n ��n�������
void PtzAuxAutoOn(U32 nCh, int n);

//����N ����
//n ��n�������
void PtzAuxAutoOff(U32 nCh, int n);
//��̨��������
void PtzReSetup(NC_PORTPARAMS param);

//����˵�� : һ����������Ԥ�õ㡢Ѳ������
//����     :  ��
//����ֵ   :  ��
void PTZ_PresetsPatrolStart();

//����˵�� : һ������ɾ��Ԥ�õ㡢�˳�Ѳ������
//����     :   ��
//����ֵ   :  ��
void PTZ_PresetsPatrolStop();

//����˵�� : �趨����������
//����     :nCh ͨ��   item  ������
//����ֵ   : 

void PTZ_SetBootConfigItem(U32 nCh, U32 item);

//����˵�� : ��ȡ����������
//����     :
//����ֵ   : ������

U32 PTZ_GetBootConfigItem();

//����˵�� : ��������������
//����     :
//����ֵ   : 

void PTZ_StartBootConfigItem();

//����˵�� : ������ʱ�ƻ�
//����     :
//����ֵ   : 
void PTZ_StartSchedule();

//����˵�� : ��ȡ��ʱ�����ػ�״̬
//����     :
//����ֵ   : �ػ�״̬
//PTZ_SCHGUARD_STATUS *PTZ_GetGuardStatus(void);

//����˵�� : ��ȡѲ��״̬
//����     :
//����ֵ   : Ѳ��״̬, ����:TRUE/ ֹͣ:FALSE
BOOL PTZ_Get_PatrolStatus(U32 nCh);

//����˵�� : ��ȡ�켣״̬
//����     :
//����ֵ   : �켣״̬, ����:TRUE/ ֹͣ:FALSE
BOOL PTZ_Get_TrailStatus(U32 nCh);

//����˵�� : ��ȡ��̨����״̬
//����     :
//����ֵ   : ��̨����, ����:TRUE/ ֹͣ:FALSE
BOOL PTZ_Get_Status(U32 nCh);

//����˵�� : 3D��λ����
//����     : ͨ���š���λ���򡢷Ŵ�/��С
//����ֵ   : 
void PtzZoomZone(U32 nCh, ZONE_INFO *zone, int width, int height, int zoom);

//����˵�� : �ƶ����ٺ���
//����     : ͨ���š����ٲ�����λ�ø���ָ��
//����ֵ   : 
void PtzTraceObj(U32 nCh, TRACE_INFO *trace, int cmd);

//����˵�� : �趨����ģʽ
//����     : ģʽ:�Զ�/�ֶ���/�ֶ���
//����ֵ   : 
void PTZ_SetIrMod(int mode);
#endif

