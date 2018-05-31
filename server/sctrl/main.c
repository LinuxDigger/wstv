#include <syslog.h>
#include "jv_common.h"
#include "SYSFuncs.h"
#include "mstream.h"
#include "msnapshot.h"
#include "MRemoteCfg.h"
#include "maccount.h"
#include "mtransmit.h"
#include "mrecord.h"
#include "JVNSDKDef.h"
#include "mlog.h"
#include "mosd.h"
#include "mstorage.h"
#include "mprivacy.h"
#include "mdetect.h"
#include "rcmodule.h"
#include "sctrl.h"
#include "utl_timer.h"
#include "malarmout.h"
#include "malarmin.h"
#include "mipcinfo.h"
#include <mptz.h>
#include <mwdt.h>
#include <JvServer.h>
#include <utl_cmd.h>
#include <utl_system.h>
#include <maccount.h>
#include <arpa/inet.h>
#include <msensor.h>
#include <weber.h>
#include <osd_server.h>
#include <utl_ifconfig.h>
#include <jv_ai.h>
#include <jv_ao.h>
#include "mgb28181.h"
#include <onvif-main.h>
#include <utl_net_lan.h>
#include <jv_gpio.h>
#include <jv_ptz.h>
#include "mvoicedec.h"
#include "mioctrl.h"
#include "msoftptz.h"
#include "maudio.h"
#include "sgrpc.h"
#include "cgrpc.h"
#include "jv_rtc.h"
#include "mcloud.h"
#include "muartcomm.h"
#include "httpclient.h"
#include "upnp_device_main.h"
#include "jv_spi_flash.h"
#include "ks_api.h"
#include "alarm_service.h"
#include "jv_uartcomm.h"
#include "mbizclient.h"
#include "bls.h"
#include <malloc.h>
#include "sp_connect.h"
#include "mdebug.h"
#include "mivp.h"
#include "utl_filecfg.h"
#include "mdooralarm.h"
#include "mfirmup.h"

#define EARLYEST_YEAR_SEC	1420070400 //2015��1��1��8��

//������֤����
//U32 BoardInfo[13]; //GUID(4),SN(1),DATE(1),GROUP(1),CARDTYPE(1),MODEL(1),ENCYR_VER(1),YSTNUM(1),DEV_VER(1),USER(1)
extern U32 yst(U32*, U32);

GPARAM gp;
int debugFlag = 0;
char main_version[16];
char ipc_version[32];
int syslogValue = 0;

//���߳�
VOID *MainThrd(VOID *pArgs)
{
	Printf("gp.bRunning=%d\n", gp.bRunning);
	U32 tickCount = 0;
	pthreadinfo_add((char *)__func__);
//( ע��������ռ�İٷֱ�������ڣ�MemFree + Cached - Mapped���Եģ������������MemTotal��
//����5%��д��Ĭ��ֵ��10
//	utl_system("echo 1 > /proc/sys/vm/dirty_background_ratio");
//	utl_system("echo 10 > /proc/sys/vm/dirty_ratio");
	OpenWatchDog();
	while(gp.bRunning)
	{
		//ÿ���ӽ���һ��ι��
		FeedWatchDog();

		sleep(1);
		++tickCount;
		if ((tickCount & 0xF) == 0)
		{
			utl_system("sync;echo 3 > /proc/sys/vm/drop_caches");
		}
	}

	Printf("Begin logout...\n");

	return NULL;
}

/**
 *����������е㸴��
 *����ĳЩ�ļ�ϵͳ������������hwconfig��һ��������
 *������ļ�����������������ݣ���������/etc/conf.d/fix/hwconfig.cfg��
 *�����ݣ�����ͨ��utl_ifconfig������
 */
