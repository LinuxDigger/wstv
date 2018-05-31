#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include "libPTZ.h"

//����˵�� : �ַ���ת���ɲ�����
//����     : HI_U32 nIndex :COMBO������
//����ֵ   : ������ ; -1:ʧ��
int Ptz_nToBaudrate(int nBaud)
{
    switch(nBaud)
    {
    case 1200:
        return B1200;

    case 2400:
        return B2400;

    case 4800:
        return B4800;

    case 9600:
        return B9600;
 	case 19200 :
        return B19200;
	case 38400:
	     return B38400;
	case 57600:
	     return B57600;
	case 115200:
	     return B115200;

    default:
        return B2400;
    }
}

//����˵�� : �򿪴���
//����     : char *cFileName:������
//����ֵ   : �ļ�������,�������-1��ʾ��ʧ��
int DecoderOpenCom(const char *pFileName)
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

//����˵�� : �رմ���
//����     : int fd:���ڵ��ļ�������
//����ֵ   : 0:�ɹ�; -1:ʧ��
int DecoderCloseCom(int fd)
{
	if(fd<=0)return 0;
	return close(fd);
}


//����˵�� : ���ò�����
//����     : int fd:���ڵ��ļ�������
//           int nSpeed:������:B115200,B57600,B38400,B19200,B9600,B4800,B2400,B1200,B300
//����ֵ   : 0:�ɹ�; -1:ʧ��
int DecoderSetComBaudrate(int fd, int nSpeed)
{
	struct termios   Opt;
	tcgetattr(fd, &Opt); 
	//tcflush(fd, TCIOFLUSH);  
	int nBaud = Ptz_nToBaudrate(nSpeed);
	//printf("nSpeed=%d,nBaud=%d\n",nSpeed,nBaud);
	//���ò�����
	if(fd<=0)return -1;
	switch(nBaud)
	{
    case B300:
	case B1200:
	case B2400:
	case B4800:
	case B9600:
	case B19200:
	case B38400:
	case B57600:
	case B115200:
    	cfsetispeed(&Opt, nBaud);
    	cfsetospeed(&Opt, nBaud);
    	break;
    default:
        printf("DecoderSetCom:Unsupported baudrate!\n"); 
        return -1;
	}
	tcflush(fd, TCIOFLUSH);
    //���ô�������
	if (0 != tcsetattr(fd, TCSANOW, &Opt)) 
	{        
		printf("DecoderSetCom: tcsetattr err\n");  
		return -1;     
	}    

	//��մ�������
	tcflush(fd,TCIOFLUSH);
	return 0;
}

//����˵�� : ���ô�������λ
//����     : int fd:���ڵ��ļ�������
//           int nDatabits:����λλ��:5,6,7,8
//           int nStopbits:ֹͣλλ��:1,2
//           int nParity:У��λ��ʽ:PAR_NONE,PAR_ODD,PAR_EVEN
//����ֵ   : 0:�ɹ�; -1:ʧ��
int DecoderSetComBits(int fd, int nDatabits, int nStopbits, int nParity)
{
	struct termios   Opt;
	tcgetattr(fd, &Opt); 
	if(fd<=0)return -1;
	//��������λ��
	Opt.c_cflag &= ~CSIZE; 
	switch (nDatabits)
	{  
	case 5	:
		Opt.c_cflag |= CS5;
		break;
	case 6	:
		Opt.c_cflag |= CS6;
		break;
	case 7:		
		Opt.c_cflag |= CS7; 
		break;
	case 8:     
		Opt.c_cflag |= CS8;
		break;   
	default:    
		printf("DecoderSetCom: Unsupported data size\n"); 
		return -1;  
	}

    //����У��λ
    switch (nParity) 
    {   
	case PAR_NONE:    
		Opt.c_cflag &= ~PARENB;             // Clear parity enable 
		Opt.c_iflag &= ~INPCK;              // Clear parity checking  
		break;  
	case PAR_ODD:   
		Opt.c_cflag |= (PARODD | PARENB);   // ����Ϊ��Ч��  
		Opt.c_iflag |= INPCK;               // Enable parity checking  
		break;  
	case PAR_EVEN:  
		Opt.c_cflag |= PARENB;              // Enable parity     
		Opt.c_cflag &= ~PARODD;             // ת��ΪżЧ��     
		Opt.c_iflag |= INPCK;               // Enable parity checking 
		break;
	default:   
		printf("DecoderSetCom: Unsupported parity\n");    
		return -1;  
	}  

	//����ֹͣλ
    switch (nStopbits)
    {   
	case 1:    
		Opt.c_cflag &= ~CSTOPB;  
		break;  
	case 2:    
		Opt.c_cflag |= CSTOPB;  
	   break;
	default:    
		 printf("DecoderSetCom: Unsupported stop bits\n");  
		 return -1; 
    } 

    //��������    	
	Opt.c_cflag &= ~CRTSCTS;
	Opt.c_cflag |= (CLOCAL | CREAD);

	Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	Opt.c_lflag |= FLUSHO;	// Output flush
	
	Opt.c_oflag &= ~OPOST;
	Opt.c_cc[VMIN] = 0;
	Opt.c_cc[VTIME] = 1;
	Opt.c_iflag = IGNBRK | IGNPAR;
	tcflush(fd, TCIOFLUSH);
    //���ô�������
	if (0 != tcsetattr(fd, TCSANOW, &Opt)) 
	{        
		printf("DecoderSetCom: tcsetattr err\n");  
		return -1;     
	}    

	//��մ�������
	tcflush(fd,TCIOFLUSH);
	return 0;
}

