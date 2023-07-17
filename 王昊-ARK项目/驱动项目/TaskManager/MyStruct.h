#pragma once
#include<windows.h>
//�����������������Ϣ
typedef struct _DRIVER_INFO
{
	PVOID DllBase;
	WCHAR driverName[0x100];
	ULONG SizeOfImage;
	WCHAR driverPath[0x100];
}DRIVER_INFO, * PDRIVER_INFO;
// ����������Ľ�����Ϣ
typedef struct _PROCESS_INFO
{
	PVOID PID;
	WCHAR ProcessName[0x100];
	PVOID EProcess;
}PROCESS_INFO, * PPROCESS_INFO;
//�߳���Ϣ�ṹ��
typedef struct _THREAD_INFO
{
	ULONG Tid;		    // �߳�ID
	ULONG BasePriority;	//��̬���ȼ�
}THREAD_INFO, * PTHREAD_INFO;
// ģ����Ϣ�ṹ��
typedef struct _MODULE_INFO
{
	WCHAR wcModuleFullPath[260];	//ģ��·��
	PVOID DllBase;		//����ַ
	ULONG SizeOfImage;	//��С
}MODULE_INFO, * PMODULE_INFO;
//�ļ���Ϣ�ṹ��
typedef struct _FILEINFO
{
	char FileOrDirectory;	    //һ���ֽ����������ļ�����Ŀ¼,0��ʾĿ¼��1��ʾ�ļ�
	WCHAR wcFileName[260];	    //�ļ���
	LONGLONG Size;				//�ļ���С
	LARGE_INTEGER CreateTime;	//����ʱ��
	LARGE_INTEGER ChangeTime;	//�޸�ʱ��00
}FILE_INFO, * PFILE_INFO;
//ע�����Ϣ�ṹ��
typedef struct _REG_INFO
{
	ULONG Type;				// ���ͣ�0 Ϊ���1Ϊֵ
	WCHAR KeyName[256];		// ����
	WCHAR ValueName[256];   // ֵ������
	ULONG ValueType;		// ֵ������
	UCHAR Value[256];	    // ֵ
	ULONG ValueLength;		//ֵ�ĳ���
}REG_INFO, * PREG_INFO;
//IDT��Ϣ�ṹ��
typedef struct _IDT_INFO
{
	ULONG pFunction;		//�������ĵ�ַ
	ULONG Selector;			//��ѡ����
	ULONG ParamCount;		//��������
	ULONG Dpl;				//����Ȩ��
	ULONG GateType;			//����
}IDT_INFO, * PIDT_INFO;
//GDT��Ϣ�ṹ��
typedef struct _GDT_INFO
{
	ULONG BaseAddr;	//�λ�ַ
	ULONG Limit;	//���޳�
	ULONG Grain;	//������
	ULONG Dpl;		//��Ȩ�ȼ�
	ULONG GateType;	//����
}GDT_INFO, * PGDT_INFO;
// SSDT��Ϣ�ṹ��
typedef struct _SSDT_INFO
{
	ULONG uIndex;	//�ص���
	ULONG uFuntionAddr;	//������ַ
}SSDT_INFO, * PSSDT_INFO;