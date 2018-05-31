#include "sp_define.h"
#include "sp_ptz.h"
#include "mptz.h"
#include <math.h>
#include <SYSFuncs.h>
#include "MRemoteCfg.h"
#include "muartcomm.h"
#include "mstream.h"
#include "msensor.h"

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
int sp_ptz_move_start(int channel, int left, int up, int zoomin)
{
	__FUNC_DBG__();
	BOOL bLeft, bUp;

	if (left == 0 && up == 0)
	{
		if (zoomin > 0 && zoomin <= 255)
		{
			PtzZoomInStart(channel);			
		}
		else if (zoomin >= -255 && zoomin < 0)
		{
			PtzZoomOutStart(channel);
		}
		else if (0 == zoomin)
		{
			PtzZoomOutStop(channel);
		}
		else
		{
			printf("param zoomin error!");
			return -1;
		}
		return 0;
	}


	if (left >= 0 && left <= 255)
	{
		bLeft = TRUE;
	}
	else if (left >= -255 && left < 0)
	{
		bLeft = FALSE;
	}
	else
	{
		printf("param left error!");
		return -1;
	}

	if (up >= 0 && up <= 255)
	{
		bUp = TRUE;
	}
	else if (up >= -255 && up < 0)
	{
		bUp = FALSE;
	}
	else
	{
		printf("param up error!");
		return -1;
	}
	PtzPanTiltStart(channel, bLeft, bUp, abs(left), abs(up));

	return 0;
}

/**
 *@brief ֹͣ�ƶ��������ƶ���
 *
 *@param channel ͨ����
 */
int sp_ptz_move_stop(int channel)
{

	__FUNC_DBG__();
	PtzPanTiltStop(channel);
	return 0;
}

/**
 *@brief ��Ȧ���ƣ��佹����
 *
 *@param channel ͨ����
 *@param focusFar ��Զ�ۼ����ٶȣ���Χ-255~255
 *@param irisOpen �����Ȧ���ٶȣ���Χ-255~255
 */
int sp_ptz_fi_start(int channel, int focusFar, int irisOpen)
{
	
	__FUNC_DBG__();
	if (focusFar > 0 && focusFar < 255)
	{
		PtzFocusFarStart(channel);
	}
	else if (focusFar < 0 && focusFar > -255)
	{
		PtzFocusNearStart(channel);
	}
	else if (irisOpen > 0 && irisOpen < 255)
	{
		PtzIrisOpenStart(channel);
	}
	else if (irisOpen < 0 && irisOpen > -255)
	{
		PtzIrisCloseStart(channel);
	}
	else
	{
		printf("param irisOpen error!");
		return -1;
	}
	return 0;
}

/**
 *@brief ��Ȧ����
 *
 *@param channel ͨ����
 */
int sp_ptz_fi_stop(int channel)
{
	
	__FUNC_DBG__();
	PtzFocusNearStop(channel);

	PtzIrisOpenStop(channel);
	return 0;
}

/**
 *@brief ��ȡ���е�Ԥ�õ��λ
 */
int sp_ptz_get_free_presetno(int channel)
{
	PTZ_PRESET_INFO *pptz_preset_info;
	pptz_preset_info = PTZ_GetPreset();
	int i;
	int no;
	for (no=1;no<MAX_PRESET_CT;no++)
	{
		for (i=0;i<pptz_preset_info[channel].nPresetCt;i++)
		{
			if (no == pptz_preset_info[channel].pPreset[i])
			{
				break;
			}
		}
		if (i == pptz_preset_info[channel].nPresetCt)
		{
			return no;
		}
	}

	return -1;
}

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
int sp_ptz_preset_set(int channel, int presetno, char *name)
{
	
	__FUNC_DBG__();
	if (presetno == -1)
	{
		presetno = sp_ptz_get_free_presetno(channel);
	}
	int ret = PTZ_AddPreset(channel, presetno, name);
	if (ret < 0)
		return ret;
	WriteConfigInfo();
	return presetno;
}

/**
 *@brief ����Ԥ��λ
 *
 *@param channel ͨ����
 *@param presetno Ԥ��λ��
 */
int sp_ptz_preset_locate(int channel, int presetno)
{
	
	__FUNC_DBG__();
	PtzLocatePreset(channel, presetno);
	return 0;
}

/**
 *@brief ɾ��Ԥ��λ
 *
 *@param channel ͨ����
 *@param presetno Ԥ��λ��
 *@return 0�ɹ�
 *			 -1Ҫɾ����Ԥ�úŲ�����
 */
int sp_ptz_preset_delete(int channel, int presetno)
{
	
	__FUNC_DBG__();
	int ret = PTZ_DelPreset(channel, presetno);
	WriteConfigInfo();

	return ret;
}

