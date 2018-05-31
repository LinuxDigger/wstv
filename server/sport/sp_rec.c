#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <SYSFuncs.h>

#include "sp_define.h"
#include "sp_rec.h"
#include "mplay_remote.h"
#include "mrecord.h"

/**
 * ���Žṹ
 */
typedef struct
{
	FILE* 	fp_ps;             		//ps�ļ���ʶ��
	FILE* 	fp_index;				//��������ļ���ʶ��
//	void 	*fp;
	int 	ret;					//�ļ���ȡ������
	int 	framecount;
	int 	channel_id;				//ͨ��id
	long 	rec_start_time;			//¼�񲥷ſ�ʼʱ��
	long 	rec_end_time;			//¼�񲥷Ž���ʱ��s
} REC_Play;

/**
 *ps�ļ������ṹ
 */
typedef struct
{
	int i;						//h264֡λ��
	int len;					//֡����
	RtpPackType_e frame_type;	//֡����
} psFRAME_INFO;

RemotePlayer_t player;

//�ļ�����
#define MOUNT_PATH		"./rec/00/"
#define	FILE_FLAG		".mp4"
static void _CheckRecFile(char chStartTime[14], char chEndTime[14], char *pBuffer, int *nSize)
{
    char strT[6]={0};
	char strPath[128]={0};
	char strFolder[10]={0};
	U8	TypePlay;		//��������¼���ļ�����
	U32	i = 0, ChnPlay;	//��������¼���ļ�ͨ��

	*nSize=0;
	sprintf(strPath, "%s/", MOUNT_PATH);//"/mnt/rec",i);

	if(access(strPath, F_OK))
	{
		return;//�����������������ֹ����
	}
	memset(strFolder, 0, 10);
	strncat(strFolder, chStartTime, 8);
	strcat(strPath, strFolder);

	if(access(strPath, F_OK))
	{
		Printf("Path:%s can't access\n", strPath);
		return;//����ļ��в����������������һ����
	}
	DIR	*pDir	= opendir(strPath);
	struct dirent *pDirent	= NULL;
	while((pDirent=readdir(pDir)) != NULL)
	{
		//�����������������ͺ�ͨ��
		if(!strcmp(FILE_FLAG, pDirent->d_name+strlen(pDirent->d_name)-strlen(FILE_FLAG)))
		{
			memcpy(&pBuffer[*nSize],pDirent->d_name,9);
			*nSize += 9;
		}
	}
	pBuffer[*nSize]=0;
	closedir(pDir);
	return;
}

/**
 *@brief �����ļ�ʱ�����ļ�·��
 *
 *@param file_time �ļ���ʱ��
 *@param filepath �ļ���·��
 */
int _time2filename(long file_time,char *filepath)
{
	if(!filepath)
		return -1;
	
	char strPath[128]={0};
	char strFolder[10]={0};
	char chFileDate[10]={0};
	char chFileTime[10]={0};
	char temp[20]={0};
	char filename[20]={0};
	struct tm p_tm;
	memcpy(&p_tm,localtime((time_t *)&file_time),sizeof(struct tm));
	sprintf(chFileDate,"%.4d%.2d%.2d",p_tm.tm_year+1900,p_tm.tm_mon+1,p_tm.tm_mday);
	sprintf(chFileTime,"%.2d%.2d%.2d",p_tm.tm_hour,p_tm.tm_min,p_tm.tm_sec);
	sprintf(strPath, "%s", MOUNT_PATH);
	if(access(strPath, F_OK))
	{
		return -1;//�����������������ֹ����
	}
	memset(strFolder, 0, 10);
	strncat(strFolder, chFileDate, 8);
	strcat(strPath, strFolder);

	if(access(strPath, F_OK))
	{
		Printf("Path:%s can't access\n", strPath);
		return -1;//����ļ��в����������������һ����
	}
	DIR	*pDir	= opendir(strPath);
	struct dirent *pDirent	= NULL;
	strncat(temp, chFileTime, 6);
	strcat(temp,FILE_FLAG);
	while((pDirent=readdir(pDir)) != NULL)
	{
		if(!strcmp(temp, pDirent->d_name+strlen(pDirent->d_name)-strlen(FILE_FLAG)-strlen(chFileTime)))
		{
			strcpy(filename,pDirent->d_name);
			break;
		}
	}
	strcat(filepath,strPath);
	strcat(filepath,"/");
	strcat(filepath,filename);
	closedir(pDir);
	return 0;
}
/**
 *@brief ��ͣ¼�񲥷�
 *
 *@param channelid ͨ����
 */
