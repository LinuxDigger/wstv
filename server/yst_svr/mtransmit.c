
#include "jv_common.h"
#include "JvServer.h"
#include "mstream.h"
#include "mrecord.h"
#include "MRemoteCfg.h"
#include "mstorage.h"
#include "SYSFuncs.h"
#include "maccount.h"
#include "mlog.h"
#include "mipcinfo.h"
#include "mtransmit.h"
#include <jv_ao.h>
#include "sctrl.h"
#include "utl_ifconfig.h"
#include <mptz.h>
#include <msensor.h>
#include "mfirmup.h"
#include "JvCDefines.h"
#include "utl_timer.h"
#include "mplay_remote.h"
#include <jvlive.h>
#include "sp_connect.h"
#include "msoftptz.h"
#include "rcmodule.h"
#include "muartcomm.h"
#include "httpclient.h"
#include "md5.h"
#include "mcloud.h"
#include "jv_stream.h"
#include "mvoicedec.h"
#include "maudio.h"
#include "utl_queue.h"
#include "cJSON.h"


YST stYST;
static PNETLINKINFO g_pNetLinkInfoFirst=NULL;//��¼������Ϣ
static jv_thread_group_t	s_YstGroup;
int transmit_list_client(void);


//�ص���nClientID֮����������ӡ�
//��nClientIDΪ-1ʱ���ر���������
void mtransmit_disconnect_all_but(U32 nClientID)
{
	U32 carray[MAX_CLIENT];
	int cnt;
	pthread_mutex_lock(&stYST.mutex);
	PNETLINKINFO pNetLinkInfo = g_pNetLinkInfoFirst;
	PNETLINKINFO pNetLinkInfoPrev = NULL;
	PNETLINKINFO ret;

	cnt = 0;
	while (NULL != pNetLinkInfo)
	{
		carray[cnt++] = pNetLinkInfo->nClientID;
		pNetLinkInfoPrev = pNetLinkInfo;
		pNetLinkInfo = pNetLinkInfo->next;
	}
	pthread_mutex_unlock(&stYST.mutex);

	int i;
	for (i=0;i<cnt;i++)
	{
		printf("clientid: %d, pnetlinkinfo.clientid: %d\n", nClientID, carray[i]);
		if (nClientID != carray[i])
		{
			printf("disconnect: %d\n", carray[i]);
			if(gp.bNeedYST)
			{
				JVN_SendDataTo(carray[i], JVN_CMD_DISCONN, 0, 0, 0, 0);
			}
		}
	}
}

NETLINKINFO *__NETLINK_GetByIndex(int clientIndex, NETLINKINFO *info)
{
	pthread_mutex_lock(&stYST.mutex);
	PNETLINKINFO pNetLinkInfo = g_pNetLinkInfoFirst;
	PNETLINKINFO pNetLinkInfoPrev = NULL;
	PNETLINKINFO ret;
	int i=0;

	while (NULL != pNetLinkInfo)
	{
		//�ҵ�����Զ�����õ�����
		if (i == clientIndex)
		{
			//�ж�Ȩ��
			break;
		}
		pNetLinkInfoPrev = pNetLinkInfo;
		pNetLinkInfo = pNetLinkInfo->next;
		i++;
	}
	if (pNetLinkInfo)
	{
		memcpy(info, pNetLinkInfo, sizeof(NETLINKINFO));
		ret = info;
	}
	else
		ret = NULL;
	pthread_mutex_unlock(&stYST.mutex);
	return ret;
}

NETLINKINFO *__NETLINK_Get(U32 nClientID, NETLINKINFO *info)
{
	BOOL bPower	= FALSE;
	pthread_mutex_lock(&stYST.mutex);
	PNETLINKINFO pNetLinkInfo = g_pNetLinkInfoFirst;
	PNETLINKINFO pNetLinkInfoPrev = NULL;
	PNETLINKINFO ret;

	while (NULL != pNetLinkInfo)
	{
		//�ҵ�����Զ�����õ�����
		if (nClientID == pNetLinkInfo->nClientID)
		{
			//�ж�Ȩ��
			break;
		}
		pNetLinkInfoPrev = pNetLinkInfo;
		pNetLinkInfo = pNetLinkInfo->next;
	}
	if (pNetLinkInfo)
	{
		memcpy(info, pNetLinkInfo, sizeof(NETLINKINFO));
		ret = info;
	}
	else
		ret = NULL;
	pthread_mutex_unlock(&stYST.mutex);
	return ret;
}

int __NETLINK_Add(NETLINKINFO *info)
{
	pthread_mutex_lock(&stYST.mutex);
	PNETLINKINFO pNetLinkInfo = g_pNetLinkInfoFirst;
	PNETLINKINFO pNetLinkInfoPrev = NULL;
	PNETLINKINFO temp;

	temp = malloc(sizeof(NETLINKINFO));
	if (temp == NULL)
	{
		pthread_mutex_unlock(&stYST.mutex);
		return -1;
	}
	memcpy(temp, info, sizeof(NETLINKINFO));
	while (NULL != pNetLinkInfo)
	{
		pNetLinkInfoPrev = pNetLinkInfo;
		pNetLinkInfo = pNetLinkInfo->next;
	}
	if (g_pNetLinkInfoFirst == NULL)
		g_pNetLinkInfoFirst = temp;
	else
		pNetLinkInfoPrev->next = temp;
	pthread_mutex_unlock(&stYST.mutex);
	SPConnection_t con;
	con.conType = SP_CON_JOVISION;
	con.key = info->nClientID;
	strcpy(con.addr, info->chClientIP);
	strcpy(con.protocol, "jovision");
	strcpy(con.user, info->stNetUser.acID);
	sp_connect_add(&con);
	return 0;
}

int __NETLINK_Remove(U32 nClientID)
{
	pthread_mutex_lock(&stYST.mutex);
	PNETLINKINFO pNetLinkInfo = g_pNetLinkInfoFirst;
	PNETLINKINFO pNetLinkInfoTemp =  pNetLinkInfo;
	while(pNetLinkInfo)
	{
		if(nClientID == pNetLinkInfo->nClientID)
		{
			if(pNetLinkInfo == g_pNetLinkInfoFirst)
			{
				g_pNetLinkInfoFirst = pNetLinkInfo->next;
			}

			pNetLinkInfoTemp->next = pNetLinkInfo->next;
			free(pNetLinkInfo);
			break;
		}
		else
		{
			pNetLinkInfoTemp =  pNetLinkInfo;
			pNetLinkInfo = pNetLinkInfo->next;
		}
	}
	pthread_mutex_unlock(&stYST.mutex);
	SPConnection_t con;
	con.conType = SP_CON_JOVISION;
	con.key = nClientID;
	sp_connect_del(&con);

	return 0;
}

VOID build_bc_param(BCPACKET *pstBCPacket)
{
	if(!pstBCPacket) return ;
	char iface[20];
	utl_ifconfig_get_iface(iface);
	//��ȡ���������
	ipcinfo_t stIPCInfo;
	ipcinfo_get_param(&stIPCInfo);
	//��ȡ�������
	eth_t stETH;
	utl_ifconfig_build_attr(iface, &stETH, TRUE);

	memset(pstBCPacket, 0, sizeof(BCPACKET));
	memcpy(pstBCPacket->acGroup, &stIPCInfo.nDeviceInfo[6], sizeof(stIPCInfo.nDeviceInfo[6]));
	pstBCPacket->nYSTNO = stYST.nID;
	pstBCPacket->nPort = stYST.nPort;
	strcpy(pstBCPacket->acName, stIPCInfo.acDevName);
	pstBCPacket->nType = stIPCInfo.nDeviceInfo[7];
	pstBCPacket->nChannel = 1;
	pstBCPacket->bDHCP = stETH.bDHCP;
	pstBCPacket->nlIP = inet_addr(stETH.addr);
	pstBCPacket->nlNM = inet_addr(stETH.mask);
	pstBCPacket->nlGW = inet_addr(stETH.gateway);
	pstBCPacket->nlDNS = inet_addr(stETH.dns);
	strcpy(pstBCPacket->acMAC, stETH.mac);
	if (CheckbAppendSearchPrivateInfo())
	{
		strcpy(pstBCPacket->chPrivateInfo, "timer_count=1;");
		pstBCPacket->nPrivateSize = strlen(pstBCPacket->chPrivateInfo) + 1;
	}
	strcpy(pstBCPacket->nickName, stIPCInfo.nickName);
}

void __BC_SET_GW_CORRECTION(char *ipstr,char *gwstr)//�������ؾ������򵥴��������������JNVR����DHCP��ȡ�������ص�����
{
	if(strncmp(ipstr,"169.254.0",9)==0)
		return ;
	if(strncmp(gwstr,"0.0.0.0",16)==0)
		strncpy(gwstr,ipstr,16);
	char *p=strstr(gwstr,".");
	p=strstr(++p,".");
	p=strstr(++p,".");
	*++p = '1';
	*++p = '\0';
}

