
/*	mstorage.h
	Copyright (C) 2011 Jovision Technology Co., Ltd.
	���ļ�������֯�洢�豸������غ���
	������ʷ���svn�汾����־
*/

#ifndef __MSTORAGE_H__
#define __MSTORAGE_H__


//����Ŀ¼��Ӧ�ö����ڴ洢ģ����,lck20120305
#define MOUNT_PATH			"./rec/00/"

#define MAX_DEV_NUM			4
#define PARTS_PER_DEV		16

#define STG_NONE			0	//δ�����豸
#define STG_NOFORMAT		1	//δ��ʽ��
#define STG_FULL			2	//����
#define STG_USING			3	//ʹ����
#define STG_IDLE			4	//���У�Ҳ�ǹ������ˣ�


typedef struct tagSTORAGE
{
	U32		nSize;			//�豸����(MB)
	S32		nCylinder;		//Ӳ��������
	S32		nPartSize;		//ÿ����������
	S32		nPartition;	//���÷�������
	S32		nEntryCount;
	S32		nStatus;
	U32		nCurPart;
	BOOL	nocard;
	U32		nPartSpace[PARTS_PER_DEV];	//�����ܿռ�,MB
	U32		nFreeSpace[PARTS_PER_DEV];	//�������пռ�,MB
	BOOL	mounted;	//�Ƿ��ѹ���
}STORAGE, *PSTORAGE;


/**
 *@brief ��ʼ��
 *
 */
int mstorage_init(void);

/**
 *@brief ����
 *
 */
int mstorage_deinit(void);

/**
 *@brief ��ȡ�洢��״̬
 *
 */
int mstorage_get_info(STORAGE *storage);

/**
 *@brief ��ȡ¼�񱣴��·��
 *
 *
 */
void mstorage_get_basepath(char *szPath);

/**
 *@brief ����ʹ��״̬
 *
 */
int mstorage_enter_using();

/**
 *@brief �뿪ʹ��״̬
 *
 */
int mstorage_leave_using();

// ��ȡ���̹���·����pBuffer����ΪNULL������ʱ������������
const char* mstorage_get_partpath(U32 DiskNoUse, U32 PartNoUse, char* pBuffer, U32 nSize);

const char* mstorage_get_cur_recpath(char* strPath, int nSize);

/**
 *@brief Ҫ���㹻�Ŀռ�
 *@param size ��Ҫ��Ŀռ��С����M�ֽ�Ϊ��λ
 *@note ���û���㹻�Ŀռ䣬�����������Զ���������¼���¼
 *
 *@return ʵ��ʣ��ռ�Ĵ�С
 *
 */
int mstorage_allocate_space(int size);

S32 mstorage_format(U32 nDisk);

S32 mstorage_mount();

#endif

