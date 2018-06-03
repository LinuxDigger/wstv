#include "jv_common.h"
#include "jv_osddrv.h"
#include "mosd.h"
#include "mstream.h"
#include "utl_timer.h"
#include "mlog.h"
#include <utl_iconv.h>
#include <utl_filecfg.h>
//#include "SYSFuncs.h"
#include <jv_sensor.h>
#include "msensor.h"
#include "mipcinfo.h"
#include "mptz.h"
#include <jv_stream.h>
#include "mivp.h"

//�����ֿ�Ĵ�С
//#define SIZE_FONT_EN	4*1024
//#define SIZE_FONT_EN	12*1024
////#define SIZE_FONT_CN	256*1024	//16���ֿ�
//#define SIZE_FONT_CN	576*1024 	//24���ֿ�

//#define FILE_FONT_EN		"/progs/res/Asc16.bin"
//#define FILE_FONT_CN		"/progs/res/Hzk16.bin"

/*
 �ֿ�˵����
  1.Ӣ���������ԭϸ����Asc16.bin����Ϊ������nASC16.bin ������ԭ�ֿ����ݴ����ϲ�����ͬ�ֿ⣬ע����ģ��С��
  2.�����������ԭϸ����Hzk16.bin����Ϊ΢���ź�����msyh16_CP936.bin (��FontMaker����)
 ������ FontMaker�������� ��Ӣ���ֿ��ݲ��˽���������������Ϊ�����ֿ��������̣�

  1.Input Font��F����·��ѡ�����صĺ����ֿ�ԭ�ļ���ttf��ʽ��
  2.Encode ���뷽ʽ��ѡ��Mbcs
  3.charset �ַ����� ��ѡ��������GBK
  4.size ѡ��16
  5.Output FileType��ѡ����Ҫ�������ļ����ͣ���������Ҫbin��Ĭ����ѡ��
         Path�������ļ����·��
  6.������ɺ���build������
  7.��FontMaker���ɵ��ֿ��ļ�����ʽ��Ҫ�޸ģ����ֿ��ļ���ɾ��ͷ��ʮ���ֽ�ͷ��Ϣ����

 */

#define FILE_FONT_EN		"/progs/res/nASC16.bin"        //�������2013��5��13��
//#define FILE_FONT_EN		"/progs/res/arialbd24_CP1252.bin"			//24���ֿ�
#define FILE_FONT_CN		"/progs/res/msyh16_CP936.bin"	//16���ֿ�
//#define FILE_FONT_CN		"/progs/res/msyh24_CP936.bin"	//24���ֿ�
//#define FILE_FONT_CN		"/progs/res/FZBWKSJW24_CP936.bin"	//24���ֿ�

#define OSD_CHN_CNT HWINFO_STREAM_CNT

/*****************************�ֿ�ͷ��Ϣ�ṹlk20131214********************************/
typedef unsigned char 	BYTE;
typedef unsigned short 	WORD;
typedef unsigned long	DWORD;//��ʱ�ȷ���

typedef struct tagFontLibHeader
{
	BYTE magic[4];//'U'(or 'M'), 'F', 'L', X        'U'(or 'M')---Unicode(or MBCS) Font Library, X: ��ʾ�汾��. �ָߵ�4λ���� 0x12��ʾ Ver 1.2
	DWORD Size; /* File total size */
	BYTE nSection; //MBCS:�Ƿ���������� Unicode�����ּ�������
	BYTE YSize; /* height of font  */
	WORD wCpFlag; // codepageflag:  bit0~bit13 ÿ��bit�ֱ����һ��CodePage ��־�������1�����ʾ��ǰCodePage ��ѡ��������Ϊ��ѡ����
	char reserved[4];	  // Ԥ���ֽ�
} FL_Header;

//�ֿ������ֻ��Էǵȿ��ֿ⣬���ﴦ��Ӣ����...�Ļ�16������������ʱ���ò����˷��������ֹ�Ժ��ڸĻ�����
typedef struct tagUflCharInfo{
#ifdef	SUPPORT_MAX_FONT        // ����ô�����ṹ������֧��248����
	DWORD OffAddr;      // ��ǰ�ַ��������ݵ���ʼ��ַ
	BYTE Width;// �ַ���������صĿ��
#else
	DWORD OffAddr :26;      // ��ǰ�ַ��������ݵ���ʼ��ַ
	DWORD Width :6;       // �ַ���������صĿ��( Ŀǰ���֧�� 56 ����
#endif
} UFL_CHAR_INDEX;
/*************************************************************************************/

#define MAX_USERDEF_REGION_CNT 2
typedef struct{
	BOOL bEnable;
	int osdhandle[MAX_STREAM];
	int channelid[MAX_STREAM];
	int fontsize[MAX_STREAM];
	int osdcnt;
	mchnosd_region_t region;
}region_userdef_t;

typedef struct
{
	mchnosd_attr attrlist[MAX_STREAM];
	
	mchnosd_section_attr section_attrlist[MAX_STREAM];
	int osdhandle[MAX_STREAM];
	int osdNameHandle[MAX_STREAM];
	int osdSectionHandle[MAX_STREAM];

	region_userdef_t region[MAX_USERDEF_REGION_CNT];

	int fontsize[MAX_STREAM];
	int timer;

	S32 	fdEN;	//�ֿ���
	U8		*pEN;	//�ֿ�ӳ�䵽�ڴ�ĵ�ַ
	S32 	fdCN;
	U8		*pCN;
	S32		sizeFontEn;
	S32		sizeFontCn;
	FL_Header *flheader;	//�����ֿ�ͷ��Ϣ��lk20131214

	pthread_mutex_t mutex;
	pthread_t thread;
	BOOL needFlush;
	BOOL bRunning;


	unsigned short textColor;
	unsigned short borderColor;
	unsigned short clearColor;
} osdstatus_t;

typedef struct
{
	BOOL bTxtShow;
	mchnosd_pos_e position;
	char osdtxt[32];
	int fontsize;
} snap_osdattr_t;

typedef struct
{
	BOOL bShowOSD;
	snap_osdattr_t attr[3];
	int fontsize[3];

	int osdhandle[3];
	pthread_mutex_t mutex;

	unsigned short textColor;
	unsigned short borderColor;
	unsigned short clearColor;
} snap_osdstatus_t;

/***** ��̨�˵���ʾ���� ******/
/**
 *@brief ��̨�˵���ʾ���ܽṹ��
 */

typedef struct 
{
	int handle[VSC_OSD_CNT];
	BOOL created;	//������̨�˵��ı�־���������ٱ䱶�����ж�
	BOOL running;
	BOOL ondraw;	//��̨�˵����ݸ�����Ҫ�ػ��ı�־
	pthread_mutex_t mutex;
	pthread_t thread;

	int fontsize[VSC_OSD_CNT];
	mvisca_osd_attr osdattr;
}mvisca_osd_group_t;
static mvisca_osd_group_t mvisca_osd_group;

/***** **************** ******/

#define COLOR_TEXT		(osdstatus.textColor)//0xFFFF    //��ɫ����
#define COLOR_BORDER	(osdstatus.borderColor)//0x8000    //��ɫ���
#define COLOR_CLEAR		(osdstatus.clearColor)

 osdstatus_t osdstatus;
static snap_osdstatus_t osdstatus_snap;
static osdstatus_t osdstatus_count;

static osdstatus_t osdstatus_cde;
static multiosd_info_t multiOsd_info;

static ptzosdstatus_t ptzstatus;
static void mvisca_osd_process(void *param);
static BOOL _time_refresh_callback(int tid, void *param);
static osdstatus_t osdstatus_linecount;	//���߼���


static void *__mchnosd_thread(void *param)
{
	pthreadinfo_add((char *)__func__);

	while(osdstatus.bRunning)
	{
		if (osdstatus.needFlush)
		{
			_time_refresh_callback(0, NULL);
		}
		usleep(100*1000);
	}
	return NULL;
}

int mchnosd_init(void)
{
	int i;
	CommonColor_t cc;
	U8 *flpen;
	struct stat statbuf;
	jv_osddrv_init();
	memset(&osdstatus,0,sizeof(osdstatus));
	osdstatus.needFlush = FALSE;
	jv_osddrv_get_common_color(&cc);
	osdstatus.clearColor = cc.clear;
	osdstatus.textColor = cc.white;
	osdstatus.borderColor = cc.black;



	for (i=0; i<HWINFO_STREAM_CNT; i++)
	{
		osdstatus.osdhandle[i] = -1;
		osdstatus.osdNameHandle[i] = -1;
		osdstatus.osdSectionHandle[i] = -1;
		osdstatus_snap.osdhandle[i] = -1;
		osdstatus_count.osdhandle[i] = -1;
		osdstatus_count.osdNameHandle[i] = -1;
		osdstatus_count.osdSectionHandle[i] = -1;
		osdstatus_cde.osdhandle[i] = -1;
		osdstatus_cde.osdNameHandle[i] = -1;
		osdstatus_cde.osdSectionHandle[i] = -1;
		osdstatus_linecount.osdhandle[i] = -1;
		osdstatus_linecount.osdNameHandle[i] = -1;
		osdstatus_linecount.osdSectionHandle[i] = -1;

	}
	for (i=0; i<VSC_OSD_CNT; i++)
	{
		mvisca_osd_group.handle[i] = -1;
	}
	multiOsd_info.multiOsdHandle = -1;

	//����OSDӢ������
	osdstatus.fdEN = open(FILE_FONT_EN, O_RDONLY);
	if(osdstatus.fdEN < 0)
	{
		Printf("Open en font failed, file:%s...\n", FILE_FONT_EN);
		return -1;
	}
	memset(&statbuf,0,sizeof(statbuf));
	fstat(osdstatus.fdEN, &statbuf);
	osdstatus.sizeFontEn = statbuf.st_size;
	osdstatus.pEN = mmap(NULL, osdstatus.sizeFontEn, PROT_READ, MAP_PRIVATE, osdstatus.fdEN, 0);
	if(MAP_FAILED == osdstatus.pEN)
	{
		Printf("mmap failed...\n");
		return -1;
	}
	//����OSD��������
	osdstatus.fdCN = open(FILE_FONT_CN, O_RDONLY);
	if(osdstatus.fdCN < 0)
	{
		Printf("Open cn font failed, file:%s...\n", FILE_FONT_CN);
		return -1;
	}
	memset(&statbuf,0,sizeof(statbuf));
	fstat(osdstatus.fdCN, &statbuf);
	osdstatus.sizeFontCn = statbuf.st_size;

	osdstatus.pCN = mmap(NULL, osdstatus.sizeFontCn, PROT_READ, MAP_PRIVATE, osdstatus.fdCN, 0);
	if(MAP_FAILED == osdstatus.pCN)
	{
		Printf("mmap failed...\n");
		return -1;
	}
	/********************************��ʼ��ͷ��Ϣlk20131214************************/
	flpen = (U8*)osdstatus.pCN;
	osdstatus.flheader = (FL_Header*)flpen;
	/*****************************************************************************/
	pthread_mutex_init(&osdstatus.mutex, NULL);
	pthread_mutex_init(&osdstatus_snap.mutex, NULL);
	pthread_mutex_init(&osdstatus_count.mutex, NULL);
	pthread_mutex_init(&mvisca_osd_group.mutex, NULL);
	pthread_mutex_init(&osdstatus_cde.mutex, NULL);

	pthread_mutex_init(&osdstatus_linecount.mutex, NULL);
	osdstatus.bRunning = TRUE;
	pthread_create(&osdstatus.thread, NULL, __mchnosd_thread, NULL);

	return 0;
}