//������,lck20121013
void funcSBCDataCallback(int nBCID, U8 *pBuffer, int nSize, char chIP[16], int nPort)
{
	if(!pBuffer) return ;

	//Printf("funcSBCDataCallback, Local CloudSEE ID:%s%d\n", stYST.strGroup, stYST.nID);

	BCPACKET stBCPacket;
	memcpy(&stBCPacket, pBuffer, sizeof(BCPACKET));

	switch(stBCPacket.nCmd)
	{
	case BC_SEARCH:
//		Printf("Search...\n");
		build_bc_param(&stBCPacket);
		stBCPacket.nCmd = BC_SEARCH;
		if(gp.bNeedYST)
		{
			JVN_BroadcastRSP(nBCID, (unsigned char*)&stBCPacket, sizeof(BCPACKET), nPort);
		}
		break;
	case BC_GET:
//		Printf("Get Info, acGroup=%s, nYSTNO=%d\n", stBCPacket.acGroup, stBCPacket.nYSTNO);
		if(stBCPacket.nYSTNO==stYST.nID && 0==strcmp(stBCPacket.acGroup, stYST.strGroup))
		{
			//��֤Ȩ��
			if(0 == (maccount_check_power(stBCPacket.acID, stBCPacket.acPW) & POWER_ADMIN))
			{
				Printf("No power get...\n");
				stBCPacket.nCmd = BC_NOPOWER;
				if(gp.bNeedYST)
				{
					JVN_BroadcastRSP(nBCID, (unsigned char*)&stBCPacket, sizeof(BCPACKET), nPort);
				}
			}
			else
			{
				build_bc_param(&stBCPacket);
				stBCPacket.nCmd = BC_GET;
				if(gp.bNeedYST)
				{
					JVN_BroadcastRSP(nBCID, (unsigned char*)&stBCPacket, sizeof(BCPACKET), nPort);
				}
			}
		}
		break;
	case BC_SET:
		Printf("Set Info, acGroup=%s, nYSTNO=%d\n", stBCPacket.acGroup, stBCPacket.nYSTNO);
		if(stBCPacket.nYSTNO==stYST.nID && 0==strcmp(stBCPacket.acGroup, stYST.strGroup))
		{
			//��֤Ȩ��
			if(0 == (maccount_check_power(stBCPacket.acID, stBCPacket.acPW) & POWER_ADMIN))
			{
				Printf("No power set...\n");
				stBCPacket.nCmd = BC_NOPOWER;
				if(gp.bNeedYST)
				{
					JVN_BroadcastRSP(nBCID, (unsigned char*)&stBCPacket, sizeof(BCPACKET), nPort);
				}
			}
			else
			{
				char inet[32];
				utl_ifconfig_get_inet(inet);
				if (0 != strcmp(inet, "dhcp")
						&& 0 != strcmp(inet, "static"))
				{
					printf("ERROR: only dhcp and static can set ip\n");
					break;
				}
				eth_t stETH;
				struct in_addr stAddr;
				strcpy(stETH.name, "eth0");
				stETH.bDHCP = stBCPacket.bDHCP;
				stAddr.s_addr = stBCPacket.nlIP;
				inet_ntop(AF_INET, &stAddr, stETH.addr, sizeof(stETH.addr));
				stAddr.s_addr = stBCPacket.nlNM;
				inet_ntop(AF_INET, &stAddr, stETH.mask, sizeof(stETH.mask));
				stAddr.s_addr = stBCPacket.nlGW;
				inet_ntop(AF_INET, &stAddr, stETH.gateway, sizeof(stETH.gateway));
				stAddr.s_addr = stBCPacket.nlDNS;
				inet_ntop(AF_INET, &stAddr, stETH.dns, sizeof(stETH.dns));
				__BC_SET_GW_CORRECTION(stETH.addr,stETH.gateway);
				utl_ifconfig_eth_set(&stETH);
				mlog_write("Set AutoIP: %s",stETH.addr);
				Printf("ip:%s, netmask:%s, gw:%s, dns:%s, dhcp:%d\n",
					stETH.addr, stETH.mask, stETH.gateway, stETH.dns, stETH.bDHCP);
				//JVN_BroadcastRSP(nBCID, (unsigned char*)&stBCPacket, sizeof(BCPACKET), nPort);
			}
		}
		break;
	case BC_GET_ERRORCODE:
		//Printf("BC_GET_ERRORCODE, acGroup=%s, nYSTNO=%d\n", stBCPacket.acGroup, stBCPacket.nYSTNO);
		if(stBCPacket.nYSTNO==stYST.nID && 0==strcmp(stBCPacket.acGroup, stYST.strGroup))
		{
			//��֤Ȩ��
			if(0 == (maccount_check_power(stBCPacket.acID, stBCPacket.acPW) & POWER_ADMIN))
			{
				stBCPacket.nCmd = BC_GET_ERRORCODE;
				stBCPacket.nErrorCode = maccount_get_errorcode();
				if(gp.bNeedYST)
				{
					JVN_BroadcastRSP(nBCID, (unsigned char*)&stBCPacket, sizeof(BCPACKET), nPort);
				}
				//Printf("Response error code:0x%X\n", stBCPacket.nErrorCode);
			}
		}
		break;
	case BC_SET_WIFI: 
		{
			wifiap_t wifiApInfo,localApt;
			WIFI_INFO wifiData_FromNVR;
			static int orMatchCodeFlag = 0;
			if(orMatchCodeFlag == 0) 
			{
				orMatchCodeFlag = 1; //to avoid many times to connection
				memset(&wifiData_FromNVR,0,sizeof(WIFI_INFO));
				memcpy(&wifiData_FromNVR,stBCPacket.chPrivateInfo,stBCPacket.nPrivateSize);
		
				memcpy(wifiApInfo.name, wifiData_FromNVR.wifiId, sizeof(wifiApInfo.name));
				memcpy(wifiApInfo.passwd, wifiData_FromNVR.wifiPwd, sizeof(wifiApInfo.passwd));
				wifiApInfo.iestat[0] = wifiData_FromNVR.wifiAuth;
				wifiApInfo.iestat[1] = wifiData_FromNVR.wifiEncryp;
				utl_ifconfig_wifi_start_sta();
				printf("[%s] wifi ssid === %s pwd === %s\n",__FUNCTION__,wifiApInfo.name,\
					wifiApInfo.passwd);
				net_deinit();
				utl_ifconfig_wifi_connect(&wifiApInfo);
			}
			else 
				printf("wifi had Match Code !!!!!\n");
		}
		break;
	default:
		break;
	}
}

ACCOUNT *GetClientAccount(U32 nClientID, ACCOUNT *account)
{
	BOOL bPower	= FALSE;
	NETLINKINFO info;
	NETLINKINFO *ret;
	ret = __NETLINK_Get(nClientID, &info);
	if (ret)
	{
		memcpy(account, &info.stNetUser, sizeof(ACCOUNT));
		return account;
	}
	return NULL;
}

//�����֤�ص�����
BOOL FuncSCheckPassCallback(int nLocalChannel, char chClientIP[16], int nClientPort, char *pName, char *pWord)
{
	//-1ͨ����������ʱֱ�ӷ���
	if(-1 == nLocalChannel)
	{
		return (BOOL)1;
	}

	return (BOOL) maccount_check_power(pName, pWord);
}

// ����ֵ: �Ƿ��л�����������״̬
static BOOL __checkNeedSwitchVoiceConf()
{
	BOOL bSwitched = FALSE;

	if (utl_ifconfig_b_linkdown("eth0") 
		&& utl_ifconfig_bsupport_apmode(utl_ifconfig_wifi_get_model())
		&& WIFI_MODE_AP == utl_ifconfig_wifi_get_mode())
	{
		// APģʽ�£�û������ʱ�������������ã�������á�ȷ��APģʽ�¼����ͶԽ���������
		if (transmit_list_client() == 0)
		{
			if (!isVoiceSetting())
			{
				mvoicedec_enable();
				bSwitched = TRUE;
			}
		}
		else
		{
			if (isVoiceSetting())
			{
				mvoicedec_disable();
				bSwitched = TRUE;
			}
		}
	}

	return bSwitched;
}

