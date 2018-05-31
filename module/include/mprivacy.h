

/*	mprivacy.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯��˽�����ڵ�������غ���
	������ʷ���svn�汾����־
*/
#ifndef __MPRIVACY_H__
#define __MPRIVACY_H__

#define MAX_PYRGN_NUM		8		//�ڵ��������privacy

//��Ƶ�ڵ�����
typedef struct tagREGION
{
	BOOL	bEnable;
	RECT	stRect[MAX_PYRGN_NUM];
}REGION, *PREGION;

/**
 *@brief ��ʼ��
 *@return 0 ���ߴ����
 *
 */
int mprivacy_init(void);

/**
 *@brief ����
 *@return 0 ���ߴ����
 *
 */
int mprivacy_deinit(void);

/*
 * @brief ֹͣ
 */
int mprivacy_stop(int channelid);

/*
 * @brief ��ʼ
 */
int mprivacy_start(int channelid);

/**
 *@brief ��ȡ����
 *@param channelid ͨ����
 *@param region ͨ������
 *
 *@return 0 ���ߴ����
 *
 */
int mprivacy_get_param(int channelid, REGION *region);

/**
 *@brief ���ò���
 *@param channelid ͨ����
 *@param region ͨ������
 *
 *@return 0 ���ߴ����
 *
 */
int mprivacy_set_param(int channelid, REGION *region);

/**
 *@brief ʹ������Ч
 *@param channelid ͨ����
 *
 *@return 0 ���ߴ����
 *
 */
int mprivacy_flush(int channelid);


#endif

