#include "utl_timer.h"
#include "jpeglib.h"
#include "utl_splice.h"


/************************************************************
дbmp�ļ���ͷ��Ϣ��jpeg��ʽת��Ϊbmpʱ����
************************************************************/
int write_bmp_header(j_decompress_ptr cinfo,FILE *output_file)
{
    struct bmp_fileheader bfh;
    struct bmp_infoheader bih;

    unsigned long width;
    unsigned long height;
    unsigned short depth;
    unsigned long headersize;
    unsigned long filesize=0;

	int i=0;
	unsigned char j=0;
	unsigned char *platte;

    width=cinfo->output_width;
    height=cinfo->output_height;
    depth=cinfo->output_components;

    if (depth==1)        //�Ҷ�ͼ��
    {
        headersize=14+40+256*4;
        filesize=headersize+width*height;
    }

    if (depth==3)        //��ɫͼ��
    {
        headersize=14+40;
        filesize=headersize+width*height*depth;
    }

    memset(&bfh,0,sizeof(struct bmp_fileheader));
    memset(&bih,0,sizeof(struct bmp_infoheader));
    
    //д��ȽϹؼ��ļ���bmpͷ����
    bfh.bfType=0x4D42;          //�ļ�ͷ���̶�"BM"
    bfh.bfSize=filesize;        //�ļ���С���ļ�ͷ��С+ͼ�����ݴ�С
    bfh.bfOffBits=headersize;

    bih.biSize=40;
    bih.biWidth=width;
    bih.biHeight=height;
    bih.biPlanes=1;
    bih.biBitCount=(unsigned short)depth*8;
    bih.biSizeImage=width*height*depth;

    fwrite(&bfh,sizeof(struct bmp_fileheader),1,output_file);
    fwrite(&bih,sizeof(struct bmp_infoheader),1,output_file);

    if (depth==1)        //�Ҷ�ͼ��Ҫ��ӵ�ɫ��
    {
        platte = (unsigned char *)malloc(sizeof(unsigned char)*256*4);
        if (NULL == platte)
        {
			printf("write_bmp_header : malloc platte mem failed !\n");
			return VOS_ERR;
        }
        
        for(i=0;i<1024;i+=4)
        {
            platte[i]=j;
            platte[i+1]=j;
            platte[i+2]=j;
            platte[i+3]=0;
            j++;
        }
        fwrite(platte,sizeof(unsigned char)*1024,1,output_file);
        free(platte);
		platte = NULL;
    }

	return VOS_OK;
}

/************************************************************
дbmp�ļ���ͼ�����ݣ�jpeg��ʽת��Ϊbmpʱ����
************************************************************/
int write_bmp_data(j_decompress_ptr cinfo,unsigned char *src_buff,FILE *output_file)
{
    unsigned char *dst_width_buff;
    unsigned char *point;

    unsigned long width;
    unsigned long height;
    unsigned short depth;

	unsigned long i=0;
	unsigned long j=0;

    width=cinfo->output_width;
    height=cinfo->output_height;
    depth=cinfo->output_components;

    dst_width_buff = (unsigned char *)malloc(sizeof(unsigned char)*width*depth);
	if (NULL == dst_width_buff)
	{
		printf("write_bmp_data : malloc dst_width_buff mem failed !\n");
		return VOS_ERR;
	}
    memset(dst_width_buff,0,sizeof(unsigned char)*width*depth);

    point=src_buff+width*depth*(height-1);    //����д���ݣ�bmp��ʽ�ǵ��ģ�jpg������
    for (i=0;i<height;i++)
    {
        for (j=0;j<width*depth;j+=depth)
        {
            if (depth==1)        //����Ҷ�ͼ
            {
                dst_width_buff[j]=point[j];
            }

            if (depth==3)        //�����ɫͼ
            {
                dst_width_buff[j+2]=point[j+0];
                dst_width_buff[j+1]=point[j+1];
                dst_width_buff[j+0]=point[j+2];
            }
        }
        point-=width*depth;
        fwrite(dst_width_buff,sizeof(unsigned char)*width*depth,1,output_file);    //һ��дһ��
    }

	free(dst_width_buff);
	dst_width_buff = NULL;

	return VOS_OK;
}