int mchnosd_deinit(void)
{
	int i;
	pthread_mutex_lock(&osdstatus.mutex);
	osdstatus.needFlush = FALSE;
	//�ر�OSDӢ����Դ
	if(osdstatus.pEN)
	{
		munmap(osdstatus.pEN, osdstatus.sizeFontEn);
		if(osdstatus.fdEN > 0)
		{
			close(osdstatus.fdEN);
		}
		osdstatus.pEN = NULL;
	}
	//�ر�OSD������Դ
	if(osdstatus.pCN)
	{
		munmap(osdstatus.pCN, osdstatus.sizeFontCn);
		if(osdstatus.fdCN > 0)
		{
			close(osdstatus.fdCN);
		}
		osdstatus.pCN = NULL;
	}
	pthread_mutex_unlock(&osdstatus.mutex);
	for (i=0; i<HWINFO_STREAM_CNT; i++)
	{
		if (osdstatus.osdhandle[i] != -1)
			jv_osddrv_destroy(osdstatus.osdhandle[i]);
	}
	jv_osddrv_deinit();
	osdstatus.bRunning = FALSE;
	pthread_join(osdstatus.thread, NULL);
	pthread_mutex_destroy(&osdstatus.mutex);
	return 0;
}

int mchnosd_set_param(int channelid, mchnosd_attr *attr)
{
	jv_assert(channelid < HWINFO_STREAM_CNT, return JVERR_BADPARAM);
	jv_assert(attr != NULL, return JVERR_BADPARAM);
	if (osdstatus.attrlist[channelid].bShowOSD != attr->bShowOSD)
	{
		if (attr->bShowOSD)
			mlog_write("Channel Osd Enabled");
		else
			mlog_write("Channel Osd Disabled");
	}
	memcpy(&osdstatus.attrlist[channelid], attr, sizeof(mchnosd_attr));
	//index = sizeof(osdstatus.attrlist[channelid].channelName)-1;
	//if (((unsigned char )osdstatus.attrlist[channelid].channelName[index-1]) > 0x7f)
	//	osdstatus.attrlist[channelid].channelName[index-1] = '\0';
	utl_iconv_gb2312_fix(osdstatus.attrlist[channelid].channelName, sizeof(osdstatus.attrlist[channelid].channelName));
	return 0;
}

int mchnosd_get_param(int channelid, mchnosd_attr *attr)
{
	jv_assert(channelid < HWINFO_STREAM_CNT,return JVERR_BADPARAM);
	jv_assert(attr != NULL, return JVERR_BADPARAM);
	memcpy(attr, &osdstatus.attrlist[channelid], sizeof(mchnosd_attr));
	return 0;
}

char *mchnosd_time2str(char *timeFormat, time_t nsecond, char *str)
{
	char *p = str;
	struct tm *tmOSD = localtime(&nsecond);

	while(timeFormat && *timeFormat)
	{
		switch(*timeFormat)
		{
		case 'Y':
			if (*(timeFormat+2) == 'Y')
			{
				sprintf(p, "%04d", tmOSD->tm_year+1900);
				p += 4;
				timeFormat += 3;
				if (*timeFormat == 'Y')
					timeFormat++;
			}
			else
			{
				sprintf(p, "%02d", tmOSD->tm_year%100);
				p += 2;
				timeFormat += 1;
				if (*timeFormat == 'Y')
					timeFormat++;
			}
			break;
		case 'M':
			sprintf(p, "%02d", tmOSD->tm_mon+1);
			p += 2;
			timeFormat += 1;
			if (*timeFormat == 'M')
				timeFormat++;
			break;
		case 'D':
			sprintf(p, "%02d", tmOSD->tm_mday);
			p += 2;
			timeFormat += 1;
			if (*timeFormat == 'D')
				timeFormat++;
			break;
		case 'h':
			sprintf(p, "%02d", tmOSD->tm_hour);
			p += 2;
			timeFormat += 1;
			if (*timeFormat == 'h')
				timeFormat++;
			break;
		case 'm':
			sprintf(p, "%02d", tmOSD->tm_min);
			p += 2;
			timeFormat += 1;
			if (*timeFormat == 'm')
				timeFormat++;
			break;
		case 's':
			sprintf(p, "%02d", tmOSD->tm_sec);
			p += 2;
			timeFormat += 1;
			if (*timeFormat == 's')
				timeFormat++;
			break;
		default:
			*p++ = *timeFormat++;
		}
	}
	return str;
}

/**
 *@brief ���㽫�ַ������Ƴ���������Ҫ�Ŀ��
 *@param str ��Ҫ�����ȵ��ַ���
 *@return �ַ����Ŀ��
 *
 */
int mchnosd_get_str_width(char *str, int fontsize)
{
	int i;
	int width = 0;
	UFL_CHAR_INDEX *ufl_char_index;

	if (fontsize == 0)
	{
		//fontsize = 16;
		fontsize = 16;	//��ΪĬ��24������
	}

	for (i=0; i<strlen(str); i++)
	{
		if(str[i] > 0x7F) //����
		{
			i++;
			width += fontsize;
		}
		else	//Ӣ��
		{
			width += (fontsize/2);
		}
	}
	return width;
}

static int GetPosWithMbcs(char code_r,char code_c)
{
	int lIdx = -1;

	BYTE R = code_r & 0xFF;   //����
	BYTE C = code_c & 0xFF;   //λ��

	if ((R >= 0xA1 && R <= 0xFE) && (C >= 0xA1 && C <= 0xFE))
		lIdx = (R - 0xa1) * 94 + (C - 0xa1);  //94 = (0xFE-0xA1+1);

	return lIdx;
}

/**
 *@brief ���ַ������Ƶ�ָ���ڴ�������
 *@param str Ҫ���Ƶ��ַ���
 *@param buffer ���ڻ��Ƶ�BUFFER��ַ
 *@param width Ҫ���Ƶ������OSD�Ŀ��
 *
 */
int mchnosd_get_str_bmp_ex(char *str, unsigned char *buffer, int width, int fontsize, unsigned short text_color)
{
	S32 i, j, k,f1,f2;
	U8 *pRead8;
	U16 *pData = (U16 *)buffer;
	unsigned char *acOSD = (unsigned char*)str;
	int pos = 0;
	int index;
	U16 read16;
	U32 read24;
	UFL_CHAR_INDEX *ufl_char_index;
	int bytePerRow;


	if (osdstatus.pCN == NULL || osdstatus.pEN == NULL)
	{
		Printf("ERROR: font not init successfully\n");
		return -1;
	}

	for (i=0; i<strlen(str); i++)
	{
		pData = (U16*)(buffer + pos);
		if(acOSD[i] < 0x20)
			acOSD[i]=0x20;
		if(acOSD[i] > 0x7F) //����
		{
			//�����ֿ�ͷ��Ϣ����λ�ַ����ֿ��е�λ��lk20131214
			index = GetPosWithMbcs(acOSD[i],acOSD[i+1]);
			pRead8 = (U8*)(osdstatus.pCN + sizeof(FL_Header) + index * (FONT_HEIGHT/8*FONT_HEIGHT));
			//pRead8 = (U8*)(osdstatus.pCN + (94*(acOSD[i]-0xA1) + (acOSD[i+1]-0xA1))*(FONT_HEIGHT*FONT_HEIGHT/8));
			for (j=0; j<fontsize; j++)
			{
				f1 = j * FONT_HEIGHT / fontsize;
				read16 = (pRead8[f1<<1] << 8) | (pRead8[(f1<<1)+1] << 0);
				//��Ϊ24���ֿ⣬�õ�24λlk20131214
				//read24 = ((pRead8[f1*3] << 16) | (pRead8[(f1*3)+1] << 8) | (pRead8[(f1*3)+2] << 0)) & 0x00FFFFFF;
				for(k=0; k<fontsize; k++)
				{
					f2 = k*FONT_HEIGHT/fontsize;
					if((read16<<f2)&0x8000)
					//if((read24 << f2)&0x800000)//��Ϊ24���ֿ⣬�õ�24λlk20131214
					{
						pData[k] = text_color;
					}
					else
					{
						//pData[k] = 0xE2F6;
						pData[k] = COLOR_CLEAR;
					}
				}
				pData += width;
			}
			i++;
			pos += fontsize*2;
		}
		else	//Ӣ��
		{
			//ĳ���ַ��������صĵ�ַ
			pRead8 = osdstatus.pEN + (acOSD[i] - 0x20) * (FONT_HEIGHT * FONT_HEIGHT / 2 / 8);
			for (j = 0; j < fontsize; j++)
			{
				f1 = j * FONT_HEIGHT / fontsize;
				for (k = 0; k < fontsize / 2; k++)
				{
					f2 = k * FONT_HEIGHT / fontsize;
					if ((pRead8[f1] << f2) & 0x80)
					{
						pData[k] = text_color;
					}
					else
					{
						//pData[k] = 0xE2F6;
						pData[k] = COLOR_CLEAR;
					}
				}
				pData += width;
			}
			pos += fontsize;
		}
	}

	return 0;
}

/**
 *@brief ���ַ������Ƶ�ָ���ڴ�������
 *@param str Ҫ���Ƶ��ַ���
 *@param buffer ���ڻ��Ƶ�BUFFER��ַ
 *@param width Ҫ���Ƶ������OSD�Ŀ��
 *
 */
int mchnosd_get_str_bmp(char *str, unsigned char *buffer, int width)
{
	S32 i, j, k;
	U8 *pRead8;
	U16 *pData = (U16 *)buffer;
	char *acOSD = str;
	int pos = 0;
	U16 read16;

	if (osdstatus.pCN == NULL || osdstatus.pEN == NULL)
	{
		Printf("ERROR: font not init successfully\n");
		return -1;
	}
	for (i=0; i<strlen(acOSD); i++)
	{
		pData = (U16*)(buffer + pos);
		if(acOSD[i] > 0x7F) //����
		{
			pRead8 = (U8*)(osdstatus.pCN + (94*(acOSD[i]-0xA1) + (acOSD[i+1]-0xA1))*(FONT_HEIGHT*FONT_HEIGHT/8));
			for (j=0; j<FONT_HEIGHT; j++)
			{
				read16 = (pRead8[j<<1] << 8) | (pRead8[(j<<1)+1] << 0);
				for(k=0; k<FONT_HEIGHT; k++)
				{
					if((read16<<k)&0x8000)
					{
						pData[k] = COLOR_TEXT;
					}
					else
					{
						//pData[k] = 0xE2F6;
						pData[k] = COLOR_CLEAR;
					}
				}
				pData += width;
			}
			i++;
			pos += FONT_HEIGHT*2;
		}
		else	//Ӣ��
		{
			//ĳ���ַ��������صĵ�ַ
			pRead8 = osdstatus.pEN + (acOSD[i]-0x20)*(FONT_HEIGHT*FONT_HEIGHT/2/8);
			for (j=0; j<FONT_HEIGHT; j++)
			{
				for(k=0; k<FONT_HEIGHT/2; k++)
				{
					if((pRead8[j]<<k)&0x80)
					{
						pData[k] = COLOR_TEXT;
					}
					else
					{
						//pData[k] = 0xE2F6;
						pData[k] = COLOR_CLEAR;
					}
				}
				pData += width;
			}
			pos += FONT_HEIGHT;
		}
	}

	return 0;
}


/**
 *@brief ��ָ��ͨ���л����ַ�����
 *@param px �ַ������OSD������Ͻǵ�����Xֵ
 *@param py �ַ������OSD������Ͻǵ�����Yֵ
 *@param text Ҫ��ʾ���ַ���
 *
 */
