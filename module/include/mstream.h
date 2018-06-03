#ifndef __MSTREAM_H__
#define __MSTREAM_H__
#include "jv_ai.h"
#include "jv_stream.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int ior_reverse;				//�Ƿ�ѡ
	RECT roi[MAX_ROI_REGION];		//��Χ����VIWidthxVIHeightΪ��׼
	int roiWeight;				///< Ȩ�ء���ֵ����  0 ~ 255֮��
}mstream_roi_t;

typedef struct
{
	BOOL		bEnable;	///< �Ƿ�ʹ��
	BOOL		bAudioEn;	//�Ƿ������Ƶ����

	unsigned int	viWidth;		///< ����Ŀ��
	unsigned int	viHeight;		///< ����ĸ߶�

	unsigned int	width;			///< resolution width
	unsigned int	height;			///< resolution height
	unsigned int	framerate;		///< framerate such as 25, 30 ...
	unsigned int	nGOP_S;			///< I֡���������Ϊ��λ
	jv_venc_rc_mode_e rcMode;
	unsigned int	bitrate;		///< KBit per second
	int minQP;
	int maxQP;

	BOOL bRectStretch; //����16��9��4��3֮��Ĳ��죬�����죬���ǲü���Ϊ��ʱ��ʾ����
	VENC_TYPE vencType;		// ��Ƶ����Э������
	BOOL bLDCEnable;
	int  nLDCRatio;
}mstream_attr_t;

/**
 *@brief ���������Ļص�����
 * mstream������յ���ÿ��֡ͨ�������������
 *@param channelid ֡���ڵ�ͨ����
 *@param data ���ݵ�ַ
 *@param size ���ݳ���
 *@param type ֡����
 *@param timestamp ʱ���
 *
 */
typedef void (*mstream_transmit_callback_t)(int channelid, void *data, unsigned int size, jv_frame_type_e type, unsigned long long timestamp);

/**
 *@brief ��ʼ��
 *
 */
int mstream_init(void);

/**
 *@brief ����
 *
 */
int mstream_deinit(void);

/**
 *@brief ���������Ļص�����
 *@param callback �����Ļص�����ָ��
 *
 */
int mstream_set_transmit_callback(mstream_transmit_callback_t callback);

/**
 *@brief ���ò���
 *@param channelid ͨ����
 *@param attr ����
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int mstream_set_param(int channelid, mstream_attr_t *attr);

int mstream_stop(int channelid);
/**
 *@brief ˢ��ͨ����ʹ֮ǰ��������Ч
 *@param channelid ͨ����
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int mstream_flush(int channelid);

/**
 *@brief �ر�ͨ��
 *@param channelid ͨ����
 *
 *@retval 0 �ɹ�
 *@retval <0 errno
 *
 */
int mstream_stop(int channelid);

/**
 *@brief ���ò����ļ�д��
 *�����������ִ��#mstream_flush ����
 *
 */
#define mstream_set(channelid, key, value)\
do{\
	mstream_attr_t attr;\
	mstream_get_param(channelid, &attr);\
	attr.key = value;\
	mstream_set_param(channelid, &attr);\
}while(0)


/**
 *@brief ��ȡ����������ʱ����
 *@param channelid ͨ����
 *@param attr ����
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int mstream_get_param(int channelid, mstream_attr_t *attr);

/**
 *@brief ��ȡ���в�������������ʱ������
 *@note ��Щʱ�����ڱ������������ƣ����õĲ�����ʵ�����в�������
 *@param channelid ͨ����
 *@param attr ����
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int mstream_get_running_param(int channelid, mstream_attr_t *attr);

/**
 *@brief ����֡��
 *@param channelid ͨ����
 *@param framerate ֡�ʣ���30��25��20������
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int mstream_set_framerate(int channelid, unsigned int framerate);

/**
 *@brief ����֡��
 *@param channelid ͨ����
 *@param framerate ֡�ʣ���30��25��20������
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int mstream_set_resolution(int channelid, unsigned int width, unsigned int height);

/**
 *@brief ���ùؼ�֡���
 *@param channelid ͨ����
 *@param gop �ؼ�֡���������Ϊ��λ
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int mstream_set_gop(int channelid, unsigned int gop);

int mstream_set_bitrate(int channelid,unsigned int bitrate);

/**
 *@brief ��ȡ֧�ֵķֱ��ʵ��б�
 *@param list ��� �ֱ����б�
 *
 *@return �ֱ����б�ĸ���
 *
 */
#define mstream_get_valid_resolution jv_stream_get_valid_resolution

/**
 *@brief ��ָ�� �Ŀ�Ⱥ͸߶ȣ�Ѱ����ƥ��ķֱ���
 *
 */
void mstream_resolution_valid(int channelid, unsigned int *width, unsigned int *height);

/**
 *@brief �����������һ֡�ؼ�֡
 *��¼��ʼ�����пͻ�������ʱ���ô˺���
 *
 *@param channelid ͨ���ţ���ʾҪ�����ͨ������ؼ�֡
 *
 */
void mstream_request_idr(int channelid); 

/**
 *@brief ����ָ��������������
 *
 *@param channelid ͨ���ţ�Ŀǰ�����Ϊ��sensor������0����
 *@param brightness ����ֵ
 *
 */
void mstream_set_brightness(int channelid, int brightness); 

/**
 *@brief ����ָ����������ȥ��ǿ��
 *
 *@param channelid ͨ���ţ�Ŀǰ�����Ϊ��sensor������0����
 *@param antifog ȥ��ǿ��
 *
 */
void mstream_set_antifog(int channelid, int antifog); 

/**
 *@brief ����ָ���������ı��Ͷ�
 *
 *@param channelid ͨ���ţ�Ŀǰ�����Ϊ��sensor������0����
 *@param saturation ���Ͷ�ֵ
 *
 */
void mstream_set_saturation(int channelid, int saturation); 

/**
 * ����RTSP���������û���������
 */
int mstream_rtsp_user_changed(void);

int mstream_audio_set_param(int channelid, jv_audio_attr_t *attr);
int mstream_audio_restart(int channelid,int bEn);

/**
 * ��ȡ����Ȥ������Ϣ
 */
int mstream_get_roi(mstream_roi_t *mroi);
/**
 * ���ø���Ȥ������Ϣ
 */
int mstream_set_roi(mstream_roi_t *mroi);

#ifdef __cplusplus
}
#endif

#endif