int sp_rec_pause(int channelid)
{
	return 0;
}
/**
 *@brief ��ʼ¼��
 *
 *@param channelid ͨ����
 */
int sp_rec_start(int channelid)
{
	__FUNC_DBG__();
#ifdef GB28181_SUPPORT
	mrecord_attr_t record;
	mrecord_get_param(0, &record);
	if(!record.bEnable)
	{
		record.bEnable = TRUE;
		mrecord_set_param(0, &record);
		mrecord_flush(0);
		WriteConfigInfo();
	}
#endif
	return 0;
}
/**
 *@brief �ָ�����¼��
 *
 *@param channelid ͨ����
 */
int sp_rec_resume(int channelid)
{
	return 0;
}

/**
 *@brief ��������¼��
 *
 *@param channelid ͨ����
 */
int sp_rec_stop(int channelid)
{
#ifdef GB28181_SUPPORT
	mrecord_attr_t record;
	mrecord_get_param(0, &record);
	if (record.bEnable)
	{
		record.bEnable = FALSE;
		mrecord_set_param(0, &record);
		mrecord_flush(0);
		WriteConfigInfo();
	}
#endif
	return 0;
}

/**
 *@brief ����Ƿ�����¼��
 */
int sp_rec_b_on(int channelid)
{
	__FUNC_DBG__();
	mrecord_attr_t record;
	mrecord_get_param(0, &record);
	if(!record.bEnable)
	{
		record.bEnable = TRUE;
		mrecord_set_param(0, &record);
		mrecord_flush(0);
		WriteConfigInfo();
	}
	return 0;
}

/**
 * @brief ����������ļ�
 */
REC_HANDLE sp_rec_create(int channelid, long start_time, long end_time)
{
	char filename[128]={0};
	struct tm p_tm;
	memcpy(&p_tm,localtime((time_t *)&start_time),sizeof(struct tm));
	sprintf(filename,MOUNT_PATH"%.4d%.2d%.2d/N01%.2d%.2d%.2d"FILE_FLAG,p_tm.tm_year+1900,p_tm.tm_mon+1,p_tm.tm_mday,p_tm.tm_hour,p_tm.tm_min,p_tm.tm_sec);

#ifdef GB28181_SUPPORT


	REC_Play *rec_play = (REC_Play *) malloc(sizeof(REC_Play));
	memset(rec_play, 0, sizeof(REC_Play));

	memset(&player, 0, sizeof(RemotePlayer_t));
	player.frameRate = 25;//������һ����ʼֵ
	strcpy(player.fname, filename);
//	strcpy(player.fname, MOUNT_PATH"20150114/N01111341"FILE_FLAG);

	rec_play->ret = Remote_ReadFileInfo_MP4(&player);
	rec_play->framecount = 0;
	rec_play->channel_id = channelid;
	rec_play->rec_start_time = start_time;
	rec_play->rec_end_time = end_time;
	rec_play->fp_ps = NULL;
	rec_play->fp_index = NULL;
//	rec_play->fp = player->fp;
	return rec_play;
#endif


	return 0;
}

/**
 *@brief ����rec���,�ص��ļ�
 */
int sp_rec_destroy(REC_HANDLE handle)
{
	REC_Play *rec_play = (REC_Play *) handle;
	free(handle);

	return 0;
}

