#ifndef _UTL_ICONV_H_
#define _UTL_ICONV_H_

#ifdef __cplusplus
extern "C"
{
#endif
/**
 *@brief ����ַ�������������λ�Ƿ�����
 * �ڱ���Ƿ�ʱ����Ϊ�ַ����Ľ���
 *
 *@param str Ҫ�����ַ���
 *@param len �ַ����������󳤶�
 *
 */
int utl_iconv_gb2312_fix(char *str, int len);

/**
 *@brief ��GB2312ת��ΪUTF8����
 *@param src GB2312������ַ�����Ҳ������Ӣ�Ļ�����
 *@param des ���������
 *@param maxLen ��������󳤶�
 *
 *@note des�ĳ��ȣ�������src��1.5�������ܱ�֤�䲻Խ��
 *
 *@return ����
 */
int utl_iconv_gb2312toutf8(char *src, char *des, int maxLen);
/*
 * utf8תgb2312
 * ע��des�ĳ�������Ϊsrc������֮��
 */
void utl_iconv_utf8togb2312(char *src, char *des, int maxLen);

#ifdef __cplusplus
}
#endif

#endif

