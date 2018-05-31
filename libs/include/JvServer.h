#ifndef JVSERVER_H
#define JVSERVER_H
#include "JVNSDKDef.h"
#ifndef WIN32
	//#include "JVNSDKDef.h"
	#ifdef __cplusplus
		#define JOVISION_API extern "C"
	#else
		#define JOVISION_API
	#endif
#else
	#define JOVISION_API extern "C" __declspec(dllexport)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                     //
//                                      ���ض˽ӿ�                                                     //
//                                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/***************************************�ӿ��嵥********************************************************
JVN_InitSDK -----------------01 ��ʼ��SDK��Դ
JVN_ReleaseSDK---------------02 �ͷ�SDK��Դ
JVN_RegisterCallBack --------03 �������ض˻ص�����
JVN_ActiveYSTNO -------------04 ��������ͨ����
JVN_InitYST -----------------05 ��ʼ������ͨ����
JVN_StartChannel ------------06 ����ĳͨ���������
JVN_StopChannel -------------07 ֹͣĳͨ�����з���
JVN_SendData ----------------08 ��������(ͨ��Ⱥ��)
JVN_SendDataTo --------------09 ��������(������)
JVN_SendChatData ------------10 ����������Ϣ(����������ı�����)
JVN_EnableLog ---------------11 ����д������־�Ƿ���Ч
JVN_RegCard -----------------12 ע���Ʒ
JVN_SetLanguage -------------13 ������־/��ʾ��Ϣ����(Ӣ��/����)
JVN_GetLanguage -------------14 ��ȡ��־/��ʾ��Ϣ����(Ӣ��/����)
JVN_SendAndRecvS ------------15 ��������������
JVN_StartWebServer ----------16 ����web����
JVN_StartSelfWebServer ------17 �����Զ���web����(OEM)
JVN_StopWebServer -----------18 ֹͣweb����
JVN_SetClientLimit ----------19 ���÷ֿ���Ŀ����
JVN_GetClientLimit ----------20 ��ȡ�ֿ���Ŀ��������ֵ
JVN_EnablePCTCPServer -------21 �����ر�PC�ͻ�TCP����(������δʹ��)
JVN_EnableMOServer ----------22 �����ر�ĳͨ�����ֻ�����
JVN_SendMOData --------------23 �����ֻ����ݣ���TYPE_MO_TCP/TYPE_MO_UDP������Ч
JVN_Command -----------------24 �����ض�ָ�Ҫ��SDKִ���ض�����
JVN_StartLANSerchServer -----25 ������������������
JVN_StopLANSerchServer ------26 ֹͣ��������������
JVN_SetLocalNetCard ---------27 ������һ������(IPC)
JVN_SetDeviceName -----------28 ���ñ����豸����
JVN_SetDomainName -----------29 �����µ�������ϵͳ�������ȡ�������б�
JVN_SetLocalFilePath --------30 �Զ��屾���ļ��洢·����������־�����ɵ������ؼ��ļ���
JVN_StartStreamService ------31 ������ý�����
JVN_StopStreamService -------32 ֹͣ��ý�����
JVN_GetStreamServiceStatus --33 ��ѯ��ý�����״̬
JVN_StartBroadcastServer ----34 �����Զ���㲥����(IPC)
JVN_StopBroadcastServer -----35 �ر��Զ���㲥����(IPC)
JVN_BroadcastRSP ------------36 �����Զ���㲥Ӧ��(IPC)
JVN_SendPlay-----------------37 ��ָ��Ŀ�귢��Զ�̻ط�����,������MP4�ļ�Զ�̻ط�
JVN_EnableLANToolServer------38 �����رվ������������߷���
JVN_RegDownLoadFileName------39 ע��ص����������ڵ��������⴦��Զ�������ļ���
JVN_SetDeviceInfo------------40 �����豸��Ϣ
JVN_GetDeviceInfo------------41 ��ȡ�豸��Ϣ
JVN_SetIPCWANMode------------42 ����IPC�������⴦��ģʽ
JVN_GetNetSpeedNow-----------43 ��ȡ��ǰ����״��
JVN_ClearBuffer-------------44 ���ͨ�����ͻ���
JVN_SetLSPrivateInfo---------45 ���ñ����Զ�����Ϣ�������豸����
JVN_SetWANClientLimit--------46 ��������������Ŀ����
JVN_GetExamItem -------------47	��ѯ���Լ�����Ŀ
JVN_ExamItem ----------------48 �����Ŀ
JVN_SendChannelInfo----------49 ����ÿһ��ͨ������
JVN_SendJvnInfo--------------50 ����ÿһ��ͨ������
JVN_RegNickName--------------51 ע�����
JVN_DeleteNickName-----------52 ɾ������
JVN_GetNickName--------------53 ��ȡ����
JVN_SetNickName--------------54 ���ñ���
JVN_RegClientMsgCallBack-----55 ע��ֿ���Ϣ�ص�
JVN_GetPositionID-----------56 ��ѯ����
JVN_SetChannelInfo----------57 ��������ͨ����Ƶ��Ϣ
JVN_RTMP_Callback------------58 ע����ý������״̬�ص�
JVN_RTMP_EnableSelf----------59 �Զ�����ý�����
JVN_RTMP_SendData------------60 ������ý������
JVN_SendDownFile-----------------61 ��ָ��Ŀ�귢��Զ����������,������sv6�ļ�Զ������
JVN_SetSelfSerListFileName---62 ���Ʒ������б�
*******************************************************************************************************/                                                                                                     //