//����״���ص�����
void FuncSConnectCallback(int nLocalChannel,int nClientID, unsigned char uchType, char chClientIP[16], int nClientPort, char *pName, char *pWord, int nUserDataLen, char* pUserData)
{
	U32 i=0;
	char chMsg1[256]={0};
	sprintf(chMsg1, "Channel:%d -- Client:%s:%d, type:%d", nLocalChannel, chClientIP, nClientPort, uchType);

	if(JVN_SCONNECTTYPE_PCCONNOK == uchType || JVN_SCONNECTTYPE_MOCONNOK== uchType)
	{
	    strcat(chMsg1,"connect success!\n");
	    printf("%s", chMsg1);

		NETLINKINFO info;
		NETLINKINFO *pNetLinkInfo;
		pNetLinkInfo = __NETLINK_Get(nClientID, &info);
		if (pNetLinkInfo)
		{
			if (strcmp(chClientIP, pNetLinkInfo->chClientIP) || nClientPort != pNetLinkInfo->nClientPort)
			{
				pNetLinkInfo->nChannel = nLocalChannel;
				strcpy(pNetLinkInfo->chClientIP,chClientIP);
				pNetLinkInfo->nClientPort = nClientPort;
				pNetLinkInfo->nType = uchType;
			}
		}
		else
		{
			U32 power;
			// power = maccount_check_power(pName, pWord);
			if(strcmp(pName, "") == 0)
				power = POWER_ADMIN|POWER_FIXED;	// ΪӦ�������Ż����û����������⣬ֻҪ�����ӳɹ����������Ȩ��
			else
				power = maccount_check_power(pName, pWord);
			pNetLinkInfo = &info;
			pNetLinkInfo->nChannel = nLocalChannel;
			pNetLinkInfo->nClientID = nClientID;
			sprintf(pNetLinkInfo->chClientIP,"%s", chClientIP);
			pNetLinkInfo->nClientPort = nClientPort;
			pNetLinkInfo->nType = uchType;
			pNetLinkInfo->bChatting =  FALSE;
			sprintf(pNetLinkInfo->stNetUser.acID, "%s", pName);
			sprintf(pNetLinkInfo->stNetUser.acPW, "%s", pWord);
			pNetLinkInfo->stNetUser.nPower = power;
			pNetLinkInfo->next = NULL;
			__NETLINK_Add(pNetLinkInfo);
		}

		// ����O֡ǰ������Ƿ���Ҫ�л���������״̬
		__checkNeedSwitchVoiceConf();

		//�����߳���ͻ��˷�����Ƶ��Ϣ
		if(nLocalChannel > 0 && nLocalChannel <= HWINFO_STREAM_CNT)
		{
			pthread_t id;
			pthread_create_detached(&id, NULL, SendInfo2Client, (void*)nClientID);
		}

		mlog_write("Total Connected count: %d", transmit_list_client());
   }
   else if (uchType == JVN_SCONNECTTYPE_DISCONNOK ||
   			uchType == JVN_SCONNECTTYPE_DISCONNE)
   {
		NETLINKINFO info;
		PNETLINKINFO pNetLinkInfo = __NETLINK_Get(nClientID, &info);
		if (pNetLinkInfo)
		{
			if(nClientID == pNetLinkInfo->nClientID)
			{
				__NETLINK_Remove(nClientID);
				remote_stop_chat(-1, nClientID);
				Remote_Player_Destroy(nClientID);
			}
		}
		__checkNeedSwitchVoiceConf();
	}
};

//����״���ص�����
void FuncSOnlineCallback(int nLocalChannel, unsigned char uchType)
{
	ipcinfo_t ipcinfo;
	ipcinfo_get_param(&ipcinfo);
	switch(uchType)
	{
	case JVN_SONLINETYPE_ONLINE:
		{
			stYST.nStatus = JVN_SONLINETYPE_ONLINE;
			printf("YST On Line!\n");
			char ystID[32];
			jv_ystNum_parse(ystID,ipcinfo.nDeviceInfo[6],stYST.nID);
			mlog_write("IPCamera ID [%s] Online",ystID );
			if(strlen(ipcinfo.nickName) == 0)
			{
				if(gp.bNeedYST)
				{
					int ret = JVN_GetNickName();//����ͨ�������Ϊ�գ���Ҫ�ӷ�����ͬ��
					if(ret != 0)
						printf("JVN_GetNickName error %d\n",ret);
				}
			}
		}
		break;
	case JVN_SONLINETYPE_OFFLINE:
		{
			//���֮ǰ�����ߣ���������ߣ����ӡ
			if (stYST.nStatus == JVN_SONLINETYPE_ONLINE)
			{
				char ystID[32];
				jv_ystNum_parse(ystID,ipcinfo.nDeviceInfo[6],stYST.nID);
				mlog_write("IPCamera ID [%s] Offline",ystID);
			}
			stYST.nStatus = JVN_SONLINETYPE_OFFLINE;
			printf("YST Off Line!\n");
		}
		break;
	case JVN_SONLINETYPE_CLEAR:
		{
			stYST.nStatus = JVN_SONLINETYPE_CLEAR;
			stYST.nID = 0;
			Printf("need clear ystinfo!\n");
			//mlog_write("CloudSEE ID Is Cleared");
		}
		break;
	default:
		break;
	}
}

//�ļ������ص�����
void FuncSCheckfileCallback(int nLocalChannel, int nClientID, char chClientIP[16], int nClientPort, char chStartTime[14], char chEndTime[14], unsigned char *pBuffer, int *nSize)
{
    char strT[7]={0};
	char strPath[128]={0};
	char strFolder[10]={0};
	U8	TypePlay;		//��������¼���ļ�����
	U32	i, ChnPlay;	//��������¼���ļ�ͨ��
	BOOL bFindLink=FALSE;

	*nSize=0;

	Printf("CheckfileCallback...\n");

	NETLINKINFO info;
	PNETLINKINFO pNetLinkInfo =  __NETLINK_Get(nClientID, &info);
	if (pNetLinkInfo)
	{
		if(pNetLinkInfo->nClientID == nClientID)
		{
			Printf("Find the client id, ok\n");
			bFindLink = TRUE;
			if(pNetLinkInfo->stNetUser.nPower & (POWER_USER|POWER_ADMIN))
			{
				mlog_write("User:[%s] search record file", pNetLinkInfo->stNetUser.acID);
				Printf("Power ok...\n");
				for(i=0; i<1; i++)
				{
					sprintf(strPath, "%s/", MOUNT_PATH);//"/mnt/rec",i);
					if(access(strPath, F_OK))
					{
						break;//�����������������ֹ����
					}
					memset(strFolder, 0, 10);
					strncat(strFolder, chStartTime, 8);
					strcat(strPath, strFolder);

					if(access(strPath, F_OK))
					{
						Printf("Path:%s can't access\n", strPath);
						continue;//����ļ��в����������������һ����
					}
					DIR	*pDir	= opendir(strPath);
					struct dirent *pDirent	= NULL;
					while((pDirent=readdir(pDir)) != NULL)
					{
						//�����������������ͺ�ͨ��
						if(!strcmp(FILE_FLAG, pDirent->d_name+strlen(pDirent->d_name)-strlen(FILE_FLAG)))
						{
							sscanf(pDirent->d_name, "%c%2d%6s", &TypePlay, &ChnPlay, strT);
							Printf("RecFile Type:%c , RecChn:%d, time:%s\n", TypePlay, ChnPlay, strT);
							//if(ChnPlay == nLocalChannel)
							{
								pBuffer[*nSize]	= 'C'+i;	//DVR������Ϊ20�����̷���C��ʼ��
								*nSize += 1;
								memcpy(&pBuffer[*nSize],strT,6);		//¼��ʱ��
								*nSize += 6;
								pBuffer[*nSize]	= TypePlay;
								*nSize += 1;
								sprintf((char *)&pBuffer[*nSize], "%.2d", ChnPlay);
								*nSize += 2;
							}
						}
					}
					closedir(pDir);
				}
				return;
			}
		}
	}
	if(bFindLink)
	{
		Printf("no power to farplay or download!\n");
	}
	else
	{
		Printf("not find the link!\n");
	}
};


typedef enum
{
	SEARCH_BY_NONE,
	SEARCH_BY_YMD,
	SEARCH_BY_YM
}SEARCH_TYPE;

SEARCH_TYPE __getcheck_parse_json(unsigned char *buffer, int* nYear,int* nMonth, int* nDay, int* nType)
{
	char *str = (char *)buffer;
	cJSON *resp = cJSON_Parse(str);
	SEARCH_TYPE sType = SEARCH_BY_NONE;

	if(resp != NULL)
	{
		const char* cMethod = cJSON_GetObjectValueString(resp, "method");
		printf("cMethod=%s\n", cMethod);
		if(strcmp(cMethod, "remoteSearchRecord") == 0)
		{
			*nYear = cJSON_GetObjectValueInt(resp, "year");
			*nMonth = cJSON_GetObjectValueInt(resp, "month");
			*nDay = cJSON_GetObjectValueInt(resp, "day");
			*nType = cJSON_GetObjectValueInt(resp, "type");
			sType = SEARCH_BY_YMD; 
		}
		else if(strcmp(cMethod, "remoteRecordDate") == 0)
		{
			*nYear = cJSON_GetObjectValueInt(resp, "year");
			*nMonth = cJSON_GetObjectValueInt(resp, "month");
			sType = SEARCH_BY_YM; 
		}
	}
	cJSON_Delete(resp);
	return sType;
}

