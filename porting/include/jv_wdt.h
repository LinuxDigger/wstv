/*
 *
 * Copyright 2007-2008 VN, Inc.  All rights reserved.
 *
 */

#ifndef __WDT_H__
#define __WDT_H__
#include "jv_common.h"

typedef S32 HWDT;
/**
 *@brief �򿪿��Ź�
 *@note ���������Ȱ�װ
 *@return �ɹ������ش򿪵��豸�����ʧ�ܷ��� -1
 *
 */
HWDT jv_open_wdt();

/**
 *@brief �رտ��Ź�
 *@param S32 iDog �Ѵ򿪵��豸���
 */
void jv_close_wdt(HWDT iDog);

/**
 *@brief ι��
 *@param S32 iDog �Ѵ򿪵��豸���
 */
void jv_feed_wdt(HWDT iDog);

#endif /* __WDT_H__ */