int mchnosd_draw_text_ex(int channelid, int osdHandle, int px, int py, char *text, int fontsize, BOOL bFlush)
{
	U16 *pPixel, *pDstPixel;
	RECT rect;
	int ret, width, w, h, i=0, j=0;

	if (osdHandle == -1)
		return -1;

	unsigned short text_color = COLOR_TEXT;
	unsigned short border_color = COLOR_BORDER;

	if(osdHandle == osdstatus_count.osdhandle[channelid])
	{
		text_color = osdstatus_count.textColor;
		border_color = osdstatus_count.borderColor;
	}
	else if(osdHandle == osdstatus_cde.osdhandle[channelid])
	{
		text_color = osdstatus_cde.textColor;
		border_color = osdstatus_cde.borderColor;
	}
	else if(osdHandle == osdstatus_linecount.osdhandle[channelid])
	{
		text_color = osdstatus_linecount.textColor;
		border_color = osdstatus_linecount.borderColor;
	}
	width = mchnosd_get_str_width(text, fontsize);

	pPixel = (U16*)malloc(width*fontsize*2);
    if (pPixel == NULL)
	{
	    printf("%s  pPixel: No Enough Memmory!\n", __FUNCTION__);
		return -1;
    }
	
	ret = mchnosd_get_str_bmp_ex(text, (unsigned char*)pPixel, width, fontsize,	text_color);

	if (ret != 0)
	{
	    printf("%s  mchnosd_get_str_bmp Error!\n", __FUNCTION__);
	    free(pPixel);
	    pPixel = NULL;
		return ret;
    }

	w = width+2;
	h = fontsize+2;
	pDstPixel = (U16*)malloc(w*h*2);
    if (pDstPixel == NULL)
	{
	    printf("%s  pDstPixel: No Enough Memmory!\n", __FUNCTION__);
	    free(pPixel);
	    pPixel = NULL;
		return -1;
    }
    
	if (COLOR_CLEAR == 0)
	{
		memset((char*)pDstPixel, COLOR_CLEAR, w*h*2);
	}
	else
	{
		for (i=0;i<w*h;i++)
			pDstPixel[i] = COLOR_CLEAR;
	}
	for(i=0; i<fontsize; i++)
	{
		memcpy((char*)&pDstPixel[(i+1)*w+1], (char*)&pPixel[i*width], width*2);
	}

	int bold;
	if (fontsize > 16)
		bold = 0;
	else
		bold = 1;


//	if(!osdstatus.attrlist[channelid].osdbInvColEn)
//	{
	switch(bold)
	{
		case 0:
			for(i=1; i<=h-2; i++)
			{
				for(j=1; j<=w-2; j++)
				{
					if(text_color == pDstPixel[i*w+j])
					{
						//�жϰ˸���������أ��������0xffff����Ϊ���ɫ
						//�������
						if(text_color != pDstPixel[(i-1)*w+(j-1)])
						{
							pDstPixel[(i-1)*w+(j-1)] = border_color;
							//pDstPixel[(i-2)*w+(j-1)] = COLOR_BORDER;
							//pDstPixel[(i-2)*w+(j-2)] = COLOR_BORDER;
						}
						if(text_color != pDstPixel[(i-1)*w+(j)])
						{
							pDstPixel[(i-1)*w+(j)] = border_color;
							if(i >= 2)
								pDstPixel[(i-2)*w+(j)] = border_color;	  //ͬ��ͬ�мӵ�
							//pDstPixel[(i-2)*w+(j+1)] = COLOR_BORDER;
						}
						if(text_color != pDstPixel[(i-1)*w+(j+1)])
						{
							pDstPixel[(i-1)*w+(j+1)] = border_color;
							//pDstPixel[(i-1)*w+(j+2)] = COLOR_BORDER;
							//pDstPixel[(i-2)*w+(j+2)] = COLOR_BORDER;
						}
						//��ǰ�����
						if(text_color != pDstPixel[(i)*w+(j-1)])
						{
							pDstPixel[(i)*w+(j-1)] = border_color;
							if(j >= 2)
							{
								pDstPixel[(i)*w+(j-2)] = border_color;	  //ͬ��ͬ�мӵ�
							}
							//pDstPixel[(i-1)*w+(j-2)] = COLOR_BORDER;
						}
						if(text_color != pDstPixel[(i)*w+(j+1)])
						{
							pDstPixel[(i)*w+(j+1)] = border_color;
							if(j+2 < w)
							{
								pDstPixel[(i)*w+(j+2)] = border_color;		//ͬ��ͬ�мӵ�
							}
							//pDstPixel[(i+1)*w+(j+2)] = COLOR_BORDER;
						}
						//�������
						if(text_color != pDstPixel[(i+1)*w+(j-1)])
						{
							pDstPixel[(i+1)*w+(j-1)] = border_color;
							//pDstPixel[(i+1)*w+(j-2)] = COLOR_BORDER;
							//pDstPixel[(i+2)*w+(j-2)] = COLOR_BORDER;
						}
						if(text_color != pDstPixel[(i+1)*w+(j)])
						{
							pDstPixel[(i+1)*w+(j)] = border_color;
							if (i+2 < h)
							{
								pDstPixel[(i+2)*w+(j)] = border_color;	   //ͬ��ͬ�мӵ�
							}
							//pDstPixel[(i+2)*w+(j-1)] = COLOR_BORDER;
						}
						if(text_color != pDstPixel[(i+1)*w+(j+1)])
						{
							pDstPixel[(i+1)*w+(j+1)] = border_color;
							//pDstPixel[(i+2)*w+(j+1)] = COLOR_BORDER;
							//pDstPixel[(i+2)*w+(j+2)] = COLOR_BORDER;
						}
					}
				}
			}
			break;
		case 1:
		case 2:
			for(i=1; i<=h-2; i++)
			{
				for(j=1; j<=w-2; j++)
				{
					if(text_color == pDstPixel[i*w+j])
					{
						//�жϰ˸���������أ��������0xffff����Ϊ���ɫ
						//�������
						if(text_color != pDstPixel[(i-1)*w+(j-1)])
						{
							pDstPixel[(i-1)*w+(j-1)] = border_color;
						}
						if(text_color != pDstPixel[(i-1)*w+(j)])
						{
							pDstPixel[(i-1)*w+(j)] = border_color;
						}
						if(text_color != pDstPixel[(i-1)*w+(j+1)])
						{
							pDstPixel[(i-1)*w+(j+1)] = border_color;
						}
						//��ǰ�����
						if(text_color != pDstPixel[(i)*w+(j-1)])
						{
							pDstPixel[(i)*w+(j-1)] = border_color;
						}
						if(text_color != pDstPixel[(i)*w+(j+1)])
						{
							pDstPixel[(i)*w+(j+1)] = border_color;
						}
						//�������
						if(text_color != pDstPixel[(i+1)*w+(j-1)])
						{
							pDstPixel[(i+1)*w+(j-1)] = border_color;
						}
						if(text_color != pDstPixel[(i+1)*w+(j)])
						{
							pDstPixel[(i+1)*w+(j)] = border_color;
						}
						if(text_color != pDstPixel[(i+1)*w+(j+1)])
						{
							pDstPixel[(i+1)*w+(j+1)] = border_color;

						}
					}
				}
			}
			break;
		default:
			break;
	}
//	}
	rect.x = px;
	rect.y = py;
	rect.w = w;
	rect.h = h;
	jv_osddrv_drawbitmap(osdHandle, &rect, (unsigned char*)pDstPixel);

	if (bFlush)
		jv_osddrv_flush(osdHandle);

	free(pPixel);
	free(pDstPixel);
	pPixel = NULL;
	pDstPixel = NULL;
	return 0;
}

int mchnosd_draw_text(int channelid, int osdHandle, int px, int py, char *text, int fontsize)
{
	return mchnosd_draw_text_ex(channelid, osdHandle, px, py, text, fontsize, TRUE);
}

static BOOL bDisplayFocusValue = FALSE;
//�����Ƿ���ʾ�����ο�
void mchnosd_display_focus_reference_value(BOOL bDisplay)
{
	jv_stream_attr attr;
	if (bDisplay)
	{
		jv_stream_get_attr(0, &attr);
		attr.nGOP = 10;
		attr.maxQP = 35;
		attr.minQP = 34;
		jv_stream_set_attr(0, &attr);
		jv_stream_stop(0);
		jv_stream_start(0);
	}
	else
	{
		mstream_flush(0);
	}
	bDisplayFocusValue = bDisplay;
}

static BOOL bDebugMode = FALSE;
static char bDebugDisplay[1024];


static int debugModeTimer = -1;
static BOOL __resume_debugMode(int tid, void *param)
{
	bDebugMode = FALSE;
	return FALSE;
}

void mchnosd_debug_mode(BOOL bDebug, char *display)
{
	//�����ַ�����Ҳ��Ҫ��
	//�����п����ַ����ĳ��ȱ仯
	pthread_mutex_lock(&osdstatus.mutex);
	bDebugMode = bDebug;
	if (display)
	{
		strncpy(bDebugDisplay, display, sizeof(bDebugDisplay));
		strcat(bDebugDisplay, "        ");
		//���Ȳ���̫������Խ�磨��Ȼ�ˣ��ײ�������Խ��Ĵ���
		bDebugDisplay[19] = '\0';
	}
	pthread_mutex_unlock(&osdstatus.mutex);

	if (bDebugMode)
	{
		if (debugModeTimer == -1)
			debugModeTimer = utl_timer_create("debugModeTimer", 10*1000, __resume_debugMode, NULL);
		else
			utl_timer_reset(debugModeTimer, 10*1000, __resume_debugMode, NULL);
	}
}

static BOOL _time_refresh_callback(int tid, void *param)
{
	int i;
	static time_t last = 0;
	time_t now;

	now = time(NULL);
	if (now == last)
		if (!bDebugMode)
			if (!bDisplayFocusValue)
				return TRUE;
	last = now;
	char timestr[32];
	
	pthread_mutex_lock(&osdstatus.mutex);
	for (i=0; i<OSD_CHN_CNT; i++)
	{
		if (bDebugMode)
		{
			mchnosd_draw_text(i, osdstatus.osdhandle[i], 4, 4, bDebugDisplay, osdstatus.fontsize[i]);
		}
		else if (bDisplayFocusValue)
		{
			sprintf(timestr, "focus: %d           ", msensor_get_focus_value(0));
			mchnosd_draw_text(i, osdstatus.osdhandle[i], 4, 4, timestr, osdstatus.fontsize[i]);
		}
		else if (osdstatus.attrlist[i].bShowOSD)
		{
			mchnosd_time2str(osdstatus.attrlist[i].timeFormat, time(NULL),timestr);
			mchnosd_draw_text(i, osdstatus.osdhandle[i], 4, 4, timestr, osdstatus.fontsize[i]);
		}
	}
	pthread_mutex_unlock(&osdstatus.mutex);
#if (IVP_COUNT_SUPPORT ||(defined XW_MMVA_SUPPORT))

		if(mivp_count_bsupport())
		{
			pthread_mutex_lock(&osdstatus_count.mutex);
			for (i=0; i<OSD_CHN_CNT; i++)
			{
				if(osdstatus_count.attrlist[0].bShowOSD)
				{
					sprintf(osdstatus_count.attrlist[0].channelName,"����:%d    �뿪:%d    ",    mivp_count_in_get(),mivp_count_out_get());
					mchnosd_draw_text(i, osdstatus_count.osdhandle[i], 4, 4, osdstatus_count.attrlist[0].channelName, osdstatus_count.fontsize[i]);
				}
			}
			pthread_mutex_unlock(&osdstatus_count.mutex);
		}
#endif


#if (defined PLATFORM_hi3516D)
	pthread_mutex_lock(&osdstatus_snap.mutex);
	mchnosd_draw_text(0, osdstatus_snap.osdhandle[0], 4, 4, mchnosd_time2str(osdstatus.attrlist[0].timeFormat, time(NULL),timestr), osdstatus_snap.fontsize[0]);
	pthread_mutex_unlock(&osdstatus_snap.mutex);
#endif
	return TRUE;
}