/************************************************************
��JPEG�ļ����и�ʽת����ת����BMP��ʽ������ŵ���ʱ�ļ���
************************************************************/
int Jpeg2BMP(char *FileName,char *tmpBMPFileName)
{
	
	FILE *fp , *fp_tmp; 
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;
    unsigned char *src_buff;
    unsigned char *point;

	unsigned long Width;
	unsigned long Height;
	unsigned short depth;

	//�Ϸ���У�飬ָ�벻��Ϊ��
	if (NULL == FileName || NULL == tmpBMPFileName)
	{
		printf("Jpeg2BMP : input param error !\n");
		return VOS_ERR;
	}

	//��ԴJPEG�ļ�
	fp = fopen(FileName,"rb");
	if(NULL == fp)
	{
		printf("Jpeg2BMP : open source file %s error !\n", FileName);
		return VOS_ERR;
	}

	//��Ŀ��BMP�ļ�
	fp_tmp = fopen(tmpBMPFileName,"wb");
	if(NULL == fp_tmp)
	{
		printf("Jpeg2BMP : open dest file %s error !\n", tmpBMPFileName);
		fclose(fp);                //��������쳣��֮ǰmalloc��fopen��صĶ�Ҫ������ֹ�ڴ�й¶
		return VOS_ERR;
	}

	cinfo.err=jpeg_std_error(&jerr);    ////����Ϊlibjpeg.a���ṩ��API�ӿڣ�����ο�����ĵ�

	jpeg_create_decompress(&cinfo);  

	jpeg_stdio_src(&cinfo, fp);

	jpeg_read_header(&cinfo, TRUE);

	//��ʼ����JPEG�ļ�
	jpeg_start_decompress(&cinfo);

	Width = cinfo.output_width;
    Height = cinfo.output_height;
	depth = cinfo.output_components;

	src_buff = (unsigned char *)malloc(sizeof(unsigned char)*Width*Height*depth);
	if (NULL == src_buff)
	{
		printf("Jpeg2BMP : malloc src_buff mem failed !\n");
		fclose(fp);
		fclose(fp_tmp);
		return VOS_ERR;
	}
    memset(src_buff,0,sizeof(unsigned char)*Width*Height*depth);

    buffer=(*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo,JPOOL_IMAGE,Width*depth,1);

    point=src_buff;
    while (cinfo.output_scanline<Height)
    {
        jpeg_read_scanlines(&cinfo,buffer,1);    //��ȡһ��jpgͼ�����ݵ�buffer
        memcpy(point,*buffer,Width*depth);    //��buffer�е��������и�src_buff
        point+=Width*depth;            //һ�θı�һ��
    }

	if(VOS_OK != write_bmp_header(&cinfo,fp_tmp))            //дbmp�ļ�ͷ
	{
		printf("Jpeg2BMP : write_bmp_header return error !\n");
		fclose(fp);
		fclose(fp_tmp);
		free(src_buff);
		src_buff = NULL;
		return VOS_ERR;
	}

    if(VOS_OK != write_bmp_data(&cinfo,src_buff,fp_tmp))    //дbmp��������
	{
		printf("Jpeg2BMP : write_bmp_data return error !\n");
		fclose(fp);
		fclose(fp_tmp);
		free(src_buff);
		src_buff = NULL;
		return VOS_ERR;
	}

	//��Դ�ͷ�
	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	fclose(fp);
	fclose(fp_tmp);
	free(src_buff);
	src_buff = NULL;

	return VOS_OK;
}

