#ifndef _JV_PRODUCT_DEF_H_
#define _JV_PRODUCT_DEF_H_


// Ӳ���ͺŶ��壬Ĭ��Ϊ��˼����
typedef enum
{
	HW_TYPE_UNKNOWN,
	HW_TYPE_HA210,
	HW_TYPE_HA230,
	HW_TYPE_A4,		// 18EV200 ��A4
	HW_TYPE_C3,		// ע�⣬��C3ָ��˼ƽ̨C3H(HC301)��C3A���ǹ���C3A(HC300)
	HW_TYPE_C5,
	HW_TYPE_C8A,
	HW_TYPE_C8,
	HW_TYPE_C9,
	HW_TYPE_V3,
	HW_TYPE_V6,
	HW_TYPE_V7,
	HW_TYPE_V8,		// V8��C8��������
	HW_TYPE_C8S,	//16EV100ƽ̨
	HW_TYPE_C8H,	//16EV100ƽ̨
	HW_TYPE_C3W,	//16EV100ƽ̨
}HW_TYPE_e;

// ���Ӳ���ͺ��Ƿ�ƥ��
#define HWTYPE_MATCH(type)		((type) == hwinfo.HwType)
#define HWTYPE_NOT_MATCH(type)	(!HWTYPE_MATCH(type))

// ��鵱ǰ�豸�ͺ��Ƿ�Ϊmodel
#define PRODUCT_MATCH(model)		(0 == strcmp(hwinfo.devName, (model)))
#define PRODUCT_NOT_MATCH(model)	(!PRODUCT_MATCH(model))


// ��Ƭ��
#define PRODUCT_HA210			"HA210"			// 100W��Ƭ��
#define PRODUCT_HA230			"HA230"			// 200W��Ƭ��
#define PRODUCT_HA410			"HA410"			// ��A4��100W��Ƭ����������ʽ��Ʒ��

// ҡͷ��
#define PRODUCT_C3A				"HC300"			// 100Wҡͷ�������Ʒ�����
#define PRODUCT_C3H				"HC301"			// 200Wҡͷ��
#define PRODUCT_C4L				"C4L"			// 200Wҡͷ��������C3HӲ������ý��Э�飩
#define PRODUCT_C5				"HC530"			// 100Wҡͷ��
#define PRODUCT_C5S				"HC531"			// 200Wҡͷ��
#define PRODUCT_C6S				"C6S"			// 200Wҡͷ��,C5S������������ܻ�

#define PRODUCT_C5S_M			"C5S-M"			// 200Wҡͷ��������6700WiFi��
#define PRODUCT_V3				"HV310"			// 130Wҡͷ��
#define PRODUCT_V3H				"HV311"			// 200Wҡͷ��(8188Wifi)
#define PRODUCT_V6				"HV610"			// 200Wҡͷ��
#define PRODUCT_C3W				"C3W"			// 200Wҡͷ��

#define PRODUCT_C3C				"HC310"			// 130Wҡͷ��(��˼������100W�Ŵ�)
#define PRODUCT_C5_R1			"HC530-R1"		// 130Wҡͷ��(100W�Ŵ�)

// ��¼һ���
#define PRODUCT_V8A				"HV810A"		// ������V8������Ƶ����ˮ
#define	PRODUCT_V8				"HV810"			// V8������˫���ߣ������ȣ���MIC��WiFi8192
#define PRODUCT_C8A				"HC810A"		// ������C8������Ƶ��ˮ
#define PRODUCT_C8				"HC810"			// C8������˫���ߣ�����Ƶ��WiFi8192
#define PRODUCT_C8S				"C8S"
#define PRODUCT_V8S				"V8S"
#define PRODUCT_C8H				"C8H"

// ����
#define PRODUCT_V7				"HV710"			// V7����
#define PRODUCT_C9				"HC910"			// C9�ǻ���

// ��ó����
#define PRODUCT_HA230E			"HA230E"		// HA230��ó�棬Сά���ʰ�app
#define PRODUCT_HA230C			"HA230C"		// HA230��ó�棬CloudSee
#define PRODUCT_C3E				"HC310E"		// V3��ó�棬Сά���ʰ�app
#define PRODUCT_C3HE			"HC301E"		// C3H��ά��ó�棬Сά���ʰ�app
#define PRODUCT_C8E				"HC810E"		// C8A��ά��ó�棬Сά���ʰ�app��7601 WiFiģ��
#define PRODUCT_C8A_EN			"HC810A-EN"		// C8AСά��ó�棬Сά���ʰ�app, 8188 WiFiģ��

// �ͻ�����
#define PRODUCT_PBS				"PBS3801"		// ����ʿ���Ʋ�Ʒ������Ѹ��ʿƽ̨��Ӳ����V8Aһ��
#define PRODUCT_ZHDZ			"SH-SX1"		// �к����Ӷ��Ʋ�Ʒ������A4Ӳ��
#define PRODUCT_AB1				"AI-AB1001"		// ��ά���Ʋ�Ʒ��������A4Ӳ��
#define PRODUCT_C3H_AP			"HC301-AP"		// �ͻ�����C3HAP���ư汾��ʹ��8188WiFi
#define PRODUCT_HA230_AP		"HA230-AP"		// �ͻ�����HA230-AP���ư汾��ʹ��8188WiFi
#define PRODUCT_DQP_YTJ			"DQP-YTJ01"		// �����ն���ҡͷ����6700Wifi����GPIO��C3H����һ��

#endif