static int _mchnosd_build_osdattr(int channelid, jv_osddrv_region_attr *attr, jv_osddrv_region_attr *nameAttr)
{
	mstream_attr_t st;
	int fontsize;
	int w1, w2;
	char timestr[32];

	attr->type = OSDDRV_COLOR_TYPE_16_ARGB1555;
	//attr->jv_osddrv_inv_col_en = osdstatus.attrlist[channelid].osdbInvColEn;//��ɫ��ʱȥ���ˡ�
	mstream_get_running_param(channelid,&st);

	int tmp_w = st.width;
	JVRotate_e rotate = msensor_get_rotate();
	if(rotate==JVSENSOR_ROTATE_90 || rotate==JVSENSOR_ROTATE_270)
		tmp_w = st.height;
	if (osdstatus.attrlist[channelid].bLargeOSD)
	{
		if (tmp_w > 2000)
			fontsize = 80;
		else if (tmp_w > 1800)
			fontsize = 64;
		else if (tmp_w > 1000)
			fontsize = 48;
		else if (tmp_w >= 600)
		{
			fontsize = 32;
		}
		else
			fontsize = 16;
	}
	else
	{
		if (tmp_w > 2000)
			fontsize = 64;
		else if (tmp_w > 1800)
			fontsize = 48;
		else if (tmp_w > 1000)
			fontsize = 32;
		else if (tmp_w >= 600)
			fontsize = 16;
		else
			fontsize = 16;
	}

	w1 = mchnosd_get_str_width(mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), fontsize);
	w2 = mchnosd_get_str_width(osdstatus.attrlist[channelid].channelName, fontsize);
	if (nameAttr == NULL)
	{
		attr->rect.w = w1 > w2 ? w1: w2;
		attr->rect.w += 8;
		attr->rect.h = fontsize * 2 + (4*3);
		attr->rect.w += 3;
		attr->rect.w &= 0xFFFFFFFC;
	}
	else
	{
		attr->rect.w = w1+8;
		attr->rect.h = fontsize + (4*2);
		attr->rect.w += 3;
		attr->rect.w &= 0xFFFFFFFC;

		nameAttr->rect.w = w2+8;
		nameAttr->rect.h = attr->rect.h;
		nameAttr->rect.w += 3;
		nameAttr->rect.w &= 0xFFFFFFFC;

		nameAttr->type = OSDDRV_COLOR_TYPE_16_ARGB1555;
	}

	mchnosd_pos_e attrPos;
	if (nameAttr)
		attrPos = osdstatus.attrlist[channelid].timePos;
	else
		attrPos = osdstatus.attrlist[channelid].position;

	switch (attrPos)
	{
	default:
	case MCHNOSD_POS_LEFT_TOP:
		attr->rect.x = 4;
		attr->rect.y = 4;
		break;
	case MCHNOSD_POS_LEFT_BOTTOM:
		attr->rect.x = 4;
		attr->rect.y = st.height - attr->rect.h - 4;
		break;
	case MCHNOSD_POS_RIGHT_TOP:
		attr->rect.x = st.width - attr->rect.w - 4;
		attr->rect.y = 4;
		break;
	case MCHNOSD_POS_RIGHT_BOTTOM:
		attr->rect.x = st.width - attr->rect.w - 4;
		attr->rect.y = st.height - attr->rect.h - 4;
		break;
	}

//	switch (osdstatus.attrlist[channelid].position)
//	{
//	default:
//		break;
//	case MCHNOSD_POS_LEFT_TOP:
//		osdstatus.attrlist[channelid].timePos = MCHNOSD_POS_RIGHT_BOTTOM;
//		break;
//	case MCHNOSD_POS_LEFT_BOTTOM:
//		osdstatus.attrlist[channelid].timePos = MCHNOSD_POS_RIGHT_TOP;
//		break;
//	case MCHNOSD_POS_RIGHT_TOP:
//		osdstatus.attrlist[channelid].timePos = MCHNOSD_POS_LEFT_BOTTOM;
//		break;
//	case MCHNOSD_POS_RIGHT_BOTTOM:
//		osdstatus.attrlist[channelid].timePos = MCHNOSD_POS_LEFT_TOP;
//		break;
//	}

	if (nameAttr)
	{
		//nameAttr->jv_osddrv_inv_col_en = osdstatus.attrlist[channelid].osdbInvColEn;
		int offset;
		offset = osdstatus.attrlist[channelid].position == osdstatus.attrlist[channelid].timePos
				? attr->rect.h + 4: 0;

		switch (osdstatus.attrlist[channelid].position)
		{
		default:
		case MCHNOSD_POS_LEFT_TOP:
			nameAttr->rect.x = 4;
			nameAttr->rect.y = 4 + offset;
			break;
		case MCHNOSD_POS_LEFT_BOTTOM:
			nameAttr->rect.x = 4;
			nameAttr->rect.y = st.height - nameAttr->rect.h - 4 - offset;
			break;
		case MCHNOSD_POS_RIGHT_TOP:
			nameAttr->rect.x = st.width - nameAttr->rect.w - 4;
			nameAttr->rect.y = 4 + offset;
			break;
		case MCHNOSD_POS_RIGHT_BOTTOM:
			nameAttr->rect.x = st.width - nameAttr->rect.w - 4;
			nameAttr->rect.y = st.height - nameAttr->rect.h - 4 - offset;
			break;
		}
	}

	return fontsize;
}

int mchnosd_stop(int channelid)
{
	if (channelid>= OSD_CHN_CNT)
			return 0;
	pthread_mutex_lock(&osdstatus.mutex);
	if (osdstatus.osdhandle[channelid] != -1)
	{
		jv_osddrv_destroy(osdstatus.osdhandle[channelid]);
		osdstatus.osdhandle[channelid] = -1;
	}
	if (osdstatus.osdNameHandle[channelid] != -1)
	{
		jv_osddrv_destroy(osdstatus.osdNameHandle[channelid]);
		osdstatus.osdNameHandle[channelid] = -1;
	}
	if (osdstatus.osdSectionHandle[channelid] != -1)
	{
		jv_osddrv_destroy(osdstatus.osdSectionHandle[channelid]);
		osdstatus.osdSectionHandle[channelid] = -1;
	}
	osdstatus.needFlush = FALSE;
	pthread_mutex_unlock(&osdstatus.mutex);
	return 0;
}

BOOL mchnosd_get_seperate()
{
	//�رշֿ�����
	//if (ipcinfo_get_type() != IPCTYPE_SW)
		return jv_osddrv_max_region() > 1;
	//return FALSE;
}

int multiple_set_param(int mposition,int malignment)
{
	multiOsd_info.attrPos = mposition;
	multiOsd_info.attrAlign = malignment;
	return 0;
}
static int _multiple_osd_position(int channelid,jv_osddrv_region_attr* attr,mchnosd_region_t *region)
{
	mstream_attr_t st;
	int i;
	char timestr[32];
	mstream_get_running_param(channelid,&st);
	int tmp_w = st.width;
	JVRotate_e rotate = msensor_get_rotate();
	if(rotate==JVSENSOR_ROTATE_90 || rotate==JVSENSOR_ROTATE_270)
		tmp_w = st.height;
	int fontsize;
	if(st.width > 2000)
		fontsize = region->mainFontSize + region->subFontSize;
	else if (st.width > 1800)
	{
		fontsize = region->mainFontSize;
#if (defined PLATFORM_hi3518EV200) ||  (defined PLATFORM_hi3516EV100)
		if(hwinfo.encryptCode == ENCRYPT_200W && region->mainFontSize > 36)
			fontsize = 36;
#endif
	}
	else if (st.width > 1000)
	{
		if(region->mainFontSize > 40)
			fontsize = 40;
		else
			fontsize = region->mainFontSize;
	}
	else if(st.width >= 900)
	{
		fontsize = region->subFontSize;
	}
	else if (st.width >= 700)
	{
		if(region->subFontSize > 22)
			fontsize = 22;
		else
			fontsize = region->subFontSize;
	}
	else if (st.width >= 600)
	{
		if(region->subFontSize > 20)
			fontsize = 20;
		else
			fontsize = region->subFontSize;
	}
	else
		fontsize = 16;
	
	
	int tmp_fontsize;
	if (osdstatus.attrlist[channelid].bLargeOSD)
		{
			if (tmp_w > 2000)
				tmp_fontsize = 80;
			else if (tmp_w > 1800)
				tmp_fontsize = 64;
			else if (tmp_w > 1000)
				tmp_fontsize = 48;
			else if (tmp_w >= 600)
			{
				tmp_fontsize = 32;
			}
			else
				tmp_fontsize = 16;
		}
		else
		{
			if (tmp_w > 2000)
				tmp_fontsize = 64;
			else if (tmp_w > 1800)
				tmp_fontsize = 48;
			else if (tmp_w > 1000)
				tmp_fontsize = 32;
			else if (tmp_w >= 600)
				tmp_fontsize = 16;
			else
				tmp_fontsize = 16;
		}

	int max_w = 0;
	for(i = 0;i < TEXT_LINE_NUM;i++)
	{
		region->text_w[channelid][i] = mchnosd_get_str_width(region->text[i], fontsize);
		max_w = max_w > region->text_w[channelid][i] ? max_w : region->text_w[channelid][i];
		region->max_w[channelid] = max_w;
	}
	attr->rect.w = max_w;
	attr->rect.w += 8;
	attr->rect.w &= 0xFFFFFFFC;
	attr->rect.h = region->rows*fontsize + (4*(region->rows));//+(region->rows)*4;
	attr->rect.h += 8;
	attr->rect.h &= 0xFFFFFFFC;

/*	multiosd_pos_e timePos = (multiosd_pos_e)osdstatus.attrlist[channelid].timePos;
	multiosd_pos_e namePos = (multiosd_pos_e)osdstatus.attrlist[channelid].position;
	int tmp_num = multiOsd_info.attrPos - timePos;
	int name_num = multiOsd_info.attrPos - namePos;
	int line_num = timePos - namePos;
	tmp_num = tmp_num > 0 ? tmp_num : -tmp_num;			
	name_num = name_num > 0 ? name_num : -name_num;		
	line_num = line_num > 0 ? line_num : -line_num;		
	
//	printf("time zong:%d----st.width:%d--\n",16 + attr->rect.w + mchnosd_get_str_width(mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), tmp_fontsize),st.width);
	if(line_num != 2 && (namePos != MULTIOSD_POS_HIDE || timePos != MULTIOSD_POS_HIDE))	//�ж�time��name����ͬһ�У�����OSD��Ҫƫ��
	{
		if((16 + attr->rect.w + mchnosd_get_str_width(mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), tmp_fontsize)) >= st.width && tmp_num == 2 
			&& multiOsd_info.attrPos != MULTIOSD_POS_HIDE && timePos != MULTIOSD_POS_HIDE)	//ʱ��̫��
		{
			timePos = multiOsd_info.attrPos;
		}
 		if((16 + attr->rect.w + mchnosd_get_str_width(osdstatus.attrlist[channelid].channelName, tmp_fontsize)) >= st.width && tmp_num == 2 
			&& multiOsd_info.attrPos != MULTIOSD_POS_HIDE && namePos!= MULTIOSD_POS_HIDE)	//�豸��̫��
		{
			namePos = multiOsd_info.attrPos;
		}
	}
	int offset;
	offset = namePos == timePos
			? ( namePos == multiOsd_info.attrPos ? tmp_fontsize * 2 + 6*4 : 0)
			: ((namePos == multiOsd_info.attrPos) || 
			(timePos == multiOsd_info.attrPos) ? tmp_fontsize + 2*4 : 0);
//	printf("rect.y=%d, \n",attr->rect.y);
	switch(multiOsd_info.attrPos)
	{
	default:
	case MULTIOSD_POS_LEFT_TOP:
		attr->rect.x = 4;
		attr->rect.y = 4 + offset;
		break;
	case MULTIOSD_POS_LEFT_BOTTOM:
		attr->rect.x = 4;
		attr->rect.y = st.height - attr->rect.h - 4 - offset;		
		break;
	case MULTIOSD_POS_RIGHT_TOP:
		if(st.width < attr->rect.w)
			attr->rect.x = 4;
		else
			attr->rect.x = st.width - attr->rect.w - 4;
		attr->rect.y = 4 + offset;
		break;
	case MULTIOSD_POS_RIGHT_BOTTOM:
		if(st.width < attr->rect.w)
			attr->rect.x = 4;
		else
			attr->rect.x = st.width - attr->rect.w - 4;
		attr->rect.y = st.height - attr->rect.h - 4 - offset;
		break;
	}

*/	
	int endX,endY;
	attr->rect.x = st.width * region->x/st.viWidth;
	attr->rect.y = st.height * region->y/st.viHeight;
	endX = st.width * region->endx/st.viWidth;
	endY = st.height * region->endy/st.viHeight;
	if(endX >= st.width)
	{
		attr->rect.x = st.width - attr->rect.w - 4;
	}
	if(endY >= st.height)
	{
		attr->rect.y = st.height - attr->rect.h - 4;
	}
	if(attr->rect.x + max_w > st.width)
	{
		attr->rect.x = st.width - max_w - 4;
	}
	if(attr->rect.y + attr->rect.h > st.height)
	{
		attr->rect.y = st.height - attr->rect.h - 4;
	}
	
	if(st.width < 512)
	{
		attr->rect.x = 0;
	}
//	printf("--channel=%d----width=%d,height=%d---------\n",channelid,st.width,st.height);
//	printf("-----------x,y,w,h=%d,%d,%d,%d--\n",attr->rect.x,attr->rect.y,attr->rect.w,attr->rect.h);
	return fontsize;
}

