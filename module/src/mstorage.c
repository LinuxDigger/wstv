

#include "jv_common.h"
#include "mlog.h"
#include "mrecord.h"

#include "mstorage.h"

static STORAGE stStorage;

#define MMC_DEV		"/dev/mmcblk%d"
#define MMC_PART	"/dev/mmcblk%dp%d"

//��ȡ����������Ϣ����������strResult��
static S32 GetCmdResult(char* strCmd, char* strResult, U32 nSize)
{
	if(strResult)
	{
		FILE *fd;
		U32 ch,i;

		if((fd = popen(strCmd, "r")) == NULL)
		{
			Printf("ERROR: get_system(%s)", strCmd);
			return -1;
		}
		for(i=0; i<nSize;i++)//���ֽ�ȡ���ݣ��˴���Ҫ�Ż�,lck20111227
		{
			if((ch = fgetc(fd)) != EOF)
			{
				strResult[i] = (char)ch;
			}
			else break;
		}
		pclose(fd);
	}
    return 0;
}

//��ʽ��SD��
S32 mstorage_format(U32 nDisk)
{
	char	strCmd[128]={0};

	//ֹͣ¼�񣬽��豸�ó���
	mrecord_stop(0);
	int j = 0;
	char strDev[128] = {0};
	utl_system("umount -l "MOUNT_PATH);
	for(j=0; j<MAX_DEV_NUM; j++)
	{
		sprintf(strDev , MMC_DEV, j);
		if(access(strDev, F_OK)==0)
		{
			break;
		}
	}

	if (!hwinfo.bEMMC)
	{
		//��������ʽ��
		sprintf(strCmd , "fdisk "MMC_DEV, j);
		FILE *fd=popen(strCmd , "w");
		if(fd == NULL)
		{
			mlog_write("Format Storage Failed");
			return -1;
		}

		//��ɾ������
		fputs("o\n", fd);
		//����part1����
		fputs("n\n p\n 1\n \n \n t\n c\n" , fd);

		//���޸�д�����
		fputs("w\n", fd);
		pclose(fd);

		// д�������󣬻ᴥ���Զ����أ�����ڸ�ʽ��֮ǰ�����Զ����أ��ᵼ�¸�ʽ��ʧ��
		// ���ԣ�������ʱһ�£����Զ����������ж��
		sleep(2);
		utl_system("umount -l "MOUNT_PATH);
		
		//��ʽ������
		sprintf(strCmd , "mkdosfs -F 32 "MMC_PART" -s 128", nDisk, 1);
		utl_system(strCmd);
	}
	else
	{
		//��ʽ������
		sprintf(strCmd , "mkdosfs -F 32 "MMC_PART" -s 128", nDisk, 7);
		utl_system(strCmd);
	}

	mlog_write("Format Storage OK");
	return 0;
}

/***************************************************************
*����	:�ж�ָ��·���ռ��Ƿ���nReserved
*����	:1.strPath��Ҫ�жϵ�·��
*		 2.nReserved��Ҫ��ʣ��Ŀռ��С����λ��MB
*����ֵ	:�ռ䲻���ʾӲ����������TRUE,Ӳ�̲�������FALSE
****************************************************************/
static BOOL IsDiskFull(char *strPath , U32 nReserved)
{
	struct statfs statDisk;

	if(statfs(strPath, &statDisk) == 0)
	{
		if(statDisk.f_blocks >0)
		{
			float fFreeSize=statDisk.f_bfree*(statDisk.f_bsize/1024.0/1024.0);
	        if((U32)fFreeSize > nReserved)
	        {
	        	return FALSE;
	        }
	    }
	}

	return TRUE;
}

static VOID GetPathInfo(char *strPath , U32* nTotalSpace, U32 *nFreeSpace)
{
	//����п�ָ����ֱ���˳�
	if(!strPath||!nTotalSpace||!nFreeSpace) return ;

	struct statfs statDisk;
	if(statfs(strPath, &statDisk) == 0)
	{
		#if 1	//MB
			float fBlockSize = statDisk.f_bsize/1024.0/1024.0;
			*nTotalSpace	= statDisk.f_blocks * fBlockSize;
			*nFreeSpace		= statDisk.f_bfree * fBlockSize;
		#else	//B
			*nTotalSpace	= statDisk.f_blocks * statDisk.f_bsize;
			*nFreeSpace		= statDisk.f_bfree * statDisk.f_bsize;
		#endif
	}
}

