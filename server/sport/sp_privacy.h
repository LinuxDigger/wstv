/*
 * sp_privacy.h
 *
 *  Created on: 2013-11-18
 *      Author: LK
 */

#ifndef SP_PRIVACY_H_
#define SP_PRIVACY_H_

#include "sp_define.h"

#define MAX_PYRGN_NUM		8		//�ڵ��������privacy

#ifdef __cplusplus
extern "C" {
#endif

//��Ƶ�ڵ�����
typedef struct
{
	BOOL	bEnable;
	SPRect_t	stRect[MAX_PYRGN_NUM];
}SPRegion_t;

/**
 *@brief ��ȡ����
 *@param channelid ͨ����
 *@param region ��Ƶ�ڵ��������
 *
 *@return 0 ���ߴ����
 *
 */
int sp_privacy_get_param(int channelid, SPRegion_t *region);

/**
 *@brief ���ò���
 *@param channelid ͨ����
 *@param region ��Ƶ�ڵ��������
 *
 *@return 0 ���ߴ����
 *
 */
int sp_privacy_set_param(int channelid, SPRegion_t *region);

#ifdef __cplusplus
}
#endif

#endif /* SP_PRIVACY_H_ */
