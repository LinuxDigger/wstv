#include <termios.h>
#include <pthread.h>
#include <jv_common.h>
#include <utl_filecfg.h>
#include "mptz.h"
#include "libPTZ.h"
#include <utl_cmd.h>
#include <jv_rs485.h>
#include "jv_gpio.h"
#include <utl_timer.h>
#include <SYSFuncs.h>
#include "msensor.h"
#include <mwdt.h>
#include <msoftptz.h>
#include <jv_sensor.h>
#include <mosd.h>
#include <mstream.h>
#include "jv_uartcomm.h"

#define PTZ_CNT	1
#define PATROL_CNT	2

static BOOL g_PtzRunning = FALSE;
static pthread_t 	patrolThreadId;
static void *OnPtzPatrolThread(void *param);

static pthread_mutex_t guardmutex; 
//static PTZ_SCHGUARD_STATUS schGuard_status;	//��ʱ�����ػ�״̬
static PTZ_SCHEDULE_STATUS sch_status;		//��ʱ����״̬
//Ԥ�õ���Ϣ
typedef struct
{
	int presetno;
    int zoom;
	int focus;
	int left;
	int up;
}PTZPresetPos_t;

static struct
{
		PTZ stPtz[PTZ_CNT];//ÿ��ͨ������̨�ṹ
		PTZ_PRESET_INFO stPreInf[PTZ_CNT]; 	//Ԥ�õ���Ϣ
		PTZ_PATROL_INFO stPatrolInf[PTZ_CNT][PATROL_CNT];	//Ѳ����Ϣ
		PTZ_GUARD_T	stGuardInf[PTZ_CNT];

		PTZPresetPos_t presetPos[PTZ_CNT][MAX_PRESET_CT];// �Լ�������ʱ�����ڱ���Ԥ�õ��λ��
		PTZ_SCHEDULE_INFO stScheduleInf[PTZ_CNT];
}s_PTZInfo;

//��̨����״̬
static struct
{
    PTZ				*pPtz;			//��̨�ṹ
    PTZ_PRESET_INFO 	*pPreInf;		//Ԥ�õ���Ϣ
    PTZ_PATROL_INFO 	*pPatrolInf;	//Ѳ����Ϣ
    PTZ_GUARD_T		pGuardInf;

	BOOL		    	bPtzAuto;                   //�Ƿ����Զ�״̬
	BOOL		    	bStartPatrol;               //�Ƿ�����Ѳ��״̬
	S32		    		nCurPatrolNod;              //��ǰ��Ѳ����
	time_t          	tmCurPatrolNod;             //��ǰѲ��ʱ��

	time_t		guardLastTime;			//������ʱʱ��
	U32 		bootItem;				//����������
	S32		    nPatrolPath;          	//��ǰ��Ѳ��
	PTZ_SCHEDULE_INFO *pScheduleInf;
	BOOL		    	bStartTrail;               //�Ƿ��ڹ켣״̬
}g_PtzStatus[PTZ_CNT];

#define PTZ_CTRL_UDLR 	0	//��������ƫ����
#define PTZ_CTRL_AUTO	1	//�Զ�
#define PTZ_CTRL_FOUCS 	2	//�佹
#define PTZ_CTRL_SCAN	3	//ɨ��
#define PTZ_CTRL_GUARD 	4	//����
#define PTZ_CTRL_TRAIL	5	//�켣


static U16 ptz_ctrl = 0; // [0]:�������ң�[1]: �Զ���[2]:�佹��[3]:��ɨ������[4]:
#if 0
#define DEBUG_printf printf
#else
#define DEBUG_printf
#endif

#define PRESETPOS_FILE CONFIG_PATH"presetpos.cfg"

static void __presetpos_save()
{
	int i,ch;
	FILE *fp;
	fp = fopen(PRESETPOS_FILE, "wb");
	if (!fp)
	{
		printf("Failed open %s, because: %s\n", PRESETPOS_FILE, strerror(errno));
		return ;
	}
	ch = 0;
	for (i=0;i<MAX_PRESET_CT;i++)
	{
		if (s_PTZInfo.presetPos[ch][i].presetno > 0)
		{
			fwrite(&s_PTZInfo.presetPos[ch][i], 1, sizeof(PTZPresetPos_t), fp);
		}
	}
	fclose(fp);
}

static void __presetpos_read()
{
	int i,ch = 0;
	FILE *fp;
	fp = fopen(PRESETPOS_FILE, "rb");
	if (!fp)
	{
		printf("Failed open %s, because: %s\n", PRESETPOS_FILE, strerror(errno));
		return ;
	}
	PTZPresetPos_t pos;
	int len;
	while(1)
	{
		len = fread(&pos, 1, sizeof(pos), fp);
		if (len != sizeof(pos))
			break;

		if (pos.presetno > 0 && pos.presetno < MAX_PRESET_CT)
		{
			s_PTZInfo.presetPos[ch][pos.presetno] = pos;
		}
		else
			break;
	}
	fclose(fp);
}

PTZ *PTZ_GetInfo(void)
{
	return s_PTZInfo.stPtz;
}

PTZ_PRESET_INFO *PTZ_GetPreset(void)
{
	return s_PTZInfo.stPreInf;
}

PTZ_PATROL_INFO *PTZ_GetPatrol(void)
{
	return (s_PTZInfo.stPatrolInf[0]);
}

PTZ_GUARD_T *PTZ_GetGuard(void)
{
	return s_PTZInfo.stGuardInf;
}

PTZ_SCHEDULE_INFO *PTZ_GetSchedule(void)
{
	return s_PTZInfo.stScheduleInf;
}

//PTZ_SCHGUARD_STATUS *PTZ_GetGuardStatus(void)
//{
//	return &schGuard_status;
//}

/**/
void PtzReSetup(NC_PORTPARAMS param)
{

	 jv_rs485_lock();
	 DecoderSetCom(jv_rs485_get_fd(), &param);
	  //�ͷŻ�����
	 jv_rs485_unlock();

}
/*���ú���ģʽ*/
void PTZ_SetIrMod(int mode)
{
	if(mode>MSENSOR_DAYNIGHT_ALWAYS_NIGHT || mode<MSENSOR_DAYNIGHT_AUTO)
	{
		return;
	}
	if(jv_rs485_get_fd() > 0)
	{
		printf("[%s]:%d mode:%d\n", __FUNCTION__, __LINE__,mode);
		switch(mode)
		{
			case MSENSOR_DAYNIGHT_AUTO:
			{
				PtzLocatePreset(0, 91);
				usleep(200000);
				PtzLocatePreset(0, 3);	
				break;
			}
			case MSENSOR_DAYNIGHT_ALWAYS_DAY:
			{
				PtzLocatePreset(0, 91);
				usleep(200000);
				PtzLocatePreset(0, 1);	
				break;
			}
			case MSENSOR_DAYNIGHT_ALWAYS_NIGHT:
			{
				PtzLocatePreset(0, 91);
				usleep(200000);
				PtzLocatePreset(0, 2);	
				break;
			}
			default:
				printf("Not supporter mode\n");
				break;
		}
	}
}
/*
 * ��̨���Ƴ�ʼ��
 */
S32 PTZ_Init()
{
	S32 i;
	int ret;
	//static NC_PORTPARAMS	s_serial_param = { 2400, 8,  1, PAR_NONE, PTZ_DATAFLOW_NONE };
	PTZ *mpPtz = PTZ_GetInfo();
	jv_rs485_init();
	ret = DecoderSetCom(jv_rs485_get_fd(), &mpPtz->nHwParams);
	for (i = 0; i < PTZ_CNT; ++i)
	{
		g_PtzStatus[i].pPtz = &s_PTZInfo.stPtz[i];
		g_PtzStatus[i].pPreInf = &s_PTZInfo.stPreInf[i];
		g_PtzStatus[i].pPatrolInf = s_PTZInfo.stPatrolInf[i];
		//g_PtzStatus[i].pGuardInf = &s_PTZInfo.stGuardInf[i];
		memcpy(&g_PtzStatus[i].pGuardInf, &s_PTZInfo.stGuardInf[i], sizeof(PTZ_GUARD_T));
		g_PtzStatus[i].bPtzAuto = FALSE;
		g_PtzStatus[i].bStartPatrol = FALSE;
		g_PtzStatus[i].nCurPatrolNod = -1;
		g_PtzStatus[i].tmCurPatrolNod = 0;
		g_PtzStatus[i].nPatrolPath = -1;
		g_PtzStatus[i].pScheduleInf = &s_PTZInfo.stScheduleInf[i];
		g_PtzStatus[i].bStartTrail = FALSE;
	}
	

	g_PtzRunning = TRUE;
	pthread_create(&patrolThreadId, NULL, OnPtzPatrolThread, NULL);
	__presetpos_read();
	msoftptz_init();

    return JVERR_NO;
}

S32 PTZ_Release()
{
	jv_rs485_deinit();

	if (TRUE == g_PtzRunning)
	{
		g_PtzRunning = FALSE;
		pthread_join(patrolThreadId, NULL);
	}
	msoftptz_deinit();

	return JVERR_NO;
}

/**
 * ����Ԥ�õ�
 * @param nCh	ͨ���ţ�Ĭ�� 0
 * @param nPreset Ԥ�õ��
 */
