#ifndef _OSD_SERVER_H_
#define _OSD_SERVER_H_

typedef struct _GQMsgHeader_t
{
	unsigned char head_flag;
	unsigned char version;
	unsigned char reserv01;
	unsigned char reserv02;
	unsigned int sessionId;
	unsigned int sequence_num;
	unsigned char channel;
	unsigned char end_flag;
	unsigned short msg_id;
	unsigned int data_len;
}GQMsgHeader_t;

/**
 *@brief ��ʼ��osd_serverģ��
 * ��ģ�������������Ƕ���ַ�������ͨѶ�����OSD�ַ����ӵĹ���
 *
 */
int osd_server_init(void);

/**
 *@brief ����
 *
 */
int osd_server_deinit(void);

/**
 *@brief �������һ��JSON��ʽ���ַ���������
 *@param buf JSON��ʽ����
 *@param detail	�����ؽ��
 *@param bAccessCheck	�Ƿ����û�Ȩ��
 */
int process_osd_msg(char *buf, char *detail, int bAccessCheck);

#endif


