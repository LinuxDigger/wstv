/*
 * sp_storage.c
 *
 *  Created on: 2013-11-18
 *      Author: Administrator
 */
#include <jv_common.h>
#include "sp_storage.h"
#include "mstorage.h"

/**
 *@brief ��ȡ�洢��״̬
 *
 *@param �洢����Ϣ�ṹ���
 *@return 0 �ɹ�
 */
int sp_storage_get_info(SPStorage_t *storage)
{
	STORAGE mstorage;
	mstorage_get_info(&mstorage);

	storage->mounted = mstorage.mounted;
	storage->nCurPart = mstorage.nCurPart;
	storage->nCylinder = mstorage.nCylinder;
	storage->nEntryCount = mstorage.nEntryCount;
	memcpy(storage->nFreeSpace, mstorage.nFreeSpace, PARTS_PER_DEV);
	storage->nPartSize = mstorage.nPartSize;
	memcpy(storage->nPartSpace, mstorage.nPartSpace, PARTS_PER_DEV);
	storage->nPartition = mstorage.nPartition;
	storage->nSize = mstorage.nSize;
	storage->nStatus = mstorage.nStatus;
	storage->nocard = mstorage.nocard;

	return 0;
}

/**
 * @brief ��ʽ��sd��
 * @param ndisk
 *
 * return 0 �ɹ�
 */
int sp_storage_format(U32 nDisk)
{
	mstorage_format(nDisk);
	return mstorage_mount();
}

