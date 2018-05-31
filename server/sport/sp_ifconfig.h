/*
 * sp_ifconfig.h
 *
 *  Created on: 2013-11-20
 *      Author: Administrator
 */

#ifndef SP_IFCONFIG_H_
#define SP_IFCONFIG_H_

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
	char name[12];            	//������
	int  bDHCP;               	//��̫���Զ���ȡ�����ֶ����ñ�־λ
	char addr[16];            	//IP��ַ
	char mask[16];				//��������
	char gateway[16];			//����
	char mac[20];           	//MAC��ַ����Ӵ���Ϣ��ֹ�е�·����������MAC��ַ��IP��ַ��
	char dns[16];				//DNS
}SPEth_t;

//pppoe����
typedef struct
{
	char name[12];

	char username[32];
	char passwd[32];
}SPPppoe_t;

//wifi ����
typedef struct
{
	char	name[32];       	//AP��
	char	passwd[64];   		//��ʷ��¼������   ���Ϊ�ձ�ʾ��ʷû�м�¼����ڵ�
	int		quality;	     	//�ź�ǿ�ȣ���ֵ100
	int		keystat;     		//�Ƿ���Ҫ����  -1 ����Ҫ ����ֵ����Ҫ���룬�˴���ʱ��ֻ�жϲ���Ҫ��������
	char	iestat[8];	     	//iestat[0]:��ȫģʽ ��ӦAUTH_TYPE_e��iestat[1]���������ͣ����򣩶�Ӧ ENCODE_TYPE_e
}SPWifiAp_t;

//��������Ϣ
typedef struct
{
	/*VMS*/
	char vmsServerIp[20];	//VMS������IP��ַ
	unsigned short vmsServerPort;		//VMS�������˿�

	/*RTMP*/
	int bRTMPEnable;
	int rtmpChannel;
	char rtmpURL[128];
}SPServer_t;

/**
 *@brief ��ȡ��ǰʹ�õ���������
 *
 *@param name ��ǰʹ�õ���������ͣ�static/dhcp/ppp/wifi
 *
 *@return ����inetָ��
 */
char *sp_ifconfig_get_inet(char *inet);

/**
 *@brief ��ȡ��ǰʹ�õ������豸
 *
 *@param name ��ǰʹ�õ������豸���ƣ�eth0/wlan0
 *
 *@return ����ifaceָ��
 */
char *sp_ifconfig_get_iface(char *iface);

/**
 *@brief ��ȡETHERNET������
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int sp_ifconfig_eth_get(SPEth_t *eth);

/**
 *@brief ����ETHERNET������----���ñ�������
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int sp_ifconfig_eth_set(SPEth_t *eth);

/**
 *@brief ��ȡPPPOE�����Ӳ���
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int sp_ifconfig_ppp_get(SPPppoe_t *ppp);

/**
 *@brief ����PPPOE������
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int sp_ifconfig_ppp_set(SPPppoe_t *ppp);

/**
 *@brief ��ȡwifi���Ӳ���
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int sp_ifconfig_wifi_get_current(SPWifiAp_t *wifiap);

/**
 *@brief ��ȡWIFI������
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int sp_ifconfig_wifi_get(char *iface, SPEth_t *spEth);

/**
 *@brief ����ָ���ȵ�
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
int sp_ifconfig_wifi_connect(SPWifiAp_t *ap);

/**
 *@brief ��ȡWIFI�ȵ��б� 
 *
 *@return 0 �ɹ���< 0 ʧ��
 */
SPWifiAp_t *sp_ifconfig_wifi_list_ap();

/**
 *@brief ����wifi�ȵ��б�����
 *
 *@param list Ҫ������б�
 *
 *@return �ȵ��������
 *
 */
int sp_ifconfig_wifi_list_cnt(SPWifiAp_t *list);

//����豸���õ�STA��Ϣ
int sp_ifconfig_wifi_info_get(SPWifiAp_t *ap);

//����wifi STAģʽ
int sp_ifconfig_wifi_start_sta();

//����wifi APģʽ
int sp_ifconfig_wifi_start_ap();

//�������³�ʼ��
void sp_ifconfig_net_deinit();

/*��ȡ���������ã�����VMS, RTMP�ȷ������ĵ�ַ��Ϣ��*/
int sp_ifconfig_server_get(SPServer_t *serverInfo);

/*���������ã�����VMS, RTMP�ȷ������ĵ�ַ��Ϣ��*/
int sp_ifconfig_server_set(SPServer_t *serverInfo);

/**
 *@brief ���ݸ�����IP��ַ������Ӧ����һ��ͬ���ε�IP��ַ
 *
 *@param otherip IP��ַ����Դ��Ŀǰ�������ͻ�������ʱ��IP
 *
 *@return 0
 */
int sp_ifconfig_audo_ip(const char *otherip);



#ifdef __cplusplus
}
#endif

#endif /* SP_IFCONFIG_H_ */
