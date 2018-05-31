#ifndef _JV_AO_H_
#define _JV_AO_H_

#include <jv_ai.h>
typedef struct {
	unsigned int					aoBufTotalNum;    /* total number of channel buffer */
	unsigned int					aoBufFreeNum;	   /* free number of channel buffer */
	unsigned int					aoBufBusyNum;	   /* busy number of channel buffer */
}jv_ao_status;
/**
 *@brief ����Ƶ���
 *
 *@param channelid ��Ƶͨ����
 *@param attr ��Ƶͨ�������ò���
 *
 *@return 0 �ɹ�
 *
 */
int jv_ao_start(int channelid ,jv_audio_attr_t *attr);

/**
 *@brief �ر���Ƶ���
 *
 *@param channelid ��Ƶͨ����
 *
 *@return 0 �ɹ�
 *
 */
int jv_ao_stop(int channelid);

/**
 *@brief ����һ֡��Ƶ����
 *
 *@param channelid ��Ƶͨ����
 *@param frame ��Ƶ֡
 *
 *@return 0 �ɹ��� -1 ʧ��
 *
 */
int jv_ao_send_frame(int channelid, jv_audio_frame_t *frame);
/**
 *@brief��ȡ��ǰ��ռ�õĻ������
 *
 *@param channelid ��Ƶͨ����
 *@param aoStatus AO����ռ��״̬�ṹ��
 *
 *@return  -1 :ʧ�ܣ�0:�ɹ�
 *
 */

int jv_ao_get_status(int channelid, jv_ao_status *aoStatus);

void jv_ao_adec_end();

/**
 *@brief ��Ƶ����Ƿ���
 *
 *@param channelid ��Ƶͨ����
 *@param mute �Ƿ���
 *
 *@return 0 �ɹ��� <0 ʧ��
 *
 */
int jv_ao_mute(BOOL bMute);

int jv_ao_get();


int jv_ao_ctrl(int volCtrl);

int jv_ao_get_attr(int channelid, jv_audio_attr_t *attr);

#endif

