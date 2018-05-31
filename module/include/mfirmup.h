

#ifndef __MFIRMUP_H__
#define __MFIRMUP_H__

//�����������
enum
{
	FIRMUP_SUCCESS			= 1,		// �����ɹ�
	FIRMUP_DOWNLOAD_ERROR,				// ���ع̼�ʧ��
	FIRMUP_LATEST_VERSION,				// �������°汾
	FIRMUP_INVALID_FILE,				// �̼�У��ʧ��
	FIRMUP_OTHER_ERROR,					// ��������������ʼ��ʧ�ܡ���дʧ�ܵȣ�
	FIRMUP_PRODUCT_ERROR,				// ��Ʒ�ͺŲ�ƥ��
	FIRMUP_CANNOT_DEMOTION,				// ��֧�ֽ�������ISSI Flash��Ŀǰδ�õ���
};

//��������
enum
{
	FIRMUP_HTTP,
	FIRMUP_FILE,
	FIRMUP_FTP,
};

enum
{
	FIRMUP_LOCATION_NONE	= -1,		// ��ȡʧ��
	FIRMUP_LOCATION_MEM,				// ������ϵͳ�ڴ��У�/tmp��
	FIRMUP_LOCATION_MEDIA_MEM,			// ������ý���ڴ��У�/tmp/ramdisk����Ҫ����ramdisk������
	FIRMUP_LOCATION_CARD,				// ������TF����
	FIRMUP_LOCATION_EMMC,				// ������EMMC��
};

// ���ò�������JOVISION_CONFIG_FILE�ļ��ж�ȡ
typedef struct
{
	char product[32];		//��Ʒ���
	char url[4][256];		//������������ַ
	char vername[64];		//�����汾�ļ���
	char binname[64];		//�����ļ���
}FirmupCfg_t;

// ver�ļ��е�ģ����Ϣ
typedef struct
{
	char name[32];//ģ�����ƣ�boot,kernel,fs,config
	int ver;//�汾��
	unsigned int offset;//��ģ�����ļ��е�ƫ�Ƶ�ַ
	unsigned int size;//��ģ��ĳ���
	char dev[32];//��ģ��Ҫ�յ����豸
	BOOL needUpdate;
}FirmModule_t;

// ver�ļ���Ϣ
typedef struct
{
	char product[32];		//��Ʒ���
	FirmModule_t list[20];	//ģ���б�
	int cnt;				//ģ������
	unsigned int checksum;
	unsigned int fileSize;
}FirmVersion_t;

enum
{
	FIRMUP_DOWNLOAD_START,
	FIRMUP_DOWNLOAD_PROGRESS,
	FIRMUP_DOWNLOAD_FINISH,
	FIRMUP_DOWNLOAD_TIMEOUT,
	FIRMUP_DOWNLOAD_FAILED,
	FIRMUP_UPDATE_START,		// �ļ������ɣ���ʼ��д��param1Ϊ�ܹ���Ҫ��д��block����
	FIRMUP_UPDATE_FINISH,		// ��д��ɣ�param1Ϊ����д��������param2Ϊ�ܿ�����
	FIRMUP_UPDATE_FAILED,		// ����ʧ�ܣ�param1Ϊʧ��ԭ��ȡֵΪFIRMUP_FAILED��
};

// ���س�ʱ
#define FIRMUP_VER_TIMEOUT		10
#define FIRMUP_BIN_TIMEOUT		120

typedef void (*FirmupStream_Callback)(void* arg, char* data, int len);
typedef void (*FirmupEvent_Callback)(int nEvent, void* arg, int param1, int param2);


int mfirmup_init(FirmupStream_Callback cb, void* arg);

const FirmupCfg_t* mfirmup_getconfig();

const FirmVersion_t* mfirmup_getnowver();

// ռ����������
int mfirmup_claim(int nType);

// �Ƿ���������
BOOL mfirmup_b_updating();

// �ͷ���������
VOID mfirmup_release();

// �ļ����أ���url/urlfile���浽savepath/savefile�У�filename����ΪNULL
int mfirmup_download_file(const char* url, const char* urlfile, const char* savepath, const char* savefile, int timeout_sec, BOOL bBlock);

// ���������汾�ļ�
// �����Ƿ���Ҫ������TRUE��Ҫ������FALSE��������
BOOL mfirmup_check_verfile(const char* verfile, FirmVersion_t* verinfo);

// ��ȡ��׼��bin�ļ�����λ��
// ��������λ������FIRMUP_LOCATION_XXX��FIRMUP_LOCATION_NONEΪ��ȡʧ��
int mfirmup_prepare_location(char* path, int maxlen);

// �Զ�����url/ver.bin��url/bin��Ĭ��·�������Զ���������ѡ����������http/ftp�ȷ�ʽ
// ����ver.binΪ������ʽ�����غ���ж��Ƿ���Ҫ����
// bin�ļ�����Ϊ��������ʽ����ʼ���غ���������
// TimeoutΪbin�ļ����س�ʱʱ��
// autoupdate��0ʱ����������Զ���ʼ��д
// CallbackΪ�¼��ص��������¼�����ΪFIRMUP_DOWNLOAD_XXX/FIRMUP_UPDATE_XXX
int mfirmup_auto_update(const char* url, char (*verpath)[256], char (*binpath)[256], FirmVersion_t* verinfo, 
							int timeout, BOOL autoupdate, FirmupEvent_Callback Callback, void* arg);

// ��binfile��verfileΪ�����ļ���ʼ����
// CallbackΪ�¼��ص��������¼�����ΪFIRMUP_UPDATE_XXX
int mfirmup_startupdate(const char* binfile, const char* verfile, BOOL bBlock, 
							FirmupEvent_Callback Callback, void* arg);

// ȡ��������ֻ�������ع�����ȡ������ʼ��д���޷�ȡ��
int mfirmup_cancelupdate(const char* verfile, const char* binfile);

// ��ȡ��д���ȣ�����0~100
int mfirmup_get_writepercent(int* blocks, int* writed);

#endif