/**
 * @brief �����ļ�
 *
 * @param channelid ͨ����
 * @param start_time ��ʼʱ��
 * @param end_time ����ʱ��
 * @param search_file ���ؽ��
 */
REC_Search *sp_rec_search(int channelid, long start_time, long end_time)
{
	REC_Search *search_file, *list = NULL;
	int i=0;
	char buf[2048]={0};
	int nSize=0;
	char charstart_time[10]={0};
	char charend_time[10]={0};
	char rectype;
	char tmp[5]={0};
	int h,m,s;
	struct tm start_tm;
	struct tm end_tm;
	long start_time_day=0;
	memcpy(&start_tm,localtime((time_t *)&start_time),sizeof(struct tm));
	memcpy(&end_tm,localtime((time_t *)&end_time),sizeof(struct tm));
	
	Printf("%s,%d,%d,%d,%d,%d,%d\n",__func__,start_tm.tm_year+1900,start_tm.tm_mon+1,start_tm.tm_mday,start_tm.tm_hour,start_tm.tm_min,start_tm.tm_sec);
	Printf("%s,%d,%d,%d,%d,%d,%d\n",__func__,end_tm.tm_year+1900,end_tm.tm_mon+1,end_tm.tm_mday,end_tm.tm_hour,end_tm.tm_min,end_tm.tm_sec);

	strftime(charstart_time, sizeof(charstart_time), "%Y%m%d", &start_tm);
	strftime(charend_time, sizeof(charend_time), "%Y%m%d", &end_tm);
	_CheckRecFile(charstart_time,charend_time,buf,&nSize);

	//��һ��տ�ʼʱ������
	start_tm.tm_sec=0;
	start_tm.tm_min=0;
	start_tm.tm_hour=0;
	start_time_day=mktime(&start_tm);

	if(nSize==0)
	{
		printf("_CheckRecFile none\n");
		return NULL;
	}
	else
	{
		for(i=0;i<(nSize/9);i++)
		{
			search_file = malloc(sizeof(REC_Search));
			if(search_file==NULL)
			{
				Printf("malloc error\n");
				break;
			}
			strncpy(search_file->fname,buf+i*9,9);		//�ļ�·��
			strcpy(search_file->fname+9,".mp4");
			Printf("file name=%s ",search_file->fname);
			sscanf(search_file->fname, "%c%2s%2d%2d%2d%4s",&rectype,tmp, &h,&m,&s,tmp);
			Printf("file type=%c,file time=%.2d%.2d%.2d\n",rectype,h,m,s);
			
			search_file->start_time = start_time_day+h*60*60+m*60+s;	//¼���ļ���ʼʱ��
			search_file->end_time   = start_time_day+h*60*60+m*60+s+60*3;//¼���ļ�����ʱ��
			search_file->secrecy    = 0;							//��������/0Ϊ������/1Ϊ����
			search_file->type       = (rectype=='M'?SP_REC_TYPE_MOTION:SP_REC_TYPE_ALL); //¼���������
			search_file->next       = list;
			list = search_file;
		}
	}
	return list;
}

/**
 * @brief ɾ��������Ϣ
 *
 * @param list ���������#sp_rec_search �Ľ��
 *
 * @return ����������
 *
 */
int sp_rec_search_release(REC_Search *list)
{
	REC_Search *p = list;
	REC_Search *q;
	while (p)
	{
		q = p;
		p = p->next;
		free(q);
	}
	return 0;
}

/**
 * @brief ������Ϣ����
 *
 * @param list ���������#sp_rec_search �Ľ��
 *
 * @return ��������������
 *
 */
int sp_rec_search_cnt(REC_Search *list)
{
	REC_Search *p = list;
	int cnt=0;
	while (p)
	{
		p = p->next;
		cnt++;
	}
	return cnt;
}

/**
 * @brief ��λ��Ƶ����λ��
 */
int sp_rec_seek(REC_HANDLE handle, int play_range_begin)
{
	return 0;
}

