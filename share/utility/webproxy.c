/**
 *@file ���ڲ��Դ���WEB������֮��
 *
 * ��ʹ�ñ�����������������͸���ܵĻ���֮�ϣ�ʹ���佨�������ӷ���WEB����
 *
 * ʵ�ַ�ʽ֮һ��
 *1������ͨ��������
 *2������ͨ���ض˽���������ʱ�Ķ˿ںŸ�֪�����򣨻�����������
 *3������ͨ�Ͽ��������
 *4������ͨ�ֿضˣ�ʹ��֮ǰ����ʱ�Ķ˿ں�IP����WEBҳ��
 *5�����ʳɹ�
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "webproxy.h"

#define PROXY_PRINTF(fmt...)  \
do{\
	if(1){	\
		printf("[%s]:%d ", __FILE__, __LINE__);\
		printf(fmt);} \
} while(0)
	
	
#define PROXY_PRINTF1(fmt...)  \
	do{\
		if(0){	\
			printf("[%s]:%d ", __FILE__, __LINE__);\
			printf(fmt);} \
	} while(0)

typedef struct{
	int sockver;		//sockweb�İ汾����Ҫ�Ǳ���_proxy_process�г��ֵģ�����Ľ��½�����socket�رյ�����
	int sockweb;		//��web�������ӵ�socket
	received_from_webserver_callback received_from_webserver_ptr;
	void *callback_param;

	unsigned char webaddr[20];//web�����IP
	unsigned short webport;	//web����Ķ˿�

	//�������̡߳�ͬ�����
	pthread_mutex_t mutex;
	pthread_t thread;
	int running;
}proxy_privacy_t;

static proxy_privacy_t sProxy;
#if 0

static int _proxy_recv(int sock, unsigned char *buffer, int maxlen)
{
	int ret;
	fd_set rfds;
	struct timeval tv;
	
	if (sock == -1)
	{
		PROXY_PRINTF(("ERROR: -1 socket\n"));
		return -1;
	}

	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);
	
	tv.tv_sec = 15;
	tv.tv_usec = 0;
	
	ret = select(sock+1, &rfds, NULL, NULL, &tv);
	/* Don't rely on the value of tv now! */
	
	if (ret == -1)
	   perror("select()");
	else if (ret)
	{
		ret = recv(sock, buffer, maxlen, 0);
	}

	return ret;
}

static void _proxy_process(void)
{
	int ret;
	int sockver;
	unsigned char buffer[10*1024];
	while(sProxy.running)
	{
		if (sProxy.sockweb == -1)
		{
			usleep(1000);
			continue;
		}
		sockver = sProxy.sockver;
		ret = recv(sProxy.sockweb, buffer, sizeof(buffer), 0);
		if(ret == -1)
		{
			PROXY_PRINTF("recv error: %s\n", strerror(errno));
		}
		else if (ret == 0)
		{
			pthread_mutex_lock(&sProxy.mutex);
			if (sProxy.sockweb != -1 && sockver == sProxy.sockver)
			{
				PROXY_PRINTF1("sProxy.sockweb closed: %d\n", sProxy.sockweb);
				//shutdown(sProxy.sockweb, SHUT_RDWR);
				close(sProxy.sockweb);
				sProxy.sockweb = -1;

				//֪ͨ�ϲ㣬�ر�SOCKET
				if (sProxy.received_from_webserver_ptr)
					sProxy.received_from_webserver_ptr(NULL, 0);
			}
			pthread_mutex_unlock(&sProxy.mutex);

		}
		else
		{
			buffer[ret] = '\0';
			PROXY_PRINTF1("sockweb received data: [%s]\n", buffer);
			if (sProxy.received_from_webserver_ptr)
				sProxy.received_from_webserver_ptr(buffer, ret);
		}
	}
}
#endif

/**
 *@brief ��ʼ��
 *@param callback �յ�����ʱ�Ļص�����
 *@param webaddr WEB�����IP
 *@param webport WEB����Ķ˿�
 *
 *@return 0 �ɹ�
 *
 */
