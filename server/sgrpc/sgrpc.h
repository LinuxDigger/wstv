/*
 * sgrpc.h
 *
 *  Created on: 2014��10��18��
 *      Author: LiuFengxiang
 *		 Email: lfx@jovision.com
 */

#ifndef SGRPC_H_
#define SGRPC_H_

#ifdef __cplusplus
extern "C" {
#endif

int sgrpc_init();

char *sgrpc_parse_yst(const char *data);

int sgrpc_free_yst(char *resp);

/**
 *@brief ���ʻ���Ϣ�����仯ʱ����ʹ��ˢ���ʻ���Ϣ
 */
int sgrpc_account_refresh();

#ifdef __cplusplus
}
#endif

#endif /* SGRPC_H_ */