static time_t __getplay_paras_json(unsigned char *pbuf)
{
	char *str = (char *)pbuf;
	cJSON *resp = cJSON_Parse(str);
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	if(resp != NULL)
	{
		const char*  cMethod = cJSON_GetObjectValueString(resp, "method");
		if(strcmp(cMethod, "remotePlay") == 0 ||
				strcmp(cMethod, "remoteSeek") == 0)
		{
			nYear = cJSON_GetObjectValueInt(resp, "year");
			nMonth = cJSON_GetObjectValueInt(resp, "month");
			nDay = cJSON_GetObjectValueInt(resp, "day");
			nHour = cJSON_GetObjectValueInt(resp, "hour");
			nMinute = cJSON_GetObjectValueInt(resp, "minute");
			nSecond = cJSON_GetObjectValueInt(resp, "second");
		}
	}
	cJSON_Delete(resp);
	printf("year:%d month:%d day:%d hour:%d min:%d sec:%d\n",
			nYear, nMonth, nDay, nHour, nMinute, nSecond);

	struct tm stTms;
	memset(&stTms, 0, sizeof(struct tm));
	stTms.tm_year = nYear - 1900;
	stTms.tm_mon = nMonth - 1;
	stTms.tm_mday = nDay;
	stTms.tm_hour = nHour;
	stTms.tm_min = nMinute;
	stTms.tm_sec = nSecond;
	return mktime(&stTms);
}

void FuncSCheckFileNewCallBack(int nLocalChannel,
		int nClientID,
		char chClientIP[16],
		int nClientPort,
		unsigned char *pRawBuffer,
		int nRawSize,
		unsigned char **pResultBuffer,
		int *nResultSize)
{
	printf("FuncSCheckfileNewCallback nClientID:%d\n", nClientID);

	if(pRawBuffer == NULL)
		return;

	NETLINKINFO info;
	PNETLINKINFO pNetLinkInfo =  __NETLINK_Get(nClientID, &info);
	if (pNetLinkInfo)
	{
		if(pNetLinkInfo->nClientID != nClientID)
		{
			Printf("no power to remote play or download!\n");
			return;
		}
	}
	else
	{
		Printf("not find the link!\n");
		return;
	}

	char strPath[128] = {0};
	int year = 0, month = 0, day = 0;
	int nType = -1;
	int i = 0;
	char* result = NULL;

	*pResultBuffer = NULL;
	*nResultSize = 0;

	SEARCH_TYPE sType = __getcheck_parse_json(pRawBuffer, &year , &month, &day, &nType);

	printf("%s year:%d month:%d day:%d sType:%d\n", __FUNCTION__, year, month, day, sType);
	sprintf(strPath, "%s", MOUNT_PATH);
	if(access(strPath, F_OK))
	{
		printf("Path:%s can't access\n", strPath);
		return;
	}

	if(sType == SEARCH_BY_YMD)
	{
	#define MAX_SEARCH_CNT		1024
		cJSON *req = cJSON_CreateObject();
		cJSON_AddStringToObject(req, "method", "remoteSearchRecord_resp");
		cJSON *params = cJSON_CreateArray();
		cJSON_AddItemToObject(req, "list", params);

		mrecord_item_t* items = calloc(sizeof(mrecord_item_t), MAX_SEARCH_CNT);
		int date = year * 10000 + month * 100 + day;
		int cnt = mrecord_search_file(date, 0, date, 235959, items, MAX_SEARCH_CNT);
		mrecord_item_t* itr = items;
		char buffer[32] = {0};

		for (i = 0; i < cnt; ++i)
		{
			cJSON *item = cJSON_CreateObject();
			cJSON_AddItemToArray(params, item);

			cJSON_AddNumberToObject(item, "disk", itr->part);
			snprintf(buffer, sizeof(buffer), "%08d", itr->date);
			cJSON_AddStringToObject(item, "date", buffer);
			snprintf(buffer, sizeof(buffer), "%c%02d%06d.mp4", itr->type, 1, itr->fileTime);
			cJSON_AddStringToObject(item, "file", buffer);
			cJSON_AddNumberToObject(item, "type", itr->type);
			snprintf(buffer, sizeof(buffer), "%06d", itr->stime);
			cJSON_AddStringToObject(item, "start", buffer);
			snprintf(buffer, sizeof(buffer), "%06d", itr->etime);
			cJSON_AddStringToObject(item, "end", buffer);
			++itr;
		}

		free(items);

		printf("search file count:%d\n", cnt);
		cJSON_AddNumberToObject(req, "count", cnt);

		result = cJSON_PrintUnformatted(req);
		cJSON_Delete(req);
	}
	else if(sType == SEARCH_BY_YM)
	{
		char foldername[256] = {0};
		int cnt = 0;

		cJSON *req = cJSON_CreateObject();
		cJSON_AddStringToObject(req, "method", "remoteRecordDate");
		cJSON *params = cJSON_CreateArray();
		cJSON_AddItemToObject(req, "list", params);

		for (i = 1; i <= 31; ++i)
		{
			snprintf(foldername, sizeof(foldername), "%s/%04d%02d%02d", strPath, year, month, i);
			if (access(foldername, F_OK) != 0)
			{
				continue;
			}

			cJSON *item = cJSON_CreateObject();
			cJSON_AddItemToArray(params, item);
			cJSON_AddStringToObject(item, "date", strrchr(foldername, '/') + 1);
			++cnt;
		}

		printf("search folder count:%d\n", cnt);
		cJSON_AddNumberToObject(req, "count", cnt);

		result = cJSON_PrintUnformatted(req);
		cJSON_Delete(req);
	}

	if (result)
	{
		*nResultSize = strlen(result) + 1;
		*pResultBuffer = (unsigned char*)result;
		printf("%s, size: %d, result: %s\n", __func__, *nResultSize, *pResultBuffer);
	}
	return;
}

#define AO_FILE_SAVE_TEST 0
#if AO_FILE_SAVE_TEST
int __ao_save_frame(jv_audio_frame_t *frame)
{
	static int fd = -1;
	static int frame_cnt = 0;
	if (access("/etc/conf.d/ao_test.cfg", F_OK) == 0)
	{
		char path[64];

		if(fd==-1)
		{
			utl_fcfg_get_value_ex("/etc/conf.d/ao_test.cfg", "path", path, 64);
			frame_cnt = utl_fcfg_get_value_int("/etc/conf.d/ao_test.cfg","fcnt",100);
			printf("======================open file==============path:%s,fcnt:%d\n",path,frame_cnt);
			fd = open(path,O_WRONLY|O_APPEND|O_CREAT);
		}
		write(fd, frame->aData, frame->u32Len);
		frame_cnt--;
		if(frame_cnt<=0)
			utl_system("rm /etc/conf.d/ao_test.cfg");
	}
	else
	{
		if(fd>=0)
		{
			printf("======================close file==============\n");
			close(fd);
			fd = -1;
		}
	}
	return 0;
}
#endif

//�����ص�����
void FuncSChatCallback(int nLocalChannel, int nClientID, unsigned char uchType, char chClientIP[16], int nClientPort, unsigned char *pBuffer, int nSize)
{
	jv_audio_frame_t frame;

    if (uchType == JVN_RSP_CHATDATA)
	{
		memcpy(frame.aData, pBuffer, nSize);
		frame.u32Len = nSize;
		//���Ž��յ�����Ƶ֡
		jv_ao_send_frame(0,&frame);
#if AO_FILE_SAVE_TEST
		__ao_save_frame(&frame);
#endif
	}
    else
    {
    	//ǣ�����ص������еĵ���JVN_SendChatData�������ط�����
    	REMOTECFG *cfg;

    	cfg = calloc(1, sizeof(REMOTECFG));

    	if(cfg)
    	{
    		if(nSize)
    		{
    			memcpy(&cfg->stPacket, pBuffer, nSize);
    		}
    		cfg->nClientID	= nClientID;
    		cfg->nCmd		= uchType;
    		cfg->nCh		= nLocalChannel;
    		cfg->nSize		= nSize;

    		if(JVERR_NO_FREE_RESOURCE == remotecfg_sendmsg(cfg))
    		{
    			free(cfg);
    			Printf("queue no free resource...\n");
    		}
    	}

    }
}

//�ı��ص�����
void FuncSTextCallback(int nLocalChannel, int nClientID, unsigned char uchType, char chClientIP[16], int nClientPort, unsigned char *pBuffer, int nSize)
{
	//Printf("text callback, ip:%s, uchType=%d\n", chClientIP, uchType);
	REMOTECFG *cfg;

	cfg = calloc(1, REMOTE_CFG_SIZE(nSize));

	if(cfg)
	{
		if(nSize)
		{
			memcpy(&cfg->stPacket, pBuffer, nSize);
		}
		cfg->nClientID	= nClientID;
		cfg->nCmd		= uchType;
		cfg->nCh		= nLocalChannel;
		cfg->nSize		= nSize;
		cfg->bSetting	= TRUE;

		if(remotecfg_sendmsg(cfg) < 0)
		{
			free(cfg);
			Printf("queue no free resource...\n");
		}
	}
//	Printf("leave FuncSTextCallback...\n");
}