static int __hwconfig_file_generate(const char *hwconfig)
{
	char buffer[256];
	unsigned char *ptr = NULL;
	BOOL	bInEMMC = FALSE;		// EMMC��hwconfig��EMMC������

	//�ҵ�hwconfig���ڵ�MTD
	FILE *fp = fopen("/proc/mtd", "r");
	char *p;
	int mtdNo = -1;

	if (!fp)
	{
		printf("ERROR: Failed open mtd file\n");
		return -1;
	}
	while(1)
	{
		p = fgets(buffer, sizeof(buffer), fp);
		if (!p)
		{
			Printf("no hwconfig find\n");
			// fclose(fp);
			bInEMMC = TRUE;
			break;
		}

		if (strstr(buffer, "\"hwconfig\""))
		{
			//Finded
			sscanf(buffer, "mtd%d:", &mtdNo);
			break;
		}
	}
	fclose(fp);

	char mtdDev[32];

	if (bInEMMC)
	{
		S32 ret = utl_system_ex("getmtd \"hwconfig\"", mtdDev, sizeof(mtdDev));
		if (IsSystemFail(ret))
		{
			printf("ERROR: Failed get hwconfig in EMMC\n");
			return -1;
		}
	}
	else
	{
		sprintf(mtdDev, "/dev/mtdblock%d", mtdNo);
	}
	
	int fd = open(mtdDev, O_RDONLY);
	if (fd < 0)
	{
		printf("Failed open mtdDevice: %s, because: %s\n", mtdDev, strerror(errno));
		return -3;
	}

	fp = fopen(hwconfig, "wb");
	if (!fp)
	{
		printf("Failed open hwconfig: %s, because: %s\n", hwconfig, strerror(errno));
		close(fd);
		return -4;
	}

	while(1)
	{
		int len = read(fd, buffer, sizeof(buffer));
		if (len > 0)
		{
			int i;
			for (i=0;i<len;i++)
			{
				ptr = (unsigned char *)(buffer + i);
				if (*ptr && *ptr!=0xFF)
					fwrite(ptr, 1, 1, fp);
				else
				{
					//reach the file end now
					Printf("Find the file end\n");
					fclose(fp);
					close(fd);
					return 0;
				}
			}
		}
		else
		{
			printf("reach the end\n");
			break;
		}
	}
	fclose(fp);
	close(fd);
	printf("ERROR: Should not reach here. in %s\n", __func__);
	return -5;
}

/**
 * �������£�
 * ��άIPC�����		�� ʹ��0xFFFF�������й�9712+�ģ��ù�0xFFF1�ļ���оƬ��Ŀǰ�Ѳ���ʹ��
 * ��ά���û�		�� ʹ��0x4FFF
 * ��άIPC			�� ʹ��0x5010, 0x5011, 0x5013, 0x5020 ������0x5011��0xFFF1һ��������9712+������
 * ��ά����			�� ʹ��0x4010, 0x4013, 0x4020 �� �����10�� 13�� 20����ֱ���
 */
BOOL __check_valid(unsigned int dwCardType)
{
	/*
		#define JV_OEM_ID_NONE   0xE71AFFFF
		#define JV_OEM_ID_4010   0xE71A4010
		#define JV_OEM_ID_4013   0xE71A4013
		#define JV_OEM_ID_4020   0xE71A4020
		#define JV_OEM_ID_4010P  0xE71A4011   //ov9712+
	*/
	if ((dwCardType &0xFF00) == 0x4F00) //��ά ���û�
		return TRUE;
	if ((dwCardType&0xFF00) == 0xFF00)  //��ά ����IP������ [0xFFF2]
		return TRUE;

	unsigned int lowData = dwCardType & 0x00FF;
	if (lowData == 0x11)//û��11��11ʵ������9712+
		lowData = 0x10;
//	printf("%d == %d\n", lowData , (hwinfo.encryptCode & 0xFF));
	if (lowData == (hwinfo.encryptCode & 0xFF))
		return TRUE;

	return FALSE;
}