/*
 *@brief �����û��Զ�����ʾ����
 *
 *@param channelid ͨ���ţ�-1��ʾ����ͨ��
 *@param param �����С�Ȳ���
 */
int mchnosd_region_create(int channelid, mchnosd_region_t *region)
{
	int i;
	int handle = -1;

	for (i=0;i<MAX_USERDEF_REGION_CNT;i++)
	{
		if (!osdstatus.region[i].bEnable)
		{
			handle = i;
			break;
		}
	}
	if (handle == -1)
	{
		printf("ERROR: no enough userdef region\n");
		pthread_mutex_unlock(&osdstatus.mutex);
		return JVERR_NO_FREE_RESOURCE;
	}

	osdstatus.region[handle].bEnable = TRUE;
	memcpy(&osdstatus.region[handle].region, region, sizeof(mchnosd_region_t));

	int cs,ce;
	if (channelid == -1)
	{
		cs = 0;
		ce = HWINFO_STREAM_CNT;
	}
	else
	{
		cs = channelid;
		ce = channelid+1;
	}
	jv_osddrv_region_attr attr;
	attr.jv_osddrv_inv_col_en=FALSE;
	attr.type=OSDDRV_COLOR_TYPE_16_ARGB1555;

	osdstatus.region[handle].osdcnt=0;
         
    for (;cs<ce;cs++)
    {
    	int fontsize;
		fontsize = _multiple_osd_position(cs,&attr,&osdstatus.region[handle].region);
		

    	osdstatus.region[handle].fontsize[cs] = fontsize;
    	osdstatus.region[handle].osdhandle[cs] = jv_osddrv_create(cs,&attr);
		if(osdstatus.region[handle].osdhandle[cs]<0)
		{
			osdstatus.region[handle].bEnable = FALSE;
			printf("----->jv_osddrv_create error F-->%s L-->%d\n",__FILE__,__LINE__);
			pthread_mutex_unlock(&osdstatus.mutex);
			return JVERR_NO_FREE_RESOURCE;			
		}
    	osdstatus.region[handle].channelid[cs] = cs;
    	osdstatus.region[handle].osdcnt++;
    }


	return handle;
}

int mchosd_region_stop()
{
	if(multiOsd_info.multiOsdHandle != -1)
	{
		mchnosd_region_destroy(multiOsd_info.multiOsdHandle);
		multiOsd_info.multiOsdHandle = -1;
	}
	return 0;
}
/*
 *@brief �����û��Զ�����ʾ����
 *
 *@param handle region�ľ��,  #mchnosd_region_create �ķ���ֵ
 */
int mchnosd_region_destroy(int handle)
{
	pthread_mutex_lock(&osdstatus.mutex);

	region_userdef_t *uregion = &osdstatus.region[(int)handle];
	multiOsd_info.multiOsdHandle = -1;
	int i;
	for (i=0;i<uregion->osdcnt;i++)
	{
		jv_osddrv_destroy(uregion->osdhandle[i]);
	}
	memset(uregion, 0, sizeof(region_userdef_t));
	pthread_mutex_unlock(&osdstatus.mutex);

	return 0;
}

/**
 *@brief
 *
 *@param handle region�ľ���� #mchnosd_region_create �ķ���ֵ
 *@param draw
 */
int mchnosd_region_draw(int handle, mchnosd_section_attr *draw)
{
	pthread_mutex_lock(&osdstatus.mutex);
	if ((int)handle > MAX_USERDEF_REGION_CNT || (int)handle < 0)
	{
		pthread_mutex_unlock(&osdstatus.mutex);	
		multiOsd_info.multiOsdHandle = handle;
		return JVERR_BADPARAM;
	}
	region_userdef_t *uregion = &osdstatus.region[(int)handle];
	multiOsd_info.multiOsdHandle = handle;
	int i;
	for(i=0;i<uregion->osdcnt;i++)
	{
		if (uregion->osdhandle[i] < 0)
			continue;
		jv_assert(i < HWINFO_STREAM_CNT,return JVERR_BADPARAM);
		int j=0;
		int fontsize = uregion->fontsize[i];
		jv_osddrv_clear(uregion->osdhandle[i]);
		for(j=0;j<draw->text_line_num;j++)
		{
			utl_iconv_gb2312_fix(draw->text[j],sizeof(draw->text[j]));
			if(multiOsd_info.attrAlign == MULTIOSD_LEFT)
				mchnosd_draw_text_ex(uregion->channelid[i], uregion->osdhandle[i], 4, 4+j*(fontsize+4), draw->text[j], fontsize, TRUE);
			else
				mchnosd_draw_text_ex(uregion->channelid[i], uregion->osdhandle[i], 4+(uregion->region.max_w[uregion->channelid[i]]-uregion->region.text_w[uregion->channelid[i]][j]), 4+j*(fontsize+4), draw->text[j], fontsize, TRUE);
		}
		jv_osddrv_flush(uregion->osdhandle[i]);
	}
	pthread_mutex_unlock(&osdstatus.mutex);

	return 0;
}


static BOOL __need_seperate_osd()
{
	//�رշֿ�����....��ά��Ҳ�����˹���
//	if (ipcinfo_get_type() != IPCTYPE_SW)
		return jv_osddrv_max_region() > 1;
	return FALSE;
}

//static int check_ipc_type()
//{
//	if (ipcinfo_get_type() == IPCTYPE_JOVISION)
//		return JV_IPCTYPE_JOVISION;
//	return JV_IPCTYPE_SW;
//}

int mchnosd_snap_add_txt(int id,char *str)
{
	if(str)
	{
		strncpy(osdstatus_snap.attr[id].osdtxt,str,sizeof(osdstatus_snap.attr[0].osdtxt));
		osdstatus_snap.attr[id].bTxtShow=1;
	}
	return 0;
}
int mchnosd_snap_del_txt(int id)
{
	osdstatus_snap.attr[id].bTxtShow=0;
	return 0;
}

//ץͼͨ��OSD 4ͨ��
int __chnosd_snap_flush(int fontsize)
{
	int channelid = 0;
	jv_osddrv_region_attr attr, tmAttr;
	int chn_snap = 0;
	int grop_snap = 3;
	int txt_cnt=2;
	osdstatus_snap.bShowOSD = osdstatus.attrlist[channelid].bShowOSD;
	osdstatus_snap.attr[chn_snap].position = osdstatus.attrlist[channelid].position;
//	osdstatus_snap.attr[1].position = osdstatus.attrlist[channelid].position;
//	osdstatus_snap.attr[2].position = osdstatus.attrlist[channelid].position;
	char timestr[20];
	pthread_mutex_lock(&osdstatus_snap.mutex);
	if (osdstatus_snap.bShowOSD)
	{
		if(osdstatus_snap.attr[0].position != MCHNOSD_POS_HIDE)
		{
			if(fontsize <= 0)
				osdstatus_snap.fontsize[chn_snap] = 32;
			else
				osdstatus_snap.fontsize[chn_snap] = fontsize;
			attr.rect.x = 4;
			attr.rect.y = 4;
			attr.type = OSDDRV_COLOR_TYPE_16_ARGB1555;
			int w1 = mchnosd_get_str_width(mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), osdstatus_snap.fontsize[chn_snap]);
			int	w2 = mchnosd_get_str_width(osdstatus.attrlist[channelid].channelName, osdstatus_snap.fontsize[chn_snap]);
			attr.rect.w = w1 > w2 ? w1:w2;
			attr.rect.h = osdstatus_snap.fontsize[chn_snap] * txt_cnt + (4*3);
			if(osdstatus_snap.attr[0].bTxtShow)
			{
				txt_cnt++;
				int	w3 = mchnosd_get_str_width(osdstatus_snap.attr[0].osdtxt, osdstatus_snap.fontsize[chn_snap]);
				attr.rect.w = attr.rect.w > w3 ? attr.rect.w: w3;
				attr.rect.h = osdstatus_snap.fontsize[chn_snap] * txt_cnt + (4*3);
			}
			attr.rect.w += 8;
			attr.rect.w += 3;
			attr.rect.w &= 0xFFFFFFFC;
			if (osdstatus_snap.osdhandle[chn_snap] >= 0)
				jv_osddrv_destroy(osdstatus_snap.osdhandle[chn_snap]);
			osdstatus_snap.osdhandle[chn_snap] = jv_osddrv_create(grop_snap,&attr);
			if(osdstatus_snap.osdhandle[chn_snap]<0)
			{
				printf("----->jv_osddrv_create error F-->%s L-->%d\n",__FILE__,__LINE__);
				pthread_mutex_unlock(&osdstatus_snap.mutex);
				return 0;
			}
			mchnosd_draw_text(channelid, osdstatus_snap.osdhandle[chn_snap], 4, 4, mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), osdstatus_snap.fontsize[chn_snap]);
			mchnosd_draw_text(channelid, osdstatus_snap.osdhandle[chn_snap], 4, (attr.rect.h-12)/txt_cnt+4+2, osdstatus.attrlist[channelid].channelName, osdstatus_snap.fontsize[chn_snap]);
			if(osdstatus_snap.attr[0].bTxtShow)
			{
				mchnosd_draw_text(channelid, osdstatus_snap.osdhandle[chn_snap], 4, (attr.rect.h-12)/txt_cnt*2 + 4 + 2, osdstatus_snap.attr[0].osdtxt, osdstatus_snap.fontsize[chn_snap]);
			}
		}
		osdstatus.needFlush = TRUE;
	}
	else
	{
	    if(osdstatus_snap.attr[0].position != MCHNOSD_POS_HIDE)
        {
			if (osdstatus_snap.osdhandle[chn_snap] != -1)
			{
				jv_osddrv_destroy(osdstatus_snap.osdhandle[chn_snap]);
				osdstatus_snap.osdhandle[chn_snap] = -1;
			}
	    }
    }
	pthread_mutex_unlock(&osdstatus_snap.mutex);

	return 0;
}

