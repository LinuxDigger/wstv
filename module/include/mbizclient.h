#ifndef MBIZCLIENT_H
#define MBIZCLIENT_H

typedef struct
{
	char		yst_no[32]; 		/* ����ͨ����*/
	char		dev_type[32];		/* �豸���� */
	char		dev_version[32];	/* �豸�汾 */
	char		username[32];		/* �û��� */
	char		password[32];		/* ���� */
	char		dev_chans;			/* �豸ͨ������ */
	char		sdcard; 			/* �Ƿ����SD��0 ������ 1���� */
	char		cloud;				/* �Ƿ�֧���ƴ洢 0-��֧�� 1-֧�� */
	char		netlibversionstr[60];	/* �����汾 */
	U32 		netlibversion;			/* �����汾 */
}BizDevInfo;

int mbizclient_init();

int mbizclient_updateuserinfo_server(char *username, char *password);

int mbizclient_PushDevInfo(BizDevInfo *info, BOOL bModifyUserInfo);

/** 
 *	@brief ���ͱ�����Ϣ
 *	@param yst_no	 ��������ͨ��
 *	@param channel	 ����ͨ��
 *	@param amt		 ������Ϣ���� 0�ı���Ϣ��1ͼƬ�ϴ���ɣ�2��Ƶ�ϴ���ɡ���������ͨ����Ϊ0)
 *	@param aguid	 ����GUID
 *	@param atype	 ��������
 *	@param atime	 ����ʱ��
 *	@param apicture  ����ͼƬ��ַ
 *	@param avideo	 ������Ƶ��ַ
 *	@return ZUINT 0 �ɹ� -1 ʧ��.
 */
int mbizclient_PushAlarm(char* devname, char* yst_no, U8 channel, U8 amt,
							char* aguid, U8 atype, U32 atime, char* apicture, char* avideo);

#endif