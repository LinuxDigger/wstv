
#include "jv_common.h"
#include "jv_wdt.h"
#include  "utl_cmd.h"
static HWDT iDog = 0;
/**
 * @brief ����һ���ֶ��رտ��Ź��Ĺ���
 * @param argc
 * @param argv
 * @return
 */
static int  ManualCloseWatchDog(int argc, char *argv[])
{
	if(iDog>0)
		jv_close_wdt(iDog);
	return 0;
}
/**
 *@brief �򿪿��Ź�
 *@note ���������Ȱ�װ
 *@return �ɹ������ش򿪵��豸�����ʧ�ܷ��� -1
 *
 */
HWDT OpenWatchDog()
{

	iDog = jv_open_wdt();
	if(iDog>0)
		utl_cmd_insert("wdtoff","Manual close watchdog","exp: wdtoff",ManualCloseWatchDog);
	return iDog;
}

/**
 *@brief �رտ��Ź�
 *@param S32 iDog �Ѵ򿪵��豸���
 */
void CloseWatchDog()
{
	if(iDog>0)
	{
		jv_close_wdt(iDog);
		iDog = -1;
	}
	return;
}

/**
 *@brief ι��
 *@param S32 iDog �Ѵ򿪵��豸���
 */
void FeedWatchDog()
{
	if(iDog>0)
		jv_feed_wdt(iDog);
	return;
}