void inline PtzSetPreset(U32 nCh, S32 nPreset)
{
    //ȡ�û�����
     jv_rs485_lock();
     PPTZ pPtz = g_PtzStatus[nCh].pPtz;
     DecoderSetPreset(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nPreset);
     //�ͷŻ�����
     jv_rs485_unlock();
}
/**
 * ����Ԥ�õ�
 * @param nCh	ͨ���ţ�Ĭ�� 0
 * @param nPreset Ԥ�õ��
 */
void inline PtzClearPreset(U32 nCh, S32 nPreset)
{
    //ȡ�û�����
     jv_rs485_lock();
     PPTZ pPtz = g_PtzStatus[nCh].pPtz;
     DecoderClearPreset(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nPreset);
     //�ͷŻ�����
     jv_rs485_unlock();
}

/*
 * ���Ԥ�õ�
 * @param nCh: ͨ��
 * @param nPreset: Ԥ�õ�
 * @return: 0 �ɹ�
 * 			-1 Ԥ�õ�Ƿ�
 * 			-2 Ԥ�õ��ظ�
 * 			-3 Ԥ�õ�������������
 */
S32 PTZ_AddPreset(U32 nCh, U32 nPreset, char *name)
{
	U32 i;
	int argv[2];
	if (0 >= nPreset || 255 < nPreset)
	{
		return -1;
	}

	if (MAX_PRESET_CT <= g_PtzStatus[nCh].pPreInf->nPresetCt)
		return -3;

	//���Ԥ�õ��Ƿ��ظ�
	for (i = 0; i < g_PtzStatus[nCh].pPreInf->nPresetCt; ++i)
	{
		if (nPreset == g_PtzStatus[nCh].pPreInf->pPreset[i])
		{
			return -2;
		}
	}

	//���Ԥ�õ㣬���Ԥ�õ�CTEXT������Ԥ�õ��б�
	++g_PtzStatus[nCh].pPreInf->nPresetCt;
	g_PtzStatus[nCh].pPreInf->pPreset[i] = nPreset;
	strncpy(g_PtzStatus[nCh].pPreInf->name[i], name, sizeof(g_PtzStatus[nCh].pPreInf->name[0]));
	s_PTZInfo.presetPos[nCh][nPreset].presetno = -1;
	if( g_PtzStatus[nCh].pPtz->nProtocol== PTZ_PROTOCOL_SELF)
	{
		s_PTZInfo.presetPos[nCh][nPreset].presetno = nPreset;
		s_PTZInfo.presetPos[nCh][nPreset].focus = argv[1];
		s_PTZInfo.presetPos[nCh][nPreset].zoom = argv[0];
		printf("%s: Get lenpos %x, %x,i=%d,presetnum=%d\n",__func__,argv[0],argv[1],i,g_PtzStatus[nCh].pPreInf->pPreset[i]);
	}
	if (msoftptz_b_support(nCh))
	{
		JVPTZ_Pos_t pos;
		msoftptz_pos_get(nCh, &pos);
		s_PTZInfo.presetPos[nCh][nPreset].presetno = nPreset;
		s_PTZInfo.presetPos[nCh][nPreset].left = pos.left;
		s_PTZInfo.presetPos[nCh][nPreset].up = pos.up;
	}
	//��Ҫ�����ⶫ��
	if (s_PTZInfo.presetPos[nCh][nPreset].presetno)
	{
		__presetpos_save();
	}
    PtzSetPreset(nCh, nPreset);

	return JVERR_NO;
}

S32 PTZ_AddPreset_From_Dooralarm(U32 nCh, char *name)
{
	int nPreset = g_PtzStatus[nCh].pPreInf->nPresetCt+1;
	if(0 == PTZ_AddPreset(nCh,nPreset,name))
	{
		return nPreset;
	}
	else
		return -1;
}


/*
 * ɾ��Ԥ�õ�
 * @param nCh: ͨ��
 * @param nPreset: Ԥ�õ�
 * @return: 0 �ɹ�
 * 			-1 ɾ����Ԥ�õ㲻����
 */
S32 PTZ_DelPreset(U32 nCh, U32 nPreset)
{
	S32 i, times, nIndex = -1;
	//ɾ��Ԥ�õ��Ӱ�쵽Ѳ��������ֹͣѲ��
	PTZ_StopPatrol(nCh);

	//���ɾ����Ԥ�õ��Ƿ����
	for (i = 0; i < g_PtzStatus[nCh].pPreInf->nPresetCt; ++i)
	{
		if (nPreset == g_PtzStatus[nCh].pPreInf->pPreset[i])
		{
			nIndex = i;
			break;
		}
	}

	if (0 > nIndex)
	{
		return -1;
	}

	//ɾ���ĵ��������ǰ��
	times = g_PtzStatus[nCh].pPreInf->nPresetCt - 1;
	for (i = nIndex; i < times; ++i)
	{
		g_PtzStatus[nCh].pPreInf->pPreset[i] = g_PtzStatus[nCh].pPreInf->pPreset[i + 1];
		strcpy(g_PtzStatus[nCh].pPreInf->name[i], g_PtzStatus[nCh].pPreInf->name[i + 1]);
	}
	g_PtzStatus[nCh].pPreInf->pPreset[i] = 0;
	--g_PtzStatus[nCh].pPreInf->nPresetCt;

	//����Ѳ�����飬��������Ҫɾ����Ԥ�õ�
	int nPatrol;
	PATROL_NOD  aPatrol[MAX_PATROL_NOD];
	U32         nPatrolSize;
	for(nPatrol=0; nPatrol<PATROL_CNT; nPatrol++)
	{
		nPatrolSize = 0;
		memset(aPatrol, 0, sizeof(aPatrol));
		for (i = 0; i < g_PtzStatus[nCh].pPatrolInf[nPatrol].nPatrolSize; ++i)
		{
			if (nPreset != g_PtzStatus[nCh].pPatrolInf[nPatrol].aPatrol[i].nPreset)
			{
				aPatrol[nPatrolSize].nPreset = g_PtzStatus[nCh].pPatrolInf[nPatrol].aPatrol[i].nPreset;
				aPatrol[nPatrolSize].nStayTime = g_PtzStatus[nCh].pPatrolInf[nPatrol].aPatrol[i].nStayTime;
				++nPatrolSize;
			}
		}
		g_PtzStatus[nCh].pPatrolInf[nPatrol].nPatrolSize = nPatrolSize;
		memcpy(g_PtzStatus[nCh].pPatrolInf[nPatrol].aPatrol, aPatrol, sizeof(aPatrol));
	}
	PtzClearPreset(nCh, nPreset);

	return JVERR_NO;
}

static void _Ptz_patrolinfo_save(int chan,int patrol_path)
{
	FILE *fp;
	fp = fopen(PATROL_FLAG_FLAG, "wb");
	if (!fp)
	{
		printf("In softptz position save function,Failed open %s, because: %s\n", PATROL_FLAG_FLAG, strerror(errno));
		return ;
	}

	PATROL_INFO painfo;
	painfo.chan = chan;
	painfo.patrol_path = patrol_path;

	fwrite(&painfo, 1, sizeof(PATROL_INFO), fp);

	fclose(fp);
}


/*
 * ��ʼѲ��
 * @param nCh: ͨ��	nPatrol��Ѳ����
 */
S32 PTZ_StartPatrol(U32 nCh, U32 nPatrol)
{
	if(g_PtzStatus[nCh].pPatrolInf[nPatrol].nPatrolSize >= 1)
	{
		g_PtzStatus[nCh].bStartPatrol = TRUE;
		g_PtzStatus[nCh].nCurPatrolNod = 0;
		g_PtzStatus[nCh].tmCurPatrolNod = time(NULL);
		g_PtzStatus[nCh].nPatrolPath = nPatrol;
		PtzLocatePreset(nCh, g_PtzStatus[nCh].pPatrolInf[nPatrol].aPatrol[0].nPreset);
		_Ptz_patrolinfo_save(nCh,nPatrol);
	}
	return JVERR_NO;
}

/*
 * ֹͣѲ��
 * @param nCh: ͨ��
 */
S32 PTZ_StopPatrol(U32 nCh)
{
#if 0
    if(ptzHandle >= 0)
	{
		mchnosd_region_destroy(ptzHandle);
		printf(">>>4    Patrol stop,mchnosd_region_destroy!\n");
		ptzHandle = -1;
        PresetId = -1;
	}
#endif
	g_PtzStatus[nCh].bStartPatrol = FALSE;
	g_PtzStatus[nCh].nPatrolPath = -1;;
	if(0 == access(PATROL_FLAG_FLAG, F_OK))
		utl_system("rm "PATROL_FLAG_FLAG);
	return JVERR_NO;
}
//��ͣ����
S32 Ptz_Guard_Pause(U32 nCh)
{
	g_PtzStatus[nCh].guardLastTime = 0;
	return JVERR_NO;
}

S32 inline PTZ_Auto(U32 nCh, U32 nSpeed)
{
	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (TRUE == g_PtzStatus[nCh].bPtzAuto)
    {
    	g_PtzStatus[nCh].bPtzAuto = FALSE;

        //ȡ�û�����
         jv_rs485_lock();

         DecoderAutoStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);

         //�ͷŻ�����
         jv_rs485_unlock();
    }
    else
    {
    	g_PtzStatus[nCh].bPtzAuto = TRUE;
        //ȡ�û�����
        jv_rs485_lock();

        DecoderAutoStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);

        //�ͷŻ�����
        jv_rs485_unlock();
    }
    return JVERR_NO;
}

