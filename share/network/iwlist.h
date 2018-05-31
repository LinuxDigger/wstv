#ifndef _IWLIST_H_
#define _IWLIST_H_

typedef enum
{
	AUTH_TYPE_OPEN,			//wep_open
	AUTH_TYPE_SHARED,		//wep_shared
	AUTH_TYPE_AUTO,			//wep_auto..
	AUTH_TYPE_WPA,			//wpa-psk
	AUTH_TYPE_WPA2,			//wpa2-psk
	AUTH_TYPE_NONE,			//����
	AUTH_TYPE_MAX
}AUTH_TYPE_e;

typedef enum
{
	ENCODE_TYPE_NONE,		//����
	ENCODE_TYPE_WEP,
	ENCODE_TYPE_TKIP,		//wpa
	ENCODE_TYPE_AES,
	ENCODE_TYPE_MAX
}ENCODE_TYPE_e;

typedef struct  
{
	char			name[32];       	//AP��
	char			passwd[16];	   		//��ʷ��¼������   ���Ϊ�ձ�ʾ��ʷû�м�¼����ڵ�
	int				quality;	     	//�ź�ǿ�ȣ���ֵ100
	int				keystat;     		//0����Ҫ���룬��0��Ҫ����
//	AUTH_TYPE_e		auth_type;			//��ȫģʽ֧�����ġ�wpa-psk��wpa2-psk��open��shared
//	ENCODE_TYPE_e 	encode_type;		//�������ͣ����򣩶�Ӧ��ϵ�����ģ�none��wpa-psk,wpa2-psk:TKIP,AES(CCMP);open,shared:WEP
	char			iestat[8];	     	//iestat[0]:��ȫģʽ ��ӦAUTH_TYPE_e��iestat[1]���������ͣ����򣩶�Ӧ ENCODE_TYPE_e
}wifiapOld_t;

typedef struct  
{
	char			name[32];       	//AP��
	char			passwd[64];	   		//��ʷ��¼������   ���Ϊ�ձ�ʾ��ʷû�м�¼����ڵ�
	int				quality;	     	//�ź�ǿ�ȣ���ֵ100
	int				keystat;     		//0����Ҫ���룬��0��Ҫ����
//	AUTH_TYPE_e		auth_type;			//��ȫģʽ֧�����ġ�wpa-psk��wpa2-psk��open��shared
//	ENCODE_TYPE_e 	encode_type;		//�������ͣ����򣩶�Ӧ��ϵ�����ģ�none��wpa-psk,wpa2-psk:TKIP,AES(CCMP);open,shared:WEP
	char			iestat[8];	     	//iestat[0]:��ȫģʽ ��ӦAUTH_TYPE_e��iestat[1]���������ͣ����򣩶�Ӧ ENCODE_TYPE_e
										//iestat[4]:�ź�ǿ�ȣ�8188ģ���quality�������⣬��Ҫ�ô��ֶ���level�����˽ṹ���������ֶΣ�ֻ������˶�Ӧ
}wifiap_t;

//˵��:wifiapOld_tֻ�������ֿش���wifi�б���(�������зֿ�)����������wifiap_t


//��̫������
typedef struct 
{
	int		flags;               //��̫���Զ���ȡ�����ֶ����ñ�־λ
	char	name[12];            //������
	char	Gateway[16];		  
	char	IP[16];              //IP��ַ
	char	Mac[20];             //MAC��ַ����Ӵ���Ϣ��ֹ�е�·����������MAC��ַ��IP��ַ��
	char	DNS[16];			  
	char	Netmask[16];
}ETHERNET;

//pppoe����
typedef struct 
{
	int		flags;
	int		state;
	char	name[12];
	char	Gateway[16];
	char	IP[16];
	char	Mac[20];
	char	DNS[16];
	char	Netmask[16];
	
	char	username[32];
	char	passwd[32];
}PPPOE;

//wifi����
typedef struct
{
	char	name[12];
	int		state;
	char	Gateway[16];
	char	IP[16];
	char	Mac[20];
	char	DNS[16];
	char	Netmask[16];
    int		current_ap;      //��ǰ���ӵ�wifi�ڵ���wifiap�е�λ�� ���㿪ʼ
	wifiap_t	wifiap[100];
}WIFINET;

//ipc ��������
typedef struct 
{
	int			net_alive;        //��ǰ���缤���־����ʼΪ0��ETHERNETΪ0x1��PPPOEΪ0x2��WIFIΪ0x3 

	ETHERNET	stETH;		//��̫��������Ϣ,lck20120529
	ETHERNET	ethernet;    //��̫��
	PPPOE		pppoe;          //pppoe
	WIFINET		wifi;         //wifi

	int			bRunning;
	int			bInThread;
}IPC_NET;

extern IPC_NET ipc_net;


int iwlist_get_ap_list(wifiap_t *aplist, char* wifi, int maxApCnt);

#endif

