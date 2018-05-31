#ifndef _UTL_IFCONFIG_H_
#define _UTL_IFCONFIG_H_

#include "iwlist.h"

#define 	PACKET_SIZE 4096
#define 	ROUTE_LINE_SIZE 77

/*������������*/
#define   ETH_NET			0x01
#define   PPPOE_NET			0x02
#define   WIFI_NET			0x03
#define   PPPOE_LINKING		0X04
#define   WIFI_LINKING		0X05
#ifdef __cplusplus
extern "C" {
#endif

//��̫������
typedef struct 
{
	char name[12];            //������
	int  bDHCP;               //��̫���Զ���ȡ�����ֶ����ñ�־λ
	char addr[16];              //IP��ַ
	char	mask[16];
	char gateway[16];		  
	char mac[20];             //MAC��ַ����Ӵ���Ϣ��ֹ�е�·����������MAC��ַ��IP��ַ��
	char dns[16];
}eth_t;


//pppoe����
typedef struct 
{
	char name[12];
	
	char username[32];
	char passwd[32];
}pppoe_t;

typedef enum
{
	WIFI_MODE_STA=1,
	WIFI_MODE_AP=2
}wifi_mode_e;

typedef enum
{
	WIFI_MODEL_UNKNOWN,
	WIFI_MODEL_REALTEK8188,
	WIFI_MODEL_REALTEK8192,
	WIFI_MODEL_RALINK7601,
	WIFI_MODEL_SCI9083,
	WIFI_MODEL_M88WI6700,

	WIFI_MODEL_NONE,
}WifiModelType;


/**
 *@brief ��ȡָ�������豸����IP��Ϣ
 *
 *@param ifname �����豸������eth0/ra0
 *@param attr IP��Ϣ
 *
 */
int utl_ifconfig_build_attr(char *ifname, eth_t *attr, BOOL bRefresh);

//����ϵ�������AP�ȵ��б�
wifiap_t* utl_ifconfig_wifi_power_list_ap();

/**
 *@brief ��ȡWIFI�ȵ��б�
 *
 *@param bResearch �Ƿ�ǿ������ɨ��
 *
 *@return 0 �ɹ���< 0 ʧ�� 
 *
 */
wifiap_t *utl_ifconfig_wifi_list_ap(int bResearch);

//��ȡWIFI�ȵ��б�
wifiap_t *utl_ifconfig_wifi_get_ap();

int utl_ifconfig_wifi_bsupport();

/*
 * @brief ����APģʽ
 */
int utl_ifconfig_wifi_start_ap();

/*
 * @brief ����STAģʽ
 */
int utl_ifconfig_wifi_start_sta();

/**
 *@brief ����wifi�ȵ��б�����
 *
 *@param list Ҫ������б�
 *
 *@return �ȵ��������
 *
 */
int utl_ifconfig_wifi_list_cnt(wifiap_t *list);
/**
 * @brief ����wifi�������ļ�         ԭ������wifi_connect��ߣ����Ǳ����ļ�֮����������Ҫ����
 *                             ���Ե����ó�����
 * @param apҪ���ӵ�ap��Ϣ
 * @return 0�ɹ�
 */
int utl_ifconfig_wifi_conf_save(wifiap_t *ap);
/**
 * @brief ɾ��wifi�������ļ�
 * @param
 * @return 0�ɹ�  -1ʧ��
 */
int utl_ifconfig_wifi_save_remove();

//���Ŀǰʹ������wifiģ��
WifiModelType utl_ifconfig_wifi_get_model();

//���Ŀǰwifi��ʹ��ģʽ
wifi_mode_e utl_ifconfig_wifi_get_mode();

//�任wifi��ʹ��ģʽ
int utl_ifconfig_wifi_mode_change();

//�任APģʽǰ��ʹ��STA��������
void utl_ifconfig_wifi_power_search();

/**
 *@brief ����ָ���ȵ�
 *
 *@return 0 �ɹ���< 0 ʧ�� 
 */
int utl_ifconfig_wifi_connect(wifiap_t *ap);

/**
 *@brief ����SSID��ȡ�ȵ�ṹ��
 *
 *
 */
wifiap_t *utl_ifconfig_wifi_get_by_ssid(char *ssid);

/**
 *@brief ��ȡ���ӵ��ȵ�
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int utl_ifconfig_wifi_get_current(wifiap_t *wifiap);

/**
 *@brief ����ETHERNET������
 *
 *@return 0 �ɹ���< 0 ʧ�� 
 */
int utl_ifconfig_eth_set(eth_t *eth);

/**
 *@brief ��ȡETHERNET������
 *
 *@return 0 �ɹ���< 0 ʧ�� 
 */
int utl_ifconfig_eth_get(eth_t *eth);

/**
 *@brief ��ȡwifi������
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int utl_ifconfig_wifi_get(eth_t *wifi);

//�洢�û����õ�wifi��Ϣ
int utl_ifconfig_wifi_info_save(wifiap_t *ap);

//��ȡ�û����õ�wifi��Ϣ
int utl_ifconfig_wifi_info_get(wifiap_t *ap);

//���wifi STA��IP��ַ
void utl_ifconfig_wifi_sta_ip_clear();

//wifi WPS
void utl_ifconfig_wifi_WPS_start();

//smart connect
void utl_ifconfig_wifi_smart_connect();

//smart close
void utl_ifconfig_wifi_smart_connect_close(BOOL bBlock);

//get wether received wifi info
BOOL utl_ifconfig_wifi_smart_get_recvandsetting();

// ���������ȵ㣬���ap�еİ�ȫģʽ����������Ϊ-1���������������ӣ�����ֱ������
// bInConfig��ʾ�Ƿ���WiFi�����У�TRUE��ʾ�������ã�����ֹͣ���ú�����ʧ���������õĲ���
// ����ֵ��0���ɹ���<0��ʧ��
int utl_ifconfig_wifi_connect_ap(wifiap_t* ap, BOOL bInConfig);

/**
 *@brief ����PPPOE������
 *
 *@return 0 �ɹ���< 0 ʧ�� 
 */
int utl_ifconfig_ppp_set(pppoe_t *ppp);

/**
 *@brief ��ȡPPPOE�����Ӳ���
 *
 *@return 0 �ɹ���< 0 ʧ�� 
 */
int utl_ifconfig_ppp_get(pppoe_t *ppp);

/**
 *@brief ��ȡ�����״̬
 *@param iface ������
 *
 *@return 1 ���ӳɹ���  0 ����ʧ��
 *
 */
int utl_ifconfig_check_status(char *iface);

/**
 *@brief ���õ�ǰʹ�õ�����
 *
 *@param name ��ǰʹ�õ���������ƣ�eth0/ra0/ppp0
 *
 *@return 0 �ɹ���< 0 ʧ�� 
 *
 */
//int utl_ifconfig_set_current(char *name);

/**
 *@brief ��ȡ��ǰʹ�õ���������
 *
 *@param name ��ǰʹ�õ���������ͣ�static/dhcp/ppp/wifi
 *
 *@return ����inetָ�� 
 *
 */
char *utl_ifconfig_get_inet(char *inet);

char *utl_ifconfig_get_iface(char *iface);

/**
 *@brief ��������Ƿ�׼����.��δ����
 *
 *@return 0 δ׼����
 *@return >0 ��׼����
 */
int utl_ifconfig_net_prepared(void);

void net_init(unsigned int macBase);
void net_deinit();
void net_check_destroy();

/**
 *@brief �ȴ�����׼���ã���Ҫ��DHCPʱ��IP��ַ
 *
 *@param timeoutSec ��ʱʱ�䣬��λΪ��
 *
 *@return 0 �ɹ��� -1 δ�ܳɹ��ȵ�
 */
int utl_ifconfig_waiting_network_prepare(int timeoutSec);

/**
 *@brief �����Ƿ�Ͽ�
 */
BOOL utl_ifconfig_b_linkdown(const char *if_name);

int __bNetCard_Exist(char *ifname,int bAll);

//�豸�Ƿ�֧������
BOOL utl_ifconfig_bSupport_ETH_check();

BOOL utl_ifconfig_bsupport_apmode();

BOOL utl_ifconfig_bsupport_smartlink();

// �豸�Ƿ�������sta
int utl_ifconfig_wifi_STA_configured();

#ifdef __cplusplus
}
#endif

#endif