//ֹͣѲ����
static void __Ptz_StopPatrol(U32 nCh)
{
	PTZ_StopPatrol(nCh);
}

//��ͣ����
static void __Ptz_Guard_Pause(U32 nCh)
{
	g_PtzStatus[nCh].guardLastTime = 0;
}

//��������
static void __Ptz_Guard_Reset(U32 nCh)
{
	g_PtzStatus[nCh].guardLastTime = time(NULL);
}

/*
 * ��ȡ��̨����״̬�������ڿ�����Ϊ 1������Ϊ 0
 * @param nCh: ͨ��
 */
BOOL PTZ_Get_Status(U32 nCh)
{
	BOOL ptzctrl = FALSE;
	BOOL zoom = FALSE;
	ptzctrl = g_PtzStatus[nCh].bStartPatrol||g_PtzStatus[nCh].bStartTrail||zoom||(ptz_ctrl>0)
		||(g_PtzStatus[nCh].guardLastTime>0&&(g_PtzStatus[nCh].pGuardInf.guardType==GUARD_PRESET));
		
	Printf("pat%d  zoom%d  other%d  guard%d\n",g_PtzStatus[nCh].bStartPatrol,zoom,ptz_ctrl,
		((g_PtzStatus[nCh].guardLastTime>0)&&(g_PtzStatus[nCh].pGuardInf.guardType==GUARD_PRESET)));
	return	ptzctrl;
}

/*
 * ��ȡѲ��״̬
 * @param nCh: ͨ��
 */
BOOL PTZ_Get_PatrolStatus(U32 nCh)
{
	return	g_PtzStatus[nCh].bStartPatrol;
}
/*
 * ��ȡ�켣״̬
 * @param nCh: ͨ��
 */
BOOL PTZ_Get_TrailStatus(U32 nCh)
{
	return	g_PtzStatus[nCh].bStartTrail;
}
/*
 * Ѳ���̺߳���
 */
static void *OnPtzPatrolThread(void *param)
{
	U32 i;
	U32 nChnCnt = 1;
	int firstdone=0;//��̨���ƶ���Ϊֹͣ�ı�־
	time_t	now;
	U32 *nCurNod;
	S32 nPatrol;
	PATROL_NOD * pPatrol;

	pthreadinfo_add((char *)__func__);

	Printf("Thread Func: %s pid: %d\n", __func__, utl_thread_getpid());
	for (i = 0; i < nChnCnt; ++i)
		g_PtzStatus[i].guardLastTime = time(NULL);
	while (TRUE == g_PtzRunning)
	{
		now = time(NULL);
		for (i = 0; i < nChnCnt; ++i)
		{
			nPatrol = g_PtzStatus[i].nPatrolPath;
			//û����Ѳ����
			if (FALSE == g_PtzStatus[i].bStartPatrol || g_PtzStatus[i].pPatrolInf[nPatrol].nPatrolSize == 0)
			{
				//����
				if (g_PtzStatus[i].pGuardInf.guardTime > 0 && g_PtzStatus[i].guardLastTime != 0)
				{
					if (g_PtzStatus[i].guardLastTime + g_PtzStatus[i].pGuardInf.guardTime < now)
					{
						switch(g_PtzStatus[i].pGuardInf.guardType)
						{
						default:
							break;
						case GUARD_PRESET:
							PtzLocatePreset(i, g_PtzStatus[i].pGuardInf.nRreset);
							break;
						case GUARD_PATROL:
							PTZ_StartPatrol(i, 0);
							break;
						case GUARD_TRAIL:
							PtzTrailStart(i, g_PtzStatus[i].pGuardInf.nTrail);
							break;
						case GUARD_SCAN:
							break;
						}
						g_PtzStatus[i].guardLastTime = 0;
					}
				}

				continue;
			}
			
			//��ǰѲ��Ԥ�õ㵽ʱ�䣬ת����һ��
			nCurNod = (U32 *)&g_PtzStatus[i].nCurPatrolNod;
			pPatrol = g_PtzStatus[i].pPatrolInf[nPatrol].aPatrol;
			//if(jv_ptz_move_done(0))
			{
				if(firstdone==0)//��̨�ƶ�ֹͣ����ʼ��ʱ������ͣ��ʱ��
				{
					g_PtzStatus[i].tmCurPatrolNod = now;
					firstdone=1;
				}
				if (now - g_PtzStatus[i].tmCurPatrolNod > pPatrol[*nCurNod].nStayTime)
				{
					//g_PtzStatus[i].tmCurPatrolNod = now;
					++g_PtzStatus[i].nCurPatrolNod;
					firstdone=0;
					if (g_PtzStatus[i].nCurPatrolNod >= g_PtzStatus[i].pPatrolInf[nPatrol].nPatrolSize)
					{
						g_PtzStatus[i].nCurPatrolNod = 0;
					}
					Printf("patrol ch%d= 	Patrol:%d	go preset%d		nPatrolSize:%d\n", i,nPatrol, pPatrol[*nCurNod].nPreset,
						g_PtzStatus[i].pPatrolInf[nPatrol].nPatrolSize);
					PtzLocatePreset(i, pPatrol[*nCurNod].nPreset);
					usleep(50000);	//����˯��һ�²����ٵ����¸�Ԥ�õ�
				}
			}
		}
		//˯һ�ᣬ��Ȼcpuһֱ�Ӹ߲���
		usleep(200000);
	}
	printf("out of OnPtzPatrolThread\n");
	return NULL;
}
BOOL __ismirror()
{
	msensor_attr_t snAttr;
	msensor_getparam(&snAttr);
	if((snAttr.effect_flag>>EFFECT_MIRROR)&0x01)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL __isturn()
{
	msensor_attr_t snAttr;
	msensor_getparam(&snAttr);
	if((snAttr.effect_flag>>EFFECT_TURN)&0x01)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//��ʼ����
void inline PtzUpStart(U32 nCh, S32 nSpeed)
{
	if (msoftptz_b_support(nCh))
	{
		if(__isturn())
		{
			msoftptz_move_start(nCh, 0, -nSpeed, 0);
		}else
		{
			msoftptz_move_start(nCh, 0, nSpeed, 0);
		}
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		if(__isturn())
		{
			jv_gpio_write(0, 6, 1);
		}
		else
		{
			jv_gpio_write(0, 7, 1);
		}
		
		return;
	}
	
    //ȡ�û�����
    jv_rs485_lock();
    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bUpDownSwitch)
    {
        Printf("PTZ Control:DecoderUpStart,fd: %d nAddr: %d nProtocol: %d nBaudRate: %d nSpeed: %d\n",jv_rs485_get_fd(),pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
        DecoderUpStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
    }
    else
    {
        DecoderDownStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
    }
    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_StopPatrol(nCh);
	__Ptz_Guard_Pause(nCh);
}
//ֹͣ����
void inline PtzUpStop(U32 nCh)
{
	if (msoftptz_b_support(nCh))
	{
		msoftptz_move_stop(nCh);
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		jv_gpio_write(0, 7, 0);
		jv_gpio_write(0, 6, 0);
		jv_gpio_write(2, 2, 0);
		jv_gpio_write(2, 4, 0);
		return;
	}

    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bUpDownSwitch)
    {
        DecoderUpStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderDownStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}
//��ʼ����
void inline PtzDownStart(U32 nCh, S32 nSpeed)
{
	if (msoftptz_b_support(nCh))
	{
		//����ķ�תֻ�������˶�ʱ������
		if(__isturn())
		{
			msoftptz_move_start(nCh, 0, nSpeed, 0);
		}else
		{
			msoftptz_move_start(nCh, 0, -nSpeed, 0);
		}
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		if (__isturn())
		{
			jv_gpio_write(0, 7, 1);
		}
		else
		{
			jv_gpio_write(0, 6, 1);
		}
		
		return;
	}

    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bUpDownSwitch)
    {
        DecoderDownStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
    }
    else
    {
        DecoderUpStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
    }

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
}
//ֹͣ����
void inline PtzDownStop(U32 nCh)
{
	if (msoftptz_b_support(nCh))
	{
		msoftptz_move_stop(nCh);
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		//��������
		jv_gpio_write(0, 7, 0);
		jv_gpio_write(0, 6, 0);
		jv_gpio_write(2, 2, 0);
		jv_gpio_write(2, 4, 0);
		return;
	}
	
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bUpDownSwitch)
    {
        DecoderDownStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderUpStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}
//��ʼ����
void inline PtzLeftStart(U32 nCh, S32 nSpeed)
{
	if (msoftptz_b_support(nCh))
	{
		//����ľ���ֻ�������˶�ʱ������
		if(__ismirror())
		{
			msoftptz_move_start(nCh, -nSpeed,0 , 0);
		}else
		{
			msoftptz_move_start(nCh, nSpeed,0 , 0);
		}
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		if (__ismirror())
		{
			jv_gpio_write(2, 4, 1);
		}
		else
		{
			jv_gpio_write(2, 2, 1);
		}
		
		return;
	}

    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bLeftRightSwitch)
    {
        DecoderLeftStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
    }
    else
    {
        DecoderRightStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
    }

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
}
//ֹͣ����
void inline PtzLeftStop(U32 nCh)
{
	if (msoftptz_b_support(nCh))
	{
		msoftptz_move_stop(nCh);
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		//��������
		jv_gpio_write(0, 7, 0);
		jv_gpio_write(0, 6, 0);
		jv_gpio_write(2, 2, 0);
		jv_gpio_write(2, 4, 0);
		return;
	}
	
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bLeftRightSwitch)
    {
        DecoderLeftStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderRightStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}