/****************************************************************************
*����  : JVN_InitSDK
*����  : ��ʼ��SDK��Դ�����뱻��һ������
*����  : [IN] nYSTPort    ����������ͨ����ͨ�ŵĶ˿�,0ʱĬ��9100
         [IN] nLocPort    �����ṩ���ط���ͨ�ŵĶ˿�,0ʱĬ��9101
		 [IN] nSPort      ����������ͨ����������(������룬ע���)�Ķ˿�,0ʱĬ��9102
		 [IN] nVersion    ���ذ汾�ţ���������ͨ��������֤
		 [IN] lRSBuffer   ���ض������շ���֡���ݵ���С���棬Ϊ0ʱĬ��150K��
		                  IPC�������ݵ�֡���ܴܺ󣬿ɸ������֡ȷ����ֵ�����������Ӧ�ֿض�Ҳ��Ҫ��Ӧ����.
						  �ò�����������IPC��
		 [IN] bLANSpeedup �Ƿ�����������Ż�����(IPC�������������������⴦��,������ͨ������Ʒ������ΪFALSE)
		 [IN] stDeviceType ��Ʒ���� ���JVNSDKDef.h �� STDeviceType
		                   ��ͨ�忨������stDeviceType.nType = 1;stDeviceType.nMemLimit = 1;
						   ��ͨIPC������ stDeviceType.nType = 3;stDeviceType.nMemLimit = 1;
						   �ڴ����ز���DVR������stDeviceType.nType = 2;stDeviceType.nMemLimit = 3;
		 [IN] nDSize       ������sizeof(STDeviceType),����У�鴫��ṹ�Ϸ���
		 [IN] pchPackOnline ��������
		 [IN] nLen			�������ݳ���
		 [IN] nOEMID	OEM��ţ�0 �ǹ�˾��Ʒ 1 ��̩�����ƣ�2ΪСά�Ż���Ʒ���Ժ�����OEMֱ�ӱ���ۼ�
*����ֵ: TRUE     �ɹ�
         FALSE    ʧ��
*����  : ���ض˳���ռ�������˿ڣ�һ������������ͨ������ͨ�ţ�һ��������ֿ�ͨ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_InitSDK(int nYSTPort, int nLocPort, int nSPort, int nVersion, long lRSBuffer, int bLANSpeedup, STDeviceType stDeviceType, int nDSize,char *pchPackOnline, int nLen,int nOEMID);
#else
	JOVISION_API bool __stdcall	JVN_InitSDK(int nYSTPort, int nLocPort, int nSPort, int nVersion, long lRSBuffer, BOOL bLANSpeedup, STDeviceType stDeviceType, int nDSize,char *pchPackOnline, int nLen,int nOEMID);
#endif

/****************************************************************************
*����  : JVN_ReleaseSDK
*����  : �ͷ�SDK��Դ��������󱻵��� 
*����  : ��
*����ֵ: ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_ReleaseSDK();
#else
	JOVISION_API void __stdcall	JVN_ReleaseSDK();
#endif

/****************************************************************************
*����  : JVN_RegisterSCallBack
*����  : �������ض˻ص����� 
*����  : [IN] ConnectCallBack   �ֿ�����״���ص�����
         ...
*����ֵ: ��
*����  : ���ض˻ص�����������
             �ֿ������֤����            (�����֤)
             ������ͨ������ͨ��״̬������(����״̬)
		     ��ֿض�ͨ��״̬������      (����״̬)
		     ¼�����������              (¼���������)
			 Զ����̨���ƺ�����          (Զ����̨����)
			 ��������/�ı����캯��       (Զ���������ı�����)
			 �طſ���       (Ŀǰ�Ļطſ���mp4�ļ�)
			 ������ط�����Ϣ+�ƴ洢����״̬֪ͨ��Ϣ
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_RegisterCallBack(FUNC_SCHECKPASS_CALLBACK CheckPassCallBack,
											FUNC_SONLINE_CALLBACK OnlineCallBack,
											FUNC_SCONNECT_CALLBACK ConnectCallBack,
											FUNC_SCHECKFILE_CALLBACK CheckFileCallBack,
											FUNC_SYTCTRL_CALLBACK YTCtrlCallBack,
											FUNC_SCHAT_CALLBACK ChatCallBack,
											FUNC_STEXT_CALLBACK TextCallBack,
											FUNC_SFPLAYCTRL_CALLBACK FPlayCtrlCallBack,
											FUNC_RECVSERVERMSG_CALLBACK FRecvMsgCallBack);
#else
	JOVISION_API void __stdcall	JVN_RegisterCallBack(FUNC_SCHECKPASS_CALLBACK CheckPassCallBack,
												  FUNC_SONLINE_CALLBACK OnlineCallBack,
												  FUNC_SCONNECT_CALLBACK ConnectCallBack,
												  FUNC_SCHECKFILE_CALLBACK CheckFileCallBack,
												  FUNC_SYTCTRL_CALLBACK YTCtrlCallBack,
												  FUNC_SCHAT_CALLBACK ChatCallBack,
												  FUNC_STEXT_CALLBACK TextCallBack,
												  FUNC_SFPLAYCTRL_CALLBACK FPlayCtrlCallBack,
												  FUNC_RECVSERVERMSG_CALLBACK FRecvMsgCallBack);
#endif

/****************************************************************************
*����  : JVN_RegClientMsgCallBack
*����  : ���÷ֿ���Ϣ�ص���
*����  : [IN] FRecvMsgCallBack   �ֿ���Ϣ�ص���������Ƶ��ͣ����Ƶ�ָ�
*����ֵ: ��

*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_RegClientMsgCallBack(FUNC_RECVCLIENTMSG_CALLBACK FRecvMsgCallBack);
#else
	JOVISION_API void __stdcall JVN_RegClientMsgCallBack(FUNC_RECVCLIENTMSG_CALLBACK FRecvMsgCallBack);
#endif

/****************************************************************************
*����  : JVN_ActiveYSTNO
*����  : ��������ͨ����
*����  : [IN]  pchPackGetNum  ����Ϣ(STGetNum�ṹ)
         [IN]  nLen           ��Ϣ����(sizeof(STGetNum))
         [OUT] nYSTNO         �ɹ����������ͨ����
*����ֵ: TRUE  �ɹ�
         FALSE ʧ��
*����  : ����ͨ��������ַ���ڲ���ȡ��
         ��������ʹ�ã�ֻ���ؼ����YST���룬�����ڲ����κμ�¼��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_ActiveYSTNO(char *pchPackGetNum, int nLen, int *nYSTNO);
#else
	JOVISION_API bool __stdcall	JVN_ActiveYSTNO(char *pchPackGetNum, int nLen, int &nYSTNO);
#endif

/****************************************************************************
*����  : JVN_InitYST
*����  : ��ʼ������ͨ����
*����  : [IN] ����ͨ�������Ϣ(STOnline�ṹ)
         [IN] ��Ϣ����
*����ֵ: ��
*����  : �ú�����������ͨ������ͨ����ǰ���ã�����ͨ������ͨ��������ʧ�ܣ�
         
		 �ú���ֻ�����һ�Σ�����������ͨ����ֻҪ����Ҫ��������ͨ����ģ�
		 ����������ǰ����һ�θýӿڼ��ɣ�
         
		 �ú�����¼����ͨ���뼰����������ʱʹ����Щ������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_InitYST(char *pchPackOnline, int nLen);
#else
	JOVISION_API void __stdcall	JVN_InitYST(char *pchPackOnline, int nLen);
#endif

/****************************************************************************
*����  : JVN_StartChannel
*����  : ����ĳͨ���������
*����  : [IN] nChannel  ����ͨ���� >=1  
                        �ر�:nChannel=-1ʱ����ͨ���������ڱܿ���Ƶͨ����������ͨ����

         [IN] nType     �����������ͣ�0����������������1ֻ��������������

		 [IN] bJVP2P    ������ά����ֱ�����ڴ治�������û��Ӳ��ʱ�ù��ܲ������ã�
		                ��ΪTRUEʱΪ�ಥ��ʽ��ΪFALSEʱΪ��ͳ��ʽ��
						������û�չ��ʱ��Ӧ������ѡ�����ģʽ(����ʱ)�ʹ�������ģʽ(С����ʱ)��
						Ĭ��Ϊ����ģʽ���ɣ�
						û��������Ҫ,�ɽ��ù�����ΪFALSE,����չ�ָ��û���

		 [IN] lBufLen   ͨ����Ƶ֡��������С����λ�ֽ�, 
		                ��ͨ��ʽʱ�����Сָ����֡����������(��>400*1024,Ӧ�������������)��
						JVP2P��ʽʱ������Ϊ>=8M����СΪ1024000��
						�����ֵ���ò��㣬���ܲ���ÿ��֡����ĩβ��֡��ʧ��ɿ��ٵ�����

		 [IN] chBufPath �����ļ�·������ʹ���ļ�����ʱ��Ϊ""; 

*����ֵ: TRUE  �ɹ�
         FALSE ʧ��
*����  : ÿ��ͨ������Ƶ֡�ߴ粻ͬ��������軺����Ҳ��ͬ����Ӧ�ò����ã�
         �ú������ظ����ã������б仯ʱ�Ž���ʵ�ʲ������ظ��ĵ��ûᱻ���ԣ�
		 Ӧ�ò��ڸ�����ĳЩ���ú󣬿����µ��øýӿ����÷���

         ��������jvp2p���������ַ�ʽ���л��棺�ڴ淽ʽ���ļ���ʽ
		 ��lbuflen �� chBufPath������һ������Ч�ģ����ͬʱ��Ч�����Ȳ����ڴ淽ʽ�����߶���Ч��ʧ��

		 ���飺1.�ڴ淽ʽʱÿ��ͨ������>=8M�ڴ棬Ч����ѣ��ڴ���������������ڴ淽ʽ��
		       2.DVR���ڴ�ǳ����ŵ��豸���԰�װӲ�̲��Ҳ����ļ��洢��ʽʹ��jvp2p��
			     DVR�ڴ�ϳ�����豸��������ÿ��ͨ������>=1M����Ƶ�����������򲻽���ʹ�ã�
				 DVR�ڴ�����豸�޷�ʹ��jvp2p������չ�ָ��û�����Ĭ���ṩ'����ģʽ'���ɣ�
			   3.ÿ��ͨ�������Ե�������jvp2p���ܣ�������Ӧ�ò����ļ���Լ���ͨ�����������ľ�����
			     ��������ͨ��ͳһ������رոù��ܣ�����ʹ��jvp2p��������ͨ����ʹ�ã����򶼲��ã�

         bJVP2P=TRUEʱ������"���������ͱ�֤����"��ģʽ���У��������ض�ֻҪ�ṩ�˱�Ҫ�Ļ����ϴ���������2M��
		 �������㼸ʮ�ϰ���ͬʱ���ӣ�����Ͽ�ͨ��άVIPת��������ȷ��������������ʵʱ���Բ
		 Զ�̻�����ʵ�ʻ��������ʱ2s-7s��
		 bJVP2P=FALSEʱ������"����������ʱ"��ģʽ���У�����������ȫȡ���ڴ�����豸��Դ����һ�ִ�ͳ���䷽ʽ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_StartChannel(int nChannel, int nType, int bJVP2P, long lBufLen, char chBufPath[256]);
#else
	JOVISION_API bool __stdcall	JVN_StartChannel(int nChannel, int nType, BOOL bJVP2P, long lBufLen, char chBufPath[MAX_PATH]);
#endif

/****************************************************************************
*����  : JVN_StopChannel
*����  : ֹͣĳͨ�����з��� 
*����  : [IN] nChannel ����ͨ���� >=1
*����ֵ: ��
*����  : ֹͣĳ������Ҳ��ͨ���ظ�����JVN_StartChannelʵ�֣�
         ����ֹͣ���з���ֻ��ͨ���ýӿ�ʵ�֡�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_StopChannel(int nChannel);
#else
	JOVISION_API void __stdcall	JVN_StopChannel(int nChannel);
#endif

/****************************************************************************
*����  : JVN_SendData
*����  : �������� 
*����  : [IN] nChannel   ����ͨ���� >=1
         [IN] uchType    �������ͣ���ƵI;��ƵP;��ƵB;��ƵS;��Ƶ;�ߴ�;�Զ�������;֡����ʱ����
         [IN] pBuffer    ������������,��Ƶ/��Ƶ/�Զ�������ʱ��Ч
		 [IN] nSize      �������ݳ���,��Ƶ/��Ƶ/�Զ�������ʱ��Ч
		 [IN] nWidth     uchType=JVN_DATA_Sʱ��ʾ֡��/uchType=JVN_CMD_FRAMETIMEʱ��ʾ֡���(��λms)
		 [IN] nHeight    uchType=JVN_DATA_Sʱ��ʾ֡��/uchType=JVN_CMD_FRAMETIMEʱ��ʾ�ؼ�֡����
*����ֵ: ��
*����  : ��ͨ��Ϊ��λ����ͨ�����ӵ����зֿط�������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SendData(int nChannel,unsigned char uchType,unsigned char *pBuffer,int nSize,int nWidth,int nHeight);
#else
	JOVISION_API void __stdcall	JVN_SendData(int nChannel,unsigned char uchType,BYTE *pBuffer,int nSize,int nWidth,int nHeight);
#endif

/****************************************************************************
*����  : JVN_SendDataTo
*����  : �������� 
*����  : [IN] nChannel   ����ͨ���� >=1
         [IN] uchType    �������ͣ�Ŀǰֻ���ڳߴ緢�ͳߴ�;�Ͽ�ĳ����;�Զ�������
         [IN] pBuffer    ������������
		 [IN] nSize      �������ݳ���
		 [IN] nWidth     uchType=JVN_DATA_Sʱ��ʾ֡��/uchType=JVN_CMD_FRAMETIMEʱ��ʾ֡���(��λms)
		 [IN] nHeight    uchType=JVN_DATA_Sʱ��ʾ֡��/uchType=JVN_CMD_FRAMETIMEʱ��ʾ�ؼ�֡����
*����ֵ: ��
*����  : ��ͨ�����ӵ�ĳ������ֿط�������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SendDataTo(int nClientID,unsigned char uchType,unsigned char *pBuffer,int nSize,int nWidth,int nHeight);
#else
	JOVISION_API void __stdcall	JVN_SendDataTo(int nClientID,unsigned char uchType,BYTE *pBuffer,int nSize,int nWidth,int nHeight);
#endif

/****************************************************************************
*����  : JVN_SendChatData
*����  : ����������Ϣ(����������ı�����)
*����  : [IN] nChannel   ����ͨ���� >=1,�㲥ʽ����ʱ��Ч;
         [IN] nClientID  �ֿ�ID,��ָ���ֿط���,��ʱnChannel��Ч,���ȼ�����nChannel;
         [IN] uchType      �������ͣ���������;
		                             �ı�����;
		                             ͬ����������;
                                     ͬ���ı�����;
								     ��������;
								     �ı�����;
								     �����ر�;
								     �ı��ر�;
         [IN] pBuffer    ������������
         [IN] nSize      �������ݳ���
*����ֵ: true   �ɹ�
         false  ʧ��
*����  : �����߽��������ݷ��͸�������������ķֿض�;
         nChannel��nClientID����ͬʱ<=0,������ͬʱ��Ч;
		 nChannel��nClientIDͬʱ>0ʱ,nClientID���ȼ���,��ʱֻ��ָ���ֿط��͡�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SendChatData(int nChannel,int nClientID,unsigned char uchType,unsigned char *pBuffer,int nSize);
#else
	JOVISION_API bool __stdcall	JVN_SendChatData(int nChannel,int nClientID,unsigned char uchType,BYTE *pBuffer,int nSize);
#endif

/****************************************************************************
*����  : JVN_EnableLog
*����  : ����д������־�Ƿ���Ч 
*����  : [IN] bEnable  TRUE:����ʱд��־��FALSE:��д�κ���־
*����ֵ: ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_EnableLog(int bEnable);
#else
	JOVISION_API void __stdcall	JVN_EnableLog(bool bEnable);
#endif

/****************************************************************************
*����  : JVN_SetLanguage
*����  : ������־/��ʾ��Ϣ����(Ӣ��/����) 
*����  : [IN] nLgType  JVN_LANGUAGE_ENGLISH/JVN_LANGUAGE_CHINESE
*����ֵ: ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SetLanguage(int nLgType);
#else
	JOVISION_API void __stdcall	JVN_SetLanguage(int nLgType);
#endif

/****************************************************************************
*����  : JVN_GetLanguage
*����  : ��ȡ��־/��ʾ��Ϣ����(Ӣ��/����) 
*����  : ��
*����ֵ: JVN_LANGUAGE_ENGLISH/JVN_LANGUAGE_CHINESE
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_GetLanguage();
#else
	JOVISION_API int __stdcall	JVN_GetLanguage();
#endif

/****************************************************************************
*����  : JVN_SetClientLimit
*����  : ���÷ֿ���Ŀ����
*����  : [IN] nChannel  ͨ����(>0;=0;<0)
         [IN] nMax      ��Ŀ����ֵ <0ʱ��ʾ������
		                nChannel<0 ʱ��ʾ�ֿ�����Ŀ����ΪnMax; 
						nChannel=0 ʱ��ʾ����ͨ��ʹ����ͬ��ͨ���ֿ���Ŀ����ΪnMax; 
						nChannel>0 ʱ��ʾ��ͨ���ֿ���Ŀ����ΪnMax
*����ֵ: ��
*����  : ���ظ�����,�����һ������Ϊ׼;
         �������޺͵�ͨ�����޿�ͬʱ������;

         ����ͨ��Ʒ�����ϸ�������������ֻʹ�øýӿھ��ܴﵽ����������Ŀ�ģ�
		 ������Ҫ�����������������Ĳ�Ʒ������ϵ���JVN_SetWANClientLimit��
		 �޶���������������
		 ��
		 ���ͬʱʹ��JVN_SetClientLimit��JVN_SetWANClinetLimit,��
		             JVN_SetClientLimit�޶����ǻ�����������
		             JVN_SetWANClientLimit�����޶�����������������
		 ���ֻʹ��JVN_SetClientLimit,�޶��ľ���(������������)������Ŀ��
		 ���ֻʹ��JVN_SetWANClientLimit,�޶��ľ�������������Ŀ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SetClientLimit(int nChannel, int nMax);
#else
	JOVISION_API void __stdcall	JVN_SetClientLimit(int nChannel, int nMax);
#endif

/****************************************************************************
*����  : JVN_GetClientLimit
*����  : ��ȡ�ֿ���Ŀ��������ֵ
*����  : [IN] nChannel  ͨ����(>0;<0)
		                nChannel<0 ʱ��ʾ��ȡ�ֿ�����Ŀ����; 
						nChannel>0 ʱ��ʾ��ȡ��ͨ���ֿ���Ŀ����;
*����ֵ: ��Ŀ����ֵ <=0��ʾ������
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_GetClientLimit(int nChannel);
#else
	JOVISION_API int __stdcall	JVN_GetClientLimit(int nChannel);
#endif

/****************************************************************************
*����  : JVN_SetWANClientLimit
*����  : ���������ֿ���Ŀ����
*����  : [IN] nWANMax   ��Ŀ����ֵ <0ʱ��ʾ������
		                >=0��ʾ�����ֿ�����Ŀ����ΪnWANMax; 
		 
*����ֵ: ��
*����  : ���ظ�����,�����һ������Ϊ׼;
         �������޺͵�ͨ�����޿�ͬʱ������;

		 ����ͨ��Ʒ�����ϸ�������������ֻʹ��JVN_SetClientLimit�ӿھ��ܴﵽ����������Ŀ�ģ�
		 ������Ҫ�����������������Ĳ�Ʒ������ϵ���JVN_SetWANClientLimit��
		 �޶���������������
		 ��
		 ���ͬʱʹ��JVN_SetClientLimit��JVN_SetWANClinetLimit,��
		 JVN_SetClientLimit�޶����ǻ�����������
		 JVN_SetWANClientLimit�޶�����������������

		 ���ֻʹ��JVN_SetClientLimit,�޶��ľ���(������������)������Ŀ��
		 
		 ���ֻʹ��JVN_SetWANClientLimit,�޶��ľ�������������Ŀ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SetWANClientLimit(int nWANMax);
#else
	JOVISION_API void __stdcall	JVN_SetWANClientLimit(int nWANMax);
#endif

/****************************************************************************
*����  : JVN_RegCard
*����  : ע���Ʒ
*����  : [IN] chGroup    ����ţ�����"A" "AAAA"
         [IN] pBuffer    ������������(SOCKET_DATA_TRAN�ṹ)
		 [IN] nSize      ���������ܳ���
*����ֵ: TRUE  �ɹ�
         FALSE ʧ��
*����  : ��������������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_RegCard(char chGroup[4], unsigned char *pBuffer,int nSize);
#else
	JOVISION_API bool __stdcall	JVN_RegCard(char chGroup[4], BYTE *pBuffer,int nSize);
#endif

/****************************************************************************
*����  : JVN_SendAndRecvS
*����  : ��������������
*����  : [IN] chGroup       ����ţ�����"A" "AAAA"
         [IN] pBuffer       ������������
         [IN] nSize         ���������ܳ���
         [OUT] pRecvBuffer  �������ݻ��壬�ɵ����߷���
         [IN/OUT] &nRecvLen ������ջ��峤�ȣ�����ʵ�����ݳ���
         [IN] nTimeOut      ��ʱʱ��(��)
*����ֵ: TRUE  �ɹ�
FALSE ʧ��
*����  : ��������������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SendAndRecvS(char chGroup[4],unsigned char *pBuffer,int nSize, 
		                               unsigned char *pRecvBuffer,int *nRecvLen,int nTimeOut);
#else
	JOVISION_API bool __stdcall	JVN_SendAndRecvS(char chGroup[4], BYTE *pBuffer,int nSize, 
		                                         BYTE *pRecvBuffer,int &nRecvLen,int nTimeOut);
#endif

/****************************************************************************
*����  : JVN_StartWebServer
*����  : ����web����
*����  : [IN] chHomeDir   Ŀ���ļ����ڱ���·�� ��"D:\\test"
         [IN] chDefIndex  Ŀ���ļ���(����)
		 [IN] chLocalIP   ����ip
         [IN] nPort       web����˿�
*����ֵ: TRUE  �ɹ�
		 FALSE ʧ��
*����  : web�����ܼ�����Ŀ���ļ����ͻ���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_StartWebServer(char chHomeDir[256], char chDefIndex[256], char chLocalIP[30], int nPort);
#else
	JOVISION_API bool __stdcall JVN_StartWebServer(char chHomeDir[256], char chDefIndex[256], char chLocalIP[30], int nPort);
#endif

/****************************************************************************
*����  : JVN_StartSelfWebServer
*����  : �����Զ���web����(OEM)
*����  : [IN] chHomeDir   Ŀ���ļ����ڱ���·�� ��"D:\\test"
         [IN] chDefIndex  Ŀ���ļ���(����)
		 [IN] chLocalIP   ����ip
         [IN] nPort       web����˿�
		 [IN] chSelfWebPos   �Զ�����վ�ؼ�index�ļ�λ�� ��"www.afdvr.com/cloudsee"
		 [IN] bOnlyUseLocal  ��ʹ�ñ���web���񣬲�ʹ��������վ
*����ֵ: TRUE  �ɹ�
		 FALSE ʧ��
*����  : web�����ܼ�����Ŀ���ļ����ͻ���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_StartSelfWebServer(char chHomeDir[256], char chDefIndex[256], 
		                                     char chLocalIP[30], int nPort,
											 char chSelfWebPos[500], int bOnlyUseLocal);
#else
	JOVISION_API bool __stdcall JVN_StartSelfWebServer(char chHomeDir[256], char chDefIndex[256], 
		                                               char chLocalIP[30], int nPort,
													   char chSelfWebPos[500], BOOL bOnlyUseLocal);
#endif

/****************************************************************************
*����  : JVN_StopWebServer
*����  : ֹͣweb����
*����  : ��
*����ֵ: ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_StopWebServer();
#else
	JOVISION_API void __stdcall JVN_StopWebServer();
#endif

/****************************************************************************
*����  : JVN_Command
*����  : �����ض�ָ�Ҫ��SDKִ���ض�����
*����  : [IN] nChannel  ����ͨ�� ==0ʱ����������Ƶͨ����Ч(����������ͨ��)
         [IN] nCMD  ָ������
*����ֵ: ��
*����  : ֧�ֵ�ָ��ο����Ͷ���
         Ŀǰ��֧�֣�CMD_TYPE_CLEARBUFFER
		 ���ض˽�����ĳ�����������ϣ���ͻ������������µ���ǰ���µ����ݣ���ִ�и�ָ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_Command(int nChannel, int nCMD);
#else
	JOVISION_API void __stdcall JVN_Command(int nChannel, int nCMD);
#endif
	
/****************************************************************************
*����  : JVN_StartLANSerchServer
*����  : ������������������
*����  : [IN] nChannelCount ��ǰ�豸�ܵ�ͨ����
         [IN] nPort         ����˿ں�(<=0ʱΪĬ��9103,����ʹ��Ĭ��ֵ��ֿض�ͳһ)
*����ֵ: ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_StartLANSerchServer(int nPort);
#else
	JOVISION_API bool __stdcall JVN_StartLANSerchServer(int nPort);
#endif

/****************************************************************************
*����  : JVN_StopLANSerchServer
*����  : ֹͣ��������������
*����  : ��
*����ֵ: ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_StopLANSerchServer();
#else
	JOVISION_API void __stdcall JVN_StopLANSerchServer();
#endif
	
/****************************************************************************
*����  : JVN_SetLocalNetCard
*����  : ������һ������ eth0,eth1,... for linux or 0, 1, 2,...for win  
*����  : [IN] strCardName   ����
*����ֵ: �ɹ� TRUE ,ʧ�� FALSE
*����  : 2012 5
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetLocalNetCard(char* strCardName);
#else
	JOVISION_API BOOL __stdcall JVN_SetLocalNetCard(char* strCardName);
#endif

#ifndef WIN32
	JOVISION_API int JVN_GetLocalNetCard(char* strCardName);
#else
	JOVISION_API BOOL __stdcall JVN_GetLocalNetCard(char* strCardName);
#endif
/****************************************************************************
*����  : JVN_EnablePCTCPServer
*����  : ������ر�PC�û���TCP����
*����  : [IN] bEnable ����/�ر�
*����ֵ: TRUE  �ɹ�
		 FALSE ʧ��
*����  : TCP�����ܽ��շֿ���TCP��ʽ���ӣ���TCP��ʽ��ֿط������ݣ�
         Ŀǰ��ά�ֿض�δʹ�ø�TCP����,û��������Ҫ�ɲ�ʹ�øù��ܣ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_EnablePCTCPServer(int bEnable);
#else
	JOVISION_API bool __stdcall JVN_EnablePCTCPServer(BOOL bEnable);
#endif

/****************************************************************************
*����  : JVN_EnableMOServer
*����  : ����/�ر�ĳͨ�����ֻ����� 
*����  : [IN] nChannel   ����ͨ���� >=1 ��=0ʱ������ر�����ͨ�����ֻ�����
         [IN] bEnable    TRUEΪ������FALSEΪ�ر�
		 [IN] nOpenType  ���������ͣ�����Ϊ0;bEnableΪTRUEʱ��Ч��
		 [IN] bSendVSelf  �Ƿ񵥶������ֻ���Ƶ���ݣ����ΪTRUE�����ֻ����ݺ�PC������ȫ���룬
		                 �ֻ����ݱ�����JVN_SendMOData���ͣ������ֻ���Ƶ���ݽ�����Ҫ�������ͣ�
						 ������ֻ����ֿ��ã����ֻ��͸��ֿص�����ͬ��������bSendVSelf=FALSE���ɣ�
		 [IN] bProtocol  �Ƿ����Զ���Э��,��bSendVSelfʱ��Ч��
		                 TRUEʱ��JVN_SendMOData�����ݴ����Ҫ�������ͷβ��ʹ�ֻ���������������ݣ�
						         �ɰ����ط��͵�JPG���ݺͱ�׼H264���ݾ������ַ��ͷ�ʽ��
						 FALSEʱ��JVN_SendMOData�����ݴ����ʽ�ͷֿ�����һ�£���ֿ����ݵ�����������������ݣ�
						         ������ֻ����ֿ��ã������ֻ��������ǵ���������������ʹ�ø÷�ʽ��
		[IN] nBufSize   �ֻ����ͻ����С��������500000,��0ʱĬ�ϴ�С500000					 
*����ֵ: ��
*����  : �ú���ֻ��JVN_StartChannel�����˵�ͨ�������ã�JVN_StopChannel֮����Ҫ���������ֻ�����
         û������ͨ�������ܽ����ֻ����ӣ�
         �ظ����ý������һ�ε���Ϊ׼��
		 ���ھɰ�ֿ�Э�鲻ͬ������ȫ��Ϊ�ֿ�ʹ�ã��ù��ܲ��ܽ������ֺͽ��ã�
		 ʹ�øð�����ʱ��˾��Ʒ�ѿ�����ȫ֧��h264���������ݺͷֿ���ͬ������֧��JGP���ݣ�
		 �����ֻ����������ɣ�
		 <*****ʹ�ý���*****>��
		 1.������ֿص����ݺ͸��ֻ���������ȫ��ͬ��������ʽΪJVN_EnableMOServer(0, TRUE, 0, FALSE, FALSE);
		 2.������ֿص����ݺ͸��ֻ������ݸ�ʽ��ͬ������һ��������������������ʽΪJVN_EnableMOServer(0, TRUE, 0, TRUE, FALSE);
		 3.������ֿص����ݺ͸��ֻ������ݸ�ʽ��ͬ����Ҫ���ֻ��˵�������������ʽΪJVN_EnableMOServer(0, TRUE, 0, TRUE, TRUE);
		 �����ϸ��ֻ���������ֿ����ݷֿ�������С֡��С�������õ�2�ַ�ʽЧ����ѣ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_EnableMOServer(int nChannel, int bEnable, int nOpenType, int bSendVSelf, int bProtocol,int nBufSize);
#else
	JOVISION_API BOOL __stdcall	JVN_EnableMOServer(int nChannel, BOOL bEnable, int nOpenType, BOOL bSendVSelf, BOOL bProtocol,int nBufSize);
#endif

/****************************************************************************
*����  : JVN_SendMOData
*����  : �����ֻ����ݣ���TYPE_MO_TCP/TYPE_MO_UDP������Ч 
*����  : [IN] nChannel   ����ͨ���� >=1
         [IN] uchType    �������ͣ���Ƶ:�Զ�������;
         [IN] pBuffer    ������������,��Ƶ/�Զ�������ʱ��Ч
		 [IN] nSize      �������ݳ���,��Ƶ/�Զ�������ʱ��Ч
*����ֵ: ��
*����  : ��ͨ��Ϊ��λ����ͨ��TYPE_MO_TCP/TYPE_MO_UDP���ӵ������ֻ�����һЩ�Զ������ݣ�
         ����JGP���ݵ���̭���ֻ����ݺ�PC�ֿ���������ȫ��ͬ���ýӿڲ�֧��JPG���ݣ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SendMOData(int nChannel,unsigned char uchType,unsigned char *pBuffer,int nSize);
#else
	JOVISION_API void __stdcall	JVN_SendMOData(int nChannel,unsigned char uchType,BYTE *pBuffer,int nSize);
#endif

/****************************************************************************
*����  : JVN_StartStreamService
*����  : ������ý�����
*����  : [IN] nChannel   ����ͨ���� >=1
         [IN] pSServerIP    ��ý�������IP;
         [IN] nPort   ��ý��������˿�
	     [IN] bAutoConnect ��������ʧ���Ƿ��Զ�����
*����ֵ: �ɹ�
         ʧ��
*����  : ��ͨ������ͨ��ʽ����ʱ����ý���������Ч��
         ��ͨ������ά����ֱ����ʽ����ʱ����ý���������Ч��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_StartStreamService(int nChannel,char *pSServerIP,int nPort,int bAutoConnect);
#else
	JOVISION_API BOOL __stdcall JVN_StartStreamService(int nChannel,char *pSServerIP,int nPort,BOOL bAutoConnect);
#endif

/****************************************************************************
*����  : JVN_StopStreamService
*����  : ֹͣ��ý�����
*����  : [IN] nChannel   ����ͨ���� >=1
*����ֵ: ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_StopStreamService(int nChannel);
#else
	JOVISION_API void __stdcall JVN_StopStreamService(int nChannel);
#endif

/****************************************************************************
*����  : JVN_GetStreamServiceStatus
*����  : ��ѯ��ý�����״̬
*����  : [IN] nChannel   ����ͨ���� >=1
         [OUT] pSServerIP    ��ý�������IP;
         [OUT] nPort   ��ý��������˿�
		 [OUT] bAutoConnect ��ý������Ƿ������Զ�����
*����ֵ: ��ý������Ƿ�ɹ�����
*����  : 2012 2
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_GetStreamServiceStatus(int nChannel,char *pSServerIP,int *nPort,int *bAutoConnect);
#else
	JOVISION_API BOOL __stdcall JVN_GetStreamServiceStatus(int nChannel,char *pSServerIP,int *nPort,BOOL *bAutoConnect);
#endif

/****************************************************************************
*����  : JVN_SetDomainName 
*����  : �����µ�������ϵͳ�������ȡ�������б�
*����  : [IN]  pchDomainName     ����
         [IN]  pchPathName       �����µ��ļ�·���� ���磺"/down/YSTOEM/yst0.txt"
*����ֵ: TRUE  �ɹ�
         FALSE ʧ��
*����  : ϵͳ��ʼ��(JVN_InitSDK)�������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetDomainName(char *pchDomainName,char *pchPathName);
#else
	JOVISION_API BOOL __stdcall	JVN_SetDomainName(char *pchDomainName,char *pchPathName);
#endif

/****************************************************************************
*����  : JVN_SetDeviceName
*����  : ���ñ����豸���� 
*����  : [IN] chDeviceName   �豸����
*����ֵ: ��
*����  : Ϊ�豸��һ���������������豸�����п�����������ʾʹ�ã�
         �ظ����������һ�ε���Ϊ��Ч��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SetDeviceName(char chDeviceName[100]);
#else
	JOVISION_API void __stdcall	JVN_SetDeviceName(char chDeviceName[100]);
#endif

/****************************************************************************
*����  : JVN_SetLocalFilePath
*����  : �Զ��屾���ļ��洢·����������־�����ɵ������ؼ��ļ��� 
*����  : [IN] chLocalPath  ·�� ���磺"C:\\jovision"  ����jovision���ļ���
*����ֵ: ��
*����  : ����ʹ���ڴ濽��ʱ��ע���ʼ�����ַ�������'\0'����
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetLocalFilePath(char chLocalPath[256]);
#else
	JOVISION_API BOOL __stdcall	JVN_SetLocalFilePath(char chLocalPath[256]);
#endif

/****************************************************************************
*����  : JVN_StartBroadcastServer
*����  : �����������㲥����
*����  : [IN] nPort    ����˿ں�(<=0ʱΪĬ��9106,����ʹ��Ĭ��ֵ��ֿض�ͳһ)
         [IN] BCData   �㲥���ݻص�����
*����ֵ: �ɹ�/ʧ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_StartBroadcastServer(int nPort, FUNC_SBCDATA_CALLBACK BCData);
#else
	JOVISION_API BOOL __stdcall JVN_StartBroadcastServer(int nPort, FUNC_SBCDATA_CALLBACK BCData);
#endif

/****************************************************************************
*����  : JVN_StopBroadcastServer
*����  : ֹͣ�������㲥����
*����  : ��
*����ֵ: ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_StopBroadcastServer();
#else
	JOVISION_API void __stdcall JVN_StopBroadcastServer();
#endif

/****************************************************************************
*����  : JVN_BroadcastRSP
*����  : �������㲥��Ӧ
*����  : [IN] nBCID  �㲥��ʶ��ȡ�Իص�����
         [IN] pBuffer �㲥��������
		 [IN] nSize   �㲥�������ݳ���
		 [IN] nDesPort �㲥Ŀ��˿ڣ�ȡ�Իص�������������ֿ�Լ���̶�
*����ֵ: �ɹ�/ʧ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_BroadcastRSP(int nBCID, unsigned char *pBuffer, int nSize, int nDesPort);
#else
	JOVISION_API BOOL __stdcall JVN_BroadcastRSP(int nBCID, BYTE *pBuffer, int nSize, int nDesPort);
#endif

/****************************************************************************
*����  : JVN_SendPlay
*����  : ���ͻط�MP4���� 
*����  : [IN] nClientID   ���Ӻ�
         [IN] uchType     ����
		 [IN] nConnectionType     ��������
		 [IN] ucFrameType    ֡����	JVN_DATA_I JVN_DATA_S...
         [IN] pBuffer    ������������,����
		 [IN] nSize      �������ݳ���,����
         [IN] nWidth    ���
		 [IN] nHeight      �߶�
		 [IN] nTotalFram      ��֡��
		 [IN] time  ʱ���
		 [IN] frame ֡���к�
*����ֵ: ��
*����  : ��ͨ�����ӵ�ĳ������ֿط�������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SendPlay(int nClientID,unsigned char uchType,int nConnectionType,unsigned char ucFrameType,unsigned char *pBuffer,int nSize,int nWidth,int nHeight,int nTotalFram,unsigned long long time,int frame);
#else
	JOVISION_API void __stdcall	JVN_SendPlay(int nClientID,BYTE uchType,int nConnectionType,BYTE ucFrameType,BYTE *pBuffer,int nSize,int nWidth,int nHeight,int nTotalFram,char*  time,int frame);
#endif


/****************************************************************************
*����  : JVN_EnableLANToolServer
*����  : ������رվ������������߷���
*����  : [IN] bEnable         ����/�ر�
         [IN] nPort           ����ʹ�õĶ˿ڣ�=0ʱĬ��Ϊ9104
         [IN] LanToolCallBack ���߻ص�����
*����ֵ: TRUE  �ɹ�
		 FALSE ʧ��
*����  : �������������߻��򱾵�ѯ�ʺ�����Ϣ�����ҷ���������صĸ�����Ϣ
         ��Ϣ��ϸ˵����ο��ص�����˵����
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_EnableLANToolServer(int bEnable, int nPort, FUNC_LANTOOL_CALLBACK LanToolCallBack);
#else
	JOVISION_API bool __stdcall JVN_EnableLANToolServer(BOOL bEnable, int nPort, FUNC_LANTOOL_CALLBACK LanToolCallBack);
#endif

/****************************************************************************
*����  : JVN_RegDownLoadFileName
*����  : ע��ص����������ڵ��������⴦��Զ�������ļ���
*����  : [IN] DLFNameCallBack Զ�������ļ�������ص�����
*����ֵ: ��
*����  : ��ͨ��Ʒ����ʹ�ã�������Ҫ��ĿɶԿͻ��˵������ļ��������δ���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_RegDownLoadFileName(FUNC_DLFNAME_CALLBACK DLFNameCallBack);
#else
	JOVISION_API bool __stdcall JVN_RegDownLoadFileName(FUNC_DLFNAME_CALLBACK DLFNameCallBack);
#endif

/****************************************************************************
*����  : JVN_SetIPCWANMode
*����  : ����IPC�������⴦��ģʽ
*����  : ��
*����ֵ: ��
*����  : ��ͨ��Ʒ����ʹ�ã�������Ҫ��Ĵ�������Ʒ����ʹ�ã�
         ���ú��������佫�����ر��������ٶ��������ӵĸ��ţ�
		 ��JVN_InitSDK��JVN_StartChannelǰ���ü��ɣ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SetIPCWANMode();
#else
	JOVISION_API void __stdcall JVN_SetIPCWANMode();
#endif

/****************************************************************************
*����  : JVN_SetDeviceInfo
*����  : �����豸��Ϣ
*����  : [IN] pstInfo �豸��Ϣ�ṹ��
      	 [IN] nSize   �豸��Ϣ�ṹ���С,����ǰ�����
	     [IN] nflag   ����ѡ���־ 
*����ֵ: �ɹ�����0��ʧ�ܷ���-1
*����  : �������豸֧�ֵ�����ģʽΪ����+wifi����ǰ����ʹ�õ���wifi���������: 
		 nflag = DEV_SET_NET;//�����豸֧�ֵ�����ģʽ
		 pstInfo->nNetMod= NET_MOD_WIFI | NET_MOD_WIRED;
         pstInfo->nNetMod= NET_MOD_WIFI;//�豸��ǰʹ�õ���wifi
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetDeviceInfo(STDEVINFO *pstInfo, int nSize,int nflag);
#else
	JOVISION_API int JVN_SetDeviceInfo(STDEVINFO *pstInfo, int nSize,int nflag);
#endif
	
/****************************************************************************
*����  : JVNS_GetDeviceInfo
*����  : ��ȡ�豸��Ϣ
*����  : [OUT] pstInfo �豸��Ϣ�ṹ��
	     [IN]  nSize   �豸��Ϣ�ṹ���С,����ǰ�����
*����ֵ: �ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_GetDeviceInfo(STDEVINFO *pstInfo,int nSize);
#else
	JOVISION_API int JVN_GetDeviceInfo(STDEVINFO *pstInfo,int nSize);
#endif

/****************************************************************************
*����  : JVN_GetNetSpeedNow
*����  : ��ȡ��ǰ����״��
*����  : [IN] nChannel	ͨ����
		 [IN] nClientID ���Ӻ�
		 [IN] nInterval	������, �ò���ȱʡʱΪĬ��ʱ����16��
		 [OUT] pSpeed ���ص�ǰ����״������λB/s
		 [OUT] pSendOK TRUE��ʾ��������,FALSE��ʾ�з���ʧ�ܣ�ͨ������Ϊ������
*����ֵ: �ɹ�����ֵ>0��ʧ�ܷ���-1
*����  : ���ص�ǰ����״��ֵ��ʧ��ʱԭ����Ҫ����Ƶ����û�п��������߲�������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_GetNetSpeedNow(int nChannel, int nClientID, int nInterval, int* pSpeed, int* pSendOK, unsigned long *pdwFrameDelay);
#else
	JOVISION_API int __stdcall JVN_GetNetSpeedNow(int nChannel, int nClientID, int nInterval, int* pSpeed, BOOL* pSendOK, DWORD *pdwFrameDelay);
#endif

	/****************************************************************************
*����  : JVN_SetLSPrivateInfo
*����  : ���ñ����Զ�����Ϣ�������豸����
*����  : [IN]	chPrivateInfo  �Զ�����Ϣ
		 [IN]	nSize          �Զ�����Ϣ����		 
*����ֵ: ��
*����  : ���ظ����ã������һ�ε���Ϊ׼��֮ǰ�����ݻᱻ���ǣ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SetLSPrivateInfo(char chPrivateInfo[500], int nSize);
#else
	JOVISION_API void __stdcall JVN_SetLSPrivateInfo(char chPrivateInfo[500], int nSize);
#endif

/****************************************************************************
*����  : JVNS_ClearBuffer
*����  : ���ͨ�����ͻ���
*����  : [IN]	nChannel	ͨ����
*����ֵ: ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_ClearBuffer(int nChannel);
#else
	JOVISION_API void __stdcall	JVN_ClearBuffer(int nChannel);
#endif

/****************************************************************************
*����  : JVN_RegNickName
*����  : ע�����
*����  : [IN]	chNickName  ������֧��6-32�ֽ�UTF8�ַ�������ֹʹ�������ַ�����ʽ���ַ���,�ַ����б������һ�������ַ�����ֹʹ��@ ����& * _
*����ֵ: ����ע�������0��ʼ����ע��; -1������Ч; -2δ����; -3����ע����
*����  : ע�᷵�ؽ�����ص�����FUNC_RECVSERVERMSG_CALLBACK ����Ϊ0
*****************************************************************************/

