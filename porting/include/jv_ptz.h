/*
 * jv_ptz.h
 *
 *  Created on: 2014��8��12��
 *      Author: lfx  20451250@qq.com
 *      Company:  www.jovision.com
 */
#ifndef JV_PTZ_H_
#define JV_PTZ_H_

#include <jv_common.h>

#if 0
#define FUNC_DEBUG() do{printf("%s running\n", __func__);}while(0)
#else
#define FUNC_DEBUG() do{}while(0)
#endif

/**
 *@brief λ����Ϣ
 */
typedef struct{
	unsigned int left;		// left right position. ȡֵΪ 0 �� 0xFFFF
	unsigned int up;		// up down position. ȡֵΪ 0 �� 0xFFFF
}JVPTZ_Pos_t;

typedef struct{
	BOOL bValid; //�Ƿ�֧��
	BOOL bLeftUpTogether; //�Ƿ�֧����������ͬʱ��ת
}JVPTZ_Capability_t;
typedef enum
{
	Horizontal=0,
	Vertical,
	ALL,
}ENUM_LEFT_OR_UP;

/**
 *@brief ��ʼ��
 */
int jv_ptz_init();

/**
 *@brief ����
 */
int jv_ptz_deinit();

typedef struct{
	int (*fptr_jv_ptz_get_capability)(int channel, JVPTZ_Capability_t *capability);
	int (*fptr_jv_ptz_self_check)(int channel);
	int (*fptr_jv_ptz_get_origin_pos)(int channel, JVPTZ_Pos_t* pos);
	int (*fptr_jv_ptz_move_start)(int channel, int left, int up, int zoomin);
	int (*fptr_jv_ptz_move_stop)(int channel);
	int (*fptr_jv_ptz_move_to)(int channel, JVPTZ_Pos_t *position,ENUM_LEFT_OR_UP direction,int speed);
	BOOL (*fptr_jv_ptz_move_done)(int channel);
	int (*fptr_jv_ptz_pos_get)(int channel, JVPTZ_Pos_t *position);
}jv_ptz_func_t;


extern jv_ptz_func_t g_ptzfunc;

/**
 *@brief ���ӿ�ʵ�ֵ���̨���ƣ��Ƿ�֧��
 */
static int jv_ptz_get_capability(int channel, JVPTZ_Capability_t *capability)
{
	FUNC_DEBUG();
	if (g_ptzfunc.fptr_jv_ptz_get_capability)
		return g_ptzfunc.fptr_jv_ptz_get_capability(channel, capability);
	return JVERR_FUNC_NOT_SUPPORT;
}

/**
 *@brief ���ӿ�ʵ�ֵ���̨���ƣ��Ƿ�֧��
 */
static int jv_ptz_self_check(int channel)
{
	FUNC_DEBUG();
	if (g_ptzfunc.fptr_jv_ptz_self_check)
		return g_ptzfunc.fptr_jv_ptz_self_check(channel);
	return JVERR_FUNC_NOT_SUPPORT;
}

/**
 *@brief ���ӿ�ʵ�ֻ�ȡ��̨��ԭ��λ��
 */
static int jv_ptz_get_origin_pos(int channel, JVPTZ_Pos_t* pos)
{
	FUNC_DEBUG();
	if (g_ptzfunc.fptr_jv_ptz_get_origin_pos)
		return g_ptzfunc.fptr_jv_ptz_get_origin_pos(channel, pos);
	return JVERR_FUNC_NOT_SUPPORT;
}

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
static int jv_ptz_move_start(int channel, int left, int up, int zoomin)
{
	FUNC_DEBUG();
	if (g_ptzfunc.fptr_jv_ptz_move_start)
		return g_ptzfunc.fptr_jv_ptz_move_start(channel, left, up, zoomin);
	return JVERR_FUNC_NOT_SUPPORT;
}

/**
 *@brief ֹͣ�ƶ��������ƶ���
 *
 *@param channel ͨ����
 */
static int jv_ptz_move_stop(int channel)
{
	FUNC_DEBUG();
	if (g_ptzfunc.fptr_jv_ptz_move_stop)
		return g_ptzfunc.fptr_jv_ptz_move_stop(channel);
	return JVERR_FUNC_NOT_SUPPORT;
}

/**
 *@brief ���þ���ĳ������
 *@param position �������õ�λ��
 *
 *@return 0 if success
 */
static int jv_ptz_move_to(int channel, JVPTZ_Pos_t *position,ENUM_LEFT_OR_UP direction,int speed)
{
	FUNC_DEBUG();
	if (g_ptzfunc.fptr_jv_ptz_move_to)
		return g_ptzfunc.fptr_jv_ptz_move_to(channel, position,direction,speed);
	return JVERR_FUNC_NOT_SUPPORT;
}

/**
 *@brief ���jv_ptz_mvoe_to ����jv_ptz_move_start�Ƿ����
 */
static BOOL jv_ptz_move_done(int channel)
{
	FUNC_DEBUG();
	if (g_ptzfunc.fptr_jv_ptz_move_done)
		return g_ptzfunc.fptr_jv_ptz_move_done(channel);
	return TRUE;
}

/**
 *@brief ��ȡ��ǰ����
 *@param position �������õ�λ��
 *
 *@return 0 if success
 */
static int jv_ptz_pos_get(int channel, JVPTZ_Pos_t *position)
{
	FUNC_DEBUG();
	if (g_ptzfunc.fptr_jv_ptz_pos_get)
		return g_ptzfunc.fptr_jv_ptz_pos_get(channel, position);
	return JVERR_FUNC_NOT_SUPPORT;
}


#endif /* JV_PTZ_H_ */