//��ʼ����
void inline PtzRightStart(U32 nCh, S32 nSpeed)
{
	if (msoftptz_b_support(nCh))
	{
		//����ľ���ֻ�������˶�ʱ������
		if(__ismirror())
		{
			msoftptz_move_start(nCh, nSpeed,0 , 0);
		}else
		{
			msoftptz_move_start(nCh, -nSpeed,0 , 0);
		}
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		if (__ismirror())
		{
			jv_gpio_write(2, 2, 1);
		}
		else
		{
			jv_gpio_write(2, 4, 1);
		}
		
		return;
	}

    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bLeftRightSwitch)
    {
        DecoderRightStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
    }
    else
    {
        DecoderLeftStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);
    }

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
}
//ֹͣ����
void inline PtzRightStop(U32 nCh)
{
	if (msoftptz_b_support(nCh))
	{
		msoftptz_move_stop(nCh);
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		//��������
		jv_gpio_write(0, 7, 0);
		jv_gpio_write(0, 6, 0);
		jv_gpio_write(2, 2, 0);
		jv_gpio_write(2, 4, 0);
		return;
	}

    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bLeftRightSwitch)
    {
        DecoderRightStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderLeftStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}


//�෽λ�ƶ�
//bLeft Ϊ��ʱ���ƣ�Ϊ�������ƣ�leftSpeedΪ0ʱ���ƶ�
//bUp Ϊ�������ƣ�Ϊ��ʱ���ƣ�upSpeedΪ0ʱ���ƶ�
void PtzPanTiltStart(U32 nCh, BOOL bLeft, BOOL bUp, int leftSpeed, int upSpeed)
{
	if (msoftptz_b_support(nCh))
	{
		if (!bLeft)
			leftSpeed = -leftSpeed;
		if (!bUp)
			upSpeed = -upSpeed;
		msoftptz_move_start(nCh, leftSpeed, upSpeed, 0);
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		if (leftSpeed != 0)
		{
			if (bLeft)
			{
				jv_gpio_write(2, 2, 1);
			}
			else
			{
				jv_gpio_write(2, 4, 1);
			}
		}
		if (upSpeed != 0)
		{
			if (bUp)
			{
				jv_gpio_write(0, 7, 1);
			}
			else
			{
				jv_gpio_write(0, 6, 1);
			}
		}
			
		return;
	}
	
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (pPtz->bLeftRightSwitch)
    {
    	bLeft = !bLeft;
    }
	DecoderPanTiltStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, bLeft, bUp, leftSpeed, upSpeed);

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}

void PtzPanTiltStop(U32 nCh)
{
	if (msoftptz_b_support(nCh))
	{
		msoftptz_move_stop(nCh);
		return ;
	}

	if (!strcmp(hwinfo.devName, "YL"))
	{
		//��������
		jv_gpio_write(0, 7, 0);
		jv_gpio_write(0, 6, 0);
		jv_gpio_write(2, 2, 0);
		jv_gpio_write(2, 4, 0);
		return;
	}
	
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    DecoderPanTiltStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}


//��ʼ��Ȧ+
void inline PtzIrisOpenStart(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bIrisZoomSwitch)
    {
        DecoderIrisOpenStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomInStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomOutStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
#if 1
    jv_iris_adjust(0, TRUE);
#endif
}
//ֹͣ��Ȧ+
void inline PtzIrisOpenStop(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bIrisZoomSwitch)
    {
        DecoderIrisOpenStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomInStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomOutStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
}
//��ʼ��Ȧ-
void inline PtzIrisCloseStart(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bIrisZoomSwitch)
    {
        DecoderIrisCloseStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomOutStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomInStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
 #if 1
    jv_iris_adjust(0, FALSE);
#endif
}
//ֹͣ��Ȧ-
void inline PtzIrisCloseStop(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bIrisZoomSwitch)
    {
        DecoderIrisCloseStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomOutStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomInStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
}
//��ʼ�佹+
void inline PtzFocusNearStart(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bFocusZoomSwitch)
    {
        DecoderFocusNearStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomInStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomOutStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
}
//ֹͣ�佹+
void inline PtzFocusNearStop(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bFocusZoomSwitch)
    {
        DecoderFocusNearStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomInStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomOutStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}
