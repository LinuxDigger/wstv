#ifndef _WEBPROXY_H_
#define _WEBPROXY_H_

/**
 *@brief �ص���������WEB�����лظ�ʱ����
 *@param buffer �յ������ݡ�����ֵΪNULLʱ����ζ����Ҫ�ر�SOCKET��
 *@param len �յ������ݵĳ���
 *
 */
typedef void (*received_from_webserver_callback)(unsigned char *buffer, int len, void *callback_param);


/**
 *@brief ��ʼ��
 *@param callback �յ�����ʱ�Ļص�����
 *@param webaddr WEB�����IP
 *@param webport WEB����Ķ˿�
 *
 *@return 0 �ɹ�
 *
 */
int proxy_init(received_from_webserver_callback callback, char *webaddr, unsigned short webport);

/**
 *@brief ����
 *
 *@return 0 �ɹ�
 *
 */
int proxy_deinit(void);

/**
 *@brief ���յ������ݣ�ͨ��127.0.0.1 ת����WEB����
 *@param data Ҫת��������
 *@param len ���ݳ���
 *
 *@return 0 �ɹ�-1 ����socketʧ��
 */
int proxy_send2server(unsigned char *data, int len, void *callback_param);

/**
 *@brief �ر���WEB���������
 *
 *
 */
void proxy_close_socket(void);


#endif

