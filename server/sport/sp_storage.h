/*
 * sp_storage.h
 *
 *  Created on: 2013-11-18
 *      Author: Administrator
 */

#ifndef SP_STORAGE_H_
#define SP_STORAGE_H_

#include "sp_define.h"

#define MAX_DEV_NUM			4
#define PARTS_PER_DEV		16

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	unsigned int		nSize;			//�豸����(MB)
	unsigned int		nCylinder;		//Ӳ��������
	unsigned int		nPartSize;		//ÿ����������
	unsigned int		nPartition;	//���÷�������
	unsigned int		nEntryCount;
	unsigned int		nStatus;
	unsigned int		nCurPart;
	BOOL	nocard;
	unsigned int		nPartSpace[PARTS_PER_DEV];	//�����ܿռ�,MB
	unsigned int		nFreeSpace[PARTS_PER_DEV];	//�������пռ�,MB
	BOOL	mounted;	//�Ƿ��ѹ���
}SPStorage_t;

/**
 *@brief ��ȡ�洢��״̬
 *
 *@param �洢����Ϣ�ṹ���
 *@return 0 �ɹ�
 */
int sp_storage_get_info(SPStorage_t *storage);

/**
 * @brief ��ʽ��sd��
 * @param ndisk
 *
 * return 0 �ɹ�
 */
int sp_storage_format(unsigned int nDisk);

#ifdef __cplusplus
}
#endif

#endif /* SP_STORAGE_H_ */