#ifndef WIN32
	JOVISION_API int JVN_RegNickName(char chNickName[36]);
#else
	JOVISION_API int __stdcall JVN_RegNickName(char chNickName[36]);
#endif

/****************************************************************************
*����  : JVN_DeleteNickName
*����  : ɾ������
*����  : [IN]	chNickName  ������֧��6-32�ֽ�UTF8�ַ�������ֹʹ�������ַ�����ʽ���ַ���,�ַ����б������һ�������ַ�����ֹʹ��@ ���� & * _
*����ֵ: ����ɾ�������0��ʼ����ɾ��; -1������Ч; -2δ����; -3����ɾ����
*����  : ɾ���Ƿ�ɹ����ص�����FUNC_RECVSERVERMSG_CALLBACK ����Ϊ2
*****************************************************************************/

#ifndef WIN32
	JOVISION_API int JVN_DeleteNickName(char chNickName[36]);
#else
	JOVISION_API int __stdcall JVN_DeleteNickName(char chNickName[36]);
#endif

/****************************************************************************
*����  : JVN_GetNickName
*����  : ��ȡ����
*����  : ��
*����ֵ: ���ػ�ȡ�����0��ʼ���л�ȡ; -1������Ч;-2���ڻ�ȡ�У��Ժ�����
*����  : ��ȡ������ص�����FUNC_RECVSERVERMSG_CALLBACK ����Ϊ3
*****************************************************************************/

