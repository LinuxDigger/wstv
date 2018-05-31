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
	if (hwinfo.bHomeIPC == 1)
	{
		return -1;
	}
	
	jv_gpio_muxctrl(0x200f00C0,0x3); 	//6_0����ΪUARTRX
	jv_gpio_muxctrl(0x200f00C8,0x3); 	//6_2����ΪUARTTX
	jv_gpio_muxctrl(0x200f00C4,0x0); 	//6_1 ����Ϊ6_1��������485��дת��
	jv_gpio_dir_set_bit(6,1,1);		//6_1����Ϊ���
	jv_gpio_write(6,1,1);			//6_1д1����485�������
	
	fd485 = __DecoderOpenCom("/dev/ttyAMA1");
	pthread_mutex_init(&mutex485, NULL);

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