int __seperate_osd_snap_flush(int fontsize)
{
	int channelid = 0;
	jv_osddrv_region_attr attr;
	int snap_time = 0;
	int snap_osd = 1;
	int grop_snap = 2;		//mstarֻ������������0,1����������2����ץͼ
	int txt_cnt=2;
	osdstatus_snap.bShowOSD = osdstatus.attrlist[channelid].bShowOSD;
	osdstatus_snap.attr[snap_time].position = osdstatus.attrlist[channelid].position;
	char timestr[20];
	pthread_mutex_lock(&osdstatus_snap.mutex);
	if (osdstatus_snap.bShowOSD)
	{
		if(osdstatus_snap.attr[0].position != MCHNOSD_POS_HIDE)
		{
			osdstatus_snap.fontsize[snap_time] = fontsize;
			attr.rect.x = 4;
			attr.rect.y = 4;
			attr.type = OSDDRV_COLOR_TYPE_16_ARGB1555;
			int w1 = mchnosd_get_str_width(mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), osdstatus_snap.fontsize[snap_time]);
			int w2 = mchnosd_get_str_width(osdstatus.attrlist[channelid].channelName, osdstatus_snap.fontsize[snap_time]);
			attr.rect.w = w1;
			attr.rect.h = osdstatus_snap.fontsize[snap_time];//osdstatus_snap.fontsize[snap_time] * txt_cnt + (4*3);
			
			attr.rect.w += 8;
			attr.rect.w += 3;
			attr.rect.w &= 0xFFFFFFFC;
			if (osdstatus_snap.osdhandle[snap_time] >= 0)
				jv_osddrv_destroy(osdstatus_snap.osdhandle[snap_time]);
			osdstatus_snap.osdhandle[snap_time] = jv_osddrv_create(grop_snap,&attr);
			if(osdstatus_snap.osdhandle[snap_time]<0)
			{
				printf("----->jv_osddrv_create error F-->%s L-->%d\n",__FILE__,__LINE__);
				pthread_mutex_unlock(&osdstatus_snap.mutex);
				return 0;
			}
			mchnosd_draw_text(channelid, osdstatus_snap.osdhandle[snap_time], 4, 4, mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), osdstatus_snap.fontsize[snap_time]);

			attr.rect.x = 4;
			attr.rect.y = 4 + osdstatus_snap.fontsize[snap_time] + 4;
			attr.rect.w = w2;
			attr.rect.h = osdstatus_snap.fontsize[snap_time];
			attr.rect.w += 8;
			attr.rect.w += 3;
			attr.rect.w &= 0xFFFFFFFC;
			if (osdstatus_snap.osdhandle[snap_osd] >= 0)
				jv_osddrv_destroy(osdstatus_snap.osdhandle[snap_osd]);
			osdstatus_snap.osdhandle[snap_osd] = jv_osddrv_create(grop_snap,&attr);
			if(osdstatus_snap.osdhandle[snap_osd]<0)
			{
				printf("----->jv_osddrv_create error F-->%s L-->%d\n",__FILE__,__LINE__);
				pthread_mutex_unlock(&osdstatus_snap.mutex);
				return 0;
			}
			mchnosd_draw_text(channelid, osdstatus_snap.osdhandle[snap_osd], 4, 4, osdstatus.attrlist[channelid].channelName, osdstatus_snap.fontsize[snap_time]);
	
		}
		osdstatus.needFlush = TRUE;
	}
	else
	{
		if(osdstatus_snap.attr[0].position != MCHNOSD_POS_HIDE)
		{
			if (osdstatus_snap.osdhandle[snap_time] != -1)
			{
				jv_osddrv_destroy(osdstatus_snap.osdhandle[snap_time]);
				osdstatus_snap.osdhandle[snap_time] = -1;
			}
			if (osdstatus_snap.osdhandle[snap_osd] != -1)
			{
				jv_osddrv_destroy(osdstatus_snap.osdhandle[snap_osd]);
				osdstatus_snap.osdhandle[snap_osd] = -1;
			}
		}
	}
	pthread_mutex_unlock(&osdstatus_snap.mutex);
	return 0;
}


int mosd_set_snap_flush(int fontsize)
{
	__chnosd_snap_flush(fontsize);
	return 0;
}

int mchnosd_ivp_cde_destroy()
{
#ifdef IVP_CDE_SUPPORT
	if(mivp_cde_bsupport())
	{
		int i;
		pthread_mutex_lock(&osdstatus_cde.mutex);
		for(i=0; i<OSD_CHN_CNT; i++)
		if (osdstatus_cde.osdhandle[i] != -1)
		{
			jv_osddrv_destroy(osdstatus_cde.osdhandle[i]);
			osdstatus_cde.osdhandle[i] = -1;
		}
		//printf("[%s] line:%d\n", __FUNCTION__, __LINE__);
		pthread_mutex_unlock(&osdstatus_cde.mutex);
	}
#endif

	return 0;
}

int mchnosd_ivp_cde_draw(int rate)
{
#ifdef IVP_CDE_SUPPORT
	if(mivp_cde_bsupport())
	{
		MIVP_t mivp;
		mivp_get_param(0,&mivp);
		int i;
		pthread_mutex_lock(&osdstatus_cde.mutex);
		if(osdstatus_cde.attrlist[0].bShowOSD)
		{
			if(rate >= mivp.st_cde_attr.nCDEThreshold)
			{
				osdstatus_cde.textColor = 0xFC00;
				osdstatus_cde.borderColor = 0x8000;
			}
			else
			{
				osdstatus_cde.textColor = osdstatus.textColor;
				osdstatus_cde.borderColor = osdstatus.borderColor;
			}
			sprintf(osdstatus_cde.attrlist[0].channelName,"Percent:%3d��", rate);
			for (i=0; i<1; i++)
			{
				mchnosd_draw_text(i, osdstatus_cde.osdhandle[i], 4, 4, osdstatus_cde.attrlist[0].channelName, osdstatus_cde.fontsize[i]);
			}
		}
		//printf("[%s] line:%d	rate=%d\n", __FUNCTION__, __LINE__, rate);
		pthread_mutex_unlock(&osdstatus_cde.mutex);
	}
#endif

	return 0;
}
//����ͳ��osd
int __chnosd_ivpcount_flush(int channelid)
{
	jv_osddrv_region_attr attr;
	MIVP_t mivp;
	mivp_count_get(0, &mivp);
	osdstatus_count.attrlist[channelid].bShowOSD = (mivp.st_count_attr.bShowCount && mivp.st_count_attr.bOpenCount)?1:0;
	osdstatus_count.attrlist[channelid].position = (mchnosd_pos_e)mivp.st_count_attr.eCountOSDPos;
	sprintf(osdstatus_count.attrlist[channelid].channelName,"����:%d    �뿪:%d    ",    mivp_count_in_get(),mivp_count_out_get());
	IVPCountColor_t color;
	mivp_count_show_color_get(&color);
	osdstatus_count.textColor = color.text;
	osdstatus_count.borderColor = color.border;
	osdstatus_count.clearColor = color.clear;
	pthread_mutex_lock(&osdstatus_count.mutex);
	if (osdstatus_count.attrlist[channelid].bShowOSD && mivp.st_count_attr.bOpenCount)
	{
		if (osdstatus_count.attrlist[channelid].position != MCHNOSD_POS_HIDE)
		{
			{
				mstream_attr_t st;
				mstream_get_running_param(channelid,&st);
				attr.rect.x = 4;
				attr.rect.y = 4;
				osdstatus_count.fontsize[channelid] = osdstatus.fontsize[channelid];
				int offset = 0;
				if(osdstatus.attrlist[channelid].position == osdstatus_count.attrlist[channelid].position)
					offset += osdstatus.fontsize[channelid]+4*4;
				if(osdstatus.attrlist[channelid].timePos == osdstatus_count.attrlist[channelid].position)
					offset += osdstatus.fontsize[channelid]+4*4;

				attr.type = OSDDRV_COLOR_TYPE_16_ARGB1555;
				int w2 = mchnosd_get_str_width(osdstatus_count.attrlist[channelid].channelName, osdstatus_count.fontsize[channelid]);
				attr.rect.w = w2 + 4*4;
				attr.rect.h = osdstatus_count.fontsize[channelid] + 4*2;
				attr.rect.w += 3;
				switch(osdstatus_count.attrlist[channelid].position)
				{
				case MCHNOSD_POS_LEFT_TOP:
					attr.rect.y += offset;
					break;
				case MCHNOSD_POS_LEFT_BOTTOM:
					attr.rect.y = st.height - attr.rect.h - offset;
					break;
				case MCHNOSD_POS_RIGHT_TOP:
					attr.rect.x = st.width - attr.rect.w;
					attr.rect.y += offset;
					break;
				case MCHNOSD_POS_RIGHT_BOTTOM:
					attr.rect.x = st.width - attr.rect.w;
					attr.rect.y = st.height - attr.rect.h - offset;
					break;
				default:
					break;
				}

				if (osdstatus_count.osdhandle[channelid] >= 0)
					jv_osddrv_destroy(osdstatus_count.osdhandle[channelid]);
				osdstatus_count.osdhandle[channelid] = jv_osddrv_create(
						channelid, &attr);
				if(osdstatus_count.osdhandle[channelid]<0)
				{
					printf("----->jv_osddrv_create error F-->%s L-->%d\n",__FILE__,__LINE__);
					pthread_mutex_unlock(&osdstatus_count.mutex);
					return 0;
				}
				mchnosd_draw_text(0, osdstatus_count.osdhandle[channelid], 4, 4,
						osdstatus_count.attrlist[channelid].channelName, osdstatus_count.fontsize[channelid]);
			}
		}
		osdstatus.needFlush = TRUE;
	}
	else
	{
	    if(osdstatus_count.attrlist[channelid].position != MCHNOSD_POS_HIDE)
        {
			if (osdstatus_count.osdhandle[channelid] != -1)
			{
				jv_osddrv_destroy(osdstatus_count.osdhandle[channelid]);
				osdstatus_count.osdhandle[channelid] = -1;
			}
	    }
    }
	pthread_mutex_unlock(&osdstatus_count.mutex);

	return 0;
}