#ifndef WIN32
	JOVISION_API int JVN_GetNickName();
#else
	JOVISION_API int __stdcall JVN_GetNickName();
#endif

/****************************************************************************
*����  : JVN_SendJvnInfo
*����  : ����ÿһ��ͨ������
*����  : [IN]	chnn_info	ͨ������
		 [IN]	svrAddr		��������ַ��ͨ���ص�����FUNC_RECVSERVERMSG_CALLBACK��ȡ		 
*����ֵ: ��
*����  : д���������Ϣ���յ��ص�������ٽ��в�����Ϣ�ķ���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SendJvnInfo(JVSERVER_INFO *jvsvr_info, struct sockaddr_in *svrAddr);
#else
	JOVISION_API void __stdcall JVN_SendJvnInfo(JVSERVER_INFO *jvsvr_info, SOCKADDR_IN *svrAddr);
#endif

/****************************************************************************
*����  : JVN_SendChannelInfo
*����  : ����ÿһ��ͨ������
*����  : [IN]	nMsgLen		��Ϣ����
		 [IN]	chChnInfo	ͨ����ϢCHANNEL_INFO(���), 	
		 [IN]	nConnCount  ÿһ�������Ӧһ������������int��ʾ����2�ֽڱ�ʾp2p�����������1�ֽڱ�ʾת�����������θ�1�ֽڱ�ʾ�ֻ�������
*����ֵ: ��
*����  : �״�����ʱ����ͨ������Ϣ���Ժ�CHANNEL_INFO�еĲ��������仯ʱ�ٴε���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SendChannelInfo(int nMsgLen, char *chChnInfo, int nConnCount);
#else
	JOVISION_API void __stdcall	JVN_SendChannelInfo(int nMsgLen, char *chChnInfo, int nConnCount);
#endif


/****************************************************************************
*����  : JVN_GetExamItem
*����  : ��ȡ�����Ŀ
*����  : [IN,OUT]	pExamItem  ��Ҫ��ϵ���Ŀ�б�4BYTE ���� +�ı� [+ 4BYTE + �ı�...]
[IN]	nSize          ����ص�����		 
*����ֵ: int ���Լ�������Ŀ ==0 û�м���� <0 ��Ŀ�б���̫С
	*****************************************************************************/
	
