#ifndef _M_PLAY_REMOTE_H
#define _M_PLAY_REMOTE_H


#include "jv_common.h"
#include "m_mp4.h"

typedef enum
{
	EN_PLAYER_TYPE_NORMAL,		//��ͨ�ļ�����
	EN_PLAYER_TYPE_TIMEPOINT,	//����ʱ�������ļ�����
} PlayerType_e;

typedef enum
{
	EN_PLAYER_MODE_ONE,			//��������,������ɼ�ֹͣ
	EN_PLAYER_MODE_ONE_CYCLE,	//����ѭ������
	EN_PLAYER_MODE_SEQUENCE,	//˳�򲥷�,Ĭ�ϰ���ʱ������
	EN_PLAYER_MODE_RANDOM,		//�������
} PlayerMode_e;

typedef enum
{
	EN_PLAYER_NO = 0,			//�ǻط�״̬
	EN_PLAYER_PLAY,				//����
	EN_PLAYER_PAUSE,			//��ͣ
	EN_PLAYER_STEP,				//��֡
	EN_PLAYER_BACKSTEP,			//����
	EN_PLAYER_BACKFAST,			//����
	EN_PLAYER_SPEED,			//��������
	EN_PLAYER_MAX,
} PlayerStatus_e;

//����ĳЩ��������������֡�ο�
//�������ط�ʱ�Ϳ��Զ���һЩ֡�����ٲ���
typedef enum
{
	VDEC_SPEED_SKIP_NO = 0,		//����Ҫ��֡
	VDEC_SPEED_SKIP_ONLY_I = 1,	//ֻ��I֡
	VDEC_SPEED_SKIP_2 = 2,		//2֡��һ��
	VDEC_SPEED_SKIP_4 = 4,		//4֡��һ��
	VDEC_SPEED_SKIP_8 = 8,		//8֡��һ��
	VDEC_SPEED_SKIP_BUTT
	
} VDEC_SKIP_FRAME_E;

typedef struct tag_I_index
{
	S32	nFrames;		//�ؼ�֡��֡��ţ�������֡�����
	S32	nFilePos;		//�ؼ�֡���ļ��е�λ��
}I_Index;

typedef struct
{
	U32				clientId;		//�ֿ�id��
	U32				nConnectionType;//��������
	U64				msLast;			//�ϴε��ȵ�ʱ�䣬��λms������֡�ʿ���
	U64				msWant;			//�������ȵ�ʱ�䣬��λms������֡�ʿ���
	U64				msWantA;		//��Ƶ�������ȵ�ʱ�䣬��λms������֡�ʿ���

//	BOOL			bMute;			//�Ƿ���
	float 			speed;			//�����ٶ�
	U32				nTimeSlice;		//֡�������λms
//	RECT_T 			posOutput;		//����Ļ�ϵ����λ��
//	mplayer_status_callback_t callback;	//�ص�����
	char				fname[256];		//Ҫ���ŵ��ļ���
//	char				nextfname[256];	//��һ��Ҫ���ŵ��ļ�
//	char				priorfname[256];	//��һ��Ҫ���ŵ��ļ�
//	BOOL			nextImmediately;		//���ϸ�����һ���ļ�
	int				setPosInFrame;	//����Ҫ���ŵ�λ�ã���λΪ֡
	BOOL			IFrameBackward;	//I֡����
	int				IFrameBackSpeed;	//�����ٶȡ�
										// 0 ��ʾ��֡���ţ����Ѿ����ţ�
										// 1 ��ʾ��֡���ţ�����һ֡��Ҫ����
										// 4,8,16��ʾ��8���٣�16���ٲ���
	PlayerStatus_e	playStatus;			//����״̬����ͣ�����������֮���
	PlayerStatus_e	oldPlayStatus;		//��ͣǰ�Ĳ���״̬

	VDEC_SKIP_FRAME_E skipflag;		//��֡��ţ��Ƿ���Ҫ��֡����

	U32				frameRate;		//���ŵ�ԭʼ֡��
	U32				frameDecoded;	//�ѽ���֡����
	BOOL 			bMp4;
	void 			*fp;
	U32				nWidth;
	U32				nHeight;
//======mp4ʹ�õ��ֶ�
	MP4_READ_INFO   mp4Info;
	BOOL 			bFileOver;
	U32				frameDecodedA;	//�Ѿ����ŵ���Ƶ֡,������֡�ʿ���
//end
//======sv5ʹ�õ��ֶ�
//	FILE 			*fp;				//�ļ�����
	JVS_FILE_HEADER	header;			//�ļ�ͷ��Ϣ
	I_Index 		*I_IndexList;	//I֡�б�
	int 			I_Cnt;			//I֡����
//end

	pthread_t thread;
	BOOL running;
	BOOL reqEnd;	//�ֿ����󲥷����
//	BOOL created;

}RemotePlayer_t;

//Զ�̻طų�ʼ��
void Remote_Player_Init();

//�ر�Զ�̻ط�
void Remote_Player_Release();

//����Զ�̻طŲ�����
//clientId			:�ֿ�id
//fname 			:Ҫ���ŵ��ļ���
//nConnectionType	:��������
//nSeekFrame        :��λ��֡,�Ͷ�λ��ʱ��㲻��ͬʱʹ��
//nSeekTime	        :��λ��ʱ���
//return 0 �ɹ���<0 ʧ��
int Remote_Player_Create(S32 clientId, int nConnectionType, PlayerType_e playType, PlayerMode_e playMode, void *param);
int Remote_Tutk_Player_Create(S32 clientId, char *fname, int nConnectionType);

//�ر�Զ�̻طŲ�����
int Remote_Player_Destroy(S32 clientId);
int Remote_Tutk_Player_Destroy(S32 clientId);

//����
int Remote_Player_Fast(S32 clientId);

//����
int Remote_Player_Slow(S32 clientId);

//�����ٶȲ���
int Remote_Player_PlayNormal(S32 clientId);

//��ͣ
int Remote_Player_Pause(S32 clientId);
int Remote_Tutk_Player_Pause(S32 clientId);


//ȡ����ͣ
int Remote_Player_Resume(S32 clientId);
int Remote_Tutk_Player_Resume(S32 clientId);

//���ò���λ��
int Remote_Player_Seek(S32 clientId, U32 value);

//sv5��֡����ת��������ͨ��֡����
int Remote_Frametype_sv5_to_yst(S32 typeStorage);

//��ȡmp4�ļ���Ϣ
int Remote_ReadFileInfo_MP4(RemotePlayer_t *player);
//��ȡmp4֡
BOOL Remote_Read_Frame_MP4(void *handle, MP4_READ_INFO *pInfo, PAV_UNPKT pPack);

//timeʱ����¼��ط�
typedef struct
{
	int clientId;
	int connectType;
	unsigned int play_time;
	unsigned int base_time;
}REPLAY_INFO;

int Remote_Player_Time_Destroy(S32 clientId);
void Remote_Player_Time(S32 clientId, int nConnectionType, unsigned int play_time,unsigned long long base_time);


//qq����Զ�̻ط�
void Remote_play_history_video(int channelno,char* filename,int starttime,int timeoffset);
int Remote_Destroy_history_video();

#endif