//��̨���ƻص�����
void FuncSYTCtrlCallback(int nLocalChannel, int nClientID, int nType, char chClientIP[16], int nClientPort)
{
	BOOL bFindLink = FALSE;
	int ytChn = 0;
	NETLINKINFO info;
	PNETLINKINFO pNetLinkInfo = __NETLINK_Get(nClientID, &info);

	U32 speed = 	(nType & 0xFF000000) >> 24;

	if (speed == 0)
	{
	//	speed = PTZ_SPEED_NOR;
		//�����һ���ֶ��ƶ�����Ѳ��ʱ���ڹ�����ٶ�
		speed=abs(msoftptz_speed_get(0));
	}

	nType &= 0xFF;
	if (pNetLinkInfo)
	{
		if(pNetLinkInfo->nClientID == nClientID)
		{
			bFindLink = TRUE;
			if(pNetLinkInfo->stNetUser.nPower & (POWER_USER|POWER_ADMIN))
			{
				Printf("YTCtrl: 0x%x, speed: %d\n", nType, speed);
				switch(nType)
				{
				case JVN_YTCTRL_U://��
					PtzUpStart(ytChn, speed);
					break;
				case JVN_YTCTRL_D://��
					PtzDownStart(ytChn, speed);
					break;
				case JVN_YTCTRL_L://��
					PtzLeftStart(ytChn, speed);
					break;
				case JVN_YTCTRL_R://��
					PtzRightStart(ytChn, speed);
					break;
				case JVN_YTCTRL_LU:
					PtzPanTiltStart(ytChn, 1, 1, speed, speed);//����
					break;
				case JVN_YTCTRL_RU:
					PtzPanTiltStart(ytChn, 0, 1, speed, speed);//����
					break;
				case JVN_YTCTRL_LD:
					PtzPanTiltStart(ytChn, 1, 0, speed, speed);//����
					break;
				case JVN_YTCTRL_RD:
					PtzPanTiltStart(ytChn, 1, 0, speed, speed);//����
					break;					
				case JVN_YTCTRL_A://�Զ�
					PtzAutoStart(ytChn, speed);
					break;
				case JVN_YTCTRL_GQD://��Ȧ��
					PtzIrisOpenStart(ytChn);
					break;
				case JVN_YTCTRL_GQX://��ȦС
					PtzIrisCloseStart(ytChn);
					break;
				case JVN_YTCTRL_BJD://�佹��
					PtzFocusNearStart(ytChn);
					break;
				case JVN_YTCTRL_BJX://�佹С
					PtzFocusFarStart(ytChn);
					break;
				case JVN_YTCTRL_BBD://�䱶��
					PtzZoomInStart(ytChn);
					break;
				case JVN_YTCTRL_BBX://�䱶С
					PtzZoomOutStart(ytChn);
					break;
				case JVN_YTCTRL_UT:     //��ֹͣ
					PtzUpStop(ytChn);
					break;
				case JVN_YTCTRL_DT:     //��ֹͣ
					PtzDownStop(ytChn);
					break;
				case JVN_YTCTRL_LT:     //��ֹͣ
					PtzLeftStop(ytChn);
					break;
				case JVN_YTCTRL_RT:     //��ֹͣ
					PtzRightStop(ytChn);
					break;
				case JVN_YTCTRL_AT:     //�Զ�ֹͣ
					PtzAutoStop(ytChn);
					break;
				case JVN_YTCTRL_GQDT:   //��Ȧ��ֹͣ
					PtzIrisOpenStop(ytChn);
					break;
				case JVN_YTCTRL_GQXT:   //��ȦСֹͣ
					PtzIrisCloseStop(ytChn);
					break;
				case JVN_YTCTRL_BJDT:   //�佹��ֹͣ
					PtzFocusNearStop(ytChn);
					break;
				case JVN_YTCTRL_BJXT:   //�佹Сֹͣ
					PtzFocusFarStop(ytChn);
					break;
				case JVN_YTCTRL_BBDT:   //�䱶��ֹͣ
					PtzZoomInStop(ytChn);
					break;
				case JVN_YTCTRL_BBXT:   //�䱶Сֹͣ
					PtzZoomOutStop(ytChn);
					break;
				case JVN_YTCTRL_FZ1:
					PtzAuxAutoOn(ytChn, 1);
					break;
				case JVN_YTCTRL_FZ2:
					PtzAuxAutoOn(ytChn, 2);
					break;
				case JVN_YTCTRL_FZ3:
					PtzAuxAutoOn(ytChn, 3);
					break;
				case JVN_YTCTRL_FZ4:
					//speed��Ϊ�������
					PtzAuxAutoOn(ytChn, speed);
					break;
				case JVN_YTCTRL_FZ1T:
					PtzAuxAutoOff(ytChn, 1);
					break;
				case JVN_YTCTRL_FZ2T:
					PtzAuxAutoOff(ytChn, 2);
					break;
				case JVN_YTCTRL_FZ3T:
					PtzAuxAutoOff(ytChn, 3);
					break;
				case JVN_YTCTRL_FZ4T:
					PtzAuxAutoOff(ytChn, speed);
					break;
				}
			}
			if(pNetLinkInfo->stNetUser.nPower & (POWER_USER|POWER_ADMIN))
			{
				switch(nType)
				{
				case JVN_YTCTRL_RECSTART:
					mrecord_set(0, bEnable, TRUE);
					mrecord_flush(0);
					break;
				case JVN_YTCTRL_RECSTOP:
					mrecord_set(0, bEnable, FALSE);
					mrecord_flush(0);
					break;
				default:
					break;
				}
			}
			return;
		}
	}
}

enum
{
	YST_EVENT_PLAY,
};

typedef struct
{
	int		EventType;
	union
	{
		PLAY_INFO PlayInfo;
	};
}YstEvent_t;

static BOOL __yst_PlayCtrl(PLAY_INFO* pData)
{
	int offset = 0;
	char filePath[128] = {0};

	//��֤����
	if(pData == NULL)
	{
		return FALSE;
	}

	switch(pData->ucCommand)
	{
	case JVN_REQ_PLAY://����Զ�̻ط�
		{
			int ret = Remote_Player_Create(pData->nClientID, pData->nConnectionType,
					EN_PLAYER_TYPE_NORMAL, EN_PLAYER_MODE_ONE, (void *)pData->strFileName);
		}
		break;
	case JVN_CMD_PLAYUP://���
		{
			Remote_Player_Fast(pData->nClientID);
		}
		break;

	case JVN_CMD_PLAYDOWN://����
		{
			Remote_Player_Slow(pData->nClientID);
		}
		break;

	case JVN_CMD_PLAYDEF://ԭ�ٲ���
		{
			Remote_Player_PlayNormal(pData->nClientID);
		}
		break;

	case JVN_CMD_PLAYSTOP://ֹͣ����
	case JVN_CMD_TIMEJSON_PLAYSTOP:
		{
			Remote_Player_Destroy(pData->nClientID);
		}
		break;

	case JVN_CMD_PLAYPAUSE://��ͣ����
		{
			Remote_Player_Pause(pData->nClientID);
		}
		break;

	case JVN_CMD_PLAYGOON://��������
		{
			Remote_Player_Resume(pData->nClientID);
		}
		break;

	case JVN_CMD_PLAYSEEK://seek��ĳһ֡
		{
			Remote_Player_Seek(pData->nClientID, pData->nSeekPos);
		}
		break;

	case JVN_REQ_TIMEJSON_PLAY:
		{
			if(pData->chPkgBuf == NULL)
				break;

			Printf("chPkgBuf:%s\n", pData->chPkgBuf);
			time_t timePoint = __getplay_paras_json(pData->chPkgBuf);
			Remote_Player_Create(pData->nClientID, pData->nConnectionType,
					EN_PLAYER_TYPE_TIMEPOINT, EN_PLAYER_MODE_SEQUENCE, (void *)timePoint);
		}
		break;
		
	case JVN_CMD_PLAYSEEK_TIME:
		{
			if(pData->chPkgBuf == NULL)
				break;

			Printf("chPkgBuf:%s\n", pData->chPkgBuf);
			time_t timePoint = __getplay_paras_json(pData->chPkgBuf);
			Remote_Player_Seek(pData->nClientID, timePoint);
		}
		break;
		
	default:
		return FALSE;
	}
	return TRUE;
}

static void* __yst_work_thread(void* param)
{
	int iMqHandle = (int)param;
	int ret;
	YstEvent_t*		pEvent = NULL;

	if (iMqHandle < 0)
	{
		return NULL;
	}

	while (1)
	{
		ret = utl_queue_recv(iMqHandle, &pEvent, -1);
		if (ret != 0 || pEvent == NULL)
		{
			Printf("%s, timeout happened when mq_receive data\n", __func__);
			usleep(1);
			continue;
		}

		switch(pEvent->EventType)
		{
		case YST_EVENT_PLAY:
			__yst_PlayCtrl(&pEvent->PlayInfo);
			break;
		default:
			break;
		}

		free(pEvent);
		pEvent = NULL;
	}

	return NULL;
}

