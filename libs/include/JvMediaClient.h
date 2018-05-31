/**
 * @file JvMediaClient.h
 * @brief ��ά��ý��ͻ���
 * @author ����ͨ
 * @copyright Copyright 2015 by Jovision
 */

#pragma once

#ifdef BUILD_JMC_DYNAMIC
#  ifdef LINUX
#    define JMC_API __attribute__((visibility ("default")))
#  else
#    define JMC_API __declspec(dllexport)
#  endif
#else
#  define JMC_API
#endif

typedef void* JMC_HANDLE;

#define JMC_LOGLEVEL_ALL					0
#define JMC_LOGLEVEL_DEBUG					1
#define JMC_LOGLEVEL_INFO					2
#define JMC_LOGLEVEL_WARN					3
#define JMC_LOGLEVEL_ERROR					4
#define JMC_LOGLEVEL_NONE					5

#define JMC_EVENTTYPE_CONNECTED				0x01 //�����ѽ���
#define JMC_EVENTTYPE_CONNECTFAILED			0x02 //����ʧ��
#define JMC_EVENTTYPE_DISCONNECTED			0x03 //�����ѶϿ�
#define JMC_EVENTTYPE_INSKEYFRAME			0x11 //����ؼ�֡(����ģʽ����ʱ���в���ؼ�֡�Ȳ���)
#define JMC_EVENTTYPE_STOPSTREAM			0x12 //ֹͣ��(����ģʽ���޲�������������ʱ�յ����ź�)
#define JMC_EVENTTYPE_STARTSTREAM			0x13 //������(����ģʽ������ֹͣ״̬���²������ӽ���ʱ�յ����ź�)

#define JMC_DATATYPE_METADATA				0x00
#define JMC_DATATYPE_H264_I					0x01
#define JMC_DATATYPE_H264_BP				0x02
#define JMC_DATATYPE_ALAW					0x11
#define JMC_DATATYPE_ULAW					0x12

#define JMC_DATATYPE_CUSTOM                 0x20 //�Զ������ݣ����ʺϷ��ʹ�����������
                                                 //dts��pts�����ᱻ����,��֧����ά��չЭ��ķ������Ϳͻ���֧��
                                                 //֧������(�����˵����Ŷ�)�ͷ�����(���Ŷ˵�������),������Ч�ʽϵ�,Ӧ����Ƶ��


typedef struct _JMC_Metadata_t
{
	int nVideoWidth;			//��Ƶ���
	int nVideoHeight;			//��Ƶ�߶�
	int nVideoFrameRateNum;		//��Ƶ֡�ʷ���
	int nVideoFrameRateDen;		//��Ƶ֡�ʷ�ĸ
	int nAudioDataType;			//��Ƶ��������(JMC_DATATYPE_*)
	int nAudioSampleRate;		//��Ƶ������
	int nAudioSampleBits;		//��Ƶ����λ��
	int nAudioChannels;			//��Ƶ������
	const char* szPublisherInfo;//�����������Ϣ(��ʹ��ʱӦ����ΪNULL)
	const char* szPublisherVer;	//�����˰汾��Ϣ(ֻ�ڻص�ʱ��Ч,����Metadataʱ���ֶ�Ӧ����ΪNULL)
	int nDuration;				//�������ʱ��(һ�㴫0��¼���ļ�ʱ��)
}
JMC_Metadata_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief �¼��ص�
 * @param hHandle ���Ӿ��
 * @param pUserData �û�����(JMC_Connectʱ����)
 * @param nEvent �¼�����(JMC_EVENTTYPE_*)
 * @param pParam �¼�����,�������¼����Ͷ���
 */
typedef void (*FunJvRtmpEventCallback_t)(JMC_HANDLE hHandle, void* pUserData, int nEvent, const char* pParam);

/*
 * @brief ��������֡(����ģʽ��Ч)
 * @param hHandle ���Ӿ��
 * @param pUserData �û�����(JMC_Connectʱ����)
 * @param nType ��������(JMC_DATATYPE_*)
 * @param pData ֡������
 * @param nSize ֡���ݳ���
 * @param nPts Ԥ��ʱ���
 * @param nDts ����ʱ���(������B֡����µ���nPts����)
 */
typedef void (*FunJvRtmpDataCallback_t)(JMC_HANDLE hHandle, void* pUserData, int nType, void* pData, int nSize, int nPts, int nDts);

/*
 * @brief ��ȡ�汾�ַ���
 */
JMC_API const char* JMC_GetVersion();

/*
 * @brief ������־
 * @param nLevel ��־�ȼ�(Ĭ�ϵȼ�JMC_LOGLEVEL_NONE)
 * @param szPath ��־�ļ�Ŀ¼
 * @param nMaxSize ��־��ռ���ռ�,<=0ʱ��������,��ʹ�����ƣ����ý�������Ϊ128*1024����
 */
JMC_API void JMC_EnableLog(int nLevel, const char* szPath, int nMaxSize);

/*
 * @brief ע��ص�����
 * @param funEventCallback �¼��ص�
 * @param funDataCallback ���ݻص�
 */
JMC_API void JMC_RegisterCallback(FunJvRtmpEventCallback_t funEventCallback, FunJvRtmpDataCallback_t funDataCallback);

/*
 * @brief ��������
 * @param szUrl RTMP URL
 * @param bPublic ������(TRUE)�򲥷���(FALSE)
 * @param nBufSize ���ͻ�������С(����ģʽ����Ч������ֵ ƽ������Kbps * �ؼ�֡����s * (1.5~2.0) * 1024 / 8)
 * @param pUserData �û����ݣ����ڻص���������
 * @return �ɹ��������Ӿ����ʧ�ܷ���NULL
 */
JMC_API JMC_HANDLE JMC_Connect(const char* szUrl, int bPublic, int nBufSize, void* pUserData);

/*
 * @brief ��������֡(����ģʽ��Ч)
 * @param hHandle ���Ӿ��
 * @param nType ��������(JVRTMP_DATATYPE_*)
 * @param pData ֡������
 * @param nSize ֡���ݳ���
 * @param nPts Ԥ��ʱ���
 * @param nDts ����ʱ���(������B֡����µ���nPts����)
 * @return �ɹ��������������ͻ�����������0�����ӶϿ�����-1
 */
JMC_API int JMC_SendFrame(JMC_HANDLE hHandle, int nType, void* pData, int nSize, int nPts, int nDts);

/*
 * @brief �ر�����
 * @param hHandle ���Ӿ��
 */
JMC_API void JMC_Close(JMC_HANDLE hHandle);

#ifdef __cplusplus
}
#endif