//��Ⱥ�ܶ�osd
int __chnosd_ivpcde_flush(int channelid)
{
	jv_osddrv_region_attr attr;
	MIVP_t mivp;
	mivp_get_param(0,&mivp);
	osdstatus_cde.attrlist[channelid].bShowOSD = mivp.st_cde_attr.bEnable;
	osdstatus_cde.attrlist[channelid].position = MCHNOSD_POS_LEFT_TOP;
	int rate = mivp_cde_get_rate();
	if(rate >= mivp.st_cde_attr.nCDEThreshold)
	{
		osdstatus_cde.textColor = 0xFC00;
		osdstatus_cde.borderColor = 0x8000;
	}
	else
	{
		osdstatus_cde.textColor = osdstatus.textColor;
		osdstatus_cde.borderColor = osdstatus.borderColor;
	}
	sprintf(osdstatus_cde.attrlist[channelid].channelName,"Percent:%3d��", rate);
	pthread_mutex_lock(&osdstatus_cde.mutex);
	if (osdstatus_cde.attrlist[channelid].bShowOSD)
	{
		if (osdstatus_cde.attrlist[channelid].position != MCHNOSD_POS_HIDE)
		{
			{
				mstream_attr_t st;
				mstream_get_running_param(channelid,&st);
				attr.rect.x = 4;
				attr.rect.y = 4;
				osdstatus_cde.fontsize[channelid] = osdstatus.fontsize[channelid];
				int offset = 0;
				if(osdstatus.attrlist[channelid].position == osdstatus_cde.attrlist[channelid].position)
					offset += osdstatus.fontsize[channelid]+4*4;
				if(osdstatus.attrlist[channelid].timePos == osdstatus_cde.attrlist[channelid].position)
					offset += osdstatus.fontsize[channelid]+4*4;

				attr.type = OSDDRV_COLOR_TYPE_16_ARGB1555;
				int w2 = mchnosd_get_str_width(osdstatus_cde.attrlist[channelid].channelName, osdstatus_cde.fontsize[channelid]);
				attr.rect.w = w2 + 4*4;
				attr.rect.h = osdstatus_cde.fontsize[channelid] + 4*2;
				attr.rect.w += 3;
				switch(osdstatus_cde.attrlist[channelid].position)
				{
				case MCHNOSD_POS_LEFT_TOP:
					attr.rect.y += offset;
					break;
				case MCHNOSD_POS_LEFT_BOTTOM:
					attr.rect.y = st.height - attr.rect.h - offset;
					break;
				case MCHNOSD_POS_RIGHT_TOP:
					attr.rect.x = st.width - attr.rect.w;
					attr.rect.y += offset;
					break;
				case MCHNOSD_POS_RIGHT_BOTTOM:
					attr.rect.x = st.width - attr.rect.w;
					attr.rect.y = st.height - attr.rect.h - offset;
					break;
				default:
					break;
				}

				if (osdstatus_cde.osdhandle[channelid] >= 0)
				{
					jv_osddrv_destroy(osdstatus_cde.osdhandle[channelid]);
					osdstatus_cde.osdhandle[channelid] = -1;
				}
				osdstatus_cde.osdhandle[channelid] = jv_osddrv_create(channelid, &attr);
				if(osdstatus_cde.osdhandle[channelid]<0)
				{
					printf("[%s] line:%d\n",__FILE__,__LINE__);
					pthread_mutex_unlock(&osdstatus_cde.mutex);
					return 0;
				}
				mchnosd_draw_text(0, osdstatus_cde.osdhandle[channelid], 4, 4,
						osdstatus_cde.attrlist[channelid].channelName, osdstatus_cde.fontsize[channelid]);
			}
		}
		osdstatus.needFlush = TRUE;
	}
	else
	{
	    if(osdstatus_cde.attrlist[channelid].position != MCHNOSD_POS_HIDE)
        {
			if (osdstatus_cde.osdhandle[channelid] != -1)
			{
				jv_osddrv_destroy(osdstatus_cde.osdhandle[channelid]);
				osdstatus_cde.osdhandle[channelid] = -1;
			}
	    }
    }
	pthread_mutex_unlock(&osdstatus_cde.mutex);

	return 0;
}



/*********************** ��̨�˵� **********************/

int mvisca_draw_text_ex(int channelid, int osdHandle, int px, int py, char *text, int fontsize, BOOL color)
{
	U16 *pDstPixel;
	RECT rect;
	int ret, width, i=0, j=0;

	if (osdHandle == -1)
		return -1;

	unsigned short text_color = COLOR_TEXT;
	unsigned short border_color = COLOR_BORDER;

	if(color == TRUE)
	{
		text_color = 0x83E0;
		border_color = 0x8000;
	}

	width = mchnosd_get_str_width(text, fontsize);
	pDstPixel = (U16*)malloc(width*fontsize*2);
    if (pDstPixel == NULL)
	{
	    printf("%s  pPixel: No Enough Memmory!\n", __FUNCTION__);
		return -1;
    }
	ret = mchnosd_get_str_bmp_ex(text, (unsigned char*)pDstPixel, width, fontsize,	text_color);

	if (ret != 0)
	{
	    printf("%s  mchnosd_get_str_bmp Error!\n", __FUNCTION__);
	    free(pDstPixel);
	    pDstPixel = NULL;
		return ret;
    }

	rect.x = px;
	rect.y = py;
	rect.w = width;
	rect.h = fontsize;
	jv_osddrv_drawbitmap(osdHandle, &rect, (unsigned char*)pDstPixel);

	free(pDstPixel);
	pDstPixel = NULL;
	return 0;
}

int mvisca_osd_copy(mvisca_osd_attr *from, int len)
{
	pthread_mutex_lock(&mvisca_osd_group.mutex);
	memcpy(&mvisca_osd_group.osdattr, from, len);
	mvisca_osd_group.ondraw = TRUE;
	pthread_mutex_unlock(&mvisca_osd_group.mutex);

	return 0;
}

int mvisca_osd_build(int channelid)
{
	if(channelid >= VSC_OSD_CNT)
		return 0;
	pthread_mutex_lock(&mvisca_osd_group.mutex);
	mvisca_osd_group.created = TRUE;	//������̨��־����ΪTRUE
	jv_osddrv_region_attr attr;
	mstream_attr_t st;
	mstream_get_running_param(channelid,&st);
	if (osdstatus.attrlist[channelid].bLargeOSD)
	{
		if (st.width > 1800)
			mvisca_osd_group.fontsize[channelid] = 48;
		else if (st.width > 1000)
			mvisca_osd_group.fontsize[channelid] = 32;
		else if (st.width > 600)
			mvisca_osd_group.fontsize[channelid] = 24;
		else
			mvisca_osd_group.fontsize[channelid] = 16;
	}
	else
	{
		if (st.width > 1800)
			mvisca_osd_group.fontsize[channelid] = 32;
		else if (st.width > 600)
			mvisca_osd_group.fontsize[channelid] = 24;
		else
			mvisca_osd_group.fontsize[channelid] = 16;
	}

	attr.type = OSDDRV_COLOR_TYPE_16_ARGB1555;
	attr.rect.w = (OSD_MAX_COL-4)*mvisca_osd_group.fontsize[channelid]/2;
	attr.rect.h = OSD_MAX_ROW*(mvisca_osd_group.fontsize[channelid]+2);
	attr.rect.x = (st.width-attr.rect.w)/2;
	attr.rect.y = (st.height-attr.rect.h)/2;
	
	//printf("[%s]:%d	[%d*%d]	font=%d	x=%d,y=%d,w=%d,h=%d\n",__FUNCTION__,__LINE__,st.width,st.height,
	//		mvisca_osd_group.fontsize[channelid], attr.rect.x,attr.rect.y,attr.rect.w,attr.rect.h);
	if (mvisca_osd_group.handle[channelid] >= 0)
	{
		jv_osddrv_destroy(mvisca_osd_group.handle[channelid]);
		mvisca_osd_group.handle[channelid] = -1;
	}
	mvisca_osd_group.handle[channelid] = jv_osddrv_create(channelid, &attr);
	if(mvisca_osd_group.handle[channelid] < 0)
	{
		printf("[%s]:%d create osd error:%d\n",__FUNCTION__,__LINE__, mvisca_osd_group.handle[channelid]);
		mvisca_osd_group.created = FALSE;
		pthread_mutex_unlock(&mvisca_osd_group.mutex);
		return -1;
	}
	pthread_mutex_unlock(&mvisca_osd_group.mutex);
	
	return 0;
}

static void mvisca_osd_process(void *param)
{
	mvisca_osd_group.running = TRUE;
	pthreadinfo_add((char *)__func__);
	int i,j;

    while(mvisca_osd_group.running)
    {
		pthread_mutex_lock(&mvisca_osd_group.mutex);
		if(mvisca_osd_group.ondraw==TRUE)
		{
			for(j=0; j<VSC_OSD_CNT; j++)
			{
				if(mvisca_osd_group.handle[j]>=0)
				{		
					if(mvisca_osd_group.osdattr.newPage)//��ҳ�ػ�������
					{
						jv_osddrv_clear(mvisca_osd_group.handle[j]);
						for(i=0; i<OSD_MAX_ROW; i++)
						{
							if(strlen(mvisca_osd_group.osdattr.text[i]) > 1)//���ݹ���Ҳ����
							{
								utl_iconv_gb2312_fix(mvisca_osd_group.osdattr.text[i], sizeof(mvisca_osd_group.osdattr.text[i]));
								mvisca_draw_text_ex(j, mvisca_osd_group.handle[j], 4, 4+i*(mvisca_osd_group.fontsize[j]), mvisca_osd_group.osdattr.text[i], mvisca_osd_group.fontsize[j], FALSE);
							}
						}
					}
					else//��ҳ���ݸı����
					{
						int a = mvisca_osd_group.osdattr.change[0]-1;
						int b = mvisca_osd_group.osdattr.change[1]-1;
						if(a>-1 && strlen(mvisca_osd_group.osdattr.text[a])>1)
						{
							utl_iconv_gb2312_fix(mvisca_osd_group.osdattr.text[a], sizeof(mvisca_osd_group.osdattr.text[a]));
							if(mvisca_osd_group.osdattr.text[a][0] == 'o')//���������ù��ܾͱ�ɫ
								mvisca_draw_text_ex(j, mvisca_osd_group.handle[j], 4, 4+a*(mvisca_osd_group.fontsize[j]), mvisca_osd_group.osdattr.text[a], mvisca_osd_group.fontsize[j], TRUE);
							else
								mvisca_draw_text_ex(j, mvisca_osd_group.handle[j], 4, 4+a*(mvisca_osd_group.fontsize[j]), mvisca_osd_group.osdattr.text[a], mvisca_osd_group.fontsize[j], FALSE);
						}
						if(b>-1 && strlen(mvisca_osd_group.osdattr.text[b])>1)
						{
							utl_iconv_gb2312_fix(mvisca_osd_group.osdattr.text[b], sizeof(mvisca_osd_group.osdattr.text[b]));
							if(mvisca_osd_group.osdattr.text[b][0] == 'o')//���������ù��ܾͱ�ɫ
								mvisca_draw_text_ex(j, mvisca_osd_group.handle[j], 4, 4+b*(mvisca_osd_group.fontsize[j]), mvisca_osd_group.osdattr.text[b], mvisca_osd_group.fontsize[j], TRUE);
							else
								mvisca_draw_text_ex(j, mvisca_osd_group.handle[j], 4, 4+b*(mvisca_osd_group.fontsize[j]), mvisca_osd_group.osdattr.text[b], mvisca_osd_group.fontsize[j], FALSE);
						}
					}

					jv_osddrv_flush(mvisca_osd_group.handle[j]);
				}
			}
			mvisca_osd_group.ondraw = FALSE;
		}
		pthread_mutex_unlock(&mvisca_osd_group.mutex);
        usleep(1000);                
    }
}

