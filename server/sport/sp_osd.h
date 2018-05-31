/*
 * sp_osd.h
 *
 *  Created on: 2013-11-20
 *      Author: LK
 */

#ifndef SP_OSD_H_
#define SP_OSD_H_

#include "sp_define.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	SP_CHNOSD_POS_LEFT_TOP=0,
	SP_CHNOSD_POS_LEFT_BOTTOM,
	SP_CHNOSD_POS_RIGHT_TOP,
	SP_CHNOSD_POS_RIGHT_BOTTOM,
	SP_CHNOSD_POS_HIDE,
}SPChnOsdPos_e;

/**
 *@brief channel osd status
 */
typedef struct
{
	BOOL				bShowOSD;			///< �Ƿ���ͨ������ʾOSD
	char				timeFormat[32];		///< ʱ���ʽ YYYY-MM-DD hh:mm:ss ���������
	SPChnOsdPos_e		position;			///< OSD��λ�� 0, ���ϣ�1�����£�2�����ϣ�3������
	SPChnOsdPos_e		timePos;			///< OSD��λ�ã�ʱ���λ�� 0, ���ϣ�1�����£�2�����ϣ�3������
	char				channelName[32];	///<ͨ������
	BOOL osdbInvColEn;		//�Ƿ�ɫlk20131218
	BOOL bLargeOSD;			//�Ƿ��ó���OSD
}SPChnOsdAttr_t;

/**
 *@brief ��ȡOSD�Ĳ���
 *
 *@param channelid ͨ����
 *@param attr ���ڴ洢Ҫ��ȡ�����Ե�ָ��
 *
 *@return 0 �ɹ�
 */
int sp_chnosd_get_param(int channelid, SPChnOsdAttr_t *attr);

/**
 *@brief ����OSD�Ĳ���
 *
 *@param channelid ͨ����
 *@param attr Ҫ���õ�����
 *
 *@return 0 �ɹ�
 */
int sp_chnosd_set_param(int channelid, SPChnOsdAttr_t *attr);


#ifdef __cplusplus
}
#endif

#endif /* SP_OSD_H_ */