//��ʼ�佹-
void inline PtzFocusFarStart(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bFocusZoomSwitch)
    {
        DecoderFocusFarStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomOutStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomInStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
}
//ֹͣ�佹-
void inline PtzFocusFarStop(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (0 == pPtz->bFocusZoomSwitch)
    {
        DecoderFocusFarStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomOutStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomInStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}
//��ʼ�䱶-
void inline PtzZoomOutStart(U32 nCh)
{	
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (1 == pPtz->bIrisZoomSwitch)
    {
        DecoderIrisOpenStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (1 == pPtz->bFocusZoomSwitch)
    {
        DecoderFocusNearStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomOutStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomInStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
}
//ֹͣ�䱶-
void inline PtzZoomOutStop(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (1 == pPtz->bIrisZoomSwitch)
    {
        DecoderIrisOpenStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (1 == pPtz->bFocusZoomSwitch)
    {
        DecoderFocusNearStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomOutStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomInStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}
//��ʼ�䱶+
void inline PtzZoomInStart(U32 nCh)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (1 == pPtz->bIrisZoomSwitch)
    {
        DecoderIrisCloseStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (1 == pPtz->bFocusZoomSwitch)
    {
        DecoderFocusFarStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomInStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomOutStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
}
//ֹͣ�䱶+
void inline PtzZoomInStop(U32 nCh)
{	
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    if (1 == pPtz->bIrisZoomSwitch)
    {
        DecoderIrisCloseStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (1 == pPtz->bFocusZoomSwitch)
    {
        DecoderFocusFarStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else if (0 == pPtz->bZoomSwitch)
    {
        DecoderZoomInStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }
    else
    {
        DecoderZoomOutStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);
    }

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}

//����N ��ʼ
//n ��n�������
void PtzAuxAutoOn(U32 nCh, int n)
{
	//ȡ�û�����
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderAUXNOn(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, n);

	//�ͷŻ�����
	jv_rs485_unlock();
}

//����N ����
//n ��n�������
void PtzAuxAutoOff(U32 nCh, int n)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
        DecoderAUXNOff(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, n);

    //�ͷŻ�����
    jv_rs485_unlock();
}

//ת��Ԥ�õ�
void inline PtzLocatePreset(U32 nCh, S32 nPreset)
{
    //ȡ�û�����
	int argv[2];
	int i;
	msensor_attr_t snAttr;
    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    //ȡ�û�����
    jv_rs485_lock();
    if((nPreset>0) && (nPreset< MAX_PRESET_CT))
    {
    	if ( pPtz->nProtocol==PTZ_PROTOCOL_SELF)
    	{
			argv[0] = s_PTZInfo.presetPos[nCh][nPreset].zoom;
			argv[1] = s_PTZInfo.presetPos[nCh][nPreset].focus;
    	}
    	if (msoftptz_b_support(nCh))
    	{
    		JVPTZ_Pos_t pos;
    		pos.left = s_PTZInfo.presetPos[nCh][nPreset].left;
    		pos.up = s_PTZInfo.presetPos[nCh][nPreset].up;
    		msoftptz_goto(nCh, &pos);
    	}
    }
    if(nPreset==300)
    {
    	msensor_getparam(&snAttr);
    	if((snAttr.effect_flag>>EFFECT_MIRROR)&0x01)
		{
    		snAttr.effect_flag&=(~(1<<EFFECT_MIRROR));
		}
    	else
    	{
      		snAttr.effect_flag|=(1<<EFFECT_MIRROR);
    	}
		msensor_setparam(&snAttr);
		 msensor_flush(0);
    }
    else  if(nPreset==301)
    {
    	msensor_getparam(&snAttr);
		if((snAttr.effect_flag>>EFFECT_TURN)&0x01)
		{
			snAttr.effect_flag&=(~(1<<EFFECT_TURN));
		}
		else
		{
			snAttr.effect_flag|=(1<<EFFECT_TURN);
		}
		 msensor_setparam(&snAttr);
		 msensor_flush(0);
    }
    else  if(nPreset==302)
    {
    	 CloseWatchDog();
    }
    else  if(nPreset==303)
    {
    	OpenWatchDog();
    }
	if(g_PtzStatus[nCh].bStartPatrol == TRUE)
	{
		PTZ_PATROL_INFO *patrol = PTZ_GetPatrol();
		DecoderLocatePreset(jv_rs485_get_fd(),
                pPtz->nAddr,
                pPtz->nProtocol,
                pPtz->nBaudRate,
                nPreset,
                (patrol[g_PtzStatus[nCh].nPatrolPath].nPatrolSpeed >> 2));
	}
	else
	{
		DecoderLocatePreset(jv_rs485_get_fd(),
                        pPtz->nAddr,
                        pPtz->nProtocol,
                        pPtz->nBaudRate,
                        nPreset,
                        0);
	}

    Printf("goto preset %d\n", nPreset);

    //�ͷŻ�����
    jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);

}

//�Ŵű���ת��Ԥ�õ�
void PtzAlarm_gotoPreset(U32 nCh,S32 nPreset)
{
	msoftptz_setalarmPreSpeed_flag();
	PtzLocatePreset(nCh,nPreset);
}

//�Զ�
void inline PtzAutoStart(U32 nCh ,S32 nSpeed)
{
	if (msoftptz_b_support(nCh) && msoftptz_auto_support(nCh))
	{
		msoftptz_move_auto(nCh, nSpeed);
		return ;
	}

    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    DecoderAutoStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);

    //�ͷŻ�����
    jv_rs485_unlock();

	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
}

//ֹͣ
void inline PtzAutoStop(U32 nCh)
{
	if (msoftptz_b_support(nCh) && msoftptz_auto_support(nCh))
	{
		msoftptz_move_stop(nCh);
		return ;
	}
	
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
    DecoderAutoStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);

    //�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
}

/////////////////////////////////////Ѳ�����������////////////////////////////////////

//����˵�� : ��Ԥ�õ���ӵ�Ѳ��������
//����     : PPTZ pPtz:Ҫ��ӵ�����̨�ṹ
//           U32 nPreset:Ԥ�õ�
//����ֵ   : 0:�ɹ�; -1:���������; -2:��Ԥ�õ��Ѿ�����(��������ظ�Ԥ�õ㣬ȥ��)
S32 AddPatrolNod(PTZ_PATROL_INFO *pPatrol, U32 nPreset, U32 nStayTime)
{
    if(pPatrol->nPatrolSize >= MAX_PATROL_NOD)
    {
        return -1;
    }

	pPatrol->aPatrol[pPatrol->nPatrolSize].nPreset = nPreset;
    if(nStayTime < 5)
        nStayTime = 5;
	pPatrol->aPatrol[pPatrol->nPatrolSize].nStayTime = nStayTime;
	pPatrol->nPatrolSize++;

    return 0;
}

//����˵�� : ��Ԥ�õ��Ѳ��������ɾ��
//����     : PPTZ pPtz:�ڸ���̨�ṹ�в���Ԥ�õ�ɾ��
//           U32 nIndex:ɾ����Ԥ�õ���Ѳ�������е��±�
//����ֵ   : 0:�ɹ�; -1:ʧ��
S32 DelPatrolNod(PTZ_PATROL_INFO *pPatrol, U32 nIndex)
{
    U32 i;

    if (0 >= pPatrol->nPatrolSize ||nIndex >= pPatrol->nPatrolSize)
    {
    	Printf("size(%d) or index(%d) error\n", pPatrol->nPatrolSize, nIndex);
    	return -1;
    }

    pPatrol->nPatrolSize--;
	//�����Ԥ�õ�ǰ��1
	for (i = nIndex; i<pPatrol->nPatrolSize; i++)
	{
		memcpy(&(pPatrol->aPatrol[i]), &(pPatrol->aPatrol[i+1]), sizeof(PATROL_NOD));
	}

	memset(&(pPatrol->aPatrol[i]), 0 , sizeof(PATROL_NOD));

    return 0;
}

//����˵�� : ��Ԥ�õ���ӵ�Ѳ��������
//����     : PPTZ pPtz:Ҫ�޸ĵ���̨�ṹ
//           U32 nOldPreset:Ҫ�޸ĵ�Ԥ�õ�
//			U32 nNewPreset:�޸ĳɵ�ֵ
//			U32 nNewStayTime:ͣ��ʱ��
//����ֵ   : 0:�ɹ�; -1:Ԥ�õ㲻���� -2��Ԥ�õ��ظ�
S32 ModifyPatrolNod(PTZ_PATROL_INFO *pPatrol, U32 nOldPreset, U32 nNewPreset, U32 nNewStayTime)
{
    U32 i;
    S32 index = -1;

    //�޸�ֵ�ʹ��޸�ֵһ�£�ֱ�Ӳ����޸�
    if (nOldPreset == nNewPreset)
    {
        for (i=0; i<pPatrol->nPatrolSize; i++)
        {
            if (pPatrol->aPatrol[i].nPreset == nOldPreset)
            {
            	pPatrol->aPatrol[i].nPreset = nNewPreset;
            	pPatrol->aPatrol[i].nStayTime = nNewStayTime;
        		return 0;
            }
        }
        return -1;
    }
    //�ڵ�ǰ���������޸ĵĵ��Ƿ��Ѿ�����
	else
	{
		for (i = 0; i < pPatrol->nPatrolSize; i++)
		{
			if (pPatrol->aPatrol[i].nPreset == nNewPreset)
			{
				return -2;
			}
			else if (pPatrol->aPatrol[i].nPreset == nOldPreset)
			{
				index = i;
			}
		}
	    //�޸�
		if (0 <= index)
		{
			pPatrol->aPatrol[i].nPreset = nNewPreset;
			pPatrol->aPatrol[i].nStayTime = nNewStayTime;
		    return 0;
		}
	    return -1;
	}
	return 0;
}

//��ʼ¼�ƹ켣
S32 PtzTrailStartRec(U32 nCh, U32 nTrail)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderSetScanOnPreset(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nTrail);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
	return 0;
}

//ֹͣ¼�ƹ켣
S32 PtzTrailStopRec(U32 nCh, U32 nTrail)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderSetScanOffPreset(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nTrail);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
	return 0;
}

//��ʼ�켣
S32 PtzTrailStart(U32 nCh, U32 nTrail)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderLocateScanPreset(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nTrail);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
	return 0;
}

//ֹͣ�켣
S32 PtzTrailStop(U32 nCh, U32 nTrail)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderStopScanPreset(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nTrail);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
	return 0;
}

//���û���ֹͣ����
S32 PtzGuardSet(U32 nCh, PTZ_GUARD_T *guard)
{
	__Ptz_StopPatrol(nCh);
	memcpy(&g_PtzStatus[nCh].pGuardInf, guard, sizeof(PTZ_GUARD_T));
	DEBUG_printf("PtzGuardSet: time=%d, preset:%d, type:%d\n",g_PtzStatus[nCh].pGuardInf.guardTime,
		g_PtzStatus[nCh].pGuardInf.nRreset, g_PtzStatus[nCh].pGuardInf.guardType);
	__Ptz_Guard_Reset(nCh);
	return 0;
}

//������ɨ��߽�
S32 PtzLimitScanLeft(U32 nCh)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderSetLeftLimitPosition(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);

	//�ͷŻ�����
	jv_rs485_unlock();
	
	__Ptz_StopPatrol(nCh);
	return 0;
}

//������ɨ�ұ߽�
S32 PtzLimitScanRight(U32 nCh)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderSetRightLimitPosition(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);

	//�ͷŻ�����
	jv_rs485_unlock();
	
	__Ptz_StopPatrol(nCh);
	return 0;
}

//������ɨ�ٶ�
S32 PtzLimitScanSpeed(U32 nCh, int nScan, int nSpeed)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	pPtz->scanSpeed = nSpeed;
	DecoderSetLimitScanSpeed(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nScan, nSpeed);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
	return 0;
}

//��ʼ��ɨ
S32 PtzLimitScanStart(U32 nCh, int nScan)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderLimitScanStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nScan);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
	return 0;
}

//������ɨ�ϱ߽�
S32 PtzLimitScanUp(U32 nCh)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderSetUpLimitPosition(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);

	//�ͷŻ�����
	jv_rs485_unlock();
	
	__Ptz_StopPatrol(nCh);
	return 0;
}

//������ɨ�±߽�
S32 PtzLimitScanDown(U32 nCh)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderSetDownLimitPosition(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);

	//�ͷŻ�����
	jv_rs485_unlock();
	
	__Ptz_StopPatrol(nCh);
	return 0;
}

//��ʼ��ֱɨ��
S32 PtzVertScanStart(U32 nCh, int nScan)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderVertScanStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nScan);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
	return 0;
}

//ֹͣ��ɨ
S32 PtzLimitScanStop(U32 nCh, int nScan)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderLimitScanStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nScan);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
	return 0;
}

//��ʼ���ɨ��
S32 PtzRandomScanStart(U32 nCh, int nSpeed)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	pPtz->scanSpeed = nSpeed;
	DecoderRandomScanStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
	return 0;
}

//��ʼ֡ɨ��
S32 PtzFrameScanStart(U32 nCh, int nSpeed)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	pPtz->scanSpeed = nSpeed;
	DecoderFrameScanStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
	return 0;
}

//��ʼ����ɨ��
S32 PtzWaveScanStart(U32 nCh, int nSpeed)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	pPtz->scanSpeed = nSpeed;
	DecoderWaveScanStart(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, nSpeed);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Pause(nCh);
	__Ptz_StopPatrol(nCh);
	return 0;
}

//��������ɨ��
S32 PtzWaveScanStop(U32 nCh, int nSpeed)
{
	jv_rs485_lock();

	PPTZ pPtz = g_PtzStatus[nCh].pPtz;
	DecoderWaveScanStop(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate);

	//�ͷŻ�����
	jv_rs485_unlock();
	__Ptz_Guard_Reset(nCh);
	return 0;
}