static S32 _mstorage_refresh()
{
	int i, j;
	char strDev[MAX_PATH];
	BOOL bSdHasPartFlag = FALSE;
	
	for(j=0; j<MAX_DEV_NUM; j++)
	{
		sprintf(strDev , MMC_DEV, j);
		if(access(strDev, F_OK)==0)
		{
			break;
		}
	}
	
	if(STG_USING == stStorage.nStatus
		|| STG_IDLE == stStorage.nStatus)
	{
		GetPathInfo(MOUNT_PATH, &stStorage.nPartSpace[stStorage.nCurPart], &stStorage.nFreeSpace[stStorage.nCurPart]);
		Printf("Storage using, return, space:%d, free:%d...\n", 
			stStorage.nPartSpace[stStorage.nCurPart], stStorage.nFreeSpace[stStorage.nCurPart]);
		return 0;
	}

	i = 0;
	if (hwinfo.bEMMC)
	{
		i = 6;	// EMMC�ĵ�7�������洢
	}
	for(; i<PARTS_PER_DEV; i++)
	{
		sprintf(strDev , MMC_PART, j, i+1);
		if(!access(strDev , F_OK))//�����������
		{
			bSdHasPartFlag = TRUE;
			break;
		}
	}

	if(i == PARTS_PER_DEV)//�������������
	{
		if(-1 != mstorage_format(j))
		{
			int nPart = 1;
			if (hwinfo.bEMMC)
			{
				nPart = 7;
			}
			sprintf(strDev , MMC_PART, j, nPart);
			bSdHasPartFlag= TRUE;
		}	
	}
	
	if(bSdHasPartFlag)
	{
		if (access(MOUNT_PATH, F_OK))
			utl_system("mkdir -p "MOUNT_PATH);

		char tmp_buf[64] = {0};
		snprintf(tmp_buf, sizeof(tmp_buf), "mount %s %s", strDev, MOUNT_PATH);
		if(0 == utl_system(tmp_buf))
		{
			stStorage.mounted = TRUE;
			stStorage.nStatus = STG_IDLE;
			GetPathInfo(MOUNT_PATH, &stStorage.nPartSpace[i], &stStorage.nFreeSpace[i]);
			Printf("nPart%d, TotalSpace:%d, FreeSpace:%d\n", i+1, stStorage.nPartSpace[i], stStorage.nFreeSpace[i]);
		}
		else
		{
			stStorage.nPartSpace[i] = FALSE;
			Printf("mount dev:%s failed, errno: %s\n", strDev, strerror(errno));
		}

	}

	return 0;
}

//��ȡ�洢����Ϣ
S32 mstorage_mount()
{
	char strDev[MAX_PATH]={0};
	char strCmd[MAX_PATH]={0};
	char strResult[MAX_PATH*10]={0};
	char *pStart;
	U32 i,j;

	memset(&stStorage, 0, sizeof(STORAGE));
	stStorage.nStatus = STG_NONE;

	for(i=0; i<MAX_DEV_NUM; i++)
	{
		sprintf(strDev , MMC_DEV, i);
		if(access(strDev, F_OK)==0)
		{
			Printf("Disk FOUND:%s\n", strDev);
			break;
		}

	}
	if(i == MAX_DEV_NUM)
	{
		stStorage.nocard=TRUE;
		Printf("No Disk\n");
		return -1;
	}

	if (hwinfo.bEMMC)
	{
		// EMMCʹ�õ�7�������д洢
		i = 7;
		snprintf(strDev, sizeof(strDev), "/dev/mmcblk0p7");
	}
	
	//ʹ��fdisk��ȡ���̵���Ϣ
	sprintf(strCmd , "fdisk %s -l" , strDev);
	GetCmdResult(strCmd, strResult, MAX_PATH*10);

	//���������ӡ��Ϣ��ȡӲ�̵������Ϣ
	if((pStart = strstr(strResult, "B,")))//��ȡ����������,��λ:�ֽ�
	{
		U64 ulSize;
		sscanf(pStart+strlen("B,"), "%lld", &ulSize);
		stStorage.nSize = ulSize*1.0/1024/1024;
		Printf("Disk Size:%d MB\n" , stStorage.nSize);
	}

	if((pStart = strstr(strResult, "sectors/track,")))//��ȡ����������
	{
		pStart += strlen("sectors/track,");
		sscanf(pStart, "%d", &stStorage.nCylinder);
		Printf("nTotalCylinder:%d\n", stStorage.nCylinder);
	}

	for(j=0;j<10;j++)               //gyd20120405 ж��ʱ��Ҫ�ȴ�ж�سɹ�
	{
		utl_system("umount -l "MOUNT_PATH);
		break;
	}
	stStorage.nStatus = STG_NOFORMAT;
	_mstorage_refresh();
	//��¼���سɹ��ķ���
	stStorage.nCurPart	= i;
    stStorage.nocard=FALSE;
	return 0;
}

