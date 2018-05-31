/*
 * utl_jpg2bmp.h
 *
 *  Created on: 2015-09-06
 *      Author: zwq
 */

#ifndef UTL_JPG2BMP_H_
#define UTL_JPG2BMP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/************************************
�ṩ������jpgת����bmp��ʽ�Ĺ���
input:	pJpgData,jpg�����ַ���������Ҫ����Ϣͷ
		jpgLen,ָʾjpg��ʽ�����ĳ���
output:	pBmpData,�ռ��ɵ��������ͷţ�bmp��ʽ����������bmp��Ϣͷ
		bmpLen,ָʾbmp��ʽ�����ĳ���
return:	�ɹ�����0�������쳣����-1
************************************/
int _transformJpg2Bmp(unsigned char *pJpgData, int jpgLen,unsigned char *pBmpData, int *bmpLen);

#ifdef __cplusplus
}
#endif

#endif
