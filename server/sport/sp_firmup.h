/*
 * sp_firmup.h
 *
 *  Created on: 2015-10-10
 *      Author: Qin Lichao
 */

#ifndef SP_FIRMUP_H_
#define SP_FIRMUP_H_
#include "sp_define.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief ��ȡ��ǰ�豸�Ƿ���Ҫ����
 *
 *@version ���°汾��
 *
 *@return 0:����Ҫ������1: ��Ҫ������< 0 ��ȡʧ��
 */
int sp_firmup_update_check(char *version);

/**
 *@brief ��ʼ����
 *
 *@method ������ʽ: http, ftp, usb
 *
 *@url ftp��ʽ������Ҫ�����ļ�·��
 *
 *@return 0: �����ɹ���-1: ����ʧ��
 */
int sp_firmup_update(const char *method, const char *url);

/**
 *@brief ��ȡ��ǰ����״̬
 *
 *@phase ��ǰ�����׶�: download, erase, write
 *
 *@progress ���Ȱٷֱ�: 0-100
 *
 *@return 0: ��ȡ�ɹ���< 0 ��ȡʧ��
 */
int sp_firmup_update_get_progress(char *phase, int *progress);

#ifdef __cplusplus
}
#endif


#endif /* SP_FIRMUP_H_ */
