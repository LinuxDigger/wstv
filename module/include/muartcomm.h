#ifndef __MUART_COMM_H__
#define __MUART_COMM_H__

#define EX_PTZ_HDWARE_INFO_REQ			0x01	//��о������̨Ӳ���汾��Ϣ
#define EX_PTZ_HDWARE_INFO_RESP			0x02	//��̨��Ӧ��оӲ���汾��Ϣ
#define EX_PTZ_UPDATE_START_REQ			0x03	//��о������̨����
#define EX_PTZ_SFWARE_INFO_REQ			0x04	//��̨���������ļ����Ⱥ�MD5��
#define EX_PTZ_SFWARE_INFO_RESP			0x05	//��о��Ӧ��̨�����ļ����Ⱥ�MD5��
#define EX_PTZ_UPLOAD_START				0x06	//��̨����ʼ��������
#define EX_PTZ_UPLOAD_DATA				0x07	//��̨���������������
#define EX_PTZ_UPLOAD_OK				0x08	//��о��Ӧ��̨���ݷ������
#define EX_PTZ_UPDATE_RET				0x09	//��̨��Ӧ��о�������
#define EX_PTZ_UPLOAD_CANCEL			0xa0	//ȡ��������ԭ��:�豸æ��Ȩ�޲���
#define EX_PTZ_UPLOAD_RETRY				0xa1	//��̨�����о�ط�����

#define EX_COMTRANS_SEND				0x26	 //���ڷ�
#define EX_COMTRANS_RESV				0x27	 //������
#define EX_COMTRANS_OPEN				0x28	 //���ڹ��ܴ�
#define EX_COMTRANS_CLOSE				0x29	 //���ڹ��ܹر�
#define EX_COMTRANS_SET					0x2A	 //���ô�������

//�����������
#define	PTZ_UPDATE_SUCCESS		0x01	//�����ɹ�
#define	PTZ_UPDATE_FAILED		0x02	//����ʧ��
#define PTZ_UPDATE_LATEST		0x03	//���°汾
#define PTZ_UPDATE_INVALID		0x04	//��Ч�ļ�
#define PTZ_UPDATE_ERROR		0x05	//��������
#define PTZ_UPDATE_NOTFIT		0x06	//�ļ���ƥ��

typedef struct{
	U16 softver;			//����汾
	char product[16];		//��Ʒ��𣬹̻�����̨��
}PtzHdwareInfo_t;

typedef struct{	
	int filesize;			//����ļ����ȣ�ver�ļ� Ϊ10������
	U8 checksum[16];		//MD5У����
}PtzSfwareInfo_t;

typedef struct 
{
	BOOL running;
	pthread_mutex_t mutex;
}ptz_thread_group_t;

VOID ComTransProc(REMOTECFG *cfg);	//���ڵ���

void muartcomm_init();
#endif