//����˵�� : ���ô���
//����     : int fd:���ڵ��ļ�������
//           PNC_PORTPARAMS pParam:��������
//����ֵ   : 0:�ɹ�; -1:ʧ��
int DecoderSetCom(int fd, PNC_PORTPARAMS pParam)
{
	struct termios   Opt;
	tcgetattr(fd, &Opt); 
	if(fd<=0)return -1;
	//���ò�����
	int nBaud = Ptz_nToBaudrate(pParam->nBaudRate);
	switch(nBaud)
	{
	case B300:
	case B1200:
	case B2400:
	case B4800:
	case B9600:
	case B19200:
	case B38400:
	case B57600:
	case B115200:
    	cfsetispeed(&Opt, nBaud);
    	cfsetospeed(&Opt, nBaud);
    	break;
    default:
        printf("DecoderSetCom:Unsupported baudrate!\n"); 
        return -1;
	}

	//��������λ��
	Opt.c_cflag &= ~CSIZE;
	switch (pParam->nCharSize)
	{
	case 5	:
		Opt.c_cflag |= CS5;
		break;
	case 6	:
		Opt.c_cflag |= CS6;
		break;
	case 7:
		Opt.c_cflag |= CS7;
		break;
	case 8:
		Opt.c_cflag |= CS8;
		break;
	default:
		printf("DecoderSetCom: Unsupported data size\n");
		return -1;
	}

    //����У��λ
    switch (pParam->nParityBit)
    {
	case PAR_NONE:
		Opt.c_cflag &= ~PARENB;             // Clear parity enable
		Opt.c_iflag &= ~INPCK;              // Clear parity checking
		break;
	case PAR_ODD:
		Opt.c_cflag |= (PARODD | PARENB);   // ����Ϊ��Ч��
		Opt.c_iflag |= INPCK;               // Enable parity checking
		break;
	case PAR_EVEN:
		Opt.c_cflag |= PARENB;              // Enable parity
		Opt.c_cflag &= ~PARODD;             // ת��ΪżЧ��
		Opt.c_iflag |= INPCK;               // Enable parity checking
		break;
	default:
		printf("DecoderSetCom: Unsupported parity\n");
		return -1;
	}

	//����ֹͣλ
    switch (pParam->nStopBit)
    {
	case 1:
		Opt.c_cflag &= ~CSTOPB;
		break;
	case 2:
		Opt.c_cflag |= CSTOPB;
	   break;
	default:
		 printf("DecoderSetCom: Unsupported stop bits\n");
		 return -1;
    }

    //��������
//    if( pParam->nFlowCtl == PTZ_DATAFLOW_HW)
//   {
//	   Opt.c_cflag |= CRTSCTS;					//Ӳ������
//	   Opt.c_cflag &= ~(IXON|IXOFF|IXANY);
//   }
//   else if(pParam->nFlowCtl == PTZ_DATAFLOW_SF)
//   {
//	   Opt.c_cflag &= ~CRTSCTS;
//	   Opt.c_cflag |= IXON | IXOFF;		//�������
//
//   }
//    else
//   {
// 	   Opt.c_cflag &= ~(IXON|IXOFF|IXANY);		//��ʹ������
// 	   Opt.c_cflag &= ~CRTSCTS;
//    }
	Opt.c_cflag &= ~CRTSCTS;            //������ô����??
	Opt.c_cflag |= (CLOCAL | CREAD);

	Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	Opt.c_lflag |= FLUSHO;	// Output flush

	Opt.c_oflag &= ~OPOST;
	Opt.c_cc[VMIN] = 0;
	Opt.c_cc[VTIME] = 1;
	Opt.c_iflag = IGNBRK | IGNPAR;

	tcflush(fd, TCIOFLUSH);
    //���ô�������
	if (0 != tcsetattr(fd, TCSANOW, &Opt))
	{
		printf("DecoderSetCom: tcsetattr err\n");
		return -1;
	}
//	printf("nBaud=%d;nCharSize=%d;nStopBit=%d;nParityBit=%d;nFlowCtl=%d;\n",pParam->nBaudRate,
//			pParam->nCharSize,pParam->nStopBit,pParam->nParityBit,pParam->nFlowCtl);
	//��մ�������
	tcflush(fd,TCIOFLUSH);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void DecoderSendCommand(int fd, char *pCmd, int nLen, int nBaudRate)
{
	if(fd<=0)return ;
    if (0 != nBaudRate)
    {
        DecoderSetComBaudrate(fd, nBaudRate);        
    }
  /*  {
    	int i;
    	printf("rs485 send: \n");
    	for (i=0;i<nLen;i++)
    	{
    		printf("%02x, ", pCmd[i]);
    	}
    	printf("\nover\n");
    }*/

	write(fd, pCmd, nLen);
}
//���ڷ����޷�������
void DecoderSendCommand_Ex(int fd, unsigned char *pCmd, int nLen, int nBaudRate)
{
	if(fd<=0)return ;
    if (0 != nBaudRate)
    {
        DecoderSetComBaudrate(fd, nBaudRate);        
    }
  /*  {
    	int i;
    	printf("rs485 send: \n");
    	for (i=0;i<nLen;i++)
    	{
    		printf("%02x, ", pCmd[i]);
    	}
    	printf("\nover\n");
    }*/

	write(fd, pCmd, nLen);
}

int DecoderReceiveDate(int fd, char *pCmd, int nLen, int nBaudRate)
{
    int ret;
    if(fd<=0)return -1;
    if (0 != nBaudRate)
    {
        DecoderSetComBaudrate(fd, nBaudRate);        
    }
	ret = read(fd, pCmd, nLen);
	return ret;
}

//==========================����============================================================
void DecoderReset(int fd, int nAddress, int nProtocol, int nBaudRate)
{
    char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_STD_PELCOD :		//STD PELCO-D
	case 1: //PELCO-D (BandRate = 2400)		HUIXUN PELCO-D
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;

		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x22;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend	//PELCO-D��չ (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)//PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;	
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;	
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	//case 27: //visdom
	
	default:
		return;
	}
    
	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//==========================�ϡ��¡����ҡ��Զ�==================================================
//��ʼ
void DecoderLeftStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :		//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)		//HUIXUN 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed>>2;			//Speed  DIV 4
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0x40;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed>>2;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE3;
		byteCommand[2] = 0xE3;
		dwLength = 3;
		break;
	case 7://Pelco-D extend//pelco-d extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 1 << 1;
		byteCommand[3] = nSpeed;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
			
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);//����ָ��
}
//��ֹͣ
void DecoderLeftStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :			//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)			//HUIXUN
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x24;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0;
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}
	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//�ҿ�ʼ
void DecoderRightStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)				//HUIXUN
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = (nSpeed)>>2;			//Speed  DIV 4
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x05;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0x40;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed>>2;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE4;
		byteCommand[2] = 0xE4;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = 0x2F;
		byteCommand[5] = 0x2F;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x04;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 1 << 0;
		byteCommand[3] = nSpeed;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//��ֹͣ
void DecoderRightStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :			//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x25;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0;
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//�Ͽ�ʼ
void DecoderUpStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed)
{

	char    byteCommand[18];
    int     dwLength = 0;
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed>>2;			//Speed  DIV 4
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x08;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] =  nSpeed>>2;			//Speed  DIV 4
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE1;
		byteCommand[2] = 0xE1;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x30;
		byteCommand[5] = 0x30;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x04;
		byteCommand[6] = 0x30;
		byteCommand[7] = 0x30;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-7
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-4
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 1 << 3;
		byteCommand[3] = 0;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}
	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//��ֹͣ
void DecoderUpStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :						//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x22;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-7
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-4
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0;
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//�¿�ʼ
void DecoderDownStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed>>2;			//Speed  DIV 4
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x03;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x04;
		byteCommand[6] = 0x00;
		//�����^
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed>>2;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE2;
		byteCommand[2] = 0xE2;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x30;
		byteCommand[5] = 0x30;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x08;
		byteCommand[6] = 0x30;
		byteCommand[7] = 0x30;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x08;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-7
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-4
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x10;
		byteCommand[4] = 0x00;
		byteCommand[5] = nSpeed;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 1 << 2;
		byteCommand[3] = 0;
		byteCommand[4] = nSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��ֹͣ
void DecoderDownStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x23;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0;
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//�Զ���ʼ
void DecoderAutoStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case PTZ_PROTOCOL_SHUER_PELCOD :
	case 1: //PELCO-D
		//usleep(200000);
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = (nSpeed)>>2;			//Speed  DIV 4
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x0C;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x10;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xFF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x02;
		byteCommand[4] = nSpeed>>2;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400	//����Pelco-P�����Զ���20���е���07
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x20;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xEB;
		byteCommand[2] = 0xEB;
		dwLength = 3;
		break;
	case 7://Pelco-D extend//pelcon-d extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;		//������360����ת
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)//pelco-d tiandy
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x1B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x13;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xFF;
		byteCommand[7] = 0xFF;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;		//������360����ת
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;		//������360����ת
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;		//������360����ת
		byteCommand[3] = 0x1F;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;		//������360����ת
		byteCommand[3] = 0x1B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x90;		//������360����ת
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;		//������360����ת
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;		//������360����ת
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x33;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY//pelcon-d
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;			//�˹�ɨ��
		byteCommand[3] = 0x23;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x20;			//�˹�ɨ��
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;			//�˹�ɨ��
		byteCommand[3] = 0x1F;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x20;			//�˹�ɨ��
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x20;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x1B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x32;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		#if 0
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x87;
		byteCommand[3] = 10;
		byteCommand[4] = 10;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		
		#else
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 1;
		byteCommand[4] = 98;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		#endif
		break;
	case 27: //visdom
	default:
		return;
	}
	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//�Զ�ֹͣ
void DecoderAutoStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
	case PTZ_PROTOCOL_SELF: 			//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case PTZ_PROTOCOL_SHUER_PELCOD :
	case 1: //PELCO-D
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		//DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);	//zwq,20111129,auto
		//usleep(200000);
		break;	
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x2C;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x1A;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xFF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xEC;
		byteCommand[2] = 0xEC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x1D;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x13;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xFF;
		byteCommand[7] = 0xFF;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x20;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x21;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x1D;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x34;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x21;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x1D;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x62;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x33;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0;
		byteCommand[4] = 90;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}


//�෽λ�ƶ�
//bLeft Ϊ��ʱ���ƣ�Ϊ�������ƣ�leftSpeedΪ0ʱ���ƶ�
//bUp Ϊ�������ƣ�Ϊ��ʱ���ƣ�upSpeedΪ0ʱ���ƶ�
void DecoderPanTiltStart(int fd, int nAddress, int nProtocol, int nBaudRate, int bLeft, int bUp, int leftSpeed, int upSpeed)
{
	char    byteCommand[18];
    int     dwLength = 0;
    unsigned char leftBit;
    unsigned char upBit;

   

	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1:				//HUIXUN PELLCO-D
		if (leftSpeed)
		{
			if (bLeft)
				leftBit = 1 << 2;
			else
				leftBit = 1 << 1;
		}
		else
			leftBit = 0;

		if (upSpeed)
		{
			if (bUp)
				upBit = 1 << 3;
			else
				upBit = 1 << 4;
		}
		else
			upBit = 0 ;

		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = leftBit | upBit;
		byteCommand[4] = leftSpeed>>2;
		byteCommand[5] = upSpeed>>2;			//Speed  DIV 4
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
		break;
	
	case 4:				//STD  PELLCO-P
		if (leftSpeed)
		{
			if (bLeft)
				leftBit = 1 << 2;
			else
				leftBit = 1 << 1;
		}
		else
			leftBit = 0;

		if (upSpeed)
		{
			if (bUp)
				upBit = 1 << 3;
			else
				upBit = 1 << 4;
		}
		else
			upBit = 0 ;

		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] =  leftBit | upBit;
		byteCommand[4] = leftSpeed>>2;
		byteCommand[5] =  upSpeed>>2;
		byteCommand[6] = 0xAF;
		byteCommand[7] =  byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);

		break;
	case PTZ_PROTOCOL_YAAN:
		if (leftSpeed)
		{
			if (bLeft)
				leftBit = 1 << 1;
			else
				leftBit = 1 << 0;
		}
		else
			leftBit = 0;

		if (upSpeed)
		{
			if (bUp)
				upBit = 1 << 3;
			else
				upBit = 1 << 2;
		}
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = leftBit | upBit;
		byteCommand[3] = leftSpeed;
		byteCommand[4] = upSpeed;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
		break;
	default:
		if(leftSpeed)
		{
			if(bLeft)
				DecoderLeftStart(fd,  nAddress,  nProtocol,  nBaudRate, leftSpeed);
			else
				DecoderRightStart( fd,  nAddress,  nProtocol,  nBaudRate,  leftSpeed);
		}
		 if (upSpeed)
		{
			if (bUp)
				DecoderUpStart( fd, nAddress,  nProtocol,  nBaudRate,  upSpeed);
			else
				DecoderDownStart( fd,  nAddress,  nProtocol,  nBaudRate, upSpeed);

		}
		return ;
	}
	
}
void DecoderPanTiltStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{
	char    byteCommand[18];
    int     dwLength = 0;

	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)		//HUIXUN PELLCO-D
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
		
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0;
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	default:

		return ;
	}
	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);

}

//==========================�䱶���佹����Ȧ======================================================
//�䱶�̣�WIDE
void DecoderZoomOutStart(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x07;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x20;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE7;
		byteCommand[2] = 0xE7;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x40;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6) | (1 << 1);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;

	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderZoomOutStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x27;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