/************************************************************
������bmp�ļ�����ƴ�ӳ�һ��bmp�ļ���Ҫ�������ļ���width��depth��ͬ
************************************************************/
int BMPSplice(char *FileName,char *FileName2,char *destFileName)
{
	FILE *fp,*fp2,*fp_dest; 

	struct bmp_fileheader bfh,bfh2,bfh_dest;    //�ֱ��������Դ�ļ���һ��Ŀ���ļ����ļ�ͷ
    struct bmp_infoheader bih,bih2,bih_dest;

	unsigned long width;
    unsigned long height;
    unsigned short depth;

	unsigned char *dst_width_buff;
	unsigned char *src_buff;
    unsigned char *point;
	unsigned long i=0;
	unsigned long j=0;

	//��κϷ���У�飬����Ϊ��
	if (NULL == FileName || NULL == FileName2 || NULL == destFileName)
	{
		printf("BMPSplice : input param error !\n");
		return VOS_ERR;
	}

	fp = fopen(FileName,"rb");
	if(NULL == fp)
	{
		printf("BMPSplice : open 1st src file error !\n");
		return VOS_ERR;
	}

	fp2 = fopen(FileName2,"rb");
	if(NULL == fp2)
	{
		printf("BMPSplice : open 2nd src file error !\n");
		fclose(fp);
		return VOS_ERR;
	}

	fp_dest = fopen(destFileName,"wb");
	if(NULL == fp_dest)
	{
		printf("BMPSplice : open dest file error !\n");
		fclose(fp);
		fclose(fp2);
		return VOS_ERR;
	}

	memset(&bfh,0,sizeof(struct bmp_fileheader));
    memset(&bih,0,sizeof(struct bmp_infoheader));
	memset(&bfh2,0,sizeof(struct bmp_fileheader));
    memset(&bih2,0,sizeof(struct bmp_infoheader));
	memset(&bfh_dest,0,sizeof(struct bmp_fileheader));
    memset(&bih_dest,0,sizeof(struct bmp_infoheader));

	fread(&bfh,sizeof(struct bmp_fileheader),1,fp);
    fread(&bih,sizeof(struct bmp_infoheader),1,fp);
	fread(&bfh2,sizeof(struct bmp_fileheader),1,fp2);
    fread(&bih2,sizeof(struct bmp_infoheader),1,fp2);

	//�������ͼ�Ŀ�Ȳ�ͬ������ͼ�����Ͳ�ͬ���򲻴���
	if (bih.biWidth != bih2.biWidth || bih.biBitCount != bih2.biBitCount)
	{
		printf("BMPSplice : two src files have different width and depth !\n");
		fclose(fp);
		fclose(fp2);
		fclose(fp_dest);
		return VOS_ERR;
	}
	
	width = bih.biWidth;
	height = (bih.biHeight + bih2.biHeight);
	depth = (bih.biBitCount/8);

	//д��ȽϹؼ��ļ���bmpͷ����
    bfh_dest.bfType=0x4D42;
    bfh_dest.bfSize=(14+40) + width*height*depth;
    bfh_dest.bfOffBits=14+40;

    bih_dest.biSize=40;
    bih_dest.biWidth=width;
    bih_dest.biHeight=height;
    bih_dest.biPlanes=1;
    bih_dest.biBitCount=bih.biBitCount;
    bih_dest.biSizeImage=width*height*depth;

	//д���޸ĺ��Ŀ���ļ����ļ�ͷ
	fwrite(&bfh_dest,sizeof(struct bmp_fileheader),1,fp_dest);
    fwrite(&bih_dest,sizeof(struct bmp_infoheader),1,fp_dest);

	dst_width_buff = (unsigned char *)malloc(sizeof(unsigned char)*width*depth);
	if (NULL == dst_width_buff)
	{
		printf("BMPSplice : malloc dst_width_buff mem failed !\n");
		fclose(fp);
		fclose(fp2);
		fclose(fp_dest);
		return VOS_ERR;
	}
    memset(dst_width_buff,0,sizeof(unsigned char)*width*depth);

	src_buff = (unsigned char *)malloc(sizeof(unsigned char)*width*(bih.biHeight)*depth);
	if (NULL == src_buff)
	{
		printf("BMPSplice : malloc src_buff mem failed !\n");
		fclose(fp);
		fclose(fp2);
		fclose(fp_dest);
		free(dst_width_buff);
		dst_width_buff = NULL;
		return VOS_ERR;
	}
    memset(src_buff,0,sizeof(unsigned char)*width*(bih.biHeight)*depth);

	fread(src_buff,1,width*(bih.biHeight)*depth,fp);
	point=src_buff;
	for (i = 0 ; i < bih.biHeight ; i++)
	{
		for (j=0;j<width*depth;j+=depth)
        {
            dst_width_buff[j+0]=point[j+0];
            dst_width_buff[j+1]=point[j+1];
            dst_width_buff[j+2]=point[j+2];
        }
        point+=width*depth;
		fwrite(dst_width_buff,sizeof(unsigned char)*width*depth,1,fp_dest);    //һ��дһ��
	}

	free(src_buff);
	src_buff = NULL;

	//Ϊ�˷�ֹBMP1��BMP2�ĸ߲�ͬ����ͼƬ��ʾ�쳣�������ͷ��Ժ���������
	src_buff = (unsigned char *)malloc(sizeof(unsigned char)*width*(bih2.biHeight)*depth);
	if (NULL == src_buff)
	{
		printf("BMPSplice : malloc src_buff mem again failed !\n");
		fclose(fp);
		fclose(fp2);
		fclose(fp_dest);
		free(dst_width_buff);
		dst_width_buff = NULL;
		return VOS_ERR;
	}
    memset(src_buff,0,sizeof(unsigned char)*width*(bih.biHeight)*depth);

	fread(src_buff,1,width*(bih2.biHeight)*depth,fp2);
	point=src_buff;
	for (i = bih.biHeight ; i < height ; i++)
	{
		for (j=0;j<width*depth;j+=depth)
        {
            dst_width_buff[j+0]=point[j+0];
            dst_width_buff[j+1]=point[j+1];
            dst_width_buff[j+2]=point[j+2];
        }
        point+=width*depth;
		fwrite(dst_width_buff,sizeof(unsigned char)*width*depth,1,fp_dest);    //һ��дһ��
	}

	//��Դ�ͷ�
	free(dst_width_buff);
	free(src_buff);
	dst_width_buff = NULL;
	src_buff = NULL;
	fclose(fp);
	fclose(fp2);
	fclose(fp_dest);

	return VOS_OK;
}

