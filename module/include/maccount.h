/**
 *@file maccount.h �ʻ�����
 * Copyright (C) 2011 Jovision Technology Co., Ltd.
 * ���ļ�������֯�ʺŹ�����ش��룬���ǵ��ʺ�ģ��Ƚ϶�������ģ���һЩ�궨�壬�ṹ�����ļ���
 * ������ʷ���svn�汾����־
 */

#ifndef __MACCOUNT_H__
#define __MACCOUNT_H__

#define	SIZE_ID			20
#define	SIZE_PW			20
#define SIZE_DESCRIPT	32

/**
 *@brief �������صĴ����
 *
 */
#define	ERR_EXISTED		0x1	///< �û��Ѵ���
#define	ERR_LIMITED		0x2	///< �û�̫�࣬����������
#define	ERR_NOTEXIST	0x3	///< ָ�����û�������
#define	ERR_PASSWD		0x4	///< �������
#define	ERR_PERMISION_DENIED	0x5

#define	ERR_EXISTED_EX			0x100	///< �û��Ѵ���
#define	ERR_LIMITED_EX			0x200	///< �û�̫�࣬����������
#define	ERR_NOTEXIST_EX			0x300	///< ָ�����û�������
#define	ERR_PASSWD_EX			0x400	///< �������
#define	ERR_PERMISION_DENIED_EX	0x500

/**
 *@brief �û��鶨��
 *
 */
#define POWER_GUEST		0x0001
#define POWER_USER		0x0002
#define POWER_ADMIN		0x0004
#define POWER_FIXED		0x0010

typedef struct tagACCOUNT
{
	S32		nIndex;			///< 0~MAX_ACCOUNT��-1��ʾû��ʹ��
	char	acID[SIZE_ID];	///< IDע��󲻿ɸ��ģ�������ɾ��
	char	acPW[SIZE_PW];	///< ���룬���Ը���
	char	acDescript[SIZE_DESCRIPT];///<�˻�����
	U32		nPower;			///<Ȩ�ޣ��ɸ���
}ACCOUNT, *PACCOUNT;

/**
 *@brief ��ʼ��
 *
 */
S32 maccount_init(void);

/**
 *@brief ���һ���ʻ�
 *@param pstAccount ����ӵ��ʻ��ṹ��ָ��
 *@return �ɹ�ʱ����0�� ���򷵻�#ERR_EXISTED ��ֵ
 *
 */
S32 maccount_add(PACCOUNT pstAccount);

/**
 *@brief �޸��ʻ�
 *@param pstAccount ����ӵ��ʻ��ṹ��ָ��
 *@return �ɹ�ʱ����0�� ���򷵻�#ERR_EXISTED ��ֵ
 *
 */
S32 maccount_modify(PACCOUNT pstAccount);

/**
 *@brief ��֤�����뷽ʽ�޸�����
 *@param oldpwd ������
 *@param newpwd ������
 *@return �ɹ�ʱ����0�� ���򷵻�#ERR_EXISTED ��ֵ
 */
S32 maccount_modifypw(char* acID,char *oldpwd,char* newpwd);

/**
 *@brief �޸��ʻ�
 *@param pstAccount Ҫɾ�����ʻ��ṹ��ָ��
 *@return �ɹ�ʱ����0�� ���򷵻�#ERR_EXISTED ��ֵ
 *
 */
S32 maccount_remove(PACCOUNT pstAccount);

/**
 *@brief check if need check passwd.
 *@retval 0 need to check power
 *@retval otherwise, power of admin
 */
BOOL maccount_need_check_power(void);

/**
 *@brief ��ȡ�û�Ȩ��
 *@param id �û���
 *@param passwd �û�����
 *@retval 0 �û��������ڻ��������
 *@retval >0 �û�Ȩ��ֵ����ο�#POWER_GUEST�ȶ���
 *
 */
U32 maccount_check_power(const char *id, const char *passwd);


/**
 *�����˷���ֵ����
 *
 */
U32 maccount_check_power_ex(const char *id, const char *passwd);
/**
 *@brief ��ȡ�ʻ�����
 *@return �ʻ�����
 *
 */
int maccount_get_cnt(void);

/**
 *@brief ��ȡ�ʻ���Ϣ
 *@param index �ʻ������
 *@return �ʻ���Ϣ
 *
 */
ACCOUNT *maccount_get(int index);

/**
 *@brief ��ȡ������
 *@param no
 *@return ������
 *
 */
U32 maccount_get_errorcode();

/**
 *@brief ��ȡ��ʱ����
 *@param no
 *@return ��ʱ���ɵ�����
 *
 */
U32 maccount_get_tmppasswd();

/**
 *@brief �����Ա仯ʱ�������ʻ��е�����
 *@param nLanguage ���Ķ��壬��mipcinfo.h�ж�����ͬ��ȡֵLANGUAGE_EN��LANGUAGE_CN
 *
 */
void maccount_fix_with_language(int nLanguage);

#endif
