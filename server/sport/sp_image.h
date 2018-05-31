/*
 * sp_image.h
 *
 *  Created on: 2013-11-20
 *      Author: Administrator
 */

#ifndef SP_IMAGE_H_
#define SP_IMAGE_H_

#include "sp_define.h"

#define EFFECT_AWB				0		//�Զ���ƽ��
#define EFFECT_MIRROR			0x01	//����--�����ҷ�ת
#define EFFECT_TURN				0x02	//��ת--���·�ת
#define EFFECT_NOCOLOR			0x03	//�ڰ�ģʽ

typedef enum
{
	SP_SCENE_INDOOR,				//����
	SP_SCENE_OUTDOOR,				//����
	SP_SCENE_DEFAULT,				//Ĭ��
	SP_SCENE_SOFT					//���
}SPScene_e;
typedef enum{
	SP_SENSOR_DAYNIGHT_AUTO,  		//�Զ�
	SP_SENSOR_DAYNIGHT_ALWAYS_DAY,	//һֱ����
	SP_SENSOR_DAYNIGHT_ALWAYS_NIGHT,	//һֱҹ��
	SP_SENSOR_DAYNIGHT_TIMER,		//��ʱ����
}SPMSensorDaynightMode_e;	//��ҹģʽ

typedef struct _IMAGE_ADJUST
{
	unsigned int contrast; 		//�Աȶ� 0-255
	unsigned int brightness; 	//���� 0-255
	unsigned int saturation; 	//���Ͷ�0-255
	unsigned int sharpen; 		//���0-255

	unsigned int exposureMax; //����ع�ʱ�䡣�ع�ʱ��Ϊ �� 1/exposureMax �룬ȡֵ 3 - 100000
	unsigned int exposureMin;
	SPScene_e scene;			//����
    SPMSensorDaynightMode_e daynightMode;	//��ҹģʽ
    struct{
    	char hour;
    	char minute;
    }dayStart, dayEnd;

	BOOL bEnableAWB;			//�Ƿ��Զ���ƽ��..auto white balance
	BOOL bEnableMI;				//�Ƿ��澵��..mirror image
	BOOL bEnableST;				//�Ƿ��淭ת..screen turn
	BOOL bEnableNoC;			//�Ƿ�ڰ�ģʽ..no color

	BOOL bEnableWDynamic;		//�Ƿ�����̬..wide dynamic
	BOOL bNightOptimization;  //�Ƿ�ҹ���Ż�
	BOOL bAutoLowFrameEn;  //�Ƿ�ҹ���Զ���֡
}SPImage_t;

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief ��ȡ����Աȶ�
 *
 * @param image ����
 * @return 0�ɹ� -1ʧ��
 */
int sp_image_get_param(SPImage_t *image);

/**
 * @brief ���û�����ڲ���
 *
 * @param image �������
 * @return 0�ɹ� -1ʧ��
 */
int sp_image_set_param(SPImage_t *image);

#ifdef __cplusplus
}
#endif


#endif /* SP_IMAGE_H_ */
