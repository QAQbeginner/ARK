#pragma once
#include<windows.h>
//保存遍历到的驱动信息
typedef struct _DRIVER_INFO
{
	PVOID DllBase;
	WCHAR driverName[0x100];
	ULONG SizeOfImage;
	WCHAR driverPath[0x100];
}DRIVER_INFO, * PDRIVER_INFO;
// 保存遍历到的进程信息
typedef struct _PROCESS_INFO
{
	PVOID PID;
	WCHAR ProcessName[0x100];
	PVOID EProcess;
}PROCESS_INFO, * PPROCESS_INFO;
//线程信息结构体
typedef struct _THREAD_INFO
{
	ULONG Tid;		    // 线程ID
	ULONG BasePriority;	//静态优先级
}THREAD_INFO, * PTHREAD_INFO;
// 模块信息结构体
typedef struct _MODULE_INFO
{
	WCHAR wcModuleFullPath[260];	//模块路径
	PVOID DllBase;		//基地址
	ULONG SizeOfImage;	//大小
}MODULE_INFO, * PMODULE_INFO;
//文件信息结构体
typedef struct _FILEINFO
{
	char FileOrDirectory;	    //一个字节来保存是文件还是目录,0表示目录，1表示文件
	WCHAR wcFileName[260];	    //文件名
	LONGLONG Size;				//文件大小
	LARGE_INTEGER CreateTime;	//创建时间
	LARGE_INTEGER ChangeTime;	//修改时间00
}FILE_INFO, * PFILE_INFO;
//注册表信息结构体
typedef struct _REG_INFO
{
	ULONG Type;				// 类型，0 为子项，1为值
	WCHAR KeyName[256];		// 项名
	WCHAR ValueName[256];   // 值的名字
	ULONG ValueType;		// 值的类型
	UCHAR Value[256];	    // 值
	ULONG ValueLength;		//值的长度
}REG_INFO, * PREG_INFO;
//IDT信息结构体
typedef struct _IDT_INFO
{
	ULONG pFunction;		//处理函数的地址
	ULONG Selector;			//段选择子
	ULONG ParamCount;		//参数个数
	ULONG Dpl;				//段特权级
	ULONG GateType;			//类型
}IDT_INFO, * PIDT_INFO;
//GDT信息结构体
typedef struct _GDT_INFO
{
	ULONG BaseAddr;	//段基址
	ULONG Limit;	//段限长
	ULONG Grain;	//段粒度
	ULONG Dpl;		//特权等级
	ULONG GateType;	//类型
}GDT_INFO, * PGDT_INFO;
// SSDT信息结构体
typedef struct _SSDT_INFO
{
	ULONG uIndex;	//回调号
	ULONG uFuntionAddr;	//函数地址
}SSDT_INFO, * PSSDT_INFO;