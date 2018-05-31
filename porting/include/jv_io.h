#ifndef JV_IO_H_
#define JV_IO_H_

#include "jv_common.h"

typedef enum
{
	IO_LED_RED,
	IO_LED_GREEN,
	IO_LED_BLUE,
	IO_LED_INFRARED,
	IO_LED_WHITE,
	IO_LED_BUTT
}IOLedType_e;

typedef enum
{
	IO_OFF,
	IO_ON,
	IO_BLINK,
}IOStatus_e;

typedef enum{
	IO_KEY_RESET,
	IO_KEY_WPS,
	IO_KEY_WIFI_SET,
	IO_KEY_ONE_MIN_REC,
	IO_KEY_BUTT,
}IOKey_e;

typedef enum{
	DEV_REDBOARD,
	DEV_LIGHTBOARD,
	DEV_WIFI,
	DEV_RESET_KEY,
}IODEV_TYPE;

typedef struct{
	IOKey_e (*fptr_key_get)();
	int (*fptr_led_set)(IOLedType_e type, IOStatus_e status);
	int (*fptr_dev_status)(IODEV_TYPE type);
}jv_io_ctrl_func_t;

extern jv_io_ctrl_func_t g_ioctrlfunc;

/**
 *@brief ��ʼ��
 *
 *@return 0
 */
int jv_io_init(void);

/**
 *@brief ��ȡ���ܼ���״̬
 *
 *@return ���µĹ��ܼ�
 */
static IOKey_e jv_io_key_get()
{
	if (g_ioctrlfunc.fptr_key_get)
		return g_ioctrlfunc.fptr_key_get();
	return IO_KEY_BUTT;
}

/**
 *@brief ����led��״̬
 *@param status led��״̬
 *
 *@return �ɹ�:0  ʧ��:�����
 */
static int jv_io_led_set(IOLedType_e type, IOStatus_e status)
{
	if (g_ioctrlfunc.fptr_led_set)
		return g_ioctrlfunc.fptr_led_set(type, status);
	return JVERR_FUNC_NOT_SUPPORT;
}

static int jv_io_dev_status(IODEV_TYPE type)
{
	if (g_ioctrlfunc.fptr_dev_status)
		return g_ioctrlfunc.fptr_dev_status(type);
	return 0;	
}

#endif
