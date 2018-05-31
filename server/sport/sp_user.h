/*
 * sp_user.h
 *
 *  Created on: 2013-11-18
 *      Author: LK
 */

#ifndef SP_USER_H_
#define SP_USER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ����Ҫ�Ե�ע��һ��
 * ��ά���û����ȼ���ONVIF���в�����ǵĶ�Ӧ��ϵ���£�
 * USER_LEVEL_Administrator == POWER_ADMIN
 * USER_LEVEL_Operator == POWER_USER
 * USER_LEVEL_USER == POWER_GUEST
 */
typedef enum
{
	PS_USER_LEVEL_Administrator,
	PS_USER_LEVEL_Operator,
	PS_USER_LEVEL_User,
	PS_USER_LEVEL_Anonymous,
	PS_USER_LEVEL_Extended,
	PS_USER_LEVEL_MAX
}SPUserLevel_e;

#define	ERR_USER_EXISTED		0x1	///< �û��Ѵ���
#define	ERR_USER_LIMITED		0x2	///< �û�̫�࣬����������
#define	ERR_USER_NOTEXIST	0x3	///< ָ�����û�������
#define	ERR_USER_PASSWD		0x4	///< �������
#define	ERR_USER_PERMISION_DENIED	0x5

typedef struct
{
	char name[32];				//�û���
	char passwd[32];			//����
	SPUserLevel_e level;		//Ȩ�޷���
	char descript[32];			//˵������
}SPUser_t;

/**
 * @brief ��ȡ�û�����
 *
 * return �û�����
 */
int sp_user_get_cnt(void);

/**
 * @brief �����û���Ż�ȡ�û���Ϣ
 *
 * @param index �û����
 * @param user �����û���Ϣ
 *
 * @return 0 ��ʾ�ɹ�
 */
int sp_user_get(int index, SPUser_t *user);

/**
 * @brief �����û�����ȡ�û����
 *
 * @param �û���
 *
 * @return �û����
 */
int sp_user_get_index(const char *name);

/**
 * @brief �޸��û���Ϣ��ע���û���һ��ע�᲻���޸ģ�����ɾ��
 *
 * @param user �޸ĺ���û���Ϣ���û���Ϊԭ�û���
 *
 * return 0 �ɹ�
 */
int sp_user_set(SPUser_t *user);

/**
 * @brief ɾ���û���Ϣ
 *
 * @param user Ҫɾ�����û���Ϣ
 *
 * @return 0 �ɹ�
 */
int sp_user_del(SPUser_t *user);

/**
 * @brief ����û�
 *
 * @param user Ҫ��ӵ��û���Ϣ
 *
 * @return 0 �ɹ�
 */
int sp_user_add(SPUser_t *user);

/**
 * @brief ��֤�û�����Ȩ��
 *
 * @param �û���
 * @param �û�����
 *
 * @return 0 ��֤ʧ��
 * 		   >0 �û�Ȩ��ֵ�����գ�SPUserLevel_e
 */
SPUserLevel_e sp_user_check_power(char *name, char *passwd);

#ifdef __cplusplus
}
#endif


#endif /* SP_USER_H_ */
