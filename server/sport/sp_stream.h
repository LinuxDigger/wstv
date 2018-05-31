/*
 * sp_stream.h
 *
 *  Created on: 2013-11-20
 *      Author: LK
 */

#ifndef SP_STREAM_H_
#define SP_STREAM_H_
#include "sp_define.h"
typedef enum{
	ENCODE_H264_LEVEL_BASE,
	ENCODE_H264_LEVEL_MAIN,
	ENCODE_H264_LEVEL_HIGH,
}EncodeH264Level_e;

typedef enum
{
	SP_VENC_RC_MODE_CBR,
	SP_VENC_RC_MODE_VBR,
	SP_VENC_RC_MODE_FIXQP
}sp_venc_rc_mode_e;

typedef struct
{
	BOOL		bEnable;	///< �Ƿ�ʹ��
	BOOL		bAudioEn;	//�Ƿ������Ƶ����

	unsigned int	viWidth;		///< ����Ŀ��
	unsigned int	viHeight;		///< ����ĸ߶�

	unsigned int	width;			///< resolution width
	unsigned int	height;			///< resolution height
	unsigned int	framerate;		///< framerate such as 25, 30 ...
	unsigned int	bitrate;		///< KBit per second
	unsigned int	ngop_s;			///< I֡���������Ϊ��λ
	sp_venc_rc_mode_e rcMode;
	EncodeH264Level_e encLevel;

	int quality; 		//��Ƶ������ȡֵ 0 �� 100
	int minQP;
	int maxQP;
}SPStreamAttr_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@brief ��ȡ����
 *@param channelid ͨ����
 *@param attr ����
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int sp_stream_get_param(int channelid, SPStreamAttr_t *attr);

/**
 *@brief ���ò���
 *@param channelid ͨ����
 *@param attr ����
 *
 *@retval 0 �ɹ�
 *@retval <0 errno such as #JVERR_BADPARAM
 *
 */
int sp_stream_set_param(int channelid, SPStreamAttr_t *attr);

typedef struct{
	SPRes_t *resList;//����ֱ��ʵ��б�
	int resListCnt;//����ֱ��ʵĿ�ѡ����
	int maxNGOP;//���nGOP
	int minNGOP;//��СnGOP
	int maxFramerate;//���֡��
	int minFramerate;//��С֡��
	int maxKBitrate;//������ʣ���λΪKBit
	int minKBitrate;//��С����
	SPRes_t inputRes;//����ֱ���

	int maxRoi;//����Ȥ���������֧�ֵĸ�����0��ʾ��֧��
}SPStreamAbility_t;

int sp_stream_get_ability(int channelid,SPStreamAbility_t *ability);

/**
 *@brief ��ȡ�����õ�URI
 */
char *sp_stream_get_stream_uri(int channelid, char *uri, int maxUriLen);

/**
 *@brief ǿ�����ɹؼ�֡
 */
void sp_stream_request_idr(int channelid);

/**********************************************JVS IPC SDK LK140721***************************************/
/**
 *@brief frame type definiton
 */
typedef enum
{
	SP_FRAME_TYPE_I,
	SP_FRAME_TYPE_P,
	SP_FRAME_TYPE_B,
	SP_FRAME_TYPE_A,	///< ��Ƶ֡
	SP_FRAME_TYPE_MAX
}sp_frame_type_e;
/**
 *@brief SDK�ⲿ�ص�����
 *       ���ǽ����ÿ��֡ͨ������������ظ��ⲿ�ͻ�
 *@param channelid ֡���ڵ�ͨ���ţ�����0����
 *@param data ����buffer
 *@param size ���ݳ���
 *@param type ֡����
 *
 */
typedef void (*sp_stream_callback_t)(int channelid, void *data, unsigned int size, sp_frame_type_e type, unsigned long long timestamp);
/*
 * @brief ע���ȡ�����Ļص�����
 */
int sp_stream_register_callback(sp_stream_callback_t callback);

/**********************************************JVS IPC SDK***************************************/
#ifdef __cplusplus
}
#endif

#endif /* SP_STREAM_H_ */
