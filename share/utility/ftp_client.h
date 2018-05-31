/******************************************************************************

  Copyright (c) 2015 Jovision Technology Co., Ltd. All rights reserved.

  File Name     : ftp_client.h
  Version       : 
  Author        : Qin Lichao <qinlichao@jovision.com>
  Created       : 2015-6-12
  Description   : FTP client, upload or download file from FTP server.
  History       : 
  1.Date        : 2015-6-12
    Author      : Qin Lichao <qinlichao@jovision.com>
    Modification: Created file
******************************************************************************/
/*
����������

�����httpЭ�����ƣ������ǣ�
2��ͷ�����ɹ�
3��ͷ����Ȩ������
4��ͷ�����ļ�����
5��ͷ��������������

�����Ĵ�����Ϣ
-----------------------------------
120 Service ready in NNN minutes.
������NNNʱ���ڿ���
-----------------------------------
125 Data connection already open; transfer starting.
���������Ѿ��򿪣���ʼ��������.
-----------------------------------
150 File status okay; about to open data connection.
�ļ�״̬��ȷ�����ڴ���������.
-----------------------------------
200 Command okay.
����ִ����������.
-----------------------------------
202 Command not implemented, superfluous at this site.
����δ��ִ�У���վ�㲻֧�ִ�����.
-----------------------------------
211 System status, or system help reply.
ϵͳ״̬��ϵͳ������Ϣ��Ӧ.
-----------------------------------
212 Directory status.
Ŀ¼״̬��Ϣ.
-----------------------------------
213 File status. $XrkxmL=
�ļ�״̬��Ϣ.
-----------------------------------
214 Help message.On how to use the server or the meaning of a particular non-standard command. This reply is useful only to the human user.
������Ϣ���������ʹ�ñ�������������ķǱ�׼���
-----------------------------------
215 NAME system type. Where NAME is an official system name from the list in the Assigned Numbers document.
NAMEϵͳ���͡�
-----------------------------------
220 Service ready for new user.
�����ӵ��û��ķ����Ѿ���
-----------------------------------
221 Service closing control connection.
�������ӹر�
-----------------------------------
225 Data connection open; no transfer in progress.
���������Ѵ򿪣�û�н����е����ݴ���
-----------------------------------
226 Closing data connection. Requested file action successful (for example, file transfer or file abort).
���ڹر��������ӡ������ļ������ɹ����������磬�ļ����ͻ���ֹ��
-----------------------------------
227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).
���뱻��ģʽ
-----------------------------------
230 User logged in, proceed. Logged out if appropriate.
�û��ѵ��롣 �������Ҫ���Եǳ���
-----------------------------------
250 Requested file action okay, completed.
�������ļ������ɹ���� 63
-----------------------------------
257 "PATHNAME" created.
·���ѽ���
-----------------------------------
331 User name okay, need password.
�û������ڣ���Ҫ��������
-----------------------------------
332 Need account for login.
��Ҫ��½���˻�
-----------------------------------
350 Requested file action pending further inFORMation U
�Ա������ļ��Ĳ�����Ҫ��һ���������Ϣ
-----------------------------------
421 Service not available, closing control connection.This may be a reply to any command if the service knows it must shut down.
���񲻿��ã��������ӹرա�������Ƕ��κ�����Ļ�Ӧ�����������Ϊ������ر�
-----------------------------------
425 Can't open data connection.
����������ʧ��
-----------------------------------
426 Connection closed; transfer aborted.
���ӹرգ�������ֹ��
-----------------------------------
450 Requested file action not taken.
�Ա������ļ��Ĳ���δ��ִ��
-----------------------------------
451 Requested action aborted. Local error in processing.
����Ĳ�����ֹ�������з������ش���
-----------------------------------
452 Requested action not taken. Insufficient storage space in system.File unavailable (e.g., file busy).
����Ĳ���û�б�ִ�С�ϵͳ�洢�ռ䲻�㡣 �ļ�������
-----------------------------------
500 Syntax error, command unrecognized. This may include errors such as command line too long.
�﷨���󣬲���ʶ������ ������������й�����
-----------------------------------
501 Syntax error in parameters or arguments.
���������µ��﷨����
-----------------------------------
502 Command not implemented.
����δ��ִ��
-----------------------------------
503 Bad sequence of commands.
����Ĵ������
-----------------------------------
504 Command not implemented for that parameter.
���ڲ�����������δ��ִ��
-----------------------------------
530 Not logged in.
û�е�¼
-----------------------------------
532 Need account for storing files.
�洢�ļ���Ҫ�˻���Ϣ!
-----------------------------------
550 Requested action not taken. File unavailable (e.g., file not found, no access).
�������δ��ִ�У��ļ������á�
-----------------------------------
551 Requested action aborted. Page type unknown.
���������ֹ��ҳ������δ֪
-----------------------------------
552 Requested file action aborted. Exceeded storage allocation (for current directory or dataset).
�������ļ��Ĳ�����ֹ�� �����洢����
-----------------------------------
553 Requested action not taken. File name not allowed
�������δ��ִ�С� �ļ���������
-----------------------------------
*/

