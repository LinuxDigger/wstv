/**
 *@file utl_cmd.h ���ڵ��Ե�һ���������
 * 
 *@author Liu Fengxiang
 */

#ifndef _JV_CMD_H_
#define _JV_CMD_H_

/**
 *@brief �Ƿ�ʹ��cmd
 */
void utl_cmd_enable(int bEnable);

int utl_cmd_init();

int utl_cmd_deinit();

/**
 *@brief ����һ��������ڵ���
 *@param cmd ��������
 *@param help_general ����Ĵ��˵��
 *@param help_detail �������ϸ����
 *@param func �������еĺ���ָ�룬�䷵��ֵΪ0ʱ��ʾ��ȷ��
 *	����ֵ������Ϊִ��ʧ��
 *
 *@note cmd,help_general,help_detail �ڱ�ģ���ڲ���δΪ������ڴ棬ֻ��������ָ��
 *
 *@return 0 �ɹ���JVERR_NO_RESOURCE ������ӵ�̫����
 */
int utl_cmd_insert(char *cmd, char *help_general, char *help_detail, int (*func)(int argc, char *argv[]));

/**
 *@brief ���׼����system���ƣ�ִ��ָ��������
 *
 *@param cmd Ҫִ�е�����
 *
 *
 */
int utl_cmd_system(char *cmd);

/**
 *@brief ��ȡ������Ϣ
 *@param cmd Ҫ��ȡ������Ϣ������������ΪNULLʱ��ʾ��ȡ�����б�
 *
 *@return ���ذ����ַ�����ʧ�ܷ���NULL
 *@note ���ص��ַ�������malloc������ڴ棬������Ҫ�ͷ�
 *
 */
char *utl_cmd_get_help(char *cmd);

//�����ú���������߳���Ϣ
void pthreadinfo_add(const char *threadName);
void pthreadinfo_list(void);
int utl_thread_getpid();

#endif

