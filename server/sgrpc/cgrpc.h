/*
 * cgrpc.h
 *
 *  Created on: 2015��01��23��
 *      Author: QinLichao
 *		 Email: qinlichao@jovision.com
 */

#ifndef CGRPC_H_
#define CGRPC_H_
#ifdef __cplusplus
extern "C" {
#endif

/*��ʼ��grpc�����������߳���grpc��������������*/
int cgrpc_init();

typedef struct{
	char startTime[16];		// ��ȫ������ʼʱ��(��ʽ: hh:mm:ss)
	char endTime[16];			// ��ȫ��������ʱ��(��ʽ: hh:mm:ss)	
}timeRange_t;

typedef struct{
	char type[32]; //video,io,analyse
	char subtype[32]; //videoLost,motionDetect,videoBlock,hardDriverLost,doorAlarm,smoke,pir,invasion
	char pir_code[32]; //alarm details
	char detector_id[32]; //xxx
	char dev_id[16]; //A402153844
	char dev_type[8]; //card,dvr,ipc,nvr
	int channel;
} AlarmInfo_t;

typedef struct{
	int bEnable; //�Ƿ�����ȫ����
	timeRange_t timeRange[7];	// ��ȫ����ʱ�������
} DeploymentInfo_t;

/*����grpc��������*/
int cgrpc_keep_online();

/*����grpc������Ϣ*/
int cgrpc_alarm_report(AlarmInfo_t *alarmInfo);

/*��VMS���������Ͳ��س�����Ϣ*/
int cgrpc_alarm_deployment_push(DeploymentInfo_t *deploymentInfo);

#ifdef __cplusplus
}
#endif

#endif /* CGRPC_H_ */