/**
 * @brief ��ȡps�ļ�����ԭʼ֡��ÿ����һ�η���1֡
 */
int sp_rec_read(REC_HANDLE handle, FrameInfo_t *pBuf)
{
#ifdef GB28181_SUPPORT
//#if 1
	int last_len = 0, i = 0;
	REC_Play *rec_play = (REC_Play *) handle;

	AV_UNPKT pack;
	pack.iType = JVS_UPKT_VIDEO;
	pack.iSampleId = player.frameDecoded + 1;

	Remote_Read_Frame_MP4(player.fp, &player.mp4Info, &pack);
	if(pack.iSize)
		player.frameDecoded++;
	if(!pack.iSize)
	{
		player.bFileOver = TRUE;
		return -1;	//�˳�����
	}
	if (pack.bKeyFrame)
	{
		pBuf->type = RTP_PACK_TYPE_FRAME_I;
	}
	else
	{
		pBuf->type = RTP_PACK_TYPE_FRAME_P;
	}

	memcpy(pBuf->buf,pack.pData,pack.iSize);
	pBuf->len = pack.iSize;
	pBuf->framerate = (U32)player.mp4Info.dFrameRate;			//֡��
	pBuf->width = player.mp4Info.iFrameWidth;;					//֡��
	pBuf->height = player.mp4Info.iFrameHeight;				//֡��
#endif
	return 0;
}

//////////////////////////tutk//////////////////////////////////
/**
 * @brief ����¼��ģʽ
 *
 * @param channelid ͨ����
 * @param rectype ¼��ģʽ
 *
 * @return 0 �����Ǵ����
 */
int sp_rec_set_mode(int channelid, SPRecType_e rectype)
{
	mrecord_attr_t record;
	int ret;
	mrecord_get_param(channelid, &record);
	switch(rectype)
	{
		case SP_REC_TYPE_MOTION:
			record.alarm_enable=TRUE;
			record.detecting=TRUE;
			record.bEnable=FALSE;
			break;
		case SP_REC_TYPE_MANUAL:
			record.bEnable=TRUE;
			record.alarm_enable=FALSE;
			record.detecting=FALSE;
			break;
		case SP_REC_TYPE_STOP:
			record.detecting     = FALSE;
			record.bEnable       = FALSE;
			record.timing_enable = FALSE;
			record.alarm_enable  = FALSE;
			record.discon_enable = FALSE;
			break;
		default:
			break;
	}
	ret=mrecord_set_param(channelid, &record);
	ret|=mrecord_flush(0);
	WriteConfigInfo();
	return ret;
}

/**
 * @brief ���¼��ģʽ
 *
 * @param channelid ͨ����
 *
 * @return ¼��ģʽ
 */
SPRecType_e sp_rec_get_mode(int channelid)
{
	mrecord_attr_t record;
	mrecord_get_param(channelid, &record);
	if(record.detecting==TRUE && record.alarm_enable==TRUE)
	{
		return SP_REC_TYPE_MOTION;
	}
	else if(record.bEnable==TRUE)
	{
		return SP_REC_TYPE_MANUAL;
	}
	else if(record.detecting==FALSE && record.bEnable ==FALSE && record.timing_enable ==FALSE && record.alarm_enable == FALSE && record.discon_enable == FALSE)
	{
		return SP_REC_TYPE_STOP;
	}
	else
	{
		return SP_REC_TYPE_MANUAL;
	}
}

unsigned int sp_rec_set_length(int channelid, unsigned int len)
{
	mrecord_attr_t record;
	mrecord_get_param(channelid, &record);
	if(len!=record.file_length)
	{
		record.file_length = len;
		mrecord_set_param(channelid, &record);
		mrecord_flush(0);
		WriteConfigInfo();
	}
	return len;
}
unsigned int sp_rec_get_length(int channelid)
{
	mrecord_attr_t record;
	mrecord_get_param(channelid, &record);
	return record.file_length;
}
