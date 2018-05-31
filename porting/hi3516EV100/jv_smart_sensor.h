
/**
 *@file jv_smart_sensor.h file about judge light by sensor-self
 *@author lyl
 */
#ifndef _JV_SMART_SENSOR_H_
#define _JV_SMART_SENSOR_H_
typedef void (*jv_msensor_setcut_callback_t)(int param);
typedef int (*jv_msensor_obslog_callback_t)(char* report, char *log, ...);
int jv_msensor_set_obslog_callback(jv_msensor_obslog_callback_t callback);

/**
 *@brief �����2.0��ʼ��
 *@param	sensorid ���� SENSOR_OV2735 ...
 */
int jv_sensor_smart_init(int sensorid);

/**
 *@brief �����2.0��ҹ�ж�
 *@param	bNight��ǰ��ҹģʽ��index ��ҹ�л�ʱ�̵ȼ���adcv adc��ֵ
 */
int jv_sensor_smart_judge(int bNight, unsigned int index, unsigned int adcv);


int jv_sensor_ircut_check(int sensorid, int nightmode, jv_msensor_setcut_callback_t setcut);
#endif

