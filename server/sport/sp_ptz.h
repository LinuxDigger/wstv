/*
 * sp_ptz.h
 *
 *  Created on: 2013-11-1
 *      Author: lfx
 */

#ifndef SP_PTZ_H_
#define SP_PTZ_H_

#ifdef __cplusplus
extern "C" {
#endif

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
int sp_ptz_move_start(int channel, int left, int up, int zoomin);

/**
 *@brief ֹͣ�ƶ��������ƶ���
 *
 *@param channel ͨ����
 */
int sp_ptz_move_stop(int channel);

/**
 *@brief ��Ȧ���ƣ��佹����
 *
 *@param channel ͨ����
 *@param focusFar ��Զ�ۼ����ٶȣ���Χ-255~255
 *@param irisOpen �����Ȧ���ٶȣ���Χ-255~255
 */
int sp_ptz_fi_start(int channel, int focusFar, int irisOpen);

/**
 *@brief ��Ȧ����
 *
 *@param channel ͨ����
 */
int sp_ptz_fi_stop(int channel);

/**
 *@brief ��ȡ���е�Ԥ�õ��λ
 */
int sp_ptz_get_free_presetno(int channel);

/**
 *@brief ����Ԥ��λ
 *
 *@param channel ͨ����
 *@param presetno Ԥ��λ�ţ����Ϊ-1�������һ�����õ�
 *@param name Ԥ��λ����
 *@return: >= 0 �ɹ�����ֵ����ΪԤ�õ��
 * 			-1 Ԥ�õ�Ƿ�
 * 			-2 Ԥ�õ��ظ�
 * 			-3 Ԥ�õ�������������
 */
int sp_ptz_preset_set(int channel, int presetno, char *name);

/**
 *@brief ����Ԥ��λ
 *
 *@param channel ͨ����
 *@param presetno Ԥ��λ��
 */
int sp_ptz_preset_locate(int channel, int presetno);

/**
 *@brief ɾ��Ԥ��λ
 *
 *@param channel ͨ����
 *@param presetno Ԥ��λ��
 *@return 0�ɹ�
 *			 -1Ҫɾ����Ԥ�úŲ�����
 */
int sp_ptz_preset_delete(int channel, int presetno);

/**
 *@brief ��ȡ���ù���Ԥ�õ�ĸ���
 *
 *@param channel ͨ����
 */
int sp_ptz_preset_get_cnt(int channel);

typedef struct {
	int presetno;
	char name[32];
} SPPreset_t;
/**
 *@brief ��ȡԤ�õ���Ϣ
 *
 *@param channel ͨ����
 *@param index ���
 *@param preset ��� Ԥ�õ���Ϣ
 */
int sp_ptz_preset_get(int channel, int index, SPPreset_t *preset);

typedef struct{
	int patrolid;
}SPPatrol_t;

/**
 *@brief ����һ��Ѳ��
 *
 *@param channel
 *
 *@return index of patrol
 */
int sp_ptz_patrol_create(int channel);

/**
 *@brief ����һ��Ѳ��
 */
int sp_ptz_patrol_delete(int channel, int index);

/**
 *@brief ��ȡѲ���߸���
 */
int sp_ptz_patrol_get_cnt(int channel);

/**
 *@brief ��ȡѲ����
 *
 *@param channel ͨ����
 *@param index
 *@param patrol
 *
 *@return 0 if success
 */
int sp_ptz_patrol_get(int channel, int index, SPPatrol_t *patrol);

/**
 *@brief ��ȡѲ����
 *
 *@param channel ͨ����
 *@param patrol
 *
 *@return index of patrol
 */
int sp_ptz_patrol_get_index(int channel, SPPatrol_t *patrol);

/**
 *@brief ��ȡѲ���ߵ�Ԥ�õ�����
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 */
int sp_ptz_patrol_get_node_cnt(int channel, SPPatrol_t *patrol);

typedef struct{
	SPPreset_t preset;
	int staySeconds;
}SPPatrolNode_t;

/**
 *@brief ��ȡѲ���ߵ�Ԥ�õ�
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param index ���
 */
int sp_ptz_patrol_get_node(int channel, SPPatrol_t *patrol, int index, SPPatrolNode_t *node);

/**
 *@brief ����һ��Ѳ����
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param presetno Ԥ��λ��
 *@param staySeconds ͣ��������
 */
int sp_ptz_patrol_add_node(int channel, SPPatrol_t *patrol, int presetno, int staySeconds);

/**
 *@brief ɾ��һ��Ѳ����
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param presetindex Ԥ��λ�����š� presetindexΪ-1ʱ��ɾ�����ϵ�����Ԥ�õ�
 */
int sp_ptz_patrol_del_node(int channel, SPPatrol_t *patrol, int presetindex);

/**
 *@brief ����Ѳ���ٶ�
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param speed �ٶȣ�ȡֵ 0~255
 */
int sp_ptz_patrol_set_speed(int channel, SPPatrol_t *patrol, int speed);

/**
 *@brief ����Ѳ��ͣ��ʱ�䡣����ı�����Ԥ�õ��ͣ��ʱ��
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param staySeconds ͣ��������
 */
int sp_ptz_patrol_set_stay_seconds(int channel, SPPatrol_t *patrol, int staySeconds);

/**
 *@brief ��ʼѲ��
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 */
int sp_ptz_patrol_locate(int channel, SPPatrol_t *patrol);

/**
 *@brief ֹͣѲ��
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 */
int sp_ptz_patrol_stop(int channel, SPPatrol_t *patrol);

/**
 *@brief ��ʼ¼�ƹ켣
 *
 *@param channel ͨ����
 *@param nTrail 
 */
int sp_ptz_trail_rec_start(int channel, int nTrail);

/**
 *@brief ֹͣ¼�ƹ켣
 *
 *@param channel ͨ����
 *@param nTrail 
 */
int sp_ptz_trail_rec_stop(int channel, int nTrail);

/**
 *@brief ��ʼ�켣
 *
 *@param channel ͨ����
 *@param nTrail 
 */
int sp_ptz_trail_start(int channel, int nTrail);

/**
 *@brief ֹͣ�켣
 *
 *@param channel ͨ����
 *@param nTrail 
 */
int sp_ptz_trail_stop(int channel, int nTrail);

/**
 *@brief ����ɨ����߽�
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 */
int sp_ptz_scan_set_left(int channel, int groupid);

/**
 *@brief ����ɨ���ұ߽�
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 */
int sp_ptz_scan_set_right(int channel, int groupid);

/**
 *@brief ��ʼɨ��
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 */
int sp_ptz_scan_start(int channel, int groupid);

/**
 *@brief ����ɨ��
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 */
int sp_ptz_scan_stop(int channel, int groupid);

/**
 *@brief ����ɨ���ٶ�
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 *@param speed ɨ���ٶȣ�0~255
 */
int sp_ptz_scan_set_speed(int channel, int groupid, int speed);

/**
 *@brief ������̨�Զ�
 *
 *@param channel ͨ����
 *@param speed �ٶȣ�0~255
 */
int sp_ptz_auto(int channel, unsigned int speed);

/**
 *@brief ��������
 *
 *@param channel ͨ����
 *@param n ��n�������
 */
int sp_ptz_aux_on(int channel, int n);

/**
 *@brief ����ɨ���ٶ�
 *
 *@param channel ͨ����
 *@param n ��n�������
 */
int sp_ptz_aux_off(int channel, int n);

/**
 *@brief ��̨�˵�����
 *
 *@param channel ͨ����
 *@param n:0-on,1-off,2-ok,3-return;4-up,5-down,6-left,7-right
 */
int sp_ptz_menu_op(int channel, int n);


//3D��λ�ṹ��
typedef struct _POSITION
{
	int x;		//�ͻ���Ȧ���������� x����
	int y;		//�ͻ���Ȧ���������� y����
	int w;		//�ͻ���Ȧ�������
	int h;		//�ͻ���Ȧ�������
} SPPosition_t;
/**
 *@brief 3D��λ����
 *
 *@param :channel ͨ����
 *@param :pos Ȧ���������꼰�����Ϣ��cmd �Ŵ�/��С����
 */
int sp_ptz_position(int channel, SPPosition_t *pos, int cmd);

#ifdef __cplusplus
}
#endif



#endif /* SP_PTZ_H_ */
