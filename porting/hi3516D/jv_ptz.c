/*
 * jv_ptz.c
 *
 *  Created on: 2014��8��12��
 *      Author: lfx  20451250@qq.com
 *      Company:  www.jovision.com
 */

#include <jv_common.h>

#include "jv_ptz.h"


jv_ptz_func_t g_ptzfunc ;

/**
 *@brief ��ʼ��
 */
int jv_ptz_init()
{
	memset(&g_ptzfunc, 0, sizeof(g_ptzfunc));

	return 0;
}

/**
 *@brief ����
 */
int jv_ptz_deinit()
{
	return 0;
}


