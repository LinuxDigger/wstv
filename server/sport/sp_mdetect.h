/*
 * sp_mdetect.h
 *
 *  Created on: 2013-11-15
 *      Author: lfx
 */

#ifndef SP_MDETECT_H_
#define SP_MDETECT_H_

#include "sp_define.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	int		bEnable;			//�Ƿ����ƶ����
	int		bEnableRecord;			//�Ƿ����ƶ����¼��
	int		nSensitivity;		//������, 0 ~ 255֮��
	int		nRectNum;			//�ƶ����������������Ϊ4��0��ʾȫ������
	SPRect_t stRect[4];

	int		nDelay;
	int		bOutClient;
	int		bOutEMail;
}SPMdetect_t;

typedef void (*sp_mdetect_callback_t)(int channelid, void *param);
/**
 *@brief ���ò���
 *@param channelid Ƶ����
 *@param md Ҫ���õ����Խṹ��
 *@note �������ȷ���������Ե�ֵ������#mdetect_get_param��ȡԭ����ֵ
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int sp_mdetect_set_param(int channelid, SPMdetect_t *md);

/**
 *@brief ��ȡ����
 *@param channelid Ƶ����
 *@param md Ҫ���õ����Խṹ��
 *@return 0 �ɹ���-1 ʧ��
 *
 */
int sp_mdetect_get_param(int channelid, SPMdetect_t *md);

/**
 *@brief ����Ƿ������ƶ���ⱨ��
 *
 *@return Ϊ�棬���ʾ���ƶ���ⱨ������֮��û��
 */
BOOL sp_mdetect_balarming(int channelid);

void sp_StopMotionDetect();


#ifdef __cplusplus
}
#endif


#endif /* SP_MDETECT_H_ */