//����˵�� : �Ѳ�����ת���ַ���
//����     : HI_U32 nBaudrate:������
//����ֵ   : COMBO������ ; -1:ʧ��
char *Ptz_BaudrateToStr(U32 nBaudrate)
{
    switch(nBaudrate)
    {
    case B1200:
        return "1200";

    case B4800:
        return "4800";

    case B9600:
        return "9600";

    case B2400:
        return "2400";
	case B19200 :
        return "19200";
	case B38400:
	     return "38400";
	case B57600:
	     return "57600";
	case B115200:
	     return "115200";
     default:
        return "2400";
    }
}

//����˵�� : �ַ���ת���ɲ�����
//����     : HI_U32 nIndex :COMBO������
//����ֵ   : ������ ; -1:ʧ��
U32 Ptz_StrToBaudrate(char *cStr)
{
    int nBaud = atoi(cStr);
    switch(nBaud)
    {
    case 1200:
        return B1200;

    case 2400:
        return B2400;

    case 4800:
        return B4800;

    case 9600:
        return B9600;
 	case 19200 :
        return B19200;
	case 38400:
	     return B38400;
	case 57600:
	     return B57600;
	case 115200:
	     return B115200;

    default:
        return B2400;
    }
}

//����˵�� : ���ò�����ʱ,�Ѳ�����ת����COMBO��������
//����     : U32 nBaudrate:������
//����ֵ   : COMBO������ ; -1:ʧ��
S32 Ptz_BaudrateToIndex(U32 nBaudrate)
{
    switch(nBaudrate)
    {
    case B1200:
        return 0;

    case B2400:
        return 1;

    case B4800:
        return 2;

    case B9600:
        return 3;
    default:
        return 0;   //zwq20100531
    }
}

//����˵�� : ���ò�����ʱ,��COMBO��������ת���ɲ�����
//����     : U32 nIndex :COMBO������
//����ֵ   : ������ ; -1:ʧ��
S32 Ptz_IndexToBaudrate(U32 nIndex)
{
    switch(nIndex)
    {
    case 0:
        return B1200;

    case 1:
        return B2400;

    case 2:
        return B4800;

    case 3:
        return B9600;

    default:
        return -1;
    }
}

//����˵�� : ����Э��ʱ,��Э��ת����COMBO��������
//����     : U32 nProtocol:libPTZ.a�е�Э����
//����ֵ   : COMBO������ ; -1:ʧ��
S32 Ptz_ProtocolToIndex(U32 nProtocol)
{
    switch(nProtocol)
    {
    case 1: //PELCO-D
        return 0;

    case 4: //PELCO-P
        return 1;

    case 7: //PELCO-D ��չ
        return 2;

    case 5: //PELCO-P ��չ
        return 3;

    case 13: //PELCOD-3
        return 4;

    default:
        return 0;
    }
}

//����˵�� : ����Э��ʱ,��COMBO��������ת����libPTZ.a�е�Э����
//����     : U32 nIndex :COMBO������
//����ֵ   : Э���� ; -1:ʧ��
S32 Ptz_IndexToProtocol(U32 nIndex)
{
    switch(nIndex)
    {
    case 0: //PELCO-D
        return 1;

    case 1: //PELCO-P
        return 4;

    case 2: //PELCO-D ��չ
        return 7;

    case 3: //PELCO-P ��չ
        return 5;

    case 4: //PELCOD-3
        return 13;

    default:
        return -1;
    }
}

//��ĳһ������Ļ�м�Ŵ���ʾ
//x,y  ����Ļ�ֳ�64x64������x,y�ֱ�����Ÿ��Է������ֵ
//zoom �Ŵ�ı�������ֵΪʵ�ʱ���x16 - 16�����䱶Ϊ0. ����Ŵ�5����5 x 16 - 16
void PtzZoomPosition(U32 nCh, int x, int y, int zoom)
{
    //ȡ�û�����
    jv_rs485_lock();

    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
        DecoderZoomPosition(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, x, y, zoom);

    //�ͷŻ�����
    jv_rs485_unlock();
}

//�ͻ���Ȧ�����򣬽���3D��λ�����Ŵ���С��ʾ
//(x, y, w, h) Ȧ�������������꼰���; (width, height)��ǰ�����ֱ���
//zoom 3D��λָ��: 0xC0 �Ŵ�0xC1 ��С��
void PtzZoomZone(U32 nCh, ZONE_INFO *zone, int width, int height, int zoom)
{
	if(__isturn())
	{
		zone->y = 0 - zone->y;
	}
	if(__ismirror())
	{
		zone->x = 0 - zone->x;
	}

	if (msoftptz_b_support(nCh))
	{

		msoftptz_ZoomZone(zone->x, zone->y, zone->w, zone->h, width, height, zoom);
		return;
	}
	
	if(!hwinfo.bHomeIPC)
	{
		//ȡ�û�����
		jv_rs485_lock();
		PPTZ pPtz = g_PtzStatus[nCh].pPtz;
		DecoderZoomZone(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, 
			zone->x, zone->y, zone->w, zone->h, width, height, zoom);

		//�ͷŻ�����
		jv_rs485_unlock();
	}
}

//�ƶ�����ָ��
//(x, y, zoom, focus) �����������̨�ƶ����경�����������ľ۽�����
//cmd �ƶ�����ָ��: 0x20 ����0�㣬0x21 ����λ���ƶ�������0x22 ����ǰλ�ò���
void PtzTraceObj(U32 nCh, TRACE_INFO *trace, int cmd)
{
    //ȡ�û�����
    jv_rs485_lock();
	if(__isturn())
	{
		trace->y = 0 - trace->y;
	}
	if(__ismirror())
	{
		trace->x = 0 - trace->x;
	}
    PPTZ pPtz = g_PtzStatus[nCh].pPtz;
        DecoderTraceObj(jv_rs485_get_fd(), pPtz->nAddr, pPtz->nProtocol, pPtz->nBaudRate, 
			trace->x, trace->y, trace->zoom, trace->focus, cmd);

    //�ͷŻ�����
    jv_rs485_unlock();
}

//lyf 2014.02.20

//ע��:��Щ����������N85-HD������;
//��Ϊ�����ͺŵĸ�������Ҫ�����޸ĺ궨�塣

#define STAY_TIME			5	//������ԱҪ�����30s
#define LEFT_RIGHT_SPEED	100
#define UP_DOWN_SPEED		255
#define PRESET_COUNT		6

static int presets_patrol_timer = -1;
static int stop_flag = 0;	//������ֹ�������á�һ��Ѳ����־λ


//����˵�� :utl_timer_create �ص�����
//����     : 
//����ֵ   : 

static BOOL _addpresets_startpatrol(int id, void* param)
{
	static int nCh = 0;
	static int nPreset = 1;
	static int in = 0;
	static int inSum = 0;
	static int upflag = 1;
	static PTZ_PATROL_INFO *infoPatrol;
	int ret = 0;
	JVPTZ_Pos_t pos;
	infoPatrol = PTZ_GetPatrol();
	
	//PRESET_COUNT��Ԥ�õ�û�������Ͱ�����ֹͣ��ťʱstop_flag = 1
	if(stop_flag == 1)
	{
		utl_timer_destroy(presets_patrol_timer);
		//���ñ���
		in = 0;
		inSum = 0;
		nPreset = 1;
		upflag = 1;
		stop_flag = 0;
		return TRUE;
	}
	
	in++;
	inSum++;
	
	//��6��Ԥ�õ�
	if(inSum > PRESET_COUNT * 4)
	{
		PTZ_StartPatrol(nCh, 0);
		utl_timer_destroy(presets_patrol_timer);
		//���ñ���
		in = 0;
		inSum = 0;
		nPreset = 1;
		upflag = 1;	
		return TRUE;
	}
		
	if(in == 1)
	{		
		if(inSum==1)
		{
		
		PtzPanTiltStart(nCh, 1, 1, LEFT_RIGHT_SPEED, UP_DOWN_SPEED);
		pos.left = 32767;
    	pos.up = 65535;
		
		msoftptz_goto(nCh, &pos);
		}
		else if(inSum==5)
		{
		pos.left = 65535;
    	pos.up = 65535;
		msoftptz_goto(nCh, &pos);
		}	
		else if(inSum==9)
		{
		pos.left = 32767;
    	pos.up = 65535;
		msoftptz_goto(nCh, &pos);
		}
		else if(inSum==13)
		{
		pos.left = 0;
    	pos.up = 65535;
		msoftptz_goto(nCh, &pos);
		}
		else if(inSum==17)
		{
		pos.left = 32767;
    	pos.up = 65535;
		msoftptz_goto(nCh, &pos);
		}
		else if(inSum==21)
		{
		pos.left =32767;
    	pos.up = 0;
		msoftptz_goto(nCh, &pos);
		}
		
	}
	else if(in == 3)
	{
		PtzPanTiltStop(nCh);
		if(upflag == 0)
		{
			PtzZoomInStart(nCh);
		}
		else
		{
			PtzZoomOutStart(nCh);
		}
	}
	else if(in == 4)
	{
		if(upflag == 0)
		{
			PtzZoomInStop(nCh);
		}
		else
		{
			PtzZoomOutStop(nCh);
		}
		upflag = !upflag;
		
		ret = PTZ_AddPreset(nCh, nPreset, "Preset");
		WriteConfigInfo();
		if (ret != 0)
		{
			printf("PTZ_AddPreset Failed: %d\n", ret);
			return FALSE;
		}
		
		ret = AddPatrolNod(&infoPatrol[0], nPreset, STAY_TIME);
		if(ret != 0)
		{
			printf("AddPatrolNod Failed: %d\n", ret);
			return FALSE;
		}
		WriteConfigInfo();
		nPreset++;
		
	}

	if(in == 4)
	{	
		in = 0;
	}
	utl_timer_reset(presets_patrol_timer, 6000, _addpresets_startpatrol, NULL);

	return TRUE;
	
}