#ifndef WIN32
	JOVISION_API int  JVN_GetExamItem(char *pExamItem,int nSize);
#else
		JOVISION_API int __stdcall	JVN_GetExamItem(char *pExamItem,int nSize);
#endif
	
/****************************************************************************
*����  : JVN_ExamItem
*����  : ���ĳһ��
*����  : [IN]	nExamType  ������� ��-1 ȫ����� �������ݷ��ص���֪�������
[IN]	pUserInfo          ���ʱ�û���д����Ϣ		 
[IN]	callBackExam          ����ص�����		 
*����ֵ: ��
*****************************************************************************/
	
#ifndef WIN32
	JOVISION_API int  JVN_ExamItem(int nExamType,char* pUserInfo,FUNC_EXAM_CALLBACK callBackExam);
#else
	JOVISION_API int __stdcall	JVN_ExamItem(int nExamType,char* pUserInfo,FUNC_EXAM_CALLBACK callBackExam);
#endif

	/****************************************************************************
*����  : JVN_GetPositionID
*����  : ��ѯ��ǰ���ڵ�����
*����  : nGetType ��ѯ���� 1 ��ͨ����������ѯ 2��ͨ��afdvr��ѯ
*����ֵ: 0 ����  1 ���� -1 ����δ֪
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_GetPositionID(int nGetType);
#else
	JOVISION_API int __stdcall JVN_GetPositionID(int nGetType);
#endif

/****************************************************************************
*����  : JVN_SetNickName
*����  : �����ǳ� 
*����  : [IN] chDeviceName   �豸�ǳ�
*����ֵ: ��
*����  : 
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_SetNickName(char chNickName[32]);
#else
	JOVISION_API void __stdcall	JVN_SetNickName(char chNickName[32]);
#endif

/****************************************************************************
*����  : JVN_SetChannelInfo
*����  : ����ͨ����������Ϣ ��ͨ���������Ե��ã����������ı�����
*����  : nChannelID ��Ҫ���õ�ͨ��
		 pData �������� JVRTMP_Metadata_t
		 nSize ���ݴ�С
*����ֵ: -1 δ��ʼ�� 0 δ�ҵ�ͨ�� 1 �ɹ�
	*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetChannelInfo(int nChannelID,char* pData,int nSize);
#else
	JOVISION_API int __stdcall JVN_SetChannelInfo(int nChannelID,char* pData,int nSize);
#endif

	/****************************************************************************
*����  : JVN_RTMP_Callback
*����  : ������ý����������ӵĻص�
*����  : callBack �ص�����
*����ֵ: -1 δ��ʼ��  1 �ɹ�
*****************************************************************************/
#ifndef WIN32
JOVISION_API int JVN_RTMP_Callback(FUNC_SRTMP_CONNECT_CALLBACK callBack);
#else
JOVISION_API int __stdcall JVN_RTMP_Callback(FUNC_SRTMP_CONNECT_CALLBACK callBack);
#endif


