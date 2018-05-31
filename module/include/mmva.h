/**
 *@file		Сά�˶���Ƶ����
 *@brief		���߼������ƶ����ٵ�
 *@author	Sunyingliang
 */
#ifndef _MMVA_H_
#define _MMVA_H_
#include "jv_common.h"

typedef enum
{
    E_MMVA_MODE_NULL = 0,	//�����
    E_MMVA_MODE_COUNT ,	//����
    E_MMVA_MODE_REGIN_INOUT ,		//��������
    E_MMVA_MODE_LINE_INOUT ,		//��������
    
}E_MMVA_MODE;

typedef enum
{
    E_MMVA_DIRECT_NULL = 0,	
    E_MMVA_DIRECT_L2R ,	//
    E_MMVA_DIRECT_R2L ,	
    E_MMVA_DIRECT_U2D ,	
    E_MMVA_DIRECT_D2U ,	
    E_MMVA_DIRECT_I2O ,	
    E_MMVA_DIRECT_O2I ,	
    
}E_MMVA_DIRECT;

#ifndef MIN
   #define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
   #define MAX(x,y) ((x)>(y)?(x):(y))
#endif


int mmva_init(void);
int mmva_start( MIVP_t *Mivp);
int mmva_stop(void);
int mmva_count_in_get(void);
int mmva_count_out_get(void);
int mmva_count_reset();
//���Ʋ���
int mmva_push_param(MIVP_t *mivp);
/*ע�ᱨ���ص�*/
int mmva_register_alarmcallbk( ON_IVP_ALARM callbk);
void  mmva_set_sensitivity(int nSensitivity);

void  mmva_set_alarmtime(int ntime);

#endif