/**
 *@brief ��ȡ���ù���Ԥ�õ�ĸ���
 *
 *@param channel ͨ����
 */
int sp_ptz_preset_get_cnt(int channel)
{
	__FUNC_DBG__();
	PTZ_PRESET_INFO *pptz_preset_info;
	pptz_preset_info = PTZ_GetPreset();
	return pptz_preset_info[channel].nPresetCt;
}

/**
 *@brief ��ȡԤ�õ���Ϣ
 *
 *@param channel ͨ����
 *@param index ���
 *@param preset ��� Ԥ�õ���Ϣ
 */
int sp_ptz_preset_get(int channel, int index, SPPreset_t *preset)
{
	__FUNC_DBG__();
	PTZ_PRESET_INFO * pptz_preset_info;
	pptz_preset_info = PTZ_GetPreset();

	preset->presetno = pptz_preset_info[channel].pPreset[index];
	strcpy(preset->name, pptz_preset_info[channel].name[index]);

	return 0;
}

/**
 *@brief ����һ��Ѳ��
 *
 *@param channel
 *
 *@return index of patrol
 */
int sp_ptz_patrol_create(int channel)
{
	return 0;
}

/**
 *@brief ����һ��Ѳ��
 */
int sp_ptz_patrol_delete(int channel, int index)
{
	if(index < 0 || index > 1)
	{
		printf("only support 2 patrolpath	:%d\n",index);
		return -1;
	}
	SPPatrol_t patrol;
	patrol.patrolid = index;
	int ret = sp_ptz_patrol_del_node(channel, &patrol, -1);
	if(ret <0 )
		printf("sp_ptz_patrol_del_node error 	:%d\n", patrol.patrolid);
	return 0;
}

/**
 *@brief ��ȡѲ���߸�������ǰֻ��һ��
 */
int sp_ptz_patrol_get_cnt(int channel)
{
	return 1;
}

/**
 *@brief ��ȡѲ����
 *
 *@param index
 *@param patrol
 *
 *@return 0 if success
 */
int sp_ptz_patrol_get(int channel, int index, SPPatrol_t *patrol)
{
	patrol->patrolid = 0;
	return 0;
}

/**
 *@brief ��ȡѲ���ߵ�Ԥ�õ�����
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 */
int sp_ptz_patrol_get_node_cnt(int channel, SPPatrol_t *patrol) //patrolid,û�õ�
{
	__FUNC_DBG__();
	if(patrol->patrolid < 0 || patrol->patrolid > 1)
	{
		printf("only support 2 patrolpath	:%d\n",patrol->patrolid);
		return -1;
	}
	//printf("sp_ptz_patrol_get_node_cnt: 	patid:%d\n", patrol->patrolid);
	PTZ_PATROL_INFO *pptz_patrol_info;
	pptz_patrol_info = PTZ_GetPatrol();
	return pptz_patrol_info[patrol->patrolid].nPatrolSize;
}

/**
 *@brief ��ȡѲ���ߵ�Ԥ�õ�
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param index ���
 */
int sp_ptz_patrol_get_node(int channel, SPPatrol_t *patrol, int index,
		SPPatrolNode_t *node) //patrolid,û�õ�
{
	if(patrol->patrolid < 0 || patrol->patrolid > 1)
	{
		printf("only support 2 patrolpath	:%d\n",patrol->patrolid);
		return -1;
	}
	int nPreset;
	int i = 0;
	PTZ_PATROL_INFO *pptz_patrol_info;
	PTZ_PRESET_INFO * pptz_preset_info;
	__FUNC_DBG__();
	//printf("sp_ptz_patrol_get_node: 	patid:%d\n", patrol->patrolid);
	pptz_patrol_info = PTZ_GetPatrol();
	if (index > pptz_patrol_info[patrol->patrolid].nPatrolSize)
	{
		printf("overtop PatroSize!");
		return -1;
	}
	nPreset = pptz_patrol_info[patrol->patrolid].aPatrol[index].nPreset;

	pptz_preset_info = PTZ_GetPreset();

	node->staySeconds = pptz_patrol_info[patrol->patrolid].aPatrol[index].nStayTime;
	node->preset.presetno = pptz_patrol_info[patrol->patrolid].aPatrol[index].nPreset;
	for(i=0; i<pptz_preset_info[channel].nPresetCt; i++)
	{
		if(pptz_preset_info[channel].pPreset[i] == node->preset.presetno)
			strcpy(node->preset.name, pptz_preset_info[channel].name[i]);
	}

	return 0;
}

/**
 *@brief ����һ��Ѳ����
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param presetno Ԥ��λ��
 *@param staySeconds ͣ��������
 */