/****************************************************************************
*����  : JVN_RTMP_EnableSelfDefine
*����  : �Զ�����ý����������û��������д��ý�������,StartChannel�󼴿ɵ���
*����  : nEnable ==1 ������ý������� == 0 �����û�ȡ���Ѿ����õ���ý�������
		nChannel ��Ҫ���û�ȡ����ͨ����
		pServerURL ��Ҫ���õ���ý���������ַ �� rtmp://192.168.100.10:1935/a381_1 �ַ����ִ�Сд
nSize ���ݴ�С
*����ֵ: -1 δ��ʼ�� 0 δ�ҵ�ͨ�� 1 �ɹ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_RTMP_EnableSelfDefine(int nEnable,int nChannel,char* pServerURL);
#else
	JOVISION_API int __stdcall JVN_RTMP_EnableSelfDefine(int nEnable,int nChannel,char* pServerURL);
#endif

/****************************************************************************
*����  : JVN_RTMP_SendData
*����  : ������ý�������
*����  : nChannel ��Ҫ���͵�ͨ����
		uchType ֡���� I O P��
		pBuffer ����
		nSize ���ݴ�С
		nSpsCount ��ʱд0
		nPpsCount ��ʱд0
nSize ���ݴ�С
*����ֵ: -1 δ��ʼ�� 0 δ�ҵ�ͨ�� 1 �ɹ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_RTMP_SendData(int nChannel,unsigned char uchType,unsigned char *pBuffer,int nSize,int nSpsCount, int nPpsCount);
#else
	JOVISION_API int __stdcall JVN_RTMP_SendData(int nChannel,unsigned char uchType,unsigned char *pBuffer,int nSize,int nSpsCount, int nPpsCount);
#endif

	
/****************************************************************************
*����  : JVN_GetCloudStoreStatus
*����  : ��ѯ�����Ƿ�֧���ƴ洢
*����  : chCloudBuff ��ȡ�ƴ洢�����洢�ռ䣬�����ڴ潨�鲻С��128�ֽ� ��ʽ�� bucket:host 
		 nBuffSize����Ϊsizeof(chCloudBuff) ���鲻С��128�ֽ�
*����ֵ: -1 ϵͳδ��ʼ�� 0 δ��ѯ�� 1 ֧�� 2 ��֧�ֻ��Ѿ�����
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_GetCloudStoreStatus(char *chCloudBuff, int nBuffSize);
#else
	JOVISION_API int __stdcall JVN_GetCloudStoreStatus(char *chCloudBuff, int nBuffSize);
#endif

	

	/**
	* �ϴ�ָ�����ļ����ƴ洢
	* @param year			[in] �ϴ��������ڵ���
	* @param month			[in] �ϴ��������ڵ���
	* @param day			[in] �ϴ��������ڵ���
	* @param name			[in] �ϴ����ƴ洢����ʾ������
	* @param local_file		[in] Ҫ�ϴ��ı����ļ�·��
	* @param nFlag			[in] �ϴ��ļ���ʶ�������ɱ����ļ��������ļ�δ¼��ʱ���յ��ƴ洢ֹͣ����ñ�־��Ϊ2������Ϊ0
	* @param nType	    	[in] ���� һ������0 �Ŵű���1
	* @return 0 ���سɹ� <0 ʧ��
	* @return 0 ���سɹ�     ����ֵ[������] ʧ��
	* @retval 0 ��ʾ�ɹ�
	* @retval ���� ��ʾʧ�� ������400 403 404 411 -1�����δ��ʼ�� -4�ļ��ͱ����ı���ƥ��
	*/