#define MAX_DEL_FILE_NUM	6

int compare(const void *a, const void *b)
{
	char *str1 = (char *)a;
	char *str2 = (char *)b;

	return strcmp(str1+1, str2+1);
}

//FTW�Ļص������������������Ŀ¼�Լ�������ļ�
static S32 FTWProc(const char *file, const struct stat* sb, int flag)
{
	char acFile[MAX_DEL_FILE_NUM+1][32]={{0}};
	char acTarget[MAX_PATH]={0};
	char delFile[MAX_PATH] = {0};
	int count = 0;
	int hadDelNum = 0;
	DIR *dir;
	struct dirent *ptr;
	char cmd[128] = {0};
	int i;

	Printf("FTWProc: %s\n", file);

	dir = opendir(file);

	//����򲻿�Ŀ¼��ֱ��ɾ��
	if(NULL == dir)
	{
		//remove(file);
		sprintf(delFile,"rm -rf %s",file);
		utl_system(delFile);
		Printf("Not dir, remove...\n");
		return -1;
	}

	//�ҳ������MAX_DEL_FILE_NUM���ļ�(����Ч�ļ�)
	while((ptr = readdir(dir)) != NULL)
	{
		//����.��..����Ŀ¼
		if(strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, ".."))
		{
			if( (strstr(ptr->d_name, ".jpg") || strstr(ptr->d_name, ".mp4") || strstr(ptr->d_name, ".jdx")) && 
				(ptr->d_name[0]==REC_NORMAL || ptr->d_name[0]==REC_DISCON || ptr->d_name[0]==REC_TIME || 
				ptr->d_name[0]==REC_MOTION || ptr->d_name[0]==REC_ALARM || ptr->d_name[0]==REC_IVP) )
			{
				strcpy(acFile[count++], ptr->d_name);
				qsort(acFile, count, 32*sizeof(char), compare);
				if(count > MAX_DEL_FILE_NUM)
					count = MAX_DEL_FILE_NUM;
			}
			else
			{
				sprintf(acTarget, "%s/%s", file, ptr->d_name);
				snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", acTarget);
				utl_system(cmd);
				Printf("remove target(invalid):%s\n", acTarget);
				hadDelNum++;
				if(hadDelNum >= MAX_DEL_FILE_NUM)
					break;
			}
		}
	}
	closedir(dir);

	for(i=0; i<count&&hadDelNum<MAX_DEL_FILE_NUM; i++,hadDelNum++)
	{
		sprintf(acTarget, "%s/%s", file, acFile[i]);
		snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", acTarget);
		utl_system(cmd);
		Printf("remove target(oldest):%s\n", acTarget);
	}

	if(hadDelNum < MAX_DEL_FILE_NUM)
	{
		snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", file);
		utl_system(cmd);
		Printf("remove target(empty):%s\n", file);
		hadDelNum++;
	}

	Printf("FTWProc: remove %d file(s) or dir(s)\n", hadDelNum);
	return 0;
}

//�Զ����������������ڵ�¼���ļ�
static S32 AutoCleanup()
{
	char acFile[MAX_PATH]={0};
	char acDir[MAX_PATH]={0};
	BOOL bFindDir = FALSE;
	int year, mon, day;

	DIR *pDir	= opendir(MOUNT_PATH);
	if (pDir == NULL)
	{
		Printf("open dir [%s] error \n",MOUNT_PATH);
		return -1;
	}
	struct dirent *pDirent	= NULL;
	while((pDirent=readdir(pDir)) != NULL)
	{
		//����.��..����Ŀ¼
		if(strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, ".."))
		{
			bFindDir = TRUE;
			
			if(hwinfo.bHomeIPC)
			{//���ò�Ʒֻɾ��TF���е�¼���ļ��������ļ�����
				if(sscanf(pDirent->d_name, "%04d%02d%02d", &year, &mon, &day) == 3)
				{
					if(0 == acFile[0] || strcmp(acFile, pDirent->d_name) > 0)
					{
						strcpy(acFile, pDirent->d_name);				
					}
				}
			}
			else
			{	//���������������������Ŀ¼������ɾ��
				if(sscanf(pDirent->d_name, "%04d%02d%02d", &year, &mon, &day) != 3)
				{
					strcpy(acFile, pDirent->d_name);		
					break;
				}
				else if(0 == acFile[0] || strcmp(acFile, pDirent->d_name) > 0)
				{
					strcpy(acFile, pDirent->d_name);
				}	
			}
		}
	}
	closedir(pDir);

	//���������Ŀ¼���������Ŀ¼
	if(bFindDir)
	{
		Printf("Find dir:%s\n", acFile);
		sprintf(acDir, "%s/%s", MOUNT_PATH, acFile);
		return FTWProc(acDir, NULL, DT_DIR);
	}
	else
	{
		Printf("AutoCleanup not find dir...\n");
		return -1;
	}

	return 0;
}

