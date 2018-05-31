#ifndef IO_CTRL_H_
#define IO_CTRL_H_

typedef enum
{
	DEV_ST_NET_NONE,
	DEV_ST_ETH_OK,
	DEV_ST_ETH_DISCONNECT,
	DEV_ST_WIFI_OK,
	DEV_ST_WIFI_DISCONNECT,
	DEV_ST_WIFI_CONNECTED,
	DEV_ST_WIFI_SETTING,
	DEV_ST_WIFI_CONNECTING,
	DEV_ST_WIFI_CONNECT_OFF,
}DEV_ST_NET;

typedef enum
{
	DEV_ST_RECORD_OFF,
	DEV_ST_RECORD_ON,
}DEV_ST_RECORD;

typedef enum
{
	DEV_ST_CHAT_FREE,
	DEV_ST_CHAT_READY_1,
	DEV_ST_CHAT_READY_2,
	DEV_ST_CHAT_START,
	DEV_ST_CHAT_ONGOING,
	DEV_ST_CHAT_CHATING,
	DEV_ST_CHAT_STOP,
	DEV_ST_CHAT_STOPPING,
}DEV_ST_CHAT;

typedef enum
{
	DEV_ST_LIGHT_ALARM_OFF,
	DEV_ST_LIGHT_ALARM_ON,
}DEV_ST_LIGHT_ALARM;

typedef struct
{
	DEV_ST_NET stNet;
	DEV_ST_RECORD stRec;
	DEV_ST_LIGHT_ALARM stLightAlarm;
}DevStatus_e;

#define GPIO_I2C_FM1188_WRITE		0x01
#define GPIO_I2C_FM1188_READ	    0x03
#define GPIO_I2C_FM1188_TEST		0x06

typedef struct
{
    unsigned short reg;
    unsigned short data;
}FM1188Type;

/**
 *@brief ��ʼ��
 *
 *@return 0
 */
int mio_init(void);

/**
 *@brief ����
 *
 *@return 0
 */
int mio_deinit(void);

/**
 *@brief �����豸��ǰ������״̬
 *@param status �豸��ǰ������״̬
 *
 *@return 0
 */
int mio_set_net_st(DEV_ST_NET stNet);
//��ȡ��ǰ����״̬
DEV_ST_NET mio_get_get_st();


/**
 *@brief ������ɣ���ʼ�Խ�������״̬
 *
 *@return 0
 */
int mio_chat_start();

/**
 *@brief �ֻ��˾ܽӣ�����״̬����ʾ
 *
 *@return 0
 */
int mio_chat_refuse();

/**
 *@brief �����жϣ������쳣�����⵼��
 *
 *@return 0
 */
int mio_chat_interrupt();

/**
 *@brief �ֻ��˽����Խ�������״̬
 *
 *@return 0
 */
int mio_chat_stop();

/**
 *@brief �����豸��ǰ��¼��״̬
 *@param status ¼��״̬
 *
 *@return 0
 */
int mio_set_record_st(DEV_ST_RECORD stRec);

int mio_getboardext_status(int ext_type);

int mio_dev_LedSet(int ledMode,int blink);

/**
 *@brief �����Ƿ�reset
 *@param flag reset״̬
 *
 *@return 0
 */
int mio_set_reset_flag(int flag);

/**
 *@brief ��ȡreset״̬
 *
 *@return reset״̬
 */
int mio_get_reset_flag();

int FM1188Init();

/**
 *@brief �����豸�ⱨ��������״̬
 *@param status �ⱨ��������״̬
 *
 *@return 0
 */
int mio_set_light_alarm_st(DEV_ST_LIGHT_ALARM stLightAlarm);
/**
  *@brief ��ȡ��ǰ�ⱨ������״̬
  *@return ����״̬ 
  */
DEV_ST_LIGHT_ALARM mio_get_light_alarm_st();

#endif
