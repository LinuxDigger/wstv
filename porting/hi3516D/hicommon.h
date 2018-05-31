
#ifndef __HICOMMON_H__
#define __HICOMMON_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "hi_comm_vo.h"
#include "hi_comm_vi.h"
#include "hi_comm_venc.h"
#include "hi_comm_vdec.h"
#include "hi_comm_vpss.h"
#include "hi_comm_aio.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vdec.h"
#include "mpi_vpss.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_isp.h"
#include "hifb.h"

#define	VI_DEVID			0		//Ԥ��
#define	VI_CHNID			0

extern int MAX_FRAME_RATE;	//ĳЩsensor 30֡�����⣬����sensor�޸ġ�Ĭ��30֡

extern int VI_WIDTH;//			1280
extern int VI_HEIGHT;//			720

#define MAX_OSD_WINDOW		10

#define SAMPLE_PRT	Printf

#define JV_AIO_PAYLOAD_TYPE	PT_LPCM

typedef struct{
	int group;
	int bit;
}GpioValue_t;

typedef struct{
	GpioValue_t resetkey; //Reset����
	GpioValue_t statusled; //״ָ̬ʾ��
	GpioValue_t cutcheck; //ҹ�Ӽ��
	GpioValue_t cutday; //�а���
	GpioValue_t cutnight; //������
	GpioValue_t redlight; //����ƿ���
	GpioValue_t alarmin1; //��������1
	GpioValue_t alarmin2; //��������2
	GpioValue_t alarmout1; //�������1
	GpioValue_t sensorreset; //sensor��λ
	GpioValue_t audioOutMute;
	GpioValue_t pir; // PIR���
}higpio_values_t;

extern higpio_values_t higpios;
#define GPIO 1

#endif