/**
�䱶�Ŵ� TEL
*/
void DecoderZoomInStart(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x06;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x40;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE8;
		byteCommand[2] = 0xE8;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x20;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x20;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x40;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6) | (1 << 0);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;

	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderZoomInStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x26;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//�佹
void DecoderFocusNearStart(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;							//ʵ�����Ǿ۽���
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x01;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE5;
		byteCommand[2] = 0xE5;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x01;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x20;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6) | (1 << 3);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SELF:
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderFocusNearStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x28;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SELF:
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderFocusFarStart(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :						//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;							//ʵ�����Ǿ۽�Զ
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x09;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x02;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE6;
		byteCommand[2] = 0xE6;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x02;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x40;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x80;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x01;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6) | (1 << 2);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SELF:
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderFocusFarStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x29;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SELF:
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//��Ȧ
void DecoderIrisOpenStart(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :							//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x0B;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x10;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xEA;
		byteCommand[2] = 0xEA;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x08;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6) | (1 << 4);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderIrisOpenStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x2B;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderIrisCloseStart(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x0A;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x10;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xE9;
		byteCommand[2] = 0xE9;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x10;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x04;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x08;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6) | (1 << 5);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderIrisCloseStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x2A;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;	
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = (1 << 6);
		byteCommand[3] = 0;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//==========================����==================================================================
void DecoderAUX1On(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x0E;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xF1;
		byteCommand[2] = 0xF1;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x13;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xFF;
		byteCommand[7] = 0xFF;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x11;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x87;
		byteCommand[3] = 0;
		byteCommand[4] = 1;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		printf("aux1On\n");
		break;
	case 27: //visdom
	default:
		return;
	}
	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUX1Off(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x2E;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xF2;
		byteCommand[2] = 0xF2;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x13;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xFF;
		byteCommand[7] = 0xFF;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x12;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x88;
		byteCommand[3] = 0;
		byteCommand[4] = 1;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUX2On(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x0D;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xEF;
		byteCommand[2] = 0xEF;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x17;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xFF;
		byteCommand[7] = 0xFF;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x87;
		byteCommand[3] = 0;
		byteCommand[4] = 2;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}


	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUX2Off(int fd, int nAddress, int nProtocol, int nBaudRate)
{
	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�		
	case PTZ_PROTOCOL_SHUER_PELCOD:	//SHU ER PTZ
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x2D;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		return;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xF0;
		byteCommand[2] = 0xF0;
		dwLength = 3;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x17;
		byteCommand[5] = 0x01;
		byteCommand[6] = 0xFF;
		byteCommand[7] = 0xFF;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x02;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x88;
		byteCommand[3] = 0;
		byteCommand[4] = 2;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUX3On(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	//DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUX3Off(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	//DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUX4On(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	//DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUX4Off(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	//DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUXNOn(int fd, int nAddress, int nProtocol, int nBaudRate, int n)
{
	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
		case PTZ_PROTOCOL_YAAN:
			byteCommand[0] = 0xA5;
			byteCommand[1] = nAddress;
			byteCommand[2] = 0x87;
			byteCommand[3] = 0;
			byteCommand[4] = n;
			byteCommand[5] = 0x00;
			byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
				+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
			dwLength = 7;
			printf("aux %d On\n", n);
			break;
		default:
			{
				printf("nAddress:%d, nProtocol=%d, aux %d On\n", nAddress,nProtocol,n);
				if(n==1)
					DecoderAUX1On( fd, nAddress,  nProtocol, nBaudRate);
				else if(n==2)
					DecoderAUX2On( fd, nAddress,  nProtocol, nBaudRate);
				return;
			}
		}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderAUXNOff(int fd, int nAddress, int nProtocol, int nBaudRate, int n)
{
	char	byteCommand[18];
	int 	dwLength = 0;
	
	switch(nProtocol)
	{
		case PTZ_PROTOCOL_YAAN:
			byteCommand[0] = 0xA5;
			byteCommand[1] = nAddress;
			byteCommand[2] = 0x88;
			byteCommand[3] = 0;
			byteCommand[4] = n;
			byteCommand[5] = 0x00;
			byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
				+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
			dwLength = 7;
			printf("aux %d Off\n", n);
			break;
		default:
			{
				printf("nAddress:%d, nProtocol=%d, aux %d OFF\n", nAddress,nProtocol,n);
				if(n==1)
					DecoderAUX1Off( fd, nAddress,  nProtocol, nBaudRate);
				else if(n==2)
					DecoderAUX2Off( fd, nAddress,  nProtocol, nBaudRate);
				return;
			}

		}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//==========================��չ==================================================================
void DecoderSetLeftLimitPosition(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
	case 4:	//PELCO-P (BandRate = 9600)
		//��ѶЭ��
		DecoderSetPreset(fd, nAddress, nProtocol, nBaudRate, 92);
		return;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
//	case 4:	//PELCO-P (BandRate = 9600)
//		byteCommand[0] = 0xA0;
//		byteCommand[1] = nAddress;
//		byteCommand[2] = 0x00;
//		byteCommand[3] = 0x03;
//		byteCommand[4] = 0x00;
//		byteCommand[5] = 0x5D;
//		byteCommand[6] = 0xAF;
//		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
//			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
//		dwLength = 8;
//		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x33;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x33;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1E;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x81;
		byteCommand[3] = 0;
		byteCommand[4] = 81;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SHUER_PELCOD:
				//SHU ER
		DecoderSetPreset(fd, nAddress, 1, nBaudRate, 51);
		return;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderSetRightLimitPosition(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 4:	//PELCO-P (BandRate = 9600)
		/*
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;*/
			
	case 1: //PELCO-D (BandRate = 2400)
		//��Ѷ��̨
		DecoderSetPreset(fd, nAddress, nProtocol, nBaudRate, 93);
		return;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
//	case 4:	//PELCO-P (BandRate = 9600)
//		byteCommand[0] = 0xA0;
//		byteCommand[1] = nAddress;
//		byteCommand[2] = 0x00;
//		byteCommand[3] = 0x03;
//		byteCommand[4] = 0x00;
//		byteCommand[5] = 0x5C;
//		byteCommand[6] = 0xAF;
//		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
//			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
//		dwLength = 8;
//		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5C;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x34;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x34;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x02;
		byteCommand[3] = 0xFF;
		byteCommand[4] = 0x1F;
		byteCommand[5] = 0x01;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5C;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5C;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5C;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5C;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x5C;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x81;
		byteCommand[3] = 0;
		byteCommand[4] = 82;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SHUER_PELCOD:
			//SHU ER
		DecoderSetPreset(fd, nAddress, 1, nBaudRate, 52);
		return;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderSetUpLimitPosition(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
		DecoderSetPreset(fd, nAddress, nProtocol, nBaudRate, 95);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderSetDownLimitPosition(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
		DecoderSetPreset(fd, nAddress, nProtocol, nBaudRate, 98);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//������ɨ�ٶ�
void DecoderSetLimitScanSpeed(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan, int nSpeed)
{
	char    byteCommand[18];
	int     dwLength = 0;
	int dSpeed ;
	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF: 			//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 4 :
		dSpeed = nSpeed>>2;
		dSpeed = (dSpeed > 63 ? 63 : dSpeed);
		DecoderSetPreset(fd, nAddress, nProtocol, nBaudRate, 87);
		DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, dSpeed, 0);
		return;
	case 1 :	//��ѶЭ��
		dSpeed = nSpeed>>2;
		dSpeed = (dSpeed > 30 ? 30 : dSpeed);
		DecoderSetPreset(fd, nAddress, nProtocol, nBaudRate, 87);
		DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, dSpeed, 0);
		return;
	case PTZ_PROTOCOL_YAAN:
		//speed
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x81;
		byteCommand[3] = nSpeed;
		byteCommand[4] = 91+nScan;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}


//��ʼ��ɨ
void DecoderLimitScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan)
{
	char    byteCommand[18];
	int     dwLength = 0;
	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)	//��ѶЭ��
	case 4:	//PELCO-P (BandRate = 9600)
		DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 99, 0);
		return;
	case PTZ_PROTOCOL_YAAN:
		//start
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0;
		byteCommand[4] = 91+nScan;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SHUER_PELCOD:
				//SHU ER
		DecoderLocatePreset(fd, nAddress, 1, nBaudRate, 51, 0);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��ʼ��ֱɨ��
void DecoderVertScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan)
{
	char    byteCommand[18];
	int     dwLength = 0;
	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
		DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 89, 0);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//������ɨ
void DecoderLimitScanStop(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan)
{
	char    byteCommand[18];
	int     dwLength = 0;

	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)	//��ѶЭ��
	case 4:	//PELCO-P (BandRate = 9600)
		DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 96, 0);
		return;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0;
		byteCommand[4] = 90;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SHUER_PELCOD:
		DecoderLocatePreset(fd, nAddress, 1, nBaudRate, 53, 0);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��ʼ���ɨ��
void DecoderRandomScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed)
{
	char    byteCommand[18];
	int     dwLength = 0;

	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
			DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 101, 0);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��ʼ֡ɨ��
void DecoderFrameScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed)
{
	char    byteCommand[18];
	int     dwLength = 0;

	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
			DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 102, 0);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��ʼ����ɨ��
void DecoderWaveScanStart(int fd, int nAddress, int nProtocol, int nBaudRate, int nSpeed)
{
	char    byteCommand[18];
	int     dwLength = 0;

	switch(nProtocol)
	{
	case PTZ_PROTOCOL_YAAN:
		//speed
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x81;
		byteCommand[3] = nSpeed;
		byteCommand[4] = 97;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);

		//start
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = nSpeed;
		byteCommand[4] = 97;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)	//��ѶЭ��
	case 4:	//PELCO-P (BandRate = 9600)
			DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 100, 0);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��������ɨ��
void DecoderWaveScanStop(int fd, int nAddress, int nProtocol, int nBaudRate)
{
	char    byteCommand[18];
	int     dwLength = 0;

	switch(nProtocol)
	{
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0;
		byteCommand[4] = 90;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)	//��ѶЭ��
	case 4:	//PELCO-P (BandRate = 9600)
		DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 96, 0);
		return;
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//����Ԥ��λ
void DecoderSetPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nNumber)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :			//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x03;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x81;
		byteCommand[3] = 0;
		if (nNumber < 80)
			byteCommand[4] = nNumber+1;
		else if (nNumber > 234)
		{
			printf("ERROR: YAAN Only Support 234 Preset\n");
			return ;
		}
		else
			byteCommand[4] = nNumber + 20;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderClearPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nNumber)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :			//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		return;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x05;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		printf("YAAN Not Support ClearPreset\n");
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

void DecoderClearAllPreset(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x4D;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//����Ԥ��λ
void DecoderLocatePreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nNumber, int nPatrolSpeed)
{

	char    byteCommand[18];
    int     dwLength = 0;

	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = nPatrolSpeed;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		
		break;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = nPatrolSpeed;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;

		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = nNumber;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0;
		if (nNumber < 80)
			byteCommand[4] = nNumber+1;
		else if (nNumber > 234)
		{
			printf("ERROR: YAAN Only Support 234 Preset\n");
			return ;
		}
		else
			byteCommand[4] = nNumber + 20;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}


//��ʼԤ��λѲ��
void DecoderStartPatrol(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x62;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		return;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		return;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x41;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x63;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0;
		byteCommand[4] = 99;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//ֹͣԤ��λѲ��
void DecoderStopPatrol(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :							//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		return;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		return;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x07;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x60;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0;
		byteCommand[4] = 90;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//��ʼ�켣��¼
void DecoderSetScanOnPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x1F;					//��ǰ��byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = nScan;			//��ǰ��byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	
	case 1: //PELCO-D (BandRate = 2400)
				//��ѶЭ��   Private1
		DecoderSetPreset(fd, nAddress, nProtocol, nBaudRate, 86);
		return;

	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x1F;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:					//Private2
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x85;
		if (nScan > 4)
		{
			printf("YAAN Support 5 Scan Only\n");
			return ;
		}
		byteCommand[3] = nScan;
		byteCommand[4] = 1;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SHUER_PELCOD:				//Private3
		DecoderSetPreset( fd, nAddress, nProtocol,  nBaudRate, 97);
		return;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);

}
//�����켣��¼
void DecoderSetScanOffPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X21;					//��ǰ��byteCommand[3] = 0X0B;	
		byteCommand[4] = 0x00;
		byteCommand[5] = nScan;					//byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
		
	case 1: //PELCO-D (BandRate = 2400)
			DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 96, 0);
		return;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X21;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0X0B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x1E + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x85;
		if (nScan > 4)
		{
			printf("YAAN Support 5 Scan Only\n");
			return ;
		}
		byteCommand[3] = nScan;
		byteCommand[4] = 2;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SHUER_PELCOD:
		DecoderLocatePreset( fd, nAddress, nProtocol,  nBaudRate, 97, 0);
	case 27: //visdom
	default:
		return;
	}
	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//��ʼ�켣Ѳ��
void DecoderLocateScanPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		break;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :						//STD PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x23;					//ԭ����byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = nScan;					//ԭ����byteCommand[5] = 0x28 + nScan;		
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 1: //PELCO-D (BandRate = 2400)��Ѷ��ͨN85-HCЭ��
		DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 97, 0);
		return;
	case 2:	//HY ()
		return;
	case 3:	//Samsung-641 ()
		return;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x23;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		return;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		return;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x09;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x28 + nScan;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x85;
		if (nScan > 4)
		{
			printf("YAAN Support 5 Scan Only\n");
			return ;
		}
		byteCommand[3] = nScan;
		byteCommand[4] = 0;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SHUER_PELCOD:
		DecoderLocatePreset( fd,  nAddress,  nProtocol,  nBaudRate, 97, 0);
		return;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}
//ֹͣ�켣Ѳ��
void DecoderStopScanPreset(int fd, int nAddress, int nProtocol, int nBaudRate, int nScan)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case 0:
		return;
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
		
	case 1: //PELCO-D (BandRate = 2400)
		DecoderLocatePreset( fd, nAddress,  nProtocol,  nBaudRate, 96, 0);
		return;
	case 2:	//HY ()
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x22;
		byteCommand[3] = 0x00;
		byteCommand[4] = byteCommand[0] + byteCommand[1]
			+ byteCommand[2] + byteCommand[3];
		dwLength = 5;
		break;
	case 3:	//Samsung-641 ()
		byteCommand[0] = 0xA0;
		byteCommand[1] = 0x00;
		byteCommand[2] = nAddress;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 6://TC-615II
		byteCommand[0] = nAddress;
		byteCommand[1] = 0xFC;
		byteCommand[2] = 0xFC;
		dwLength = 3;
		break;
	case 7://Pelco-D extend	//PELCO-D��չ (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 8://PELCO-D (for���ΰҵ)//PELCO-D (for���ΰҵ)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5] + 1) & 0xFF;
		dwLength = 7;
		break;
	case 9:  //SAMSUNG
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x01;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0x00;
		byteCommand[7] = 0x00;
		byteCommand[8] = byteCommand[1] + byteCommand[2] + byteCommand[3]
			+ byteCommand[4] + byteCommand[5] + byteCommand[6] + byteCommand[7];
		byteCommand[8] = 0xFF - byteCommand[8];
		dwLength = 9;
		break;
	case 10:  //pelcod-yinxin
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 11:  //pelcod-1
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 12:  //pelcod-2
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 13:  //pelcod-3 
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 14:  //pelcod-4
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 15:  //pelcod-5
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 16:  //pelcod-6
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 17:  //JOVE-D01
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		dwLength = 6;
		break;
	case 18:  //pelcod-AONY
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 19:  //pelcod-8
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 20:  //pelcod-9
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;	
	case 21://pelco-d-simple
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 22://pelco-p-yinxin
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 23://pelco-p-1
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 24: //pelco-p-2
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 25://pelco-p-3
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;	
	case 26://pelco-p-AONY
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress-1;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case PTZ_PROTOCOL_YAAN:
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0;
		byteCommand[4] = 90;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_SHUER_PELCOD:
		DecoderLocatePreset( fd,  nAddress,  nProtocol,  nBaudRate, 97, 0);
		return;
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��ͬ��DVR������Ľӿ�
//��ʼ����Ѳ��
void DecoderSetPatrolOn(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x47;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x47;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x47;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x47;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
		
	case 0:
	case 2:	//HY ()
	case 3:	//Samsung-641 ()
	case 6://TC-615II
	case 8://PELCO-D (for���ΰҵ)
	case 9:  //SAMSUNG
	case 10:  //pelcod-yinxin
	case 11:  //pelcod-1
	case 12:  //pelcod-2
	case 13:  //pelcod-3 
	case 14:  //pelcod-4
	case 15:  //pelcod-5
	case 16:  //pelcod-6
	case 17:  //JOVE-D01
	case 18:  //pelcod-AONY
	case 19:  //pelcod-8
	case 20:  //pelcod-9
	case 21://pelco-d-simple
	case 22://pelco-p-yinxin
	case 23://pelco-p-1
	case 24: //pelco-p-2
	case 25://pelco-p-3
	case 26://pelco-p-AONY
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��������Ѳ��
void DecoderSetPatrolOff(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x4B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x4B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x4B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x4B;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
		
	case 0:
	case 2:	//HY ()
	case 3:	//Samsung-641 ()
	case 6://TC-615II
	case 8://PELCO-D (for���ΰҵ)
	case 9:  //SAMSUNG
	case 10:  //pelcod-yinxin
	case 11:  //pelcod-1
	case 12:  //pelcod-2
	case 13:  //pelcod-3 
	case 14:  //pelcod-4
	case 15:  //pelcod-5
	case 16:  //pelcod-6
	case 17:  //JOVE-D01
	case 18:  //pelcod-AONY
	case 19:  //pelcod-8
	case 20:  //pelcod-9
	case 21://pelco-d-simple
	case 22://pelco-p-yinxin
	case 23://pelco-p-1
	case 24: //pelco-p-2
	case 25://pelco-p-3
	case 26://pelco-p-AONY
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}


//���һ��Ѳ����
void DecoderAddPatrol(int fd, int nAddress, int nProtocol, int nBaudRate, int nPreset, int nSpeed, int nStayTime)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :					//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = nPreset;
		byteCommand[3] = 0x49;
		byteCommand[4] = nSpeed>>2;
		byteCommand[5] = nStayTime;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = nPreset;
		byteCommand[3] = 0x49;
		byteCommand[4] = nSpeed>>2;
		byteCommand[5] = nStayTime;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = nPreset;
		byteCommand[3] = 0x49;
		byteCommand[4] = nSpeed;
		byteCommand[5] = nStayTime;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = nPreset;
		byteCommand[3] = 0x49;
		byteCommand[4] = nSpeed;
		byteCommand[5] = nStayTime;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_YAAN:
		//YAAN ֻ֧����ͬ���ٶȺ�ͣ��ʱ�䣬�����ǵ������õ�
		//speed
		{
			byteCommand[0] = 0xA5;
			byteCommand[1] = nAddress;
			byteCommand[2] = 0x81;
			if (nSpeed < 3)
				nSpeed = 3;
			else if (nSpeed > 255)
				nSpeed = 255;
			byteCommand[3] = nSpeed;
			byteCommand[4] = 99;
			byteCommand[5] = 0x00;
			byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
				+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
			dwLength = 7;
			DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
		}

		//delay
		{
			byteCommand[0] = 0xA5;
			byteCommand[1] = nAddress;
			byteCommand[2] = 0x81;
			if (nStayTime < 3)
				nStayTime = 3;
			else if (nStayTime > 255)
				nStayTime = 255;
			byteCommand[3] = nStayTime;
			byteCommand[4] = 96;
			byteCommand[5] = 0x00;
			byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
				+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
			dwLength = 7;
			DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
		}

		//preset
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x84;
		byteCommand[3] = 0;
		if (nPreset < 80)
			byteCommand[4] = nPreset+1;
		else if (nPreset > 234)
		{
			printf("ERROR: YAAN Only Support 234 Preset\n");
			return ;
		}
		else
			byteCommand[4] = nPreset + 20;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
		
	case 0:
	case 2:	//HY ()
	case 3:	//Samsung-641 ()
	case 6://TC-615II
	case 8://PELCO-D (for���ΰҵ)
	case 9:  //SAMSUNG
	case 10:  //pelcod-yinxin
	case 11:  //pelcod-1
	case 12:  //pelcod-2
	case 13:  //pelcod-3 
	case 14:  //pelcod-4
	case 15:  //pelcod-5
	case 16:  //pelcod-6
	case 17:  //JOVE-D01
	case 18:  //pelcod-AONY
	case 19:  //pelcod-8
	case 20:  //pelcod-9
	case 21://pelco-d-simple
	case 22://pelco-p-yinxin
	case 23://pelco-p-1
	case 24: //pelco-p-2
	case 25://pelco-p-3
	case 26://pelco-p-AONY
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}


