#ifndef __MWDT_H__
#define __MWDT_H__

#include "jv_common.h"
#include "jv_wdt.h"
/**
 *@brief �򿪿��Ź�
 *@note ���������Ȱ�װ
 *@return �ɹ������ش򿪵��豸�����ʧ�ܷ��� -1
 *
 */
HWDT OpenWatchDog();

/**
 *@brief �رտ��Ź�
 *@param S32 iDog �Ѵ򿪵��豸���
 */
void CloseWatchDog();

/**
 *@brief ι��
 *@param S32 iDog �Ѵ򿪵��豸���
 */
void FeedWatchDog();

#endif /* __MWDT_H__ */