/************************************************************
��ȡbmp�ļ���ͼ�����ݲ�����˳��bmp�ļ���ʽת��Ϊjpegʱ����
************************************************************/
int read_bmp_data(unsigned char *src_buff,unsigned long width,unsigned long height,unsigned short depth)  
{  
    unsigned char *src_point;  
    unsigned char *dst_point;  
	unsigned char *tmp_buff;
	unsigned long i,j;

	//��κϷ���У��
	if (NULL == src_buff)
	{
		printf("read_bmp_data : input param error ! \n");
		return VOS_ERR;
	}

	tmp_buff = (unsigned char *)malloc(sizeof(unsigned char)*width*height*depth);
	if (NULL == tmp_buff)
	{
		printf("read_bmp_data : malloc tmp_buff mem failed ! \n");
		return VOS_ERR;
	}
	memset(tmp_buff,0,(sizeof(unsigned char)*width*height*depth));
  
    src_point=src_buff+width*depth*(height-1);  
    dst_point=tmp_buff+width*depth*(height-1);  
    for (i=0;i<height;i++)  
    {  
        for (j=0;j<width*depth;j+=depth)  
        {  
            if (depth==1)        //����Ҷ�ͼ  
            {  
                dst_point[j]=src_point[j];  
            }  
  
            if (depth==3)        //�����ɫͼ  
            {  
                dst_point[j+2]=src_point[j+0];     //����˳��BMP��JPEG������˳�����෴��
                dst_point[j+1]=src_point[j+1];  
                dst_point[j+0]=src_point[j+2];  
            }  
        }  
        dst_point-=width*depth;  
        src_point-=width*depth;  
    }  

	//����������������¸��Ƶ�Դָ���У����ͷ���Դ
	memcpy(src_buff,tmp_buff,(sizeof(unsigned char)*width*height*depth));
	free(tmp_buff);
	tmp_buff = NULL;

	return VOS_OK;
}

/************************************************************
��bmp��Դ�ļ���ʽת��Ϊjpeg��Ŀ���ļ�
************************************************************/
int BMP2Jpeg(char *FileName,char *dest_JpegFileName)
{
	FILE *fp_bmp,*fp_jpeg;

	struct bmp_fileheader bfh;
    struct bmp_infoheader bih;

	unsigned long width;
    unsigned long height;
    unsigned short depth;

	unsigned char *src_buff;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;
	unsigned char *point;

	//��κϷ���У��
	if (NULL == FileName || NULL == dest_JpegFileName)
	{
		printf("BMP2Jpeg : input param error !\n");
		return VOS_ERR;
	}

	fp_bmp = fopen(FileName,"rb");
	if(NULL == fp_bmp)
	{
		printf("BMP2Jpeg : open bmp file error !\n");
		return VOS_ERR;
	}

	fp_jpeg = fopen(dest_JpegFileName,"wb");
	if(NULL == fp_jpeg)
	{
		printf("BMP2Jpeg : open jpeg file error !\n");
		fclose(fp_bmp);
		return VOS_ERR;
	}

	memset(&bfh,0,sizeof(struct bmp_fileheader));
    memset(&bih,0,sizeof(struct bmp_infoheader));
	
	fread(&bfh,sizeof(struct bmp_fileheader),1,fp_bmp);
    fread(&bih,sizeof(struct bmp_infoheader),1,fp_bmp);

	width = bih.biWidth;
	height = bih.biHeight;
	depth = (bih.biBitCount/8);

	src_buff = (unsigned char *)malloc(sizeof(unsigned char)*width*height*depth);
	if (NULL == src_buff)
	{
		printf("BMP2Jpeg : malloc src_buff mem failed !\n");
		fclose(fp_bmp);
		fclose(fp_jpeg);
		return VOS_ERR;
	}
    memset(src_buff,0,sizeof(unsigned char)*width*height*depth);

	fread(src_buff,width*height*depth,1,fp_bmp);

	if(VOS_OK != read_bmp_data(src_buff,width,height,depth))   //��ȡBMP�ļ���Ϣ�������е���
	{
		printf("BMP2Jpeg : read_bmp_data return error !\n");
		fclose(fp_bmp);
		fclose(fp_jpeg);
		free(src_buff);
		src_buff = NULL;
		return VOS_ERR;
	}

	cinfo.err = jpeg_std_error(&jerr);         //����Ϊlibjpeg.a���ļ���API�ӿڣ�����ɲο�����ĵ�

	jpeg_create_compress(&cinfo); 
	
	jpeg_stdio_dest(&cinfo, fp_jpeg);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = depth; 
	if (depth==1)
	{
        cinfo.in_color_space = JCS_GRAYSCALE;
	}
    else
	{
        cinfo.in_color_space = JCS_RGB;
	}

	jpeg_set_defaults(&cinfo);

	jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE );

	jpeg_start_compress(&cinfo, TRUE);

	buffer=(*cinfo.mem->alloc_sarray)
            ((j_common_ptr)&cinfo,JPOOL_IMAGE,width*depth,1);
	point=src_buff+width*depth*(height-1);
    while (cinfo.next_scanline < height)
    {
        memcpy(*buffer,point,width*depth);          //������OK������ݸ��Ƶ�jpeg�ļ��У�һ��һ��
        jpeg_write_scanlines(&cinfo,buffer,1);
        point-=width*depth;
    }

	//��Դ�ͷ�
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	free(src_buff);
	src_buff = NULL;
	fclose(fp_bmp);
	fclose(fp_jpeg);

	return VOS_OK;
}

