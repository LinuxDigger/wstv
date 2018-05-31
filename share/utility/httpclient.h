#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__


/*
	$Id: httpdownload.h, v1.0.0, 2011.6.28, YellowBug $
	$@@: http���� 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#define PATH_SPLIT			'/'

typedef int	HSOCKET;
typedef void* PARAM_MGR;

#define closesocket(s)		close(s)

#define BUFSIZE 4*1024


#define DEF_HTTP_PORT			80
#define HTTP_MAX_PATH			260
#define HTTP_MAX_REQUESTHEAD	1024
#define HTTP_MAX_RESPONSEHEAD	2048
#define HTTP_DOWN_PERSIZE		128	
#define HTTP_FLUSH_BLOCK		1024

/*
	$@@ ����״̬
	$@ HDL_URLERR: �ṩ��url�Ƿ�
	$@ HDL_SERVERFAL: ����host��ַ�޷��ҵ����� 
	$@ HDL_TIMEOUT: �������ص�ַ��ʱ
	$@ HDL_READYOK: �������ص�ַ�ɹ�,׼������
	$@ HDL_DOWNING: ��������
	$@ HDL_DISCONN: �Ͽ�������
	$@ HDL_FINISH: �������
*/
enum 
{
	HDL_URLERR = 0xe0,
	HDL_SERVERFAL = 0xe1,
	HDL_SOCKFAL = 0xe2,
	HDL_SNDREQERR = 0xe3,
	HDL_SERVERERR = 0xe4,
	HDL_CRLOCFILEFAL = 0xe5,
	HDL_TIMEOUT = 0x100,
	HDL_NORESPONSE = 0x101,
	HDL_READYOK = 0x104,
	HDL_DOWNING = 0x105,
	HDL_DISCONN = 0x106,
	HDL_FINISH = 0x107
};

/*
	$@@ ���ػص�����,��Ҫ���ڼ������ع���
	$@ ��һ������: ���ر�ʶ����(����ö���е�ֵ)
	$@ �ڶ�������: ��Ҫ���ص����ֽ���
	$@ ����������: �Ѿ����ص����ֽ���
	$@ ���ĸ�����: ������һ�����ص�ʱ���(����)
	$@ ����ֵ: �´����ص��ֽ���(0: ��ʾĬ��ֵ, С��0�������ж�, ����0��λ�´�����ָ�����ֽ�)
*/
typedef int (*HTTPDOWNLOAD_CALLBACK)(int, unsigned int, unsigned int, unsigned int);

/*
	$@@ ͨ��ָ����url������Դ
	$@ url: ������Դ��ַ
	$@ filepath: �洢�ڱ��ص�·��(���ΪNULL��洢�ڵ�ǰĿ¼)
	$@ filename: �洢���ļ���(���ΪNULL��λurl�з������ļ���)
	$@ http_func: ���ع��̼�������(���ΪNULL�򲻼������ع���)
*/
int pq_http_download(const char *url, const char *local_path, const char *local_file, 
			char *mem_addr, unsigned int *already_rec, HTTPDOWNLOAD_CALLBACK http_func);

int http_download(const char* url,const char* localpath,const char *local_file, 
		char *mem_addr, unsigned int *already_rec);
	

typedef struct __HTTP_T
{
	char camera_code[64]; 	
	char secret_key[64];
	char publish_web[64];				
}mhttp_attr_t;

int Http_get_message(char   *argv,mhttp_attr_t *attr) ;

int Http_get_message_ex(char *url, char *res, int len);

// URL���룬��' '��%��\���ַ�ת��
char* Http_url_encode(char* dst, const char* url);

// URL���룬��%��ͷ��ת���ַ�ת��Ϊԭʼ�ַ�������dst��url��ͬ
char* Http_url_decode(char* dst, const char* url);

/**
 *@brief ��ָ�������ַ����post����
 *@param url �����ַ
 *@param req ��������
 *@param len �������ݳ���
 *@param resp ���ص���Ӧ���ݣ���СӦΪBUFSIZE��С
 *@param timeout �ȴ���Ӧ��ʱʱ��
 *@return ���ص����ݵ��ܳ��ȡ�
 *@note �����ܳ���Ϊ��HTTPͷ�ĳ��� + Content-Length��ָ�������ݳ���
 */
int Http_post_message(const char *url, const char *req, int len, char *resp, int timeout);


// HTTP_GET��������
PARAM_MGR Http_create_param_mgr();

void Http_destroy_param_mgr(PARAM_MGR mgr);

int Http_add_param_string(PARAM_MGR mgr, const char* key, const char* value);

int Http_add_param_int(PARAM_MGR mgr, const char * key, int value);

int Http_param_sort(PARAM_MGR mgr);

int Http_remove_param(PARAM_MGR mgr, const char* key);

const char* Http_get_param_val(PARAM_MGR mgr, const char* key);

void Http_clear_param(PARAM_MGR mgr);

int Http_param_generate_value(const PARAM_MGR mgr, char* dst, int len);

int Http_param_generate_string(const PARAM_MGR mgr, char* dst, int len);

#endif

