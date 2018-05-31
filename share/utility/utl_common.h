#ifndef _UTL_COMMON_H_
#define _UTL_COMMON_H_


#define utl_WaitTimeout(flag, time_ms)			do\
												{\
													int n = ((time_ms) + 99) / 100;\
													while (!(flag) && n--)\
														utl_sleep_ms(100);\
												} while(0)
														

//��ȡϵͳ���е�ʱ�䣬��λ���롣�����ܵ��޸�ʱ���Ӱ��
unsigned long long utl_get_ms();

unsigned int utl_get_sec();

//˯��ms
void utl_sleep_ms(unsigned long ms);

// ��ȡ��ǰʱ����UTCʱ����������
short utl_GetTZDiffSec();

//ʱ���ʽ����ز���
typedef enum
{
	UTL_TIME_HHMMSS,
} TimeType_e;

/**
  *@brief	�޸Ķ�Ӧʱ���ʽʱ�� 
  *@param	type ʱ���ʽ
  *@param	value ����(����)
  *@return  ���ض�Ӧ��ʽ���޸ĺ��ʱ��, <0 ����  
  */
int utl_time_modify(TimeType_e type, int time, int value);

/**
  *@brief	�����Ӧʱ���ʽ��ʱ��� 
  *@param	type ʱ���ʽ
  *@param	stime ��ʼʱ��
  *@param	etime ����ʱ��
  *@return  ���ض�Ӧʱ����������
  */
int utl_time_range(TimeType_e type, int stime, int etime);

int utl_get_file_size(const char* filename);

int utl_copy_file(const char* srcfile, const char* dstfile);

#endif
