#ifndef UTL_IPSCAN_H_
#define UTL_IPSCAN_H_

//lfx20130824
//���ļ��ṩ�Ĺ������ڵõ�һ�����е�IP��ַ

/**
 *@brief ɨ��ָ����IP��ַ�б��е����ݣ�������Ƿ�ռ��
 *
 *@param addrList Ҫɨ���IP��ַ�б������ֽ���
 *@param cnt Ҫɨ���IP��ַ�б�ĳ���
 *@param timeoutMS ��ʱʱ�䣬��λΪ���롣ʱ��Խ�������Խ׼ȷ
 *@param bUsed �������¼ɨ�赽��IP��ַ����Ч�ԡ�Ϊ�����ʾ�õ�ַ��Ч
 *
 *@return 0 �ɹ��� -1 ʧ��
 */
int utl_ipscan(unsigned int *addrList, int cnt, unsigned int timeoutMS, int *bUsed);


/**
 *@brief ɨ��ָ����IP��ַ���ڵ����Σ���ȡIP��ַ��ռ�õ��б�
 *
 *@param ipstr ��Ϊ�ο���IP��ַ
 *@param timeoutMS ��ʱʱ�䣬��λΪ���롣ʱ��Խ�������Խ׼ȷ
 *@param ipList IP��ַ��ռ�õ��б���ʽΪ�����ֽ���
 *@param maxCnt ��ָipList�����ɵ����IP��ַ����Ŀ
 *
 *@return ɨ�赽��IP��ַ�ĸ���, -1 ʧ��
 */
int utl_ipscan_local(const char *ipstr, unsigned int timeoutMS, unsigned int *ipList, int maxCnt);

/**
 *@brief ɨ��ָ����IP��ַ���ڵ����Σ���ȡIP��ַδ��ռ�õ��б�
 *
 *@param ipstr ��Ϊ�ο���IP��ַ
 *@param timeoutMS ��ʱʱ�䣬��λΪ���롣ʱ��Խ�������Խ׼ȷ
 *@param ipList IP��ַ���е��б���ʽΪ�����ֽ���
 *@param maxCnt ��ָipList�����ɵ����IP��ַ����Ŀ
 *
 *@return ɨ�赽��IP��ַ�ĸ���, -1 ʧ��
 */
int utl_ipscan_not_used(const char *ipstr, unsigned int timeoutMS, unsigned int *ipList, int maxCnt);

/**
 *@param ipstr ����IP��ַ����Ϊ�ο�ʹ�ã�����192.168.11.35
 *@param timeoutMS ��ʱʱ�䡣ʱ��Խ�������Խ׼ȷ
 *
 *@return ���е�IP��ַ�������ֽ����UINTֵ . 0 ��ʾδ�ҵ���-1��ʾʧ����
 *
 */
unsigned int utl_get_free_ip(const char *ipstr, unsigned int timeoutMS);

#endif

