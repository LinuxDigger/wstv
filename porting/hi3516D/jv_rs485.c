#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <jv_common.h>
#include <jv_gpio.h>

enum PARITYBIT_ENUM
{
	PAR_NONE,
	PAR_EVEN,
	PAR_ODD
};

static int fd485 = -1;
static pthread_mutex_t mutex485;  	//485���߻�����

//����˵�� : �򿪴���
//����     : char *cFileName:������
//����ֵ   : �ļ�������,�������-1��ʾ��ʧ��
static int __DecoderOpenCom(const char *pFileName)
{
    if(NULL == pFileName || 0 == pFileName[0])
    {
        return -1;
    }
    else
    {
        return open(pFileName, O_SYNC | O_RDWR | O_NOCTTY);
    }
}

int jv_rs485_init(void)
{
		
	jv_gpio_muxctrl(0x200F007C, 1); 	//9_3 ����ΪUART1_RXD
	jv_gpio_muxctrl(0x200F0084, 1); 	//9_5 ����ΪUART1_TXD
	jv_gpio_muxctrl(0x200F0078, 0); 	//9_2 ����Ϊ9_2
	jv_gpio_dir_set_bit(9, 5, 1);		//9_5����Ϊ���
	jv_gpio_dir_set_bit(9, 3, 0);		//9_3����Ϊ���� 
	jv_gpio_dir_set_bit(9, 2, 1);

	jv_gpio_write(9, 2, 1);			//9_2д1����485�������
	
	return 0;
}

int jv_rs485_deinit(void)
{
	printf("jv_rs485_deinit\n");
	if(fd485 > 0)
		close(fd485);
	pthread_mutex_destroy(&mutex485);
	return 0;
}

//���485�豸���
int jv_rs485_get_fd()
{
	return fd485;
}

//485����
void jv_rs485_lock()
{
	pthread_mutex_lock(&mutex485);
}

//����
void jv_rs485_unlock()
{
	pthread_mutex_unlock(&mutex485);
}