/**
 *@brief ����FTP������
 *@param host FTP������IP��ַ
 *@param port FTP����������˿�
 *@param user FTP��������¼�û���
 *@param pwd FTP��������¼����
 *@return >0 �������ӵ�socket��-1����ʧ��
 */
int ftp_connect(char *host, int port, char *user, char *pwd);

/**
 *@brief �ر�FTP����
 *@param c_sock ����socket
 *@return 221�رճɹ�������ʧ��
 */
int ftp_quit(int c_sock);

/**
 *@brief ���ô���ģʽ
 *@param c_sock ����socket
 *@param mode FTP����ģʽ : A=ASCII��E=EBCDIC��I=binary
 *@return 0�ɹ���-1ʧ��
 */
int ftp_type(int c_sock, char mode);

/**
 *@brief �ı乤��Ŀ¼
 *@param c_sock ����socket
 *@param path Ҫ�л�����Ŀ¼·��
 *@return 0�ɹ���-1ʧ��
 */
int ftp_cwd(int c_sock, char *path);

/**
 *@brief �ص���һ��Ŀ¼
 *@param c_sock ����socket
 *@return 0�ɹ�������ʧ��
 */
int ftp_cdup(int c_sock);

/**
 *@brief ����Ŀ¼
 *@param c_sock ����socket
 *@param path Ҫ������Ŀ¼·��
 *@return 0�ɹ�������ʧ��
 */
int ftp_mkd(int c_sock, char *path);

/**
 *@brief �б�
 *@param c_sock ����socket
 *@param path Ŀ¼·��
 *@param data �ļ��б�
 *@param data_len �ļ��б���
 *@return 0�ɹ�������ʧ��
 */
int ftp_list(int c_sock, char *path, void **data, unsigned long long *data_len);

/**
 *@brief �����ļ�
 *@param c_sock ����socket
 *@param s Դ�ļ�·��
 *@param d Ŀ���ļ�·��
 *@param stor_size �ļ���С
 *@param stop ����ֹͣ
 *@return 0�ɹ�������ʧ��
 */
int ftp_retrfile(int c_sock, char *s, char *d ,unsigned long long *stor_size, int *stop);

/**
 *@brief �ϴ��ļ�
 *@param c_sock ����socket
 *@param s Դ�ļ�·��
 *@param d Ŀ���ļ�·��
 *@param stor_size �ļ���С
 *@param stop ����ֹͣ
 *@return 0�ɹ�������ʧ��
 */
int ftp_storfile(int c_sock, char *s, char *d ,unsigned long long *stor_size, int *stop);

/**
 *@brief �޸��ļ���&�ƶ�Ŀ¼
 *@param c_sock ����socket
 *@param s Դ�ļ�·��
 *@param d Ŀ���ļ�·��
 *@return 0�ɹ�������ʧ��
 */
int ftp_renamefile(int c_sock, char *s, char *d);

/**
 *@brief ɾ���ļ�
 *@param c_sock ����socket
 *@param s Ҫɾ�����ļ�·��
 *@return 0�ɹ�������ʧ��
 */
int ftp_deletefile(int c_sock, char *s);

/**
 *@brief ɾ��Ŀ¼
 *@param c_sock ����socket
 *@param s Ҫɾ����Ŀ¼·��
 *@return 0�ɹ�������ʧ��
 */
int ftp_deletefolder(int c_sock, char *s);