static BOOL __addpresets_startpatrol_af(int id, void* param)
{
	static int nCh = 0;
	static int nPreset = 1;
	static int in = 0;
	static int inSum = 0;
	static int upflag = 1;
	static PTZ_PATROL_INFO *infoPatrol;
	int ret = 0;
	JVPTZ_Pos_t pos;
	infoPatrol = PTZ_GetPatrol();
	
	//PRESET_COUNT��Ԥ�õ�û�������Ͱ�����ֹͣ��ťʱstop_flag = 1
	if(stop_flag == 1)
	{
		utl_timer_destroy(presets_patrol_timer);
		//���ñ���
		in = 0;
		inSum = 0;
		nPreset = 1;
		upflag = 1;
		stop_flag = 0;
		return TRUE;
	}
	
	in++;
	inSum++;
	
	//��6��Ԥ�õ�
	if(inSum > PRESET_COUNT * 4)
	{
		PTZ_StartPatrol(nCh, 0);
		utl_timer_destroy(presets_patrol_timer);
		//���ñ���
		in = 0;
		inSum = 0;
		nPreset = 1;
		upflag = 1;	
		return TRUE;
	}

	if(in == 1)
	{		
		if(inSum==1)
		{
			PtzPanTiltStart(nCh, 0, 1, 0, 150);
		}
		else if(inSum==5)
		{
			PtzPanTiltStart(nCh, 1, 0, 150, 0);
		}	
		else if(inSum==9)
		{
			PtzPanTiltStart(nCh, 1, 0, 0, 150);
		}
		else if(inSum==13)
		{
			PtzPanTiltStart(nCh, 0, 1, 150, 0);
		}
		else if(inSum==17)
		{
			PtzPanTiltStart(nCh, 0, 1, 150, 150);
		}
		else if(inSum==21)
		{
			PtzPanTiltStart(nCh, 1, 0, 150, 150);
		}
		
	}
	else if(in == 3)
	{
		PtzPanTiltStop(nCh);
	}
	else if(in == 4)
	{
		upflag = !upflag;
		char name[16] = {0};
		sprintf(name, "Preset-%d", nPreset);
		ret = PTZ_AddPreset(nCh, nPreset, name);
		if (ret != 0)
		{
			printf("PTZ_AddPreset Failed: %d\n", ret);
			return FALSE;
		}
		WriteConfigInfo();
		
		ret = AddPatrolNod(&infoPatrol[0], nPreset, 12);
		if(ret != 0)
		{
			printf("AddPatrolNod Failed: %d\n", ret);
			return FALSE;
		}
		WriteConfigInfo();
		nPreset++;
		
	}

	if(in == 4)
	{	
		in = 0;
	}
	utl_timer_reset(presets_patrol_timer, 4000, __addpresets_startpatrol_af, NULL);

	return TRUE;
	
}

//����˵�� : һ����������Ԥ�õ㡢Ѳ������
//����     :
//����ֵ   : 

void PTZ_PresetsPatrolStart()
{
	int i = 0;
	int ret = 0;
	int nCh = 0;
	
	//��������Ԥ�õ㡢Ѳ��ǰ�������Ѳ����Ԥ�õ�
	
	//ɾ��Ѳ���е�Ԥ�õ�
	PTZ_PATROL_INFO *infoPatrol = &g_PtzStatus[nCh].pPatrolInf[0];

	for(i = infoPatrol->nPatrolSize - 1; i >=0; i--)
	{
		DelPatrolNod(infoPatrol, i);
	}

	//ɾ��Ԥ�õ�
	PTZ_PRESET_INFO *infoPreset = g_PtzStatus[nCh].pPreInf;

	for(i = infoPreset->nPresetCt - 1; i >= 0 ; i--)
	{
		ret = PTZ_DelPreset(nCh, infoPreset->pPreset[i]);
		if(ret != 0)
		{
			printf("delete preset error occured!\n");
			return ;		
		}
		WriteConfigInfo();			
	}
	{
		if (presets_patrol_timer == -1)
		{
			presets_patrol_timer = utl_timer_create("pp_timer", 2000, _addpresets_startpatrol, NULL);
		}
		else
		{
			utl_timer_reset(presets_patrol_timer, 1000, _addpresets_startpatrol, NULL);
		}
	}
		
}

//����˵�� : һ������ɾ��Ԥ�õ㡢�˳�Ѳ������
//����     :
//����ֵ   : 

void PTZ_PresetsPatrolStop()
{
	int i = 0;
	int ret = 0;
	int nCh = 0;
	
	stop_flag = 1;
	
	PTZ_StopPatrol(nCh);
	//PtzPanTiltStart(nCh, 1, 1, 3, 0);
	//ɾ��Ѳ���е�Ԥ�õ�
	PTZ_PATROL_INFO *infoPatrol = &g_PtzStatus[nCh].pPatrolInf[0];
	for(i = infoPatrol->nPatrolSize - 1; i >=0; i--)
	{
		DelPatrolNod(&infoPatrol[nCh], i);
		WriteConfigInfo();
	}
	
	//ɾ��Ԥ�õ�
	PTZ_PRESET_INFO *infoPreset = g_PtzStatus[nCh].pPreInf;
	for(i = infoPreset->nPresetCt - 1; i >= 0 ; i--)
	{
		ret = PTZ_DelPreset(nCh, infoPreset->pPreset[i]);
		if(ret != 0)
		{
			printf("PTZ_AddPreset Failed: %d\n", ret);
			return;
		}	
		WriteConfigInfo();
	}

	sleep(2);
}

//����˵�� : �趨����������
//����     :
//����ֵ   : 


void PTZ_SetBootConfigItem(U32 nCh, U32 item)
{
	g_PtzStatus[nCh].bootItem = item;
}

//����˵�� : ��ȡ����������
//����     :
//����ֵ   : 

U32 PTZ_GetBootConfigItem()
{
	int nCh = 0;
	return g_PtzStatus[nCh].bootItem;
}
/**
 * @brief   PTZ��������������,ɨ��켣Ҫ�ӳٵ���̨�Լ�֮��
 * @param pArgs
 * @return
 */
void PTZ_BootThrd(VOID *pArgs)
{
	pthreadinfo_add((char *)__func__);

	int nCh = 0;
	U32 item = PTZ_GetBootConfigItem();

	switch(item)
	{
		case 1:
			PTZ_StartPatrol(nCh, 0);
			break;
		case 2:
			usleep(30*1000*1000);
			PtzLimitScanStart(nCh, 0);
			break;
		case 3:
			usleep(30*1000*1000);
			PtzTrailStart(nCh, g_PtzStatus[0].pGuardInf.nTrail);
			break;
		case 4:
			PtzGuardSet(nCh, &g_PtzStatus[0].pGuardInf);
			break;
		case 5:
			usleep(30*1000*1000);
			PtzWaveScanStart(nCh, g_PtzStatus[0].pPtz->scanSpeed);
			break;
		default:
			break;
	}
}

//����˵�� : ��������������
//����     :
//����ֵ   : 

void PTZ_StartBootConfigItem()
{
	pthread_t		pidptz;
	pthread_attr_t	attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);	//����Ϊ�����߳�
	pthread_create(&pidptz, &attr, (void*)PTZ_BootThrd, NULL);
	pthread_attr_destroy (&attr);

}