int mvisca_osd_flush(int channelid)
{
	if(channelid >= VSC_OSD_CNT)
		return 0;
	
	pthread_mutex_lock(&mvisca_osd_group.mutex);
	if (mvisca_osd_group.handle[channelid] >= 0)
	{
		jv_osddrv_region_attr attr;
		int i;
		mstream_attr_t st;
		mstream_get_running_param(channelid,&st);
		if (osdstatus.attrlist[channelid].bLargeOSD)
		{
			if (st.width > 1800)
				mvisca_osd_group.fontsize[channelid] = 48;
			else if (st.width > 1000)
				mvisca_osd_group.fontsize[channelid] = 32;
			else if (st.width > 600)
				mvisca_osd_group.fontsize[channelid] = 24;
			else
				mvisca_osd_group.fontsize[channelid] = 16;
		}
		else
		{
			if (st.width > 1800)
				mvisca_osd_group.fontsize[channelid] = 32;
			else if (st.width > 600)
				mvisca_osd_group.fontsize[channelid] = 24;
			else
				mvisca_osd_group.fontsize[channelid] = 16;
		}

		attr.type = OSDDRV_COLOR_TYPE_16_ARGB1555;
		attr.rect.w = (OSD_MAX_COL-4)*mvisca_osd_group.fontsize[channelid]/2;
		attr.rect.h = OSD_MAX_ROW*(mvisca_osd_group.fontsize[channelid]+2);
		attr.rect.x = (st.width-attr.rect.w)/2;
		attr.rect.y = (st.height-attr.rect.h)/2;
		//printf("[%s]:%d	[%d*%d]	font=%d	x=%d,y=%d,w=%d,h=%d\n",__FUNCTION__,__LINE__,st.width,st.height,
		//	mvisca_osd_group.fontsize[channelid], attr.rect.x,attr.rect.y,attr.rect.w,attr.rect.h);
		jv_osddrv_destroy(mvisca_osd_group.handle[channelid]);
		mvisca_osd_group.handle[channelid] = -1;
		mvisca_osd_group.handle[channelid] = jv_osddrv_create(channelid, &attr);
		if(mvisca_osd_group.handle[channelid] < 0)
		{
			printf("[%s]:%d create osd error\n",__FUNCTION__,__LINE__);
			pthread_mutex_unlock(&mvisca_osd_group.mutex);
			return -1;
		}
		
		for(i=0; i<OSD_MAX_ROW; i++)
		{
			if(strlen(mvisca_osd_group.osdattr.text[i]) > 1)
			{
				utl_iconv_gb2312_fix(mvisca_osd_group.osdattr.text[i], sizeof(mvisca_osd_group.osdattr.text[i]));
				mvisca_draw_text_ex(channelid, mvisca_osd_group.handle[channelid], 4, 4+i*(mvisca_osd_group.fontsize[channelid]), mvisca_osd_group.osdattr.text[i], mvisca_osd_group.fontsize[channelid], FALSE);
			}
		}
		jv_osddrv_flush(mvisca_osd_group.handle[channelid]);
		mvisca_osd_group.ondraw = FALSE;
	}

	pthread_mutex_unlock(&mvisca_osd_group.mutex);

	return 0;
}
/*********************** ����Ϊ��̨�˵� **********************/

int mchnosd_flush(int channelid)
{
	if (channelid>= OSD_CHN_CNT)
  		return 0;
	jv_osddrv_region_attr attr, tmAttr;
	char timestr[20];
	jv_assert(channelid < HWINFO_STREAM_CNT,return JVERR_BADPARAM);

	pthread_mutex_lock(&osdstatus.mutex);
	if (osdstatus.attrlist[channelid].bShowOSD)
	{
		if (!__need_seperate_osd())
		{
			if(osdstatus.attrlist[channelid].position != MCHNOSD_POS_HIDE)
			{
				osdstatus.fontsize[channelid] = _mchnosd_build_osdattr(channelid, &attr, NULL);
				if (osdstatus.osdhandle[channelid] >= 0)
					jv_osddrv_destroy(osdstatus.osdhandle[channelid]);
				osdstatus.osdhandle[channelid] = jv_osddrv_create(channelid,&attr);
				if(osdstatus.osdhandle[channelid] < 0)
				{
					printf("----->jv_osddrv_create error F-->%s L-->%d\n",__FILE__,__LINE__);
					pthread_mutex_unlock(&osdstatus.mutex);
					return 0;
				}
				mchnosd_draw_text(channelid, osdstatus.osdhandle[channelid], 4, 4, mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), osdstatus.fontsize[channelid]);
				mchnosd_draw_text(channelid, osdstatus.osdhandle[channelid], 4, (attr.rect.h-12)/2 + 4 + 2, osdstatus.attrlist[channelid].channelName, osdstatus.fontsize[channelid]);
			}
		}
		else
		{
			osdstatus.fontsize[channelid] = _mchnosd_build_osdattr(channelid, &attr, &tmAttr);
			if (osdstatus.osdNameHandle[channelid] >= 0)
				jv_osddrv_destroy(osdstatus.osdNameHandle[channelid]);
			if (osdstatus.osdhandle[channelid] >= 0)
				jv_osddrv_destroy(osdstatus.osdhandle[channelid]);
			osdstatus.osdhandle[channelid] = -1;
			osdstatus.osdNameHandle[channelid] = -1;
			if(osdstatus.attrlist[channelid].position != MCHNOSD_POS_HIDE)
			{
				osdstatus.osdNameHandle[channelid] = jv_osddrv_create(channelid,&tmAttr);
				if(osdstatus.osdNameHandle[channelid] < 0)
				{
					printf("----->jv_osddrv_create error F-->%s L-->%d\n",__FILE__,__LINE__);
					pthread_mutex_unlock(&osdstatus.mutex);
					return 0;
				}
				mchnosd_draw_text(channelid, osdstatus.osdNameHandle[channelid], 4, 4, osdstatus.attrlist[channelid].channelName, osdstatus.fontsize[channelid]);
			}

			if(osdstatus.attrlist[channelid].timePos != MCHNOSD_POS_HIDE)
			{
				osdstatus.osdhandle[channelid] = jv_osddrv_create(channelid,&attr);
				if(osdstatus.osdhandle[channelid] < 0)
				{
					printf("----->jv_osddrv_create error F-->%s L-->%d\n",__FILE__,__LINE__);
					pthread_mutex_unlock(&osdstatus.mutex);
					return 0;
				}
				mchnosd_draw_text(channelid, osdstatus.osdhandle[channelid], 4, 4, mchnosd_time2str(osdstatus.attrlist[channelid].timeFormat, time(NULL),timestr), osdstatus.fontsize[channelid]);
			}
		}

		osdstatus.needFlush = TRUE;
	}
	else
	{
	    if(osdstatus.attrlist[channelid].position != MCHNOSD_POS_HIDE)
        {
			if (osdstatus.osdhandle[channelid] != -1)
			{
				jv_osddrv_destroy(osdstatus.osdhandle[channelid]);
				osdstatus.osdhandle[channelid] = -1;
			}
			if (osdstatus.osdNameHandle[channelid] != -1)
			{
				jv_osddrv_destroy(osdstatus.osdNameHandle[channelid]);
				osdstatus.osdNameHandle[channelid] = -1;
			}
	    }
    }
	pthread_mutex_unlock(&osdstatus.mutex);
#if (IVP_COUNT_SUPPORT ||(defined XW_MMVA_SUPPORT))
	if(mivp_count_bsupport())
		__chnosd_ivpcount_flush(channelid);
#endif
#ifdef IVP_CDE_SUPPORT
		if(mivp_cde_bsupport() && channelid == 0)
			__chnosd_ivpcde_flush(0);
#endif


	mvisca_osd_flush(channelid);

//#ifndef IVP_HIDE_SUPPORT
//	if(channelid == 0)
//	{
//		mivp_restart(0);
//	}
//#endif
	return 0;
}

//����ͨ������
int mchnosd_set_name(int channelid, char *strName)
{
	if (channelid == -1)
	{
		int i;
		for (i=0;i<HWINFO_STREAM_CNT;i++)
		{
			strncpy(osdstatus.attrlist[i].channelName, strName, sizeof(osdstatus.attrlist[i].channelName));
			mchnosd_flush(i);
		}
	}
	else
	{
		strncpy(osdstatus.attrlist[channelid].channelName, strName, sizeof(osdstatus.attrlist[channelid].channelName));
		mchnosd_flush(channelid);
	}
	
	return 0;
}

//��ɫ����
int mchnosd_set_be_invcol(int channelid,BOOL bInvColEn)
{
	if (channelid == -1)
	{
		int i;
		for (i = 0; i < HWINFO_STREAM_CNT; i++)
		{
			osdstatus.attrlist[i].osdbInvColEn = bInvColEn;
			mchnosd_stop(i);
			mchnosd_flush(i);
		}
	}
	else
	{
		osdstatus.attrlist[channelid].osdbInvColEn = bInvColEn;
		mchnosd_stop(channelid);
		mchnosd_flush(channelid);	}
	return 0;
}
/**
 *@brief ����ͨ���ŵ�����
 *
 *@param channelid ͨ���ţ�-1��ʾ����ͨ��
 *
 *@return 0 �ɹ�
 *
 */
int mchnosd_set_time_format(int channelid, char *timeFormat)
{
	if (channelid == -1)
	{
		int i;
		for (i=0;i<HWINFO_STREAM_CNT;i++)
		{
			strncpy(osdstatus.attrlist[i].timeFormat, timeFormat, sizeof(osdstatus.attrlist[i].timeFormat));
		}
	}
	else
	{
		strncpy(osdstatus.attrlist[channelid].timeFormat, timeFormat, sizeof(osdstatus.attrlist[channelid].timeFormat));
	}

	return 0;
}
static char content[24];                //����osd����
static int strwidth = -1;               //��¼Ԥ�õ����

ptzosdstatus_t *mosd_get_ptzstatus()
{
    return &ptzstatus;
}
static int _mosd_build_ptzosdattr(int channelid, BOOL seperate, jv_osddrv_region_attr *attr)
{
	mstream_attr_t st;
	int fontsize;
	int w1;

	attr->type = OSDDRV_COLOR_TYPE_16_ARGB1555;
	mstream_get_running_param(channelid,&st);
	//if(channelid == 1)
	//	printf("**************************###########*****   %d\n",st.width);

	if (osdstatus.attrlist[channelid].bLargeOSD)
	{
		if (st.width > 1800)
			fontsize = 64;
		else if (st.width > 1000)
			fontsize = 48;
		else if (st.width >= 600)
		{
			fontsize = 32;
		}
		else
			fontsize = 16;
	}
	else
	{
		if (st.width > 1800)
			fontsize = 48;
		else if (st.width > 1000)
			fontsize = 32;
		else if (st.width >= 600)
			fontsize = 16;
		else
			fontsize = 16;
	}
	
	w1 = mchnosd_get_str_width(ptzstatus.content, fontsize);
	attr->rect.w = w1;
	attr->rect.h = fontsize + (4*2);
	attr->rect.w += 11;
	attr->rect.w &= 0xFFFFFFFC;

	mchnosd_pos_e attrPos;
	if (seperate == TRUE)
	{
	    mchnosd_pos_e timePos;
        attrPos = osdstatus.attrlist[channelid].position;
        timePos = osdstatus.attrlist[channelid].timePos;
		printf("1attrPos  =  %d-----------\n",attrPos);
		
	    switch (attrPos)
	    {
	    default:
	    case MCHNOSD_POS_LEFT_TOP:
	    case MCHNOSD_POS_RIGHT_TOP:
	        if(timePos == MCHNOSD_POS_LEFT_BOTTOM)
	            attr->rect.x = st.width - attr->rect.w - 4;
	        else
	            attr->rect.x = 4;
	    	attr->rect.y = st.height - attr->rect.h - 4;
	    	break;
	    case MCHNOSD_POS_LEFT_BOTTOM:
	    case MCHNOSD_POS_RIGHT_BOTTOM:
	        if(timePos == MCHNOSD_POS_LEFT_TOP)
	            attr->rect.x = st.width - attr->rect.w - 4;
	        else
	            attr->rect.x = 4;
	    	attr->rect.y = 4;;
	    	break;
	    }
		
	}
	else
	{
		attrPos = osdstatus.attrlist[channelid].position;
	    switch (attrPos)
    	{
    	default:
    	case MCHNOSD_POS_LEFT_TOP:
    	case MCHNOSD_POS_RIGHT_TOP:
    	    attr->rect.x = (st.width - attr->rect.w - 4)/2;
    		attr->rect.y = st.height - attr->rect.h - 4;
    		break;
    	case MCHNOSD_POS_LEFT_BOTTOM:
    	case MCHNOSD_POS_RIGHT_BOTTOM:
    		attr->rect.x = (st.width - attr->rect.w - 4)/2;
    		attr->rect.y = 4;;
    		break;
    	}
	}

    //printf("%s  x:%3d  y:%3d  w:%3d  h:%3d  %s  [%s]\n",__FUNCTION__, attr->rect.x, attr->rect.y, 
    //    attr->rect.w, attr->rect.h, ptzstatus.content, content);
	return fontsize;
}


