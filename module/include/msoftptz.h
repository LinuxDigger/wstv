/*
 * msoftptz.h
 *
 *  Created on: 2014��8��20��
 *      Author: lfx
 */

#ifndef MSOFTPTZ_H_
#define MSOFTPTZ_H_

#include <jv_common.h>
#include <jv_ptz.h>

#define FIRSTPOS_FILE CONFIG_PATH"firstpos.dat"
#define PATROL_FLAG_FLAG CONFIG_PATH"patrol.dat"

typedef struct patrol_info
{
	int chan;
	int patrol_path;
}PATROL_INFO;

int msoftptz_init();

int msoftptz_deinit();

/**
 *@brief ���ӿ�ʵ�ֵ���̨���ƣ��Ƿ�֧��
 */
BOOL msoftptz_b_support(int channel);

/**
 *@brief ���ӿ�ʵ�ֵ���̨�����Զ���ת���Ƿ�֧���Զ�
 */

BOOL msoftptz_auto_support(int channel);

/**
 *@brief �Ƿ���Ҫ�Լ�
 */
BOOL msoftptz_b_need_selfcheck();

/**
 *@brief ʵ����̨���ƶ�������
 *
 *@param channel ͨ����
 *@param left �����ƶ����ٶȣ���Χ-255~255
 *@param up �����ƶ����ٶȣ���Χ-255~255
 *@param zoomin �Ŵ���ٶȣ���Χ-255~255
 *
 *@note ����ֵ <0 ��ʾ�෴���� ��Ϊ0�����ʾֹͣ
 */
int msoftptz_move_start(int channel, int left, int up, int zoomin);

/**
 *@brief �Զ�Ѳ��
 */
int msoftptz_move_auto(int channel, int speed);

/**
 *@brief ֹͣ�ƶ��������ƶ���
 *
 *@param channel ͨ����
 */
int msoftptz_move_stop(int channel);

/**
 *@brief ���þ���ĳ������
 *@param position �������õ�λ��
 *
 *@return 0 if success
 */
int msoftptz_goto(int channel, JVPTZ_Pos_t *position);

int msoftptz_setalarmPreSpeed_flag();

/**
 *@brief ��ȡ��ǰ����
 *@param position �������õ�λ��
 *
 *@return 0 if success
 */
int msoftptz_pos_get(int channel, JVPTZ_Pos_t *position);

/**
 *@brief ��ȡ�ϴ����ù����ٶ�
 		�ֶ�������Ѳ�������ܸı���ٶȣ�
 		�����������뱾�ε������һ�����ù����ٶ�
*@return �ٶ�
 */
int msoftptz_speed_get(int channel);
//�ͻ���Ȧ�����򣬽���3D��λ�����Ŵ���С��ʾ
//(x, y, w, h) Ȧ�������������꼰���; (width, height)��ǰ�����ֱ���
//zoom  3D��λָ��:	0xC0 �Ŵ�0xC1 ��С
int  msoftptz_ZoomZone(int x, int y, int w, int h, int width, int height,int zoom);



#endif /* MSOFTPTZ_H_ */
