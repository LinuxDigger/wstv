#ifndef _UTL_THREAD_H_
#define _UTL_THREAD_H_

#define LINUX_THREAD_STACK_SIZE (512*1024)
//���������߳�,����ͬpthread_create
//���ʹ��pthread_create��������pthread_join��Ӧ����������ڴ�й©�����û��pthread_join�������
//ע��:������pthread_attr_t����
int pthread_create_detached(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg);

//����汾
#define pthread_create_simple(pFun, parm) \
do{\
	pthread_t pid;\
	pthread_create_detached(&pid, NULL, (void*)pFun, (void*)parm);\
}while(0)

//������ͨ�߳�,ջ��С��ΪLINUX_THREAD_STACK_SIZE������ͬpthread_create
//ע��:������pthread_attr_t����
int pthread_create_normal(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg);

//�����߳�,ջ��С��ΪLINUX_THREAD_STACK_SIZE������ͬpthread_create
//���ȼ�����Ϊ51
//ע��:������pthread_attr_t����
int pthread_create_normal_priority(pthread_t *pid, const pthread_attr_t *no_use, void*(*pFunction)(void*), void *arg);

#endif