#ifndef WIN32
	JOVISION_API int JVN_FileUpload(const char *local_file, int nType);
#else
	JOVISION_API int __stdcall JVN_FileUpload(const char *local_file, int nType);
#endif
	
	
	/****************************************************************************
	*����  : JVN_PushAlarmMsg
	*����  : ���ͱ����ı���Ϣ
	*����  : alarm_para ������Ϣ�����ṹ��
	*����ֵ: 0 �ɹ� -1 ϵͳδ��ʼ�� -2 ����δ��ʼ�� -3 ���ݳ��ȳ��������800�ֽڣ�
	-4ͼƬ·����Ч -5������Ƶ·����Ч -6����ָ����Ч
	*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_PushAlarmMsg(ALARM_TEXT_PARA *alarm_para);
#else
	JOVISION_API int __stdcall JVN_PushAlarmMsg(ALARM_TEXT_PARA *alarm_para);
#endif
	
	/****************************************************************************
	*����  : JVN_InitIPCPara
	*����  : ��ʼ������IPC�ϱ�����
	*����  : ipcPara �ϱ������ṹ��
	*����ֵ: 0 �ɹ� -1 ϵͳδ��ʼ�� -2����ָ����Ч
	*����  : SDK��ʼ����ɺ�JVN_InitYST��ʼ��֮ǰ�������JVNS_InitIPCPara�ӿ�
	*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_InitIPCPara(IPC_INIT_PARA *ipcPara);
#else
	JOVISION_API int __stdcall JVN_InitIPCPara(IPC_INIT_PARA *ipcPara);
#endif
	


/****************************************************************************
*����  : JVNS_InitAccount
*����  : ��ʼ���˺�
*����  : [IN] �˺���Ϣ(AccountOnline�ṹ)
         [IN] ��Ϣ����
*����ֵ: ��
*����  : �ú�����������ͨ������ͨ����ǰ���ã�����ͨ������ͨ��������ʧ�ܣ�
         
		 �ú���ֻ�����һ�Σ�����������ͨ����ֻҪ����Ҫ��������ͨ����ģ�
		 ����������ǰ����һ�θýӿڼ��ɣ�

		 �ú�������ʵ���˺����߹���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_InitAccount(char *pchPackOnline, int nLen);
#else
	JOVISION_API void __stdcall	JVN_InitAccount(char *pchPackOnline, int nLen);
#endif


	/****************************************************************************
*����  : JVN_CloudStoreInit
*����  : ׼���ϴ�����
*����  : [IN] year ʱ�� ��
		[IN] month ʱ�� ��
		[IN] day ʱ�� ��
		[IN] name ����
		[OUT] id ��־�� �Ժ��뻺��͹ر�ʱ��Ҫ����

*����ֵ: -1 δ��ʼ����δ��ͨ 0 ʧ�� 1 �ɹ�
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_CloudStoreInit(int year,int month,int day,char* name,int len,int* id);
#else
	JOVISION_API int __stdcall	JVN_CloudStoreInit(int year,int month,int day,char* name,int len,int* id);
#endif


	
/****************************************************************************
*����  : JVN_CloudStoreSendData
*����  : �ϴ�����
*����  : [IN] id ��ʼ��ʱ��ID���
		[IN] buff ��Ҫ�ϴ�������
		[IN] len ���ݴ�С
		[IN] nType ���� 0 ��һ������ 1 �Ŵ�����

*����ֵ: >0 �ɹ� 0 ʧ�� -1 δ��ʼ����δ��ͨ
*****************************************************************************/


#ifndef WIN32
	JOVISION_API int JVN_CloudStoreSendData(int id,char* buff,int len,int nType);
#else
	JOVISION_API int __stdcall	JVN_CloudStoreSendData(int id,char* buff,int len,int nType);
#endif


	/****************************************************************************
*����  : JVN_CloudStoreFinish
*����  : �ϴ����
*����  : [IN] id ��ʼ��ʱ�ķ��صı��

*����ֵ: 1 �ɹ� ����Ϊʧ��
*****************************************************************************/

#ifndef WIN32
	JOVISION_API int JVN_CloudStoreFinish(int id);
#else
	JOVISION_API int __stdcall	JVN_CloudStoreFinish(int id);
#endif

	/****************************************************************************
*����  : JVN_GetTimeZone
*����  : ��ȡ��ǰ��ʱ�� ������������ �������õ����̺߳�̨����
*����  : [OUT] fTimeZone ��Ҫ���ص�ʱ����Ӧ����ֵ

*����ֵ: -1 δ��ʼ������� 0 ��ѯʧ�� 1 �ɹ�
*****************************************************************************/

#ifndef WIN32
	JOVISION_API int JVN_GetTimeZone(float* fTimeZone);
#else
	JOVISION_API int __stdcall	JVN_GetTimeZone(float* fTimeZone);
#endif

/****************************************************************************
*����  : JVN_SendDownFile
*����  : ����Զ���������� 
*����  : [IN] nClientID   ���Ӻ�
         [IN] uchType     ���� JVN_RSP_DOWNLOADSVHEAD��JVN_RSP_DOWNLOADSVDATA...
		 [IN] nConnectionType     ��������
         [IN] pBuffer    ������������,ע����������Ҫ��pBuff+5 ��ʼ
		 [IN] nSize      �������ݳ���
		 [IN] nTotalFram      ��֡��������
*����ֵ: true�����ͳɹ���false����ʧ��
*����  : 
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SendDownFile(int nClientID,unsigned char uchType,int nConnectionType,unsigned char *pBuffer,int nSize,int nTotalFram);
#else
	JOVISION_API int __stdcall	JVN_SendDownFile(int nClientID,unsigned char uchType,int nConnectionType,unsigned char *pBuffer,int nSize,int nTotalFram);
#endif

/****************************************************************************
*����  : JVN_RegRemoteDownLoadCallBack
*����  : ע��Զ�����ػص�����
*����  : [IN] DLFNameCallBack Զ�����ػص�����
         [IN] nType 0,mp4��ʽ���أ�1��sv7��ʽ����
*����ֵ: ��
*����  : 
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_RegRemoteDownLoadCallBack(FUNC_REMOTEDL_CALLBACK DLFNameCallBack,int nType);
#else
	JOVISION_API void __stdcall JVN_RegRemoteDownLoadCallBack(FUNC_REMOTEDL_CALLBACK DLFNameCallBack,int nType);
#endif

/****************************************************************************
*����  : JVN_ActiveHeartBeat
*����  : ����è���豸
*����  : void
*����ֵ: ��
*����  : 
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_ActiveHeartBeat(void);
#else
	JOVISION_API void __stdcall	JVN_ActiveHeartBeat(void);
#endif

/****************************************************************************
*����  : JVN_SetSelfSerListFileName 
*����  : �����Զ�������ͨ������Ϣ��ϵͳ�����ݴ���Ϣ��ȡ�������б�
*����  : [IN]  pchSerHome  �����������б�
		 [IN]  pchSerAll	 ȫ���������б�
*����ֵ: TRUE  �ɹ�
         FALSE ʧ��
*����  : ϵͳ��ʼ��(JVN_InitSDK)������ã���JVN_InitYST֮ǰ����
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetSelfSerListFileName(char *pchSerHome, char *pchSerAll);
#else
	JOVISION_API BOOL __stdcall	JVN_SetSelfSerListFileName(char *pchSerHome, char *pchSerAll);
#endif


/****************************************************************************
*����  : JVN_SetSelfCallback
*����  : ���������Զ���ص�����
*����  : call ��Ҫ���õĻص�����
*����ֵ: ��
*˵��  : ��ʼ����������Ե���
*****************************************************************************/

#ifndef WIN32
	JOVISION_API void JVN_SetSelfCallback(FUNC_SELF_COMMN_CALLBACK call);
#else
	JOVISION_API void __stdcall JVN_SetSelfCallback(FUNC_SELF_COMMN_CALLBACK call);
#endif

	/****************************************************************************
*����  : JVN_SendSelfData
*����  : �����Զ�������
*����  : 
	nClientID	�ͻ���ID
	uchType		��Ϣ����
	pBuffer		��Ҫ���͵�����
	nSize		��Ҫ���͵����ݴ�С
*����ֵ: 
*˵��  : ���ӽ�������Ե���
*****************************************************************************/
	
#ifndef WIN32
	JOVISION_API void JVN_SendSelfData(const int nClientID,const unsigned int uchType,const unsigned char *pBuffer,const unsigned int nSize);
#else
	JOVISION_API void __stdcall JVN_SendSelfData(const int nClientID,const unsigned int uchType,const unsigned char *pBuffer,const unsigned int nSize);
#endif