int sp_ptz_patrol_add_node(int channel, SPPatrol_t *patrol, int presetno,
		int staySeconds) //patrolid,û�õ�
{
	if(patrol->patrolid < 0 || patrol->patrolid > 1)
	{
		printf("only support 2 patrolpath	:%d\n",patrol->patrolid);
		return -1;
	}
    //printf("+++1    patid:%d  presetno:%d  staytime:%d\n", patrol->patrolid, presetno, staySeconds);
	__FUNC_DBG__();
	PTZ_PRESET_INFO * pptz_preset_info;
	PTZ_PATROL_INFO *pptz_patrol_info;
	int i, ret;
	pptz_preset_info = PTZ_GetPreset();
	for (i = 0; i < pptz_preset_info[channel].nPresetCt; i++)
	{
		if (presetno == pptz_preset_info[channel].pPreset[i])
		{
			break;
		}
	}
	if (i == pptz_preset_info[channel].nPresetCt)
	{
		printf("can not find!");
		return -1;
	}

	pptz_patrol_info = PTZ_GetPatrol();
    //printf("+++2    patid:%d  [i]:%d  staytime:%d\n", patrol->patrolid, presetno, staySeconds);
	ret = AddPatrolNod(&pptz_patrol_info[patrol->patrolid], presetno, staySeconds);

	WriteConfigInfo();
	if (ret == -1)
	{
		printf("overtop MAX_PATROL_NOD");
		return -1;
	}

	return 0;
}

/**
 *@brief ɾ��һ��Ѳ����
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param presetindex Ԥ��λ��Ѳ�����е������š� presetindexΪ-1ʱ��ɾ�����ϵ�����Ԥ�õ�
 */
int sp_ptz_patrol_del_node(int channel, SPPatrol_t *patrol, int presetindex) //patrolid,û�õ�
{
	if(patrol->patrolid < 0 || patrol->patrolid > 1)
	{
		printf("only support 2 patrolpath	:%d\n",patrol->patrolid);
		return -1;
	}
    //printf("---1   patid:%d  presetno:%d\n", patrol->patrolid, presetindex);
	__FUNC_DBG__();
	PTZ_PATROL_INFO *pptz_patrol_info;
	int i, ret;
	PTZ_PRESET_INFO * pptz_preset_info;
	pptz_preset_info = PTZ_GetPreset();
	pptz_patrol_info = PTZ_GetPatrol();

	if (presetindex == -1)
	{
		for (i = pptz_patrol_info[patrol->patrolid].nPatrolSize - 1; i >= 0; i--)
		{
			DelPatrolNod(&pptz_patrol_info[patrol->patrolid], i);
		}
		WriteConfigInfo();
		return 0;
	}

	/*
	for (i = 0; i < pptz_preset_info[channel].nPresetCt; i++)
	{
		if (presetindex == pptz_preset_info[channel].pPreset[i])
		{
			break;
		}
	}
	if (i == pptz_preset_info[channel].nPresetCt)
	{
		printf("can not find in preset!");
		return -1;
	}
	
	for (i=0; i < pptz_patrol_info[patrol->patrolid].nPatrolSize; i++)
	{
		if (presetindex == pptz_patrol_info[patrol->patrolid].aPatrol[i].nPreset)
		{
			break;
		}
	}
	if (i == pptz_patrol_info[patrol->patrolid].nPatrolSize)
	{
		printf("can not find in patrol!");
		return -1;
	}
	*/
	ret = DelPatrolNod(&pptz_patrol_info[patrol->patrolid], presetindex);
	if (ret == -1)
	{
		printf("delete failed!");
		return -1;
	}
	WriteConfigInfo();

	return 0;
}

/**
 *@brief ����Ѳ���ٶ�
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param speed �ٶȣ�ȡֵ 0~255
 */
int sp_ptz_patrol_set_speed(int channel, SPPatrol_t *patrol, int speed)
{
	__FUNC_DBG__();
	return 0;
}

/**
 *@brief ����Ѳ��ͣ��ʱ�䡣����ı�����Ԥ�õ��ͣ��ʱ��
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 *@param staySeconds ͣ��������
 */
int sp_ptz_patrol_set_stay_seconds(int channel, SPPatrol_t *patrol, int staySeconds)
{
	if(patrol->patrolid < 0 || patrol->patrolid > 1)
	{
		printf("only support 2 patrolpath	:%d\n",patrol->patrolid);
		return -1;
	}
	__FUNC_DBG__();
	PTZ_PATROL_INFO *pptz_patrol_info;
	int i, presetindex, ret;
	//printf("sp_ptz_patrol_set_stay_seconds:  patid:%d\n", patrol->patrolid);
	pptz_patrol_info = PTZ_GetPatrol();
	for (i = 0; i < pptz_patrol_info[patrol->patrolid].nPatrolSize; i++)
	{
		pptz_patrol_info[patrol->patrolid].aPatrol[i].nStayTime = staySeconds;
	}
	WriteConfigInfo();
	return 0;
}

