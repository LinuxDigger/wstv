/** ===========================================================================
* @path $(IPNCPATH)\interface\inc\
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file Msg_Def.h
* @brief Definition of message command, message key, and message type.
*/
#ifndef __MSG_H__
#define __MSG_H__

#if defined (__cplusplus)
extern "C" {
#endif
typedef int  (*Handle_get_ircut_staus_callback_t)(void);

#if 1		//SNS
enum E_LENS_PARAM{
	E_CMD_LENS_INIT=1,				//?�̨���init
	E_CMD_LENS_ZOOM,			//?�̨���ZOOM	
	E_CMD_LENS_FOCUS,				//?�̨���AF
	E_CMD_LENS_PRESET_POS,			//?�̨���ZOOM
	E_CMD_LENS_IRCUT,			//?�̨���IRCut
	E_CMD_LENS_IRIS,				//?�̨���Iris
	E_CMD_LENS_CONFIG,			//?�̨���????
	E_CMD_LENS_STATE,			//?�̨������䨬?2��?��
	E_CMD_LENS_DAYNIGHT,			//?�̨������䨬?2��?��

	E_CMD_PTZ_TRANSMIT,	//???��?����??�¡�?��?	
	E_CMD_PTZ_TOUR_SET,		//???��?��?2o?����??	
	E_CMD_PTZ_TOUR_START,				//???��?��?2o????��	
	E_CMD_PTZ_DIRECTION_CONTROL,		//???��?����??��????
	E_CMD_PTZ_POS_SET,					//???��?��????????����??
	E_CMD_PTZ_ZONE_SCAN,				//???��?��??��������?��
	E_CMD_PTZ_PATTERN_SCAN,			//???��?��?��?������?��
	E_CMD_PTZ_PRESET,			//???��?��?��??��?
	E_CMD_LENS_ZOOMRATIO,
	E_CMD_LENS_AFMANUALAUTO,///AF �Զ��ֶ�ģʽ�л�
	E_CMD_LENS_AFRESET,
	E_CMD_LENS_DOMERESUALTAF,
};

#pragma pack(1)

typedef struct {//0x33.����???�̨���ZOOM
	unsigned char bCmdType;
	unsigned char bCmdParam;		//AF?����?����D��
	unsigned int nZoomSection;		//ZOOM?����?����D��
	unsigned int nFocusPos;		//ZOOM??��?????
	unsigned int nFocusZeroPos;		//Focus PI point
	unsigned char nNDMin;
	unsigned char nNDMax;	
	unsigned char init;		
}T_LENS_INIT;

typedef struct {//0x33.����???�̨���ZOOM
	unsigned char bCmdType;
	unsigned char bCmdParam;		//AF?����?����D��
	unsigned int nSpeed;		//ZOOM?����?����D��
	int nZoomSection;		//ZOOM??��?????
	int nFocusPos;		//ZOOM??��?????
	unsigned char bResetRouteFlag;	///���ڿ����µĳ����ı�����¸߱����ͱ�ʱ��λ����
	unsigned char bIsDomeMove;
	unsigned char ManuInitMotorFlag;
}T_LENS_ZOOM;

typedef struct {				//0x32.����???�̨���AF
	unsigned char bCmdType;
	unsigned char bCmdParam;		//AF?����?����D��
	unsigned char bSide;		//far --2   near---1
	unsigned int nStep;		//AF��??��2?3�� ��??��1-100�ꡧ��??��???1������DD���?
}T_LENS_FOCUS;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned char bCmdParam;		//AF?����?����D��
	unsigned int nIRCut;		//IRCut1|?��
}T_LENS_IRCUT;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned char bCmdParam;		
	unsigned int nStep;		
}T_LENS_IRIS;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned char bCmdParam;		//AF?����?����D��
	unsigned int nStatus;		//
	///1-44  44��Ӧ18��
	unsigned int nZoomSection;		//ZOOM??��?????
	int nZoomPos;		//��ͷλ��20--2020
	int nFocusPos;		//�۽�λ��
	unsigned int nCurTime;
	unsigned char bDayNight;		///���� 1  ����0
	unsigned char bResetRouteFlag;	///���ڿ����µĳ����ı�����¸߱����ͱ�ʱ��λ����
	unsigned char LenRatio;
	Handle_get_ircut_staus_callback_t get_ircut_staus;
}T_LENS_STATUS;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned char bCmdParam;		
	unsigned int nRoute;		
}T_LENS_CONFIG;

typedef struct {//
	unsigned char bCmdType;
	unsigned char bCmdParam;		
	unsigned char nToDayValue;		
	unsigned char nToNightValue;		
}T_LENS_DAYNIGHT;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned char bCmdParam;		// 1,�Զ� 0���ֶ�		
}T_LENS_FOCUSMANAUTO;


typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned char bCmdParam;		// 1,��������۽� 0����		
}T_LENS_DOMECTRLAF;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned short Ratio;		// ���ʣ�viscaЭ��ת����	
	int focuspos;				
}T_LENS_ZOOMRATIO;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned short Ratio;		// ���ʣ�viscaЭ��ת����	
	int focuspos;				
}T_LENS_ZOOMDIRECT;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;
	unsigned short Ratio;		// ���ʣ�viscaЭ��ת����	
	int focuspos;				
}T_LENS_FOCUSDIRECT;

typedef struct {//0x34.����???�̨���IRCut1|?��
	unsigned char bCmdType;			
}T_LENS_RESET;
#pragma pack(0)

#endif

void *VIDEO_afMsgMain(void *arg);
void *Video_afMain(void *arg);


#if defined (__cplusplus)
}
#endif

#endif

