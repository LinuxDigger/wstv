/*
 * mvoicedec.h
 *
 *  Created on: 2014��9��22��
 *      Author: lfx
 *  ���ļ�����������Ƶ����WIFI�Ĺ���
 */

#ifndef MVOICEDEC_H_
#define MVOICEDEC_H_

/*��Ƶ��������л���24k������ ��ʹ����������WIFI���ܣ���ʱ�������ܱ�����*/
int mvoicedec_enable(void);

/*��Ƶ��������л���8k������ ����ֹ��������WIFI���ܣ���ʱ�������Ա�����*/
int mvoicedec_disable(void);

int mvoicedec_init(void);
//������ʱ����ʱֵ���͵��㲥�У��ֻ���ͨ�����ֵȷ���豸
int Start_timer_for_count();

BOOL CheckbAppendSearchPrivateInfo();

BOOL isVoiceSetting();

//�����������Ƿ��յ�����������Ϣ
BOOL isVoiceRecvAndSetting();


#endif /* MVOICEDEC_H_ */