int proxy_init(received_from_webserver_callback callback, char *webaddr, unsigned short webport)
{
	memset(&sProxy, 0, sizeof(sProxy));
	//pthread_mutex_init(&sProxy.mutex, NULL);
	//sProxy.running = 1;
	sProxy.sockweb = -1;
	sProxy.received_from_webserver_ptr = callback;
	strncpy((char *)sProxy.webaddr, webaddr, sizeof(sProxy.webaddr));
	sProxy.webport = webport;
	//pthread_create(&sProxy.thread, NULL, (void *)_proxy_process, NULL);
	return 0;
}

/**
 *@brief ����
 *
 *@return 0 �ɹ�
 *
 */
int proxy_deinit(void)
{
	//sProxy.running = 0;
	//pthread_join(sProxy.thread, NULL);
	//pthread_mutex_destroy(&sProxy.mutex);
	return 0;
}


/**
 *@brief ���յ������ݣ�ͨ��127.0.0.1 ת����WEB����
 *@param data Ҫת��������
 *@param len ���ݳ���
 *
 *@return 0 �ɹ�-1 ����socketʧ��
 */
int proxy_send2server(unsigned char *data, int len, void *callback_param)
{
	int ret = 0;
	int sock;
	unsigned char buffer[10*1024];
	struct sockaddr_in addr;

	sProxy.callback_param = callback_param;
	if (sProxy.sockweb == -1)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		{
			PROXY_PRINTF("create socket failed: %s\n", strerror(errno));
			ret = -1;
			goto END;
		}
		addr.sin_family = AF_INET;
		addr.sin_port = htons(sProxy.webport);
		addr.sin_addr.s_addr = inet_addr((const char *)sProxy.webaddr);//INADDR_LOOPBACK;
		
		PROXY_PRINTF1("test %d\n", __LINE__);
		ret = connect(sock, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in));
		PROXY_PRINTF1("test %d\n", __LINE__);
		if (ret == -1)
		{
			PROXY_PRINTF("connect error: %s\n", strerror(errno));
			close(sock);
			goto END;
		}
		sProxy.sockweb  = sock;
		sProxy.sockver++;
	}
	PROXY_PRINTF1("send: %s\n", data);
	ret = send(sProxy.sockweb, data, len, 0);
	PROXY_PRINTF1("sockweb sended\n");
	if (ret == -1)
	{
		PROXY_PRINTF("send failed: %s\n", strerror(errno));
	}
	while(1)
	{
		//ret = _proxy_recv(sProxy.sockweb, buffer, sizeof(buffer));
		PROXY_PRINTF1("test\n");
		ret = recv(sock, buffer, sizeof(buffer), 0);
		if (ret == -1)
		{
			PROXY_PRINTF("proxy_recv failed\n");
			close(sProxy.sockweb);
			sProxy.sockweb = -1;
			//usleep(1000*1000);
			//if (sProxy.received_from_webserver_ptr)
			//	sProxy.received_from_webserver_ptr(NULL, 0, sProxy.callback_param);
			break;
		}
		else if (ret == 0)
		{
			PROXY_PRINTF1("web closed the socket!\n");
			close(sProxy.sockweb);
			sProxy.sockweb = -1;
			//usleep(100*1000);
			if (sProxy.received_from_webserver_ptr)
				sProxy.received_from_webserver_ptr(NULL, 0, sProxy.callback_param);
			break;
		}
		else
		{
			PROXY_PRINTF1("web received data success!\n");
			buffer[ret] = '\0';
			PROXY_PRINTF1("[%s]\n", buffer);
			if (sProxy.received_from_webserver_ptr)
				sProxy.received_from_webserver_ptr(buffer, ret, sProxy.callback_param);
			ret = 0;
		}
	}
END:
	return ret;
}

/**
 *@brief �ر���WEB���������
 *
 *
 */
void proxy_close_socket(void)
{
	
	//pthread_mutex_lock(&sProxy.mutex);
	if (sProxy.sockweb != -1)
	{
		PROXY_PRINTF1("sProxy.sockweb closed: %d\n", sProxy.sockweb);
		shutdown(sProxy.sockweb, SHUT_RDWR);
		close(sProxy.sockweb);
		sProxy.sockweb = -1;
	}
	//pthread_mutex_unlock(&sProxy.mutex);
}