/**
 *@brief ��ʼѲ��
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 */
int sp_ptz_patrol_locate(int channel, SPPatrol_t *patrol)
{
	if(patrol->patrolid < 0 || patrol->patrolid > 1)
	{
		printf("only support 2 patrolpath	:%d\n",patrol->patrolid);
		return -1;
	}
	
	__FUNC_DBG__();
	PTZ_StartPatrol(channel, patrol->patrolid);
	return 0;
}

/**
 *@brief ֹͣѲ��
 *
 *@param channel ͨ����
 *@param patrolid Ѳ���ߺ�
 */
int sp_ptz_patrol_stop(int channel, SPPatrol_t *patrol)
{
	__FUNC_DBG__();
	PTZ_StopPatrol(channel);
	return 0;
}

/**
 *@brief ��ʼ¼�ƹ켣
 *
 *@param channel ͨ����
 *@param nTrail 
 */
int sp_ptz_trail_rec_start(int channel, int nTrail)
{
	
	__FUNC_DBG__();
	PtzTrailStartRec(channel,nTrail);
	return 0;
}

/**
 *@brief ֹͣ¼�ƹ켣
 *
 *@param channel ͨ����
 *@param nTrail 
 */
int sp_ptz_trail_rec_stop(int channel, int nTrail)
{
	
	__FUNC_DBG__();
	PtzTrailStopRec(channel,nTrail);
	return 0;
}

/**
 *@brief ��ʼ�켣
 *
 *@param channel ͨ����
 *@param nTrail 
 */
int sp_ptz_trail_start(int channel, int nTrail)
{
	
	__FUNC_DBG__();
	PtzTrailStart(channel,nTrail);
	return 0;
}

/**
 *@brief ֹͣ�켣
 *
 *@param channel ͨ����
 *@param nTrail 
 */
int sp_ptz_trail_stop(int channel, int nTrail)
{
	
	__FUNC_DBG__();
	PtzTrailStop(channel,nTrail);
	return 0;
}

/**
 *@brief ����ɨ����߽�
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 */
int sp_ptz_scan_set_left(int channel, int groupid)
{
	
	__FUNC_DBG__();
	PtzLimitScanLeft(channel);
	return 0;
}

/**
 *@brief ����ɨ���ұ߽�
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 */
int sp_ptz_scan_set_right(int channel, int groupid)
{
	
	__FUNC_DBG__();
	PtzLimitScanRight(channel);
	return 0;
}

/**
 *@brief ��ʼɨ��
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 */
int sp_ptz_scan_start(int channel, int groupid)
{
	
	__FUNC_DBG__();
	PTZ *ptz = PTZ_GetInfo();
	PtzLimitScanSpeed(channel, 0, ptz[0].scanSpeed);
	PtzLimitScanStart(channel, groupid);
	return 0;
}
/**
 *@brief ����ɨ��
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 */
int sp_ptz_scan_stop(int channel, int groupid)
{
	
	__FUNC_DBG__();
	PtzLimitScanStop(channel, groupid);
	return 0;
}

/**
 *@brief ����ɨ���ٶ�
 *
 *@param channel ͨ����
 *@param groupid ɨ�����
 *@param speed ɨ���ٶȣ�0~255
 */
int sp_ptz_scan_set_speed(int channel, int groupid, int speed)
{
	
	__FUNC_DBG__();
	PtzLimitScanSpeed(channel, groupid, speed);
	return 0;
}

int sp_ptz_auto(int channel, unsigned int speed)
{
	
	PTZ_Auto(channel, speed);
	__FUNC_DBG__();
	return 0;
}

int sp_ptz_aux_on(int channel, int n)
{
	
	PtzAuxAutoOn(channel, n);
	__FUNC_DBG__();
	return 0;
}

int sp_ptz_aux_off(int channel, int n)
{
	
	PtzAuxAutoOff(channel, n);
	__FUNC_DBG__();
	return 0;
}

/**
 *@brief ��̨�˵�����
 *
 *@param channel ͨ����
 *@param n:0-on,1-off,2-ok,3-return;4-up,5-down,6-left,7-right
 */
int sp_ptz_menu_op(int channel, int n)
{
	
	__FUNC_DBG__();
	return 0;
}
/**
 *@brief 3D��λ����
 *
 *@param :channel ͨ����
 *@param :pos Ȧ���������꼰�����Ϣ��cmd �Ŵ�/��С����
 */
int sp_ptz_position(int channel, SPPosition_t *pos, int cmd)
{
	
	__FUNC_DBG__();
	return 0;	
}