static BOOL FuncSFPlayCtrlCallback(PLAY_INFO* pData)
{
	YstEvent_t* pEvent = NULL;

	//��֤����
	if(pData == NULL)
	{
		return FALSE;
	}

	printf("Play Callback,cmd = %x nSeekPos:%d pkgSize:%d\n", pData->ucCommand, pData->nSeekPos, pData->nPkgSize);

	switch(pData->ucCommand)
	{
	case JVN_REQ_PLAY://����Զ�̻ط�
	case JVN_CMD_PLAYUP://���
	case JVN_CMD_PLAYDOWN://����
	case JVN_CMD_PLAYDEF://ԭ�ٲ���
	case JVN_CMD_PLAYSTOP://ֹͣ����
	case JVN_CMD_PLAYPAUSE://��ͣ����
	case JVN_CMD_PLAYGOON://��������
	case JVN_CMD_PLAYSEEK://seek��ĳһ֡

	case JVN_CMD_PLAYSEEK_TIME:
	case JVN_REQ_TIMEJSON_PLAY:
	case JVN_CMD_TIMEJSON_PLAYSTOP:
		break;
	default:
		return FALSE;
	}

	pEvent = calloc(1, sizeof(YstEvent_t));
	if (!pEvent)
	{
		return FALSE;
	}

	pEvent->EventType = YST_EVENT_PLAY;
	pEvent->PlayInfo = *pData;

	if (utl_queue_send(s_YstGroup.iMqHandle, &pEvent) != 0)
	{
		free(pEvent);
		return FALSE;
	}

	return TRUE;
}

void FuncSFRecvMsgCallBack(int nType, char *chPushMsg, int nMsgLen, SOCKADDR_IN *svrAddr)
{
	
}

void FuncDownLoadFileName(char chFilePathName[256])
{
	
}

void FuncSRtmpConnectCallBack(int nLocalChannel, unsigned char uchType, char *pMsg)
{
	
}

//����1Ϊ������0Ϊ�쳣
int FuncSToolDataCallBack(STLANTOOLINFO *pData)
{
	static int bSetted = 0;
	int ret;
	unsigned int power;

//	printf("===CONFIG=====\n[type:%d][%s:%s][%s%d][%s][len:%d]",
//		             pData->uchType,
//					 pData->chPName, pData->chPWord,
//					 pData->chGroup,pData->nYSTNUM,
//					 pData->chCurTime,
//					 pData->nDLen);

	if (bSetted)
	{
		printf("had been setted\n");
		return 0;
	}
	power = maccount_check_power(pData->chPName, pData->chPWord);
	if (!(POWER_ADMIN&power))
	{
		printf("ERROR: power wrong: %s, %s\n", pData->chPName, pData->chPWord);
		return 0;
	}
	switch (pData->uchType)
	{
	case 1:
		//���Թ��ߵĹ㲥
	{
		ipcinfo_t ipcinfo;

		ipcinfo_get_param(&ipcinfo);
		pData->uchType = 3;
		memcpy(pData->chGroup, &ipcinfo.nDeviceInfo[6], 4);

		pData->nCardType = ipcinfo.nDeviceInfo[7];
		pData->nDate = ipcinfo.nDeviceInfo[5];
		pData->nSerial = ipcinfo.nDeviceInfo[4];

		memcpy(&(pData->guid), &ipcinfo.nDeviceInfo[0], sizeof(GUID));//GUID
		ret = 1;
	}
		break;
	case 2:
		//���Թ��ߵ�����
	{
		bSetted = 1;
		//����ͨ��
		stYST.nID = pData->nYSTNUM;

		//ʹ������ͨ��
	    {
			ipcinfo_t ipcinfo;
			STOnline pkOnline;
			ipcinfo_get_param(&ipcinfo);
			ipcinfo.ystID = stYST.nID;
			ipcinfo_set_param(&ipcinfo);
			WriteConfigInfo();
			Printf("stYST.nID: %d\n", stYST.nID);
			memcpy(pkOnline.chGroup, &ipcinfo.nDeviceInfo[6], 4);
			pkOnline.nCardType = ipcinfo.nDeviceInfo[7];
			pkOnline.nDate = ipcinfo.nDeviceInfo[5];
			pkOnline.nSerial = ipcinfo.nDeviceInfo[4];
			pkOnline.nYstNum = stYST.nID;
			pkOnline.nChannelCount = 1;
			memcpy(&(pkOnline.gLoginKey), &ipcinfo.nDeviceInfo[0], sizeof(GUID));
			if(gp.bNeedYST)
			{
				JVN_InitYST((char *)&pkOnline, sizeof(STOnline));
			}
		}

		//����ʱ��
		struct tm	stTm, stOldTm;
		sscanf(pData->chCurTime, "%d-%d-%d %d:%d:%d", &stTm.tm_year, &stTm.tm_mon, &stTm.tm_mday,
		       &stTm.tm_hour, &stTm.tm_min, &stTm.tm_sec);
		stTm.tm_year -= 1900;
		stTm.tm_mon	-= 1;
		//��֤����������Ƿ���ȷ
		memcpy(&stOldTm, &stTm, sizeof(stTm));
		time_t now = mktime(&stTm);
		if (   stTm.tm_year != stOldTm.tm_year
		        || stTm.tm_mon  != stOldTm.tm_mon
		        || stTm.tm_mday != stOldTm.tm_mday
		        || stTm.tm_hour != stOldTm.tm_hour
		        || stTm.tm_min  != stOldTm.tm_min
		        || stTm.tm_sec  != stOldTm.tm_sec
		        || now == -1
		   )
		{
			printf("wrong time format: %s\n", pData->chCurTime);
//			return;
		}
		else
		{
			ipcinfo_set_time_ex(&now);//����ʱ��
		}

		ret = 1;
	}
		break;
	default:
		ret = 1;
		break;
	}

	return ret;
}

int transmit_list_client(void)
{
	pthread_mutex_lock(&stYST.mutex);

	PNETLINKINFO pNetLinkInfo = g_pNetLinkInfoFirst;
	PNETLINKINFO pNetLinkInfoPrev = NULL;
	int cnt = 0;

	while (NULL != pNetLinkInfo)
	{
//		printf("client connected: %s:%d, with clientid: %d, chn: %d\n", pNetLinkInfo->chClientIP, pNetLinkInfo->nClientPort, pNetLinkInfo->nClientID, pNetLinkInfo->nChannel);
		pNetLinkInfo = pNetLinkInfo->next;
		cnt++;
	}
	pthread_mutex_unlock(&stYST.mutex);
//	printf("connected cnt: %d\n", cnt);
	return cnt;
}

//�пͻ�������ʱ�����̵߳��ô˺���,lck20120531
VOID* SendInfo2Client(VOID *pArgs)
{
	U8	acBuffer[256]={0};
	U32 nClientID = (U32)pArgs, nChannel = 0;
	JVS_FILE_HEADER_EX jHeaderEx;

	pthreadinfo_add((char *)__func__);

	NETLINKINFO info;
	PNETLINKINFO pNetLinkInfo = __NETLINK_Get(nClientID, &info);


	//�ѶϿ�
	if (!pNetLinkInfo)
	{
		Printf("Disconnected already!!!!\n");
		return NULL;
	}

	nChannel = pNetLinkInfo->nChannel-1;


	if(JVN_SCONNECTTYPE_MOCONNOK == pNetLinkInfo->nType)
	{
		nChannel = SctrlGetMOChannel();
	}

	SctrlMakeHeader(nChannel, &jHeaderEx);

	memcpy(acBuffer+2, &jHeaderEx, sizeof(jHeaderEx));

	Printf("nChannel=%d, nClientID=%d--------------\n", nChannel, nClientID);
	if(gp.bNeedYST)
	{
		JVN_SendDataTo(nClientID, JVN_CMD_FRAMETIME, NULL, 0, 10, 25);	//֪ͨ����ͨ����֡���Լ��ؼ�֡���,lck20101008
	}

	if(gp.bNeedYST)
	{
		JVN_SendDataTo(nClientID, JVN_DATA_O, acBuffer, sizeof(JVS_FILE_HEADER_EX)+2, 0, 0);
	}

	mstream_request_idr(nChannel);

	return NULL;
}

void _resolution_changed(int channelid);

//H264�ֻ����֡ͷ��Ϣ
typedef struct tagPHONEHEADER
{
	U32		nHeight:16;
	U32		nWidth:16;
	U32		nIndex;
}PHONEHEADER;