BOOL __check_factory_flag()
{
	int i;
	char FilePath[256] = "";
	char DevName[32] = "";

	gp.bFactoryFlag = FALSE;

	for (i = 0; i < MAX_DEV_NUM; ++i)
	{
		snprintf(FilePath, sizeof(FilePath), "/progs/rec/%02d", i);
		snprintf(gp.FactoryFlagPath, sizeof(gp.FactoryFlagPath), "%s/check_factorytest.flag", FilePath);
		if (access(gp.FactoryFlagPath, F_OK) == 0)
		{
			gp.bFactoryFlag = TRUE;
			printf("=====Detect check_factorytest.flag %s!!!\n", gp.FactoryFlagPath);
			break;
		}
	}
	if (gp.bFactoryFlag == FALSE)
	{
		return FALSE;
	}

	// ������������ʼ��
	gp.TestCfg.nYTSpeed = 255;
	gp.TestCfg.nYTCheckTimes = 2;
	gp.TestCfg.nInterval = 0;
	gp.TestCfg.bYTOriginReset = TRUE;
	gp.TestCfg.nYTLRSteps = 0;
	gp.TestCfg.nYTUDSteps = 0;
	gp.TestCfg.nYTLREndStep = 0;
	gp.TestCfg.nYTUDEndStep = 0;

	utl_fcfg_get_value_ex(CONFIG_HWCONFIG_FILE, "product", DevName, sizeof(DevName));
	snprintf(gp.FactoryFlagPath, sizeof(gp.FactoryFlagPath), "%s/factorytestcfg_%s", FilePath, DevName);

	FILE* fp = fopen(gp.FactoryFlagPath, "r");
	char Buffer[256] = "";
	char *pKey = NULL, *pValue = NULL;

	if (fp == NULL)
	{
		printf("Open factory test cfg file %s failed: %s!!!\n", gp.FactoryFlagPath, strerror(errno));
		return gp.bFactoryFlag;
	}

	while (fgets(Buffer, sizeof(Buffer), fp) > 0)
	{
		pKey = strtok(Buffer, "=");
		pValue = strtok(NULL, "\r\n\t#");

		if (0 == strcmp(pKey, "nYTSpeed"))
		{
			gp.TestCfg.nYTSpeed = atoi(pValue);
		}
		else if (0 == strcmp(pKey, "nYTCheckTimes"))
		{
			gp.TestCfg.nYTCheckTimes = atoi(pValue);
		}
		else if (0 == strcmp(pKey, "nInterval"))
		{
			gp.TestCfg.nInterval = atoi(pValue);
		}
		else if (0 == strcmp(pKey, "bYTOriginReset"))
		{
			gp.TestCfg.bYTOriginReset = atoi(pValue);
		}
		else if (0 == strcmp(pKey, "nYTLRSteps"))
		{
			gp.TestCfg.nYTLRSteps = atoi(pValue);
			gp.TestCfg.nYTLREndStep = gp.TestCfg.nYTLRSteps / 2;
		}
		else if (0 == strcmp(pKey, "nYTUDSteps"))
		{
			gp.TestCfg.nYTUDSteps = atoi(pValue);
			gp.TestCfg.nYTUDEndStep = gp.TestCfg.nYTUDSteps / 2;
		}
		else if (0 == strcmp(pKey, "nYTLREndStep"))
		{
			gp.TestCfg.nYTLREndStep = atoi(pValue);
		}
		else if (0 == strcmp(pKey, "nYTUDEndStep"))
		{
			gp.TestCfg.nYTUDEndStep = atoi(pValue);
		}
	}

	fclose(fp);

	printf("\n****************************************************************\ngp.TestCfg:\n");
	printf("nYTSpeed: %d",			gp.TestCfg.nYTSpeed);
	printf(", nYTCheckTimes: %d",	gp.TestCfg.nYTCheckTimes);
	printf(", nInterval: %d",		gp.TestCfg.nInterval);
	printf(", bYTOriginReset: %d",	gp.TestCfg.bYTOriginReset);
	printf(", nYTLRSteps: %d",		gp.TestCfg.nYTLRSteps);
	printf(", nYTUDSteps: %d",		gp.TestCfg.nYTUDSteps);
	printf(", nYTLREndStep: %d",	gp.TestCfg.nYTLREndStep);
	printf(", nYTUDEndStep: %d",	gp.TestCfg.nYTUDEndStep);
	printf("\n****************************************************************\n");

	return gp.bFactoryFlag;
}