void _start_Schedule(PTZ_SCHEDULE_INFO *schedule, int nSch)
{
	int nCh = 0;
	DEBUG_printf("Sch[%d]	", nSch);
	
	if((sch_status.sch_LastItem!=schedule->schedule[nSch]) || (sch_status.sch_LastSch!=nSch))
	{
		DEBUG_printf(":%d	start\n", schedule->schedule[nSch]);
		switch(schedule->schedule[nSch])
		{
			case 0:
			case 1:
				PTZ_StartPatrol(nCh, (schedule->schedule[nSch]));
				break;
			case 2:
			{
				PtzLimitScanSpeed(nCh, 0, 55);
				PtzLimitScanStart(nCh, 0);
			}
				break;
			case 3:
			case 4:
			case 5:
			case 6:
				PtzTrailStart(nCh, (schedule->schedule[nSch]-3));
				break;
			case 7:
			{
				PTZ_GUARD_T *guardinfo = PTZ_GetGuard();
				PTZ_GUARD_T guard;
				guard.guardTime = guardinfo->guardTime;
				guard.nRreset = guardinfo->nRreset;
				guard.guardType = GUARD_PRESET;
				DEBUG_printf(" time:%d	type:%d	 preset:%d\n",guard.guardTime,guard.guardType, guard.nRreset);
				PtzGuardSet(nCh, &guard);
			}
				break;
			default:
				break;
		}
		sch_status.sch_LastSch = nSch;
		sch_status.sch_LastItem = schedule->schedule[nSch];
		//sch_status.sch_runflag[nSch] = TRUE;
		//sch_status.sch_runflag[1-nSch] = FALSE;
	}
	else
		DEBUG_printf(":%d	runing\n", schedule->schedule[nSch]);	
}
void _stop_Schedule(void)
{
	int nCh = 0;
	PTZ_GUARD_T guard;
	if(sch_status.sch_LastItem < 0)
	{
		DEBUG_printf("idle\n");
	}
	else if(sch_status.sch_LastItem < 2)
	{
		DEBUG_printf("stop patrol\n");
		PTZ_StopPatrol(nCh);
	}
	else if(sch_status.sch_LastItem < 3)
	{
		DEBUG_printf("stop scan\n");
		PtzLimitScanStop(nCh, 0);
	}
	else if(sch_status.sch_LastItem < 7)
	{
		DEBUG_printf("stop trail\n");
		PtzTrailStop(nCh, (sch_status.sch_LastItem-3));
	}
	else if(sch_status.sch_LastItem < 8)
	{
		DEBUG_printf("stop guard\n");
		guard.guardTime = 0;
		guard.guardType = GUARD_NO;
		PtzGuardSet(nCh, &guard);
	}
	else
		printf("unknown plan\n");
	sch_status.sch_LastItem = -1;
	sch_status.sch_LastSch = -1;
	//sch_status.sch_runflag[0] = FALSE;
	//sch_status.sch_runflag[1] = FALSE;
}
void PTZ_ScheduleThrd(VOID *pArgs)
{	
	sleep(10);
	int nCh = 0;
	static BOOL running = TRUE;
	PTZ_SCHEDULE_INFO oldSch;
	PTZ_GUARD_T guard;
	pthreadinfo_add((char *)__func__);


	while(running)
	{
		if(memcmp(PTZ_GetSchedule(), &oldSch, sizeof(PTZ_SCHEDULE_INFO)) != 0)
		{
			printf("##Schange\n");
			memcpy(&oldSch, PTZ_GetSchedule(), sizeof(PTZ_SCHEDULE_INFO));
		}
		else
		{
			DEBUG_printf("Nochange:%d	",sch_status.sch_LastItem);
		}
		if((oldSch.bSchEn[0]==FALSE) && (oldSch.bSchEn[1]==FALSE))
		{
			DEBUG_printf("no plan	");
			_stop_Schedule();
			usleep(500*1000);
			continue;
		}
		time_t tt;
		struct tm tm;
		tt = time(NULL);
		localtime_r(&tt, &tm);
		int now = tm.tm_hour*60 + tm.tm_min;
		int start1 = oldSch.schTimeStart[0].hour*60 + oldSch.schTimeStart[0].minute;
		int end1 = oldSch.schTimeEnd[0].hour*60 + oldSch.schTimeEnd[0].minute;
		int start2 = oldSch.schTimeStart[1].hour*60 + oldSch.schTimeStart[1].minute;
		int end2 = oldSch.schTimeEnd[1].hour*60 + oldSch.schTimeEnd[1].minute;
		DEBUG_printf("now[%02d:%02d]   Plan1:[%02d:%02d-%02d:%02d]  Plan2:[%02d:%02d-%02d:%02d]	", tm.tm_hour, tm.tm_min,oldSch.schTimeStart[0].hour,
			oldSch.schTimeStart[0].minute, oldSch.schTimeEnd[0].hour,oldSch.schTimeEnd[0].minute,oldSch.schTimeStart[1].hour,oldSch.schTimeStart[1].minute, 
			oldSch.schTimeEnd[1].hour,oldSch.schTimeEnd[1].minute);
		if(oldSch.bSchEn[0] == TRUE)
		{
			if(oldSch.bSchEn[1] == TRUE)
			{
				DEBUG_printf("Two:");
				if(start1 < end1)
				{
					if(start2 < end2)
					{
						if((now>=start1) && (now<end1))
						{
							DEBUG_printf("1	Day	");	
							_start_Schedule(&oldSch, 0);
						}
						else if((now>=start2) && (now<end2))
						{
							DEBUG_printf("2	Day	");
							_start_Schedule(&oldSch, 1);
						}
						else
						{
							DEBUG_printf("OUT	");
							_stop_Schedule();
						}
					}
					else
					{
						if((now>=start1) && (now<end1))
						{
							DEBUG_printf("1	Day	");	
							_start_Schedule(&oldSch, 0);
						}
						else if((now>=start2) || (now<end2))
						{
							DEBUG_printf("2	Net	");	
							_start_Schedule(&oldSch, 1);
						}
						else
						{
							DEBUG_printf("OUT	");
							_stop_Schedule();
						}
					}
				}
				else
				{
					if((now>=start1) || (now<end1))
					{
						DEBUG_printf("1	Net	");	
						_start_Schedule(&oldSch, 0);
					}
					else if((now>=start2) && (now<end2))
					{
						DEBUG_printf("2	Day	");	
						_start_Schedule(&oldSch, 1);
					}
					else
					{
						DEBUG_printf("all OUT");
						_stop_Schedule();
					}					
				}
				
			}
			else
			{
				DEBUG_printf("One:1	");
				if(start1 < end1)
				{
					if((now>=start1) && (now<end1))
					{
						DEBUG_printf("Day	");	
						_start_Schedule(&oldSch, 0);
					}
					else
					{
						DEBUG_printf("OUT	");
						_stop_Schedule();
					}
				}
				else
				{
					if((now>=start1) || (now<end1))
					{
						DEBUG_printf("Net	");
						_start_Schedule(&oldSch, 0);
					}
					else
					{
						DEBUG_printf("OUT	");
						_stop_Schedule();
					}
				}
			}
		}
		else
		{
			DEBUG_printf("One:2	");
			if(start2 < end2)
			{	
				if((now>=start2) && (now<end2))
				{
					DEBUG_printf("Day	");	
					_start_Schedule(&oldSch, 1);
				}
				else
				{
					DEBUG_printf("OUT	");
					_stop_Schedule();
				}
			}
			else
			{
				if((now>=start2) || (now<end2))
				{
					DEBUG_printf("Net	");
					_start_Schedule(&oldSch, 1);
				}
				else
				{
					DEBUG_printf("OUT	");
					_stop_Schedule();
				}
			}
		}
		usleep(500*1000);
	}
}
//����˵�� : ������̨��ʱ�ƻ�
//����     :
//����ֵ   : 
void PTZ_StartSchedule()
{
	sch_status.sch_LastItem = -1;
	sch_status.sch_LastSch = -1;
	pthread_mutex_init(&guardmutex, NULL);
	pthread_t pidSch;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&pidSch, &attr, (void *)PTZ_ScheduleThrd, NULL);
	pthread_attr_destroy(&attr);
}

#include <utl_cmd.h>



int ptz_main(int argc, char *argv[])
{
	printf("ptz cmd, argv1: %s\n", argv[1]);
	if (strcmp(argv[1], "left") == 0)
	{
		printf("PtzLeftStart\n");
		PtzLeftStart(0, 100);
	}
	else if (strcmp(argv[1], "right") == 0)
	{
		printf("PtzRightStart\n");
		PtzRightStart(0, 100);
	}
	else if (strcmp(argv[1], "up") == 0)
	{
		printf("PtzUpStart\n");
		PtzUpStart(0, 100);
	}
	else if (strcmp(argv[1], "down") == 0)
	{
		printf("PtzDownStart\n");
		PtzDownStart(0, 100);
	}
	else if (strcmp(argv[1], "stop") == 0)
	{
		printf("PtzLeftStop\n");
		PtzLeftStop(0);
	}
	else if (strcmp(argv[1], "preset") == 0)
	{
		unsigned int preset = atoi(argv[2]);
		printf("PTZ_AddPreset: %d\n", preset);
		PTZ_AddPreset(0, preset, "argv[2]");
	}
	else if (strcmp(argv[1], "locatePreset") == 0)
	{
		unsigned int preset = atoi(argv[2]);
		printf("PtzLocatePreset: %d\n", preset);
		PtzLocatePreset(0, preset);
	}
	else if (strcmp(argv[1], "zoom") == 0)
	{
		int x,y,z;
		x = atoi(argv[2]);
		y = atoi(argv[3]);
		z = atoi(argv[4]);
		//x = x * 64 / 1280;
		//y = y * 64 / 720;
		//z = z * 16 - 16;
		PtzZoomPosition(0, x, y, z);
	}
	else if (strcmp(argv[1], "aux") == 0)
	{
		int value = atoi(argv[2]);
		if (value > 0)
			PtzAuxAutoOn(0, value);
		else
			PtzAuxAutoOff(0, 0-value);
	}
	else if (strcmp(argv[1], "sensor") == 0)
	{
		int value = atoi(argv[2]);
		//jv_sensor_wdt_weight(value);
	}
	else if (strcmp(argv[1], "dropon") == 0)
	{
		PtzAuxAutoOn(0, 0x2F);
	}
	else if (strcmp(argv[1], "dropoff") == 0)
	{
		PtzAuxAutoOff(0, 0x2F);
	}
	else
	{
		PtzZoomInStart(0);
	}
	return 0;
}

int ptz_test_init() __attribute__((constructor)) ;

int ptz_test_init()
{
	char *helper = "\tcapture\n";
	utl_cmd_insert("mptz", "mptz zoom [X] [Y] [ZOOM]", helper, ptz_main);
	return 0;
}

