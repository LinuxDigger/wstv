#ifndef _WEBER_H_
#define _WEBER_H_

#define WEBER_CMD_PIPE		CONFIG_PATH"/pipe/weber.sck"

//#define MAX_CMD_LEN	1024*5
#define MAX_CMD_LEN	1024*10  //����������webrecord��event_get���ܣ�ԭ�е���Ϣ���Ȳ�����

/**
 *@brief ��ʼ��web������ĸ���ģ��
 * ��ģ�����������������CGI����ͨѶ��������������
 *
 */
int weber_init(void);

/**
 *@brief ����
 *
 */
int weber_deinit(void);

/**
 *@brief ���÷���ֵ�ַ���
 *
 */
int weber_set_result(char *result);

#endif


