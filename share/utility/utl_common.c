#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <sys/select.h>
#include "utl_common.h"


//��ȡϵͳ���е�ʱ�䣬��λ���롣�����ܵ��޸�ʱ���Ӱ��
unsigned long long utl_get_ms()
{
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
}

unsigned int utl_get_sec()
{
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return tv.tv_sec;
}

//˯��ms
void utl_sleep_ms(unsigned long ms)
{
	struct timeval start;
	start.tv_sec = 0;
	start.tv_usec = ms*1000;
	select(0,NULL,NULL,NULL,&start);
};

// ��ȡ��ǰʱ����UTCʱ����������
short utl_GetTZDiffSec()
{
	time_t tmp = time(NULL);
	struct tm localtm, gmtm;

	localtime_r(&tmp, &localtm);
	gmtime_r(&tmp, &gmtm);

	return (long)mktime(&localtm) - (long)mktime(&gmtm);
}

/**
  *@brief	�޸Ķ�Ӧʱ���ʽʱ�� 
  *@param	type ʱ���ʽ
  *@param	value ����(����)
  *@return  ���ض�Ӧ��ʽ���޸ĺ��ʱ��, <0 ����  
  */
int utl_time_modify(TimeType_e type, int time, int value)
{
	int ret = -1;
	int hour, min, sec;

	switch(type)
	{
		case UTL_TIME_HHMMSS:
			{
				sec = time % 100 + value % 60;
				if(value < 0)
				{
					min  = time % 10000 / 100 + (sec < 0 ? -1 : 0);
					if(sec < 0)
						sec += 60;
					hour = time / 10000 + (min < 0 ? -1 : 0);
					if(min < 0)
						min += 60;
					if(hour < 0)
						hour += 24; 
					ret = hour * 10000 + min % 60 * 100 + sec % 60;
				}
				else
				{
					min  = time % 10000 / 100 + value % 3600 / 60 + (sec >= 60 ? 1 : 0);
					hour = time / 10000 + value / 3600 + (min >= 60 ? 1 : 0);
					ret = hour % 24 * 10000 + min % 60 * 100 + sec % 60;
				}
			}
			break;
		default:
			break;
	}
	return ret;
}

/**
  *@brief	�����Ӧʱ���ʽ��ʱ��� 
  *@param	type ʱ���ʽ
  *@param	stime ��ʼʱ��
  *@param	etime ����ʱ��
  *@return  ���ض�Ӧʱ����������
  */
int utl_time_range(TimeType_e type, int stime, int etime)
{
	int ret = -1;
	int sHour, sMin, sSec;
	int eHour, eMin, eSec;

	switch(type)
	{
		case UTL_TIME_HHMMSS:
			{
				sHour = stime / 10000;
				sMin  = (stime % 10000) / 100;
				sSec  = (stime % 10000) % 100; 
				
				eHour = etime / 10000;
				eMin  = (etime % 10000) / 100;
				eSec  = (etime % 10000) % 100; 
				ret = (eHour - sHour) * 3600 + (eMin - sMin) * 60 + (eSec - sSec);
			}
			break;
		default:
			break;
	}
	return ret;
}

int utl_get_file_size(const char* filename)
{
	struct stat filestat;

	if (access(filename, F_OK) != 0)
	{
		return 0;
	}
	if (stat(filename, &filestat) != 0)
	{
		return 0;
	}

	return filestat.st_size;
}

int utl_copy_file(const char* srcfile, const char* dstfile)
{
	FILE	*fin = NULL, *fout = NULL;
	char	buffer[4096] = {0};
	int		bytes;

	fin	= fopen(srcfile, "rb");
	if(fin == NULL)
	{
		return -1;
	}
	fout = fopen(dstfile, "wb");
	if(fout == NULL)
	{
		fclose(fin);
		return -1;
	}
	while(!feof(fin))
	{
		bytes = fread(buffer, 1, 4096, fin);
		fwrite(buffer, 1, bytes, fout);
	}
	fclose(fout);
	fclose(fin);

	return 0;
}

