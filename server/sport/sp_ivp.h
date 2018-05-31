/*
 * sp_ivp.h
 *
 *  Created on: 2015-04-02
 *      Author: Qin Lichao
 */

#ifndef SP_IVP_H_
#define SP_IVP_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int x;
    int y;
}SPIVP_POINT;

typedef struct
{
	int						nCnt;						//��ĸ���>=2,=2ʱ�ǰ���
	SPIVP_POINT				stPoints[10];	//ͼ�εĵ������
	unsigned int			nIvpCheckMode;				//������
}SPIVPREGION_t;

typedef enum{
	SPCOUNTOSD_POS_LEFT_TOP=0,
	SPCOUNTOSD_POS_LEFT_BOTTOM,
	SPCOUNTOSD_POS_RIGHT_TOP,
	SPCOUNTOSD_POS_RIGHT_BOTTOM,
	SPCOUNTOSD_POS_HIDE,
}SPivpcountosd_pos_e;

typedef struct{
	BOOL				bEnable;					//�Ƿ���������Ƶ����

	unsigned int 		nDelay;						//������ʱ��С�ڴ�ʱ��Ķ�α���ֻ����һ���ʼ����ͻ��˲��ܴ�����
	BOOL 				bStarting;					//�Ƿ����ڱ���,���ڿͻ��˱�����һֱ��������������Ƿ�����ͻ��˷��ͱ���

	unsigned int		nRgnCnt;					//�����������
	SPIVPREGION_t		stRegion[4];//��������

	BOOL 				bDrawFrame;						//�������߻����
    BOOL 				bFlushFrame;					//��������ʱ���߻����������˸
    BOOL				bMarkObject;					//��Ǳ�������--v2
	BOOL 				bMarkAll;						//���ȫ���˶�����

	BOOL				bOpenCount;						//��������ͳ�ƹ���--v2
	BOOL				bShowCount;						//��ʾ����ͳ��--v2
	BOOL				bPlateSnap;						//����ץ��ģʽ����������ȫ��������Ч��ֻ�ܻ�����--v2

	unsigned int		nAlpha;						//����͸����
	unsigned int		nSen;						//������
	unsigned int		nThreshold;					//��ֵ
	unsigned int		nStayTime;					//ͣ��ʱ��

	BOOL 				bEnableRecord;				//�Ƿ�������¼��
	BOOL 				bOutAlarm1;					//�Ƿ������ 1·�������
	BOOL				bOutClient;					//�Ƿ�������ͻ��˱���
	BOOL				bOutEMail;					//�Ƿ����ʼ�����
	BOOL				bOutVMS;					//�Ƿ�����VMS������

	BOOL				bNeedRestart;				//�Ƿ���Ҫ����

	SPivpcountosd_pos_e	eCountOSDPos;					//λ��
	unsigned int		nCountOSDColor;					//������ɫ

	unsigned int		nCountSaveDays;					//��������
	char				sSnapRes[16];
}SPIVP_t;

/**
 *@brief ����
 *
 *@param channelid ���ܷ���ͨ��
 */
int sp_ivp_start(int channelid);

/**
 *@brief ����
 *
 *@param channelid ���ܷ���ͨ��
 */
int sp_ivp_stop(int channelid);

/**
 * @brief ��ȡ���ܷ���������Ϣ
 *
 *@param channelid ���ܷ���ͨ��
 * @param param ���ܷ�����������
 *
 * @return 0 �ɹ�
 */
int sp_ivp_get_param(int channelid, SPIVP_t *param);

/**
 * @brief ��ȡ���ܷ���������Ϣ
 *
 *@param channelid ���ܷ���ͨ��
 * @param param ���ܷ�����������
 *
 * @return 0 �ɹ�
 */
int sp_ivp_set_param(int channelid, SPIVP_t *param);


#ifdef __cplusplus
}
#endif

#endif /* SP_IVP_H_ */