//DVR�����֡���ͺ�����ͨ�����֡���͵�ӳ��
static U32 FTYPEMAP[5]={JVN_DATA_I, JVN_DATA_P, JVN_DATA_B, JVN_DATA_A, JVN_DATA_SKIP};
//ע���MStreamģ��ȥִ�еĺ��� ʵ�־��巢�͹���
VOID Transmit(S32 nChannel, VOID *pData, U32 nSize, jv_frame_type_e nType, unsigned long long timestamp)
{
	static SctrlStreamInfo LastInfo[MAX_STREAM];
	if (nChannel < 0 || nChannel >= HWINFO_STREAM_CNT)
	{
		return;
	}

	//СάAPP���������СάAPP���ͺ�ȥ����·��Ƶ��������ʡ��������
	if (!hwinfo.bXWNewServer && (nType == JV_FRAME_TYPE_A))
	{
		remote_send_chatdata(nChannel+1, pData, nSize);
	}

	SctrlStreamInfo NowInfo;

	SctrlGetStreamInfo(nChannel, &NowInfo);
	if (SctrlCheckStreamInfoChanged(&LastInfo[nChannel], &NowInfo))
	{
		if (LastInfo[nChannel].width != 0)
		{
			printf("%s, stream info changed, send O frame...\n", __func__);
			_resolution_changed(nChannel);
		}
		LastInfo[nChannel] = NowInfo;
	}

	//��ֿط�����Ƶ����
	if(stYST.bTransmit[nChannel])
	{
		if(gp.bNeedYST)
		{
			JVN_SendData(nChannel+1, FTYPEMAP[nType], pData, nSize, NowInfo.width, NowInfo.height);
		}
	}
	
	//�����ĸ������ȣ������ĸ�����,lck20121106
	if(SctrlGetMOChannel()==nChannel)
	{
		if(gp.bNeedYST)
		{
			JVN_SendMOData(1, FTYPEMAP[nType], pData, nSize);
		}
	}
}

//��ʼ������ͨ����
VOID InitYSTID()
{
	static BOOL inited = FALSE;
	ipcinfo_t ipcinfo;
	ipcinfo_get_param(&ipcinfo);
	Printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
			ipcinfo.nDeviceInfo[0],ipcinfo.nDeviceInfo[1],ipcinfo.nDeviceInfo[2],ipcinfo.nDeviceInfo[3],
			ipcinfo.nDeviceInfo[4],ipcinfo.nDeviceInfo[5],ipcinfo.nDeviceInfo[6],ipcinfo.nDeviceInfo[7]);

    
	//��ʼ������ͨ����
    if (stYST.nID != 0)
    {
		STOnline pkOnline;
		Printf("stYST.nID: %d\n", stYST.nID);
		memcpy(pkOnline.chGroup, &ipcinfo.nDeviceInfo[6], 4);
		pkOnline.nCardType = ipcinfo.nDeviceInfo[7];
		pkOnline.nDate = ipcinfo.nDeviceInfo[5];
		pkOnline.nSerial = ipcinfo.nDeviceInfo[4];
		pkOnline.nYstNum = stYST.nID;
		pkOnline.nChannelCount = 1;
		memcpy(&(pkOnline.gLoginKey), &ipcinfo.nDeviceInfo[0], sizeof(GUID));
		pkOnline.dwProductType = ((ipcinfo.nDeviceInfo[7]&0xFFFF) << 16) + (ipcinfo.nDeviceInfo[8]&0xFFFF);

		pkOnline.dwEncryVer = ipcinfo.nDeviceInfo[9]; //���ܰ汾, 0xB567709F������оƬ�а�������ͨ���룬�����ݿ�;0xF097765B�����������룬�����ݿ�;0xB56881B0:�������룬�����ݿ�;
		pkOnline.dwDevVer = ipcinfo.nDeviceInfo[11]; //����оƬӲ���汾
		pkOnline.nUIVer;//���ذ汾
		pkOnline.dwOemid = ipcinfo.nDeviceInfo[10];//����id
		pkOnline.dwUser = ipcinfo.nDeviceInfo[12];//������ԱID
		
		if (hwinfo.bHomeIPC)
		{
			pkOnline.nMaxConnNum = MAX_CLIENT_HOME;   //���������
		}
		else
		{
			pkOnline.nMaxConnNum = MAX_CLIENT;	  //���������
		}
		
		pkOnline.nZone = 86;//����-086�й�
		pkOnline.nSystemType = 0x20000022;//ϵͳ�ͺ�-��1�ֽ�ϵͳ����(0x1:windows 0x2:linux 0x3:MacOS 0x4:��׿ 0x5:����)����3�ֽ�ϵͳ�汾�ţ���ϵͳ�汾�����ע��
		/*Windows:6.1; 6.0; 5.2; 5.1; 5.0; 4.9; 4.1; 4.0; 3.1; 3.0; 2.0; 1.0 ��
		linux :2.6; 2.4; 2.2; 2.0; 1.2; 1.1; 1.0 ��
		MAC OS:10.7; 10.6; 10.5; 10.4; 10.3; 10.2; 10.1; 10.0 ��;
		Android 1.1; 1.5; 1.6; 2.0; 2.1; 2.2; 2.3; 2.4; 3.0; 3.1; 3.2; 4.0; 4.1; 4.2; 4.4��    */
		/*���磺win7: 0x103d, win XP: 0x1033; linux2.6: 0x201a, linux2.5: 0x2019; MacOS10.7: 0x306B, MacOS10.6:0x306A; Android4.0: 0x4028; Android4.4: 0x402c*/
		
		strncpy(pkOnline.chProducType, hwinfo.type, sizeof(pkOnline.chProducType));//��Ʒ�ͺ�-�ַ���

		strncpy(pkOnline.chDevType, ipcinfo.product, sizeof(pkOnline.chDevType));//Ӳ���ͺ�-�ַ���

		if(gp.bNeedYST)
		{
			printf("=========JVN_InitYST\n");
			JVN_InitYST((char *)&pkOnline, sizeof(STOnline));
		}
	}
}

//����ͨ�����̣߳�����������йر�
void* ActiveYSTThrd(VOID *pArgs)
{
	pthreadinfo_add((char *)__func__);
	InitYSTID();

	return NULL;
}

void YstOnline()
{
	//��ͨ���ڼ���оƬ�У�����Ҫ���
	//����ͨ�������Զ���������
	if(gp.bNeedYST)
	{
		pthread_t pidYst;
		pthread_create_detached(&pidYst,NULL,ActiveYSTThrd,NULL);
	}
}

//��ʼ��������ͨ����
S32 InitYST()
{
	if(!gp.bNeedYST)
		return 0;

	U32 i;
	
	for (i=0; i<HWINFO_STREAM_CNT; i++)
	{
		stYST.bTransmit[i] = FALSE;
	}
	g_pNetLinkInfoFirst = NULL;

	s_YstGroup.iMqHandle = utl_queue_create("YstWork", sizeof(YstEvent_t*), 32);
	if (s_YstGroup.iMqHandle < 0)
	{
		printf("%s, utl_queue_create failed!!!\n", __func__);
		// return -1;
	}
	pthread_create_detached(&s_YstGroup.thread, NULL, __yst_work_thread, (void*)s_YstGroup.iMqHandle);

    //��ʼ��SDK
	STDeviceType type = {6, 4};
	ipcinfo_t ipcinfo;
	ipcinfo_get_param(&ipcinfo);
	Printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
			ipcinfo.nDeviceInfo[0],ipcinfo.nDeviceInfo[1],ipcinfo.nDeviceInfo[2],ipcinfo.nDeviceInfo[3],
			ipcinfo.nDeviceInfo[4],ipcinfo.nDeviceInfo[5],ipcinfo.nDeviceInfo[6],ipcinfo.nDeviceInfo[7]);
	
	STOnline pkOnline;
	memcpy(pkOnline.chGroup, &ipcinfo.nDeviceInfo[6], 4);
	pkOnline.nCardType = ipcinfo.nDeviceInfo[7];
	pkOnline.nDate = ipcinfo.nDeviceInfo[5];
	pkOnline.nSerial = ipcinfo.nDeviceInfo[4];
	pkOnline.nYstNum = stYST.nID;
	pkOnline.nChannelCount = 1;
	memcpy(&(pkOnline.gLoginKey), &ipcinfo.nDeviceInfo[0], sizeof(GUID));
	
	pkOnline.dwProductType = ((ipcinfo.nDeviceInfo[7] & 0xFFFF) << 16) + (ipcinfo.nDeviceInfo[8] & 0xFFFF);
	
	pkOnline.dwEncryVer = ipcinfo.nDeviceInfo[9]; //���ܰ汾, 0xB567709F������оƬ�а�������ͨ���룬�����ݿ�;0xF097765B�����������룬�����ݿ�;0xB56881B0:�������룬�����ݿ�;
	pkOnline.dwDevVer = ipcinfo.nDeviceInfo[11]; //����оƬӲ���汾
	pkOnline.nUIVer; //���ذ汾
	pkOnline.dwOemid = ipcinfo.nDeviceInfo[10]; //����id
	pkOnline.dwUser = ipcinfo.nDeviceInfo[12]; //������ԱID
	
	if (hwinfo.bHomeIPC)
	{
		pkOnline.nMaxConnNum = MAX_CLIENT_HOME;   //���������

	}
	else
	{
		pkOnline.nMaxConnNum = MAX_CLIENT;	  //���������
	}
	
	pkOnline.nZone = 86;	  //����-086�й�
	pkOnline.nSystemType = 0x20000022;//ϵͳ�ͺ�-��1�ֽ�ϵͳ����(0x1:windows 0x2:linux 0x3:MacOS 0x4:��׿ 0x5:����)����3�ֽ�ϵͳ�汾�ţ���ϵͳ�汾�����ע��
	
	strncpy(pkOnline.chProducType, hwinfo.type, sizeof(pkOnline.chProducType));   //��Ʒ�ͺ�-�ַ���
	
	strncpy(pkOnline.chDevType, ipcinfo.product, sizeof(pkOnline.chDevType));	  //Ӳ���ͺ�-�ַ���

	int OEMID = 2;
	if (hwinfo.bCloudSee || hwinfo.bInternational)
	{
		// ����治�ܿ������Ż����������ֳ�ͼ��������
		OEMID = 0;
	}
	if (!JVN_InitSDK(stYST.nPort-1, stYST.nPort, stYST.nPort+1, 0, MAX_FRAME_LEN, FALSE, type, sizeof(STDeviceType),(char *)&pkOnline, sizeof(STOnline), OEMID))
	{
		printf("\n=========>JVN_InitSDK failed!!!!\n\n");
		return FAILURE;
	}

	JVN_SetLocalFilePath(CONFIG_PATH);
	JVN_SetIPCWANMode();

	pthread_mutex_init(&stYST.mutex, NULL);

    //ע��ص�����
	JVN_RegisterCallBack(FuncSCheckPassCallback,
                          FuncSOnlineCallback, FuncSConnectCallback, FuncSCheckfileCallback,
                          FuncSYTCtrlCallback, FuncSChatCallback, FuncSTextCallback,
                          FuncSFPlayCtrlCallback ,FuncSFRecvMsgCallBack);

	JVN_RegisterCheckFileCallBack(FuncSCheckFileNewCallBack);

	//���������ÿ���ע�����
	JVN_EnableLANToolServer(1, 9104, FuncSToolDataCallBack);
    //ͣд��־
	JVN_EnableLog(IPCAMDEBUG);
	JVN_SetLanguage(JVN_LANGUAGE_CHINESE);
	//���������������

	if (hwinfo.bHomeIPC)
	{
		JVN_SetClientLimit(-1, MAX_CLIENT_HOME);
	}
	else
	{
		JVN_SetClientLimit(-1, MAX_CLIENT);
	}
	
	//�����豸����,lck20121106
	JVN_SetDeviceName(ipcinfo.acDevName);
    //���Զ������¼��ص��������Թ����з��ֱ���ע�����豸����֮�󣬲�֪Ϊ�Σ�gyd20131111
    JVN_RegDownLoadFileName(FuncDownLoadFileName);

	//�����-1ͨ�������Ƶ�-1ͨ��
	char acPath[MAX_PATH]={0};
	JVN_StartChannel(-1, JVN_ALLSERVER, FALSE, 0, acPath);

		//����ͨ������СֵΪ400*1024.�����-1ͨ����Ҳû��������������Ϊ��0�ǲ��е�
	JVN_StartChannel(999, JVN_ALLSERVER, FALSE, 400*1024, acPath);

	//�����Զ���㲥ģ��
	if(JVN_StartBroadcastServer(9106, funcSBCDataCallback))
	{
		Printf("JVN_StartBroadcastServer, ok..........\n");
	}

	//��������������
	JVN_StartLANSerchServer(RC_PORT);
	
	//����������Ƶ����ʱ��� ����Զ�̻ط�ʱ���
	JVN_SetViewTimeFrame(0, 1);

	STDEVINFO sd;
	memset(&sd, 0, sizeof(sd));
	strcpy(sd.chDeviceName, hwinfo.devName);
	if (utl_ifconfig_bSupport_ETH_check() == TRUE)
	{
		sd.nNetMod |= 1 << (NET_MOD_WIRED - 1);
	}
	if (utl_ifconfig_wifi_bsupport() == TRUE)
	{
		sd.nNetMod |= 1 << (NET_MOD_WIFI - 1);
	}
	sd.nCurNetMod = NET_MOD_UNKNOW;
	int bLinkDown = utl_ifconfig_b_linkdown("eth0");
	if (bLinkDown == 0)
	{
		sd.nCurNetMod = NET_MOD_WIRED;
	}
	if(bLinkDown && utl_ifconfig_wifi_bsupport())
	{
		sd.nCurNetMod = NET_MOD_WIFI;
	}
	//֪ͨ��ͨ������ʹ�õ�����
	JVN_SetDeviceInfo(&sd, sizeof(sd), DEV_SET_NET);

	if (hwinfo.bCloudSee == TRUE)
	{
		IPC_INIT_PARA para;
		memset(&para, 0x00, sizeof(para));
		para.nIpcDevType = 2;
		para.nIpcSubDevType = 1;
		strcpy(para.chDevName,hwinfo.devName);
		strcpy(para.chDevVision, MAIN_VERSION);
		int ret = JVN_InitIPCPara(&para);
		printf("================JVN_InitIPCPara ret: %d\n", ret);
	}

	return SUCCESS;
}

//ͣ������ͨ����
S32 ReleaseYST()
{
	if(gp.bNeedYST)
	{
		JVN_StopBroadcastServer();

		JVN_ReleaseSDK();
	}

	//�ͷ��ڴ�-�ͻ���������Ϣ
	PNETLINKINFO pNetLinkInfo = g_pNetLinkInfoFirst;
	PNETLINKINFO pNetLinkInfoNext = NULL;
	while (pNetLinkInfo)
	{
		pNetLinkInfoNext = pNetLinkInfo->next;
		free(pNetLinkInfo);
		pNetLinkInfo = pNetLinkInfoNext;
	}

	pthread_mutex_destroy(&stYST.mutex);

	return SUCCESS;
}

S32 StartYSTCh(U32 nCh, BOOL bLanOnly, U32 nBufSize)
{
	if(!gp.bNeedYST)
		return FAILURE;

	char acPath[MAX_PATH]={0};
	if(FALSE == JVN_StartChannel(nCh+1, bLanOnly?JVN_ONLYNET:JVN_ALLSERVER, FALSE, nBufSize, acPath))
		{
			Printf("StartYSTCh Failed\n");
			return FAILURE;
		}
	stYST.bTransmit[nCh] = TRUE;

	return SUCCESS;
}

VOID StopYSTCh(U32 nCh)
{
	stYST.bTransmit[nCh] = FALSE;
	if(gp.bNeedYST)
	{
		JVN_StopChannel(nCh+1);
	}
}

BOOL StartMOServer(U32 nCh)
{
	//�����ֱ�Ϊ:ͨ��,�Ƿ���,��������,�Ƿ񵥶�����,�Ƿ����ֻ���ʽ
	if(gp.bNeedYST)
	{
		if(JVN_EnableMOServer(nCh+1, TRUE, 0, TRUE, FALSE, 1024*1024))
		{
			Printf("EnableMOServer ok...\n");
		}
	}

	return TRUE;
}

BOOL StopMOServer(U32 nCh)
{
	//�����ֱ�Ϊ:ͨ��,�Ƿ���,��������,�Ƿ񵥶�����,�Ƿ����ֻ���ʽ
	if(gp.bNeedYST)
	{
		if(JVN_EnableMOServer(nCh+1, FALSE, 0, FALSE, FALSE, 0))
		{
			Printf("DisableMOServer ok...\n");
		}
	}

	return TRUE;
}

//��ȡ����ͨ����
YST* GetYSTParam(YST* pstYST)
{
	if(pstYST)
	{
		memcpy(pstYST, &stYST, sizeof(YST));
	}

	return &stYST;
}

//��������ͨ����
VOID SetYSTParam(YST* pstYST)
{
	if(pstYST)
	{
		memcpy(&stYST, pstYST, sizeof(YST));
	}
}

//������Եķ�ʽ���������ݡ�����ʧ�ܵ����
void MT_TRY_SendChatData(int nChannel,int nClientID,unsigned char uchType,unsigned char *pBuffer,int nSize)
{
	BOOL bSuccess;
	int cnt = 0;
	if(!gp.bNeedYST)
		return;
	while(cnt++ < 10)
	{
		bSuccess = JVN_SendChatData(nChannel, nClientID, uchType, pBuffer, nSize);
		//printf("After send: %d\n", bSuccess);
		//break;
		if (bSuccess)
			break;
		Printf("Trying... : %d\n", cnt);
		usleep(20*1000);
	}
	//Printf("After Send\n");
}