//��ʼѲ��
void DecoderStartHWPatrol(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :						//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x4F;
		byteCommand[4] = 0x01;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x4F;
		byteCommand[4] = 0x01;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x4F;
		byteCommand[4] = 0x01;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x4F;
		byteCommand[4] = 0x01;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_YAAN://
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x99;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) % 256;
		dwLength = 7;
		break;
	case 0:
	case 2:	//HY ()
	case 3:	//Samsung-641 ()
	case 6://TC-615II
	case 8://PELCO-D (for���ΰҵ)
	case 9:  //SAMSUNG
	case 10:  //pelcod-yinxin
	case 11:  //pelcod-1
	case 12:  //pelcod-2
	case 13:  //pelcod-3 
	case 14:  //pelcod-4
	case 15:  //pelcod-5
	case 16:  //pelcod-6
	case 17:  //JOVE-D01
	case 18:  //pelcod-AONY
	case 19:  //pelcod-8
	case 20:  //pelcod-9
	case 21://pelco-d-simple
	case 22://pelco-p-yinxin
	case 23://pelco-p-1
	case 24: //pelco-p-2
	case 25://pelco-p-3
	case 26://pelco-p-AONY
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//����Ѳ��
void DecoderStopHWPatrol(int fd, int nAddress, int nProtocol, int nBaudRate)
{

	char    byteCommand[18];
    int     dwLength = 0;
    
	switch(nProtocol)
	{
	case PTZ_PROTOCOL_SELF:				//��о�Կ�
	case PTZ_PROTOCOL_SHUER_PELCOD:
	case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
	case 1: //PELCO-D (BandRate = 2400)
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case 4:	//PELCO-P (BandRate = 9600)
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 5:  //PELCO-P��չ�������� BandRate = 2400
		byteCommand[0] = 0xA0;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = 0xAF;
		byteCommand[7] = byteCommand[0] ^ byteCommand[1] ^ byteCommand[2]
			^ byteCommand[3] ^ byteCommand[4] ^ byteCommand[5] ^ byteCommand[6];
		dwLength = 8;
		break;
	case 7://Pelco-D extend
		byteCommand[0] = 0xFF;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x00;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x00;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) & 0xFF;
		dwLength = 7;
		break;
	case PTZ_PROTOCOL_YAAN://
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x83;
		byteCommand[3] = 0x00;
		byteCommand[4] = 0x90;
		byteCommand[5] = 0x00;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) % 256;
		dwLength = 7;
		break;
	case 0:
	case 2:	//HY ()
	case 3:	//Samsung-641 ()
	case 6://TC-615II
	case 8://PELCO-D (for���ΰҵ)
	case 9:  //SAMSUNG
	case 10:  //pelcod-yinxin
	case 11:  //pelcod-1
	case 12:  //pelcod-2
	case 13:  //pelcod-3 
	case 14:  //pelcod-4
	case 15:  //pelcod-5
	case 16:  //pelcod-6
	case 17:  //JOVE-D01
	case 18:  //pelcod-AONY
	case 19:  //pelcod-8
	case 20:  //pelcod-9
	case 21://pelco-d-simple
	case 22://pelco-p-yinxin
	case 23://pelco-p-1
	case 24: //pelco-p-2
	case 25://pelco-p-3
	case 26://pelco-p-AONY
	case 27: //visdom
	default:
		return;
	}

	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

//��ĳһ������Ļ�м�Ŵ���ʾ
//x,y  ����Ļ�ֳ�64x64������x,y�ֱ�����Ÿ��Է������ֵ
//zoom �Ŵ�ı�������ֵΪʵ�ʱ���x16
void DecoderZoomPosition(int fd, int nAddress, int nProtocol, int nBaudRate, int x, int y, int zoom)
{

	char    byteCommand[18];
	int     dwLength = 0;

	switch(nProtocol)
	{
	case PTZ_PROTOCOL_YAAN://
		byteCommand[0] = 0xA5;
		byteCommand[1] = nAddress;
		byteCommand[2] = 0x90;
		if (x <= 31)
		{
			byteCommand[3] = 31 - x;
		}
		else
		{
			byteCommand[3] = x + 32;
		}
		
		if (y <= 31)
		{
			byteCommand[4] = 31 - y;
		}
		else
		{
			byteCommand[4] = y + 32;
		}
		int zm = (3 * (zoom)/16) - 3;
		if (zoom == 0)
			zm = 0;
		else if (zm < 1)
			zm = 1;
			
		//zm = zoom;
		byteCommand[5] = zm;
		byteCommand[6] = (byteCommand[0] + byteCommand[1] + byteCommand[2]
			+ byteCommand[3] + byteCommand[4] + byteCommand[5]) % 256;
		dwLength = 7;
		printf("zoom: %x,%x,%x,%x,%x,%x,%x\n", 
			byteCommand[0],
			byteCommand[1],
			byteCommand[2],
			byteCommand[3],
			byteCommand[4],
			byteCommand[5],
			byteCommand[6]);
		break;
	}
	DecoderSendCommand(fd, byteCommand, dwLength, nBaudRate);
}