/****************************************************************************
*����  : JVN_EnableAcceptFile
*����  : ����������Էֿط������ļ�,��MP3
*����  : call ��Ҫ���õĻص�����
*����ֵ: ��
*˵��  : ��ʼ����������Ե���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_EnableAcceptFile(FUNC_ACCEPTFILE_CALLBACK call);
#else
	JOVISION_API void __stdcall JVN_EnableAcceptFile(FUNC_ACCEPTFILE_CALLBACK call);
#endif

/****************************************************************************
*����  : JVN_RequestFileData
*����  : ����������Էֿط������ļ�,��MP3
*����  : 
	nClientID �ͻ���ID
	nMaxLen ��ǰ����������
*����ֵ: ��
*˵��  : ��ʼ����������Ե���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_RequestFileData(const int nClientID,const int nMaxLen);
#else
	JOVISION_API void __stdcall JVN_RequestFileData(const int nClientID,const int nMaxLen);
#endif

/****************************************************************************
*����  : JVN_StartBroadcastSelfServer
*����  : �����������㲥����
*����  : [IN] nPort    ����˿ں�(<=0ʱΪĬ��9108,����ʹ��Ĭ��ֵ��ֿض�ͳһ)
         [IN] BCData   �㲥���ݻص�����
*����ֵ: �ɹ�/ʧ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_StartBroadcastSelfServer(int nPort, FUNC_SBCSELFDATA_CALLBACK BCSelfData);
#else
	JOVISION_API BOOL __stdcall JVN_StartBroadcastSelfServer(int nPort, FUNC_SBCSELFDATA_CALLBACK BCSelfData);
#endif

/****************************************************************************
*����  : JVN_StopBroadcastSelfServer
*����  : ֹͣ�������㲥����
*����  : ��
*����ֵ: ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_StopBroadcastSelfServer();
#else
	JOVISION_API void __stdcall JVN_StopBroadcastSelfServer();
#endif

/****************************************************************************
*����  : JVN_BroadcastSelfRSP
*����  : �������㲥��Ӧ
*����  : [IN] nBCID  �㲥��ʶ��ȡ�Իص�����
         [IN] pBuffer �㲥��������
		 [IN] nSize   �㲥�������ݳ���
		 [IN] nDesPort �㲥Ŀ��˿ڣ�ȡ�Իص�������������ֿ�Լ���̶�
*����ֵ: �ɹ�/ʧ��
*����  : ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_BroadcastSelfRSP(unsigned char *pBuffer, int nSize, int nDesPort);
#else
	JOVISION_API int __stdcall JVN_BroadcastSelfRSP(BYTE *pBuffer, int nSize, int nDesPort);
#endif

/****************************************************************************
*����  : JVN_RegAccpetClientCallBack
*����  : �������ش����Ƿ�������ӻص�����Ҫ����è�ۣ��ڲ���¼��ʱ���ֻ����ܽ������ӣ�ֱ�ӷ�����ʾ��Ϣ������Ӧ�ò�����������
*����  : [IN] FAccpetClientCallBack   ���ش����Ƿ�������ӻص���������������¼��طţ��ֿؽ�ֹ����
*����ֵ: ��

*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_RegAccpetClientCallBack(FUNC_ACCEPTCLIENT_CALLBACK FAccpetClientCallBack);
#else
	JOVISION_API void __stdcall JVN_RegAccpetClientCallBack(FUNC_ACCEPTCLIENT_CALLBACK FAccpetClientCallBack);
#endif

/****************************************************************************
*����  : JVN_GetSDKVer
*����  : ��ȡ�����汾����Ϣ
*����  : [OUT] pVer   ���ذ汾��
		 [OUT] pDescriptionVer   ���ذ汾�ŵ��ַ�����ʾ
*����ֵ: -1 δ��ʼ�� 0 ��ȷ��ȡ��

*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_GetSDKVer(unsigned int* pVer,char* pDescriptionVer);
#else
	JOVISION_API int __stdcall JVN_GetSDKVer(unsigned int* pVer,char* pDescriptionVer);
#endif

/****************************************************************************
*����  : ������Ƶ֡��ͷ
*����  : 
		nChannelID	��Ӧ��ͨ��
		pBuffer		��Ҫ������
		nSize		���ݴ�С
		*����ֵ: ���õ����ݴ�С
*˵��  : ��ͨ���������Ե��ã�ÿ�θı�ͨ�����������ֱ��ʵ���Ϣ(0֡)ʱ��һ��
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetFrameHead(const int nChannelID,const unsigned char *pBuffer,const unsigned int nSize);
#else
	JOVISION_API int __stdcall JVN_SetFrameHead(const int nChannelID,const unsigned char *pBuffer,const unsigned int nSize);
#endif

/****************************************************************************
*����  : JVN_SetNewPosition 
*����  : �����豸���ڵ�������Ϣ����ֹ�����õ�
*����  : [IN]  pchBuff  λ����Ϣ������
			[IN]  nLen	 ���ݴ�С
*����ֵ: 1  ���óɹ�����Ҫ�����ߺ���ã�
			0 ʧ��
*����  : ����֮��ſ��Ե���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetNewPosition(unsigned char *pchBuff, int nLen,FUNC_RECV_FROM_SERVER call);
#else
	JOVISION_API int __stdcall	JVN_SetNewPosition(unsigned char *pchBuff, int nLen,FUNC_RECV_FROM_SERVER call);
#endif
	
/****************************************************************************
*����  : JVN_EnableYST 
*����  : ��ֹ����������ͨ����
*����  : [IN]  nEnable  > 0 �������� 0 ��ֹ����
*����ֵ: 1  ���óɹ�����Ҫ�����ߺ���ã�
			0 ʧ��
*����  : ����֮��ſ��Ե���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_EnableYST(int nEnable);
#else
	JOVISION_API int __stdcall	JVN_EnableYST(int nEnable);
#endif

/****************************************************************************
*����  : JVN_ClearClientBuff 
*����  : ������������Ļ��棬ֻ����������������Ļ��棬�����������յ������Ƿ���O֡����ǰ���,ȷ���ȷ���O֡�ٷ����µ�I֡
*����  : [IN]  nClientID  �ֿ�ID
*����ֵ: 1  ����ɹ�
			0 ʧ��
*����  : �����������˶������������Ӳſ���
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_ClearClientBuff(int nClientID);
#else
	JOVISION_API int __stdcall	JVN_ClearClientBuff(int nClientID);
#endif

/****************************************************************************
*����  : JVN_RegisterCheckFileCallBack 
*����  : ע��Զ�̼����ص������Իص����ļ�����
*����  : [IN]  CheckFileCallBack  �ص�����
*����ֵ: ��
*����  : 
*****************************************************************************/
#ifndef WIN32
	JOVISION_API void JVN_RegisterCheckFileCallBack(FUNC_SCHECKFILENEW_CALLBACK CheckFileCallBack);
#else
	JOVISION_API void __stdcall	JVN_RegisterCheckFileCallBack(FUNC_SCHECKFILENEW_CALLBACK CheckFileCallBack);
#endif

/****************************************************************************
*����  : JVN_SetViewTimeFrame 
*����  :  �����Ƿ�����ʱ���
*����  : [IN]  nVideoTime�� 0 ����������Ƶ����ʱ��� 1�� ��������Ƶ����ʱ����� Ĭ�ϲ�����
         [IN]  nRemoePlay:  0 ������Զ�̻ط�ʱ��� 1�� ����Զ�̻ط�ʱ���  Ĭ�ϲ�����
*����ֵ: 0: ʧ�ܣ�1 �ɹ�
*����  : 
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetViewTimeFrame(char nVideoTime,char nRemoePlay);
#else
	JOVISION_API int __stdcall JVN_SetViewTimeFrame(char nVideoTime,char nRemoePlay);
#endif

/****************************************************************************
*����  : JVN_EnableUPNP
*����  : �����Ƿ���upnp����
*����  : [IN]  bEnable  TRUE:��upnp��FALSE:�ر�upnp
*����ֵ:  0: ʧ�ܣ�1 �ɹ�
*����  : �����Ĭ���Ǵ���upnp�������ر�upnp�������JVN_EnableUPNP(FALSE);
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_EnableUpnp(int nEnable);
#else
	JOVISION_API int __stdcall JVN_EnableUpnp(int nEnable);
#endif

/****************************************************************************
*����  : JVN_SetConUserData
*����  : ��������ʱ�û����ݡ������ݻ������ӳɹ��󴫸��ֿض�
*����  : [IN]  pUserData �û����� 
       : [IN]  len       ���ݳ���
*����ֵ:  0: ʧ�ܣ�1 �ɹ�
*����  : �ӿ����ڵ������ʼ���ӿ�֮������û����Ը����������������
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetConUserData(const char* pUserData,const int len);
#else
	JOVISION_API int __stdcall JVN_SetConUserData(const char* pUserData,const int len);
#endif

/****************************************************************************
*����  : JVN_SetConMemLimit
*����  : ��������ʣ���ڴ�����ӵ�����
*����  : [IN]  nActualLimit  �ڴ�(free+cache)С�ڴ�ֵ�󣬽�ֹʵ�����ӣ���λMB
         [IN]  nVirtualLimit �ڴ�(free+cache)С�ڴ�ֵ�󣬽�ֹ����(����)���� ��λMB
*����ֵ: 1  �ɹ�
		 0 ʧ��
*����  : �����豸�ڴ��ʵ��ʹ���������������ڳ�ʼ��֮�����
		��������ô˽ӿڣ�nActualLimitĬ��Ϊ3MB��nVirtualLimitĬ��Ϊ6MB
*****************************************************************************/
#ifndef WIN32
	JOVISION_API int JVN_SetConMemLimit(int nActualLimit, int nVirtualLimit);
#else
	JOVISION_API int __stdcall	JVN_SetConMemLimit(int nActualLimit, int nVirtualLimit);
#endif

/****************************************************************************
*����  : JVN_SetWatchSocket
*����  : ����Ҫ�����׽���
*����  : [IN]  nSocketFd �����׽���
*����ֵ:  0: ʧ�ܣ�1 �ɹ�
*����  : ���ڴ�������׽��ֺ����
*****************************************************************************/
//#ifndef WIN32
//	JOVISION_API int JVN_SetWatchSocket(const int nSocketFd);
//#else
//	JOVISION_API int __stdcall JVN_SetWatchSocket(const int nSocketFd);
//#endif

#endif