int GetBmpTmpFloder(char *srcPath, char *dstPath)
{
	if (!srcPath || !dstPath)
		return 0;

	char *p = strrchr(srcPath, '/');
	if (NULL == p)
		return 0;
	
	strncpy(dstPath, srcPath, p-srcPath);	
	return 0;
}

/************************************************************
��ǰ�漸���������з�װ��ֻ�ṩ��һ���ӿڣ����Ϊ����Դjpeg�ļ�·������һ��Ŀ��jpeg�ļ�·��
************************************************************/
int SpliceInterface(char *FileName,char *FileName2,char *DestFileName)
{
	//����п�
	if (NULL == FileName || NULL == FileName2 || NULL == DestFileName)
	{
		printf("SpliceInterface : input param error !\n");
		return VOS_ERR;
	}

	char dstPath[128];
	char tmp1Bmp[128];
	char tmp2Bmp[128];
	char tmp3Bmp[128];

	memset(dstPath, 0, 128);
	memset(tmp1Bmp, 0, 128);
	memset(tmp2Bmp, 0, 128);
	memset(tmp3Bmp, 0, 128);
	GetBmpTmpFloder(FileName, dstPath);

	sprintf(tmp1Bmp, "%s/%s", dstPath, "tmp1.bmp");
	sprintf(tmp2Bmp, "%s/%s", dstPath, "tmp2.bmp");
	sprintf(tmp3Bmp, "%s/%s", dstPath, "tmp3.bmp");
	
	/************************************************************
	1������ͼƬ�ֱ�ת��Ϊbmp��ʽ
	************************************************************/
	if(VOS_OK != Jpeg2BMP(FileName, tmp1Bmp))
	{
		printf("SpliceInterface : 1st Jpeg2BMP return error !\n");
		return VOS_ERR;
	}
	
	if(VOS_OK != Jpeg2BMP(FileName2, tmp2Bmp))
	{
		printf("SpliceInterface : 2nd Jpeg2BMP return error !\n");
		return VOS_ERR;
	}	

	/************************************************************
	2������bmp�ļ���������ƴ�ӣ��ϳ�һ��ͼƬ
	************************************************************/
	if(VOS_OK != BMPSplice(tmp1Bmp, tmp2Bmp, tmp3Bmp))
	{
		printf("SpliceInterface : BMPSplice return error !\n");
		return VOS_ERR;
	}
	
	/************************************************************
	3��bmp�ļ���ʽת��Ϊjpeg��ʽ
	************************************************************/
	if(VOS_OK != BMP2Jpeg(tmp3Bmp, DestFileName))
	{
		printf("SpliceInterface : BMP2Jpeg return error !\n");
		return VOS_ERR;
	}	

	/************************************************************
	4��ɾ����ʱBMP�ļ�
	************************************************************/
	(void)remove(tmp1Bmp);
	(void)remove(tmp2Bmp);
	(void)remove(tmp3Bmp);

	return VOS_OK;
}