/**
 *@brief ��ʼ��
 *
 */
int mstorage_init(void)
{
	mstorage_mount();
	// ����ɾ���ƴ洢Ŀ¼(��ʱ�������豸����/�ϵ���ش�)
	char alarmPath[32];
	char cmd[128];
	snprintf(alarmPath, sizeof(alarmPath), "%salarm", MOUNT_PATH);
	if(access(alarmPath, F_OK) == 0)
	{
		snprintf(cmd, sizeof(cmd), "rm -rf %s", alarmPath);
		utl_system(cmd);
	}
	return 0;
}

/**
 *@brief ����
 *
 */
int mstorage_deinit(void)
{
	//ж�ط���
	umount(MOUNT_PATH);
	stStorage.nStatus = STG_NONE;
	return 0;
}

/**
 *@brief ��ȡ�洢��״̬
 *
 */
int mstorage_get_info(STORAGE *storage)
{
	int ret;
	memset(storage, 0, sizeof(STORAGE));
	if (!stStorage.mounted)
	{
		ret = mstorage_mount();
		if (ret != 0)
			return ret;
	}
	else
	{
		int i;
		char strDev[256];
		for(i=0; i<PARTS_PER_DEV; i++)
		{
			sprintf(strDev , MMC_PART, 0, i+1);
			if(!access(strDev , F_OK))//�����������
			{
				GetPathInfo(MOUNT_PATH, &stStorage.nPartSpace[i], &stStorage.nFreeSpace[i]);
			}
		}

	}
	memcpy(storage, &stStorage, sizeof(stStorage));
	return 0;
}


/**
 *@brief ��ȡ¼�񱣴��·��
 *
 *
 */
void mstorage_get_basepath(char *szPath)
{
	sprintf(szPath, "%s", MOUNT_PATH);
	strcat(szPath, "/record/");
	return ;
}

/**
 *@brief ����ʹ��״̬
 *
 */
int mstorage_enter_using()
{
	char acDev[MAX_PATH]={0};

	if(stStorage.mounted
			&& STG_NOFORMAT != stStorage.nStatus && stStorage.nCurPart >= 0)
	{
		stStorage.nEntryCount++;
		stStorage.nStatus = STG_USING;
	}
	return 0;
}

/**
 *@brief �뿪ʹ��״̬
 *
 */
int mstorage_leave_using()
{
	stStorage.nEntryCount--;
	if(stStorage.nEntryCount <= 0)
	{
		stStorage.nEntryCount = 0;
		stStorage.nStatus = STG_IDLE;
	}
	return 0;
}

// ��ȡ���̹���·����pBuffer����ΪNULL������ʱ������������
const char* mstorage_get_partpath(U32 DiskNoUse, U32 PartNoUse, char* pBuffer, U32 nSize)
{
	static char path[256] = "";

	if (NULL == pBuffer)
	{
		pBuffer = path;
		nSize = sizeof(path);
	}

	snprintf(pBuffer, nSize, MOUNT_PATH);

	return pBuffer;
}

const char* mstorage_get_cur_recpath(char* strPath, int nSize)
{
	return mstorage_get_partpath(0, 0, strPath, nSize);
}

/**
 *@brief Ҫ���㹻�Ŀռ�
 *@param size ��Ҫ��Ŀռ��С����M�ֽ�Ϊ��λ
 *@note ���û���㹻�Ŀռ䣬�����������Զ���������¼���¼
 *
 *@return ʵ��ʣ��ռ�Ĵ�С
 *
 */
int mstorage_allocate_space(int size)
{
	int nTimes = 10;

	if(stStorage.nStatus == STG_NONE)
	{
		return -2;
	}

	while(IsDiskFull(MOUNT_PATH, size))
	{
		if(0 != AutoCleanup())
		{
			Printf("AutoCleanup failed, nTimes=%d...\n", nTimes);
			return -1;
		}
	}
	return 0;
}