/*
*	brief: ��ȡcrcУ����
*   param: data �������ݣ�len ���ݳ���
*	return: crcУ����
*/
static unsigned short _get_CRC16(unsigned char *data, unsigned short len)
{
	unsigned short crc=0xffff;
	unsigned char i;
	while(len!=0)
	{
		crc ^= *data;
		for(i=0; i<8; i++)
		{
			if((crc&0x0001) == 0)
			{
				crc = crc>>1;
			}
			else
			{  
				crc = crc>>1;
				crc ^= 0xa001;
			}
		}
		len--;
		data++;
	}
	return crc;
} 
//�ͻ���Ȧ�����򣬽���3D��λ�����Ŵ���С��ʾ
//(x, y, w, h) Ȧ�������������꼰���; (width, height)��ǰ�����ֱ���
//zoom  3D��λָ��:	0xC0 �Ŵ�0xC1 ��С
void DecoderZoomZone(int fd, int nAddress, int nProtocol, int nBaudRate, int x, int y, 
	int w, int h, int width, int height,int zoom)
{
	//printf("[%s]:%d	(%5d %5d; %4d %4d)	res:%4d*%4d	zoom:%d\n", __FUNCTION__, __LINE__, x, y, w, h, width, height, zoom);
	unsigned char byteCommand[24]={0};
	int	dwLength = 0;
	int tx=0,ty=0;

	switch(nProtocol)
	{
		case PTZ_PROTOCOL_SELF:				//��о�Կ�
		case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
		case 1: //PELCO-D (BandRate = 2400)
		{
			tx = (x>=0) ? x : ((0-x)|(1<<15));
			ty = (y>=0) ? y : ((0-y)|(1<<15));
			byteCommand[0] = 0xFF;
			byteCommand[1] = 0x5f;
			byteCommand[2] = 0x0;
			byteCommand[3] = 0x15;
			byteCommand[4] = 0x3d;
			byteCommand[5] = zoom;
			byteCommand[6] = tx>>8;
			byteCommand[7] = tx&0xff;
			byteCommand[8] = ty>>8;
			byteCommand[9] = ty&0xff;
			byteCommand[10] = w>>8;
			byteCommand[11] = w&0xff;
			byteCommand[12] = h>>8;
			byteCommand[13] = h&0xff;
			byteCommand[14] = width>>8;
			byteCommand[15] = width&0xff;
			byteCommand[16] = height>>8;
			byteCommand[17] = height&0xff;
			unsigned short ret_crc = _get_CRC16(&byteCommand[5], 13);
			byteCommand[18] = 0xff;
			byteCommand[19] = ret_crc>>8;
			byteCommand[20] = ret_crc&0xff;
			//printf("%x => %02x%02x		",ret_crc,byteCommand[19],byteCommand[20]);
			dwLength = 21;
			break;
		}
		default:
			printf("protocl error\n");
			return;
	}
//	printf("(%02x%02x %02x%02x; %02x%02x %02x%02x)  zoom:%x\n", byteCommand[6], byteCommand[7], byteCommand[8], byteCommand[9], 
//		byteCommand[10], byteCommand[11], byteCommand[12], byteCommand[13], byteCommand[5]);

	DecoderSendCommand_Ex(fd, byteCommand, dwLength, nBaudRate);
}

//�ƶ����ٹ���
//(x, y, zoom, focus) �����������̨�ƶ����경�����������ľ۽�����
//cmd �ƶ�����ָ��: 0x20 ����0�㣬0x21 ����λ���ƶ�������0x22 ����ǰλ�ò���
void DecoderTraceObj(int fd, int nAddress, int nProtocol, int nBaudRate,int x, int y, int zoom, int focus, int cmd)
{
	printf("[%s]:%d	(%5d %5d; %5d %5d)	cmd:%d	", __FUNCTION__, __LINE__, x, y, zoom, focus, cmd);
	unsigned char byteCommand[24]={0};
	int	dwLength = 0;
	//int tx=0,ty=0;

	switch(nProtocol)
	{
		case PTZ_PROTOCOL_SELF:				//��о�Կ�
		case PTZ_PROTOCOL_STD_PELCOD :				//STD PELCO-D (BandRate = 2400)
		case 1: //PELCO-D (BandRate = 2400)
		{
			//tx = (x>=0) ? x : ((0-x)|(1<<15));
			//ty = (y>=0) ? y : ((0-y)|(1<<15));
			byteCommand[0] = 0xFF;
			byteCommand[1] = 0x5f;
			byteCommand[2] = 0x0;
			byteCommand[3] = 0x15;
			byteCommand[4] = 0x3a;
			byteCommand[5] = cmd;
			byteCommand[6] = x>>24;
			byteCommand[7] = x>>16;
			byteCommand[8] = x>>8;
			byteCommand[9] = x&0xff;
			byteCommand[10] = y>>24;
			byteCommand[11] = y>>16;
			byteCommand[12] = y>>8;
			byteCommand[13] = y&0xff;
			byteCommand[14] = zoom>>8;
			byteCommand[15] = zoom&0xff;
			byteCommand[16] = focus>>8;
			byteCommand[17] = focus&0xff;
			unsigned short ret_crc = _get_CRC16(&byteCommand[5], 13);
			byteCommand[18] = 0xff;
			byteCommand[19] = ret_crc>>8;
			byteCommand[20] = ret_crc&0xff;
			printf("%x => %02x%02x		",ret_crc,byteCommand[19],byteCommand[20]);
			dwLength = 21;
			break;
		}
		default:
			printf("protocl error\n");
			return;
	}
	printf("(%02x%02x%02x%02x; %02x%02x%02x%02x) (%02x%02x %02x%02x) cmd:%x\n", byteCommand[6], byteCommand[7], byteCommand[8], byteCommand[9], 
		byteCommand[10], byteCommand[11], byteCommand[12], byteCommand[13], byteCommand[14], byteCommand[15], byteCommand[16],byteCommand[17], byteCommand[5]);

	DecoderSendCommand_Ex(fd, byteCommand, dwLength, nBaudRate);
}
