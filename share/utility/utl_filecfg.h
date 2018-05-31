/**
 *@file ��ȡ�����ļ��õ��ļ�
 * ���ļ��ĸ�ʽΪ��ÿ��һ�������ÿ��������д����key=value
 * ǰ�߲�������"="����������"="������Ҫʹ��\=ת��
 *
 *
 */

#ifndef _UTL_FILECFG_H_
#define _UTL_FILECFG_H_

#ifndef SEEK_SET
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif

typedef struct
{
	char *key;
	char *value;
}keyvalue_t;

/**
 *@brief ��ָ���ļ��л�ȡKEYֵ��Ӧ��value
 *@param fname Ҫ��ȡ���ļ���
 *@param key Ҫ���ҵ�KEYֵ
 *
 *@return ���ҵ���KEY��Ӧ��ֵ
 *
 */
char *utl_fcfg_get_value(const char *fname, const char *key);

/**
 *@brief ��ָ���ļ��л�ȡKEYֵ��Ӧ��VALUE������ΪINT
 */
int utl_fcfg_get_value_int(const char *fname, const char *key, unsigned int defval);

/**
 *@brief ��ָ���ļ��л�ȡKEYֵ��Ӧ��value
 *@param fname Ҫ��ȡ���ļ���
 *@param key Ҫ���ҵ�KEYֵ
 *@param valuebuf �������
 *@param maxlen #valuebuf �ĳ���
 *
 *@return �ɹ�ʱΪvaluebuf������ΪNull
 *
 */
char *utl_fcfg_get_value_ex(const char *fname, const char *key, char *valuebuf, int maxlen);

/**
 *@brief ����ĳ��key�Ķ�Ӧ��ֵ
 *@param fname Ҫ���õ��ļ���
 *@param key Ҫ���õ�KEYֵ�����keyֵ����������
 *
 *@return 0 �ɹ�
 *
 */
int utl_fcfg_set_value(const char *fname, const char *key, char *value);

/**
 *@brief ׼��getnext
 */
int utl_fcfg_start_getnext(const char *fname);

/**
 *@brief ����getnext
 */
int utl_fcfg_end_getnext(const char *fname);

/**
 *@brief ���ļ��л�ȡ��һ��������
 * ������ѯ�ļ�������������
 *
 *@param fname Ҫ��ȡ���ļ���
 *@param current IN/OUT ��ʼֵΪ0.���ڼ�¼��ǰȡ����һ����
 *
 *@return ������һ��������
 *
 */
keyvalue_t *utl_fcfg_get_next(const char *fname, int *current);

/**
 *@brief ˢ����Ϣ���ļ���
 *
 *@param fname Ҫˢ�µ��ļ���
 *@return 0 �ɹ�
 *
 */
int utl_fcfg_flush(const char *fname);

/**
 *@brief ����ļ����棬����Ҳ����ʹ�ø��ļ�ʱ����
 *
 *@param fname Ҫ��ȡ���ļ���
 *
 *@note �������ļ��иı�ʱ�����Ե��ô˷�����ֱ��ˢ�¸��ļ�������
 *
 *@return 0 �ɹ�
 */
int utl_fcfg_close(const char *fname);

#endif