static void __firmup_stream_callback(void* arg, char* data, int len)
{
#if YST_SVR_SUPPORT
	if(gp.bNeedYST)
	{
		JVN_SendData(1, JVN_DATA_I, (unsigned char*)data, len, 0, 0);
		JVN_SendData(2, JVN_DATA_I, (unsigned char*)data, len, 0, 0);
		JVN_SendMOData(1, JVN_DATA_I, (unsigned char*)data, len);
	}
#endif
}

//���������
S32 main(int argc, char *argv[])
{
	U32 i;
	mstream_attr_t stAttr;
	U32 nDeviceInfo[13]={0};

	jv_rtc_init();
	jv_rtc_sync(0);

	time_t ttOSD = time(NULL);
    if (ttOSD < EARLYEST_YEAR_SEC)
    {
    	ttOSD = EARLYEST_YEAR_SEC;
		stime(&ttOSD);
		jv_rtc_sync(1);
    }

	if (mdebug_checkredirectflag())
	{
		mdebug_redirectprintf();
	}

	printf("%s cmd=[0/1] to enable debug\n", argv[0]);
	printf("%s log=[LOGLEVEL] to set syslog level\n", argv[0]);

	pthreadinfo_add((char *)__func__);

	jv_flash_write_lock_init();
	jv_flash_write_lock();

	if (argc >= 2)
	{
		char *param;
		for (i=1;i<argc;i++)
		{
			param = argv[i];
			if (strncmp(param, "cmd=", 4) == 0)
			{
				utl_cmd_enable(atoi(&param[4]));
			}
			else if (strncmp(param, "log=", 4) == 0)
			{
				static char logflag[32];
				syslogValue = 1;
				sprintf(logflag, "jvlog[%s]", &param[4]);
				printf("logflag: %s\n", logflag);
			    openlog(logflag, LOG_CONS , LOG_USER);
			}
			else if (strncmp(param, "debug=", 6) == 0)
			{
				debugFlag = atoi(&param[6]);
			}
		}
	}

#if MEM_DEBUG
	extern void RegMemCmd();
	RegMemCmd();
#endif

	//���FIX�����ļ�����¼һЩֵ�ü�¼����Ϣ
	{
		if (access(CONFIG_FIXED_PATH, F_OK) != 0)
			mkdir (CONFIG_FIXED_PATH, 0777);

		__hwconfig_file_generate(CONFIG_HWCONFIG_FILE);
	}

	Printf("IPCam Init...\n");
	Printf("Config Path: %s\n",CONFIG_PATH);
	printf("building date: %s: %s\n", __DATE__,__TIME__);
	memset(&gp, 0, sizeof(gp));

	// ������־�ж�
	__check_factory_flag();

	//������֤
	if (0 == yst(nDeviceInfo, 13))
	{
		Printf("Try DEBUG jv_yst\n");
		if (0 == jv_yst(nDeviceInfo, 9))
		{
			printf("yst failed, invalid device...\n");
		#if IPCAMDEBUG	//����ǵ��԰汾��ʱ�����δ�����֤,lck20120302
		#else
			return FALSE;
		#endif
		}
	}

	FILE *fp = fopen("/tmp/encrypt", "wb");
	if (fp)
	{
		int i;
		for (i=0;i<11;i++)
		{
			fprintf(fp, "%x,", nDeviceInfo[i]);
		}
		fprintf(fp, "\n");
		fclose(fp);
	}

    //����SIGPIPE�źţ�����TCP���ӵ���������
	signal(SIGPIPE, SIG_IGN);

	utl_timer_init();
	utl_cmd_init();
	utl_system_init();
	ipcinfo_init();
	jv_gpio_init();
	jv_uartcomm_init();
	ipcinfo_set_buf(nDeviceInfo, nDeviceInfo);
	ipcinfo_set_value(sn, nDeviceInfo[4]);

	snprintf(main_version, sizeof(main_version), "%s", "V2.2");
	memset(ipc_version, 0, sizeof(ipc_version));
	snprintf(ipc_version, sizeof(ipc_version), "%s%s", MAIN_VERSION, SUB_VERSION);

	if (access(CONFIG_FIXED_FILE, F_OK) != 0)
	{
		FILE *fp = fopen(CONFIG_FIXED_FILE, "wb");
		if (fp)
		{
			fprintf(fp, "firstversion=%s\n", IPCAM_VERSION);
			fclose(fp);
		}
	}

	gp.ttNow = time(NULL);	//��ȡ��ǰʱ��
	//�������߳�״̬
	gp.nExit	= EXIT_DEFAULT;
	gp.bRunning	= TRUE;

	char temp[32];
	JVCommonParam_t comParam;
	SYSFuncs_GetValue(CONFIG_FILE, "rotate", temp, sizeof(temp));
	comParam.rotate = (JVRotate_e)atoi(temp);
	SYSFuncs_GetValue(CONFIG_FILE, "bSupportWDr", temp, sizeof(temp));
	comParam.bSupportWdr= atoi(temp);
	SYSFuncs_GetValue(CONFIG_FILE, "bEnableWdr", temp, sizeof(temp));
	comParam.bEnableWdr= atoi(temp);
	comParam.ipcType = ipcinfo_get_type();
	comParam.ipcGroup = nDeviceInfo[6];
	jv_common_init(&comParam);
	
	if (!__check_valid(nDeviceInfo[8] ))
	{
		printf("invalid product: 0x%x\n", nDeviceInfo[8]);
		return FALSE;
	}

	{
#define GREEN "\033[1;32m"
#define END "\033[0m"
		char ystid[20] = {0};
		jv_ystNum_parse(ystid, nDeviceInfo[6], nDeviceInfo[10]);
		printf(GREEN"\n==========> DEVICE YST ID: %s <==========\n\n"END, ystid);
	}

	maudio_init();
	mvoicedec_init();			//�������߳�����֮ǰ��ʼ����������������û����
    net_init(nDeviceInfo[0]);
	mlog_init(LOG_FILE);
	mio_init();
	maccount_init();
	mchnosd_init();
	mprivacy_init();
	mdetect_init();
	mrecord_init();
	malarm_init();
	malarmin_init();
	mstorage_init();
	mstream_init();
	//�Ŵű���
	mdooralarm_init(DoorAlarmInsert, DoorAlarmSend, DoorAlarmStop);

	//���ػ�����Ϣ
	ReadConfigInfo();
	WriteConfigInfo();

	mlog_write("IPCam start.");

	for(i=0; i<HWINFO_STREAM_CNT; i++)
	{
		mstream_flush(i);
	}

	mprivacy_flush(0);
    msensor_init(0);
    
#ifdef WEB_SUPPORT
	if (gp.bNeedWeb)
	{
		webserver_flush();
	}
#endif

	//����¼��
	mrecord_flush(0);

	//�����ƶ����
	mdetect_flush(0);
	
	//������������
	malarmin_flush(0);

	//��̨����
	PTZ_Init();
	//���ڳ�ʼ��
	muartcomm_init();
	
	//��̨��ʱ����
	//PTZ_StartSchedule();
	//�����������
	malarm_flush();

#ifdef WEB_SUPPORT
	if (gp.bNeedWeb)
	{
		weber_init();
	}
#endif
	osd_server_init();

	msensor_flush(0);
	//��ά���ܷ�����֧�֣�IVP��
#if ((defined  IVP_SUPPORT) ||(defined XW_MMVA_SUPPORT))
	mivp_init();
	mivp_start(0);
#endif
#ifdef YST_SVR_SUPPORT
	if (gp.bNeedYST)
	{
		//��ʼ������ͨ
		InitYST();
		int ystBufSize[MAX_STREAM] = {(1024+150)*1024, (512-152)*1024, 512*1024};		
		for(i=0; i<HWINFO_STREAM_CNT; i++)
		{
			StartYSTCh(i, FALSE, ystBufSize[i]);
		}
		StartMOServer(0);

		YstOnline();

		//����Զ������
		remotecfg_init();
		mivp_set_callback(remotecfg_mivp_callback);
		
		mstream_set_transmit_callback(Transmit);
		mdetect_set_callback(remotecfg_mdetect_callback);
		malarmin_set_callback(remotecfg_alarmin_callback);
		mivp_set_callback(remotecfg_mivp_callback);
		mivp_hide_set_callback(remotecfg_mivp_hide_callback);
		mivp_left_set_callback(remotecfg_mivp_left_callback);
		mivp_removed_set_callback(remotecfg_mivp_removed_callback);
	}
#endif

#ifdef TINY_ONVIF_SUPPORT
	if (gp.bNeedOnvif)
	{
		OnvifParam_t param = {
			"http://%s/onvif/device_service",
			8099,
			NULL//stderr
		};
		onvif_run(&param);
	}
#endif

#ifdef GB28181_SUPPORT
	mgb28181_init();
#endif

#include "sp_init.h"
	sp_init();

	mfirmup_init(__firmup_stream_callback, NULL);
	mcloud_init();
#ifdef BIZ_CLIENT_SUPPORT
	mbizclient_init();
#endif

	/*GRPC�ͻ���*/
	cgrpc_init();
	sgrpc_init();

	//���߳�
	MainThrd(NULL);

#ifdef GB28181_SUPPORT
	mgb28181_deinit();
#endif

	mrecord_stop(0);//���û��ֹͣ¼��������֮ǰ��¼���ļ�����ʧ�ܴ�СΪ0

	//�����˳�����
	Printf("ExitCode:%d\n", gp.nExit);
	jv_flash_write_unlock();
	jv_flash_write_lock_deinit();
	char acBuffer[1024]={0};
	switch(gp.nExit)
	{
	case EXIT_SHUTDOWN:
		utl_system("sync;sleep 1;poweroff");
		break;
	case EXIT_REBOOT:

		utl_system("sync;sleep 1;reboot");
		break;
	case EXIT_FIRMUP:

		utl_system("sync;sleep 1;reboot");
		break;
	case EXIT_RECOVERY:
		//Ӳ�������ָ������������ڵ�����
		utl_system("rm -rf "CONFIG_PATH"/network; "
				"cp "CONFIG_PATH"/default/interface.cfg "CONFIG_PATH"/network/");
		//break not needed
	case EXIT_SOFT_RECOVERY:
	{
		sprintf(acBuffer, "rm %s %s %s %s %s %s;",
			CONFIG_PATH"/account.dat", CONFIG_FILE, GB28181_CFG_FILE, FIRSTPOS_FILE,PATROL_FLAG_FLAG,CONFIG_PATH"/yst-pos.dat");
		utl_system(acBuffer);
		//utl_system("rm "IMPORTANT_CFG_FILE);	//�ָ�ϵͳ��ɾ��������Ϣ����������������Ӣ����ʾ���ĵ����
		maudio_speaker(VOICE_RESET, TRUE,TRUE, TRUE);
		utl_system("sync;sleep 1;reboot;");
		break;
	}
	case EXIT_KILL:
		break;
	default:
		break;
	}

	return 0;
}

