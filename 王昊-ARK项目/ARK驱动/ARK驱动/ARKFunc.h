#pragma once
#include"TongXun.h"
#include"ARKStruct.h"

#define MAKE_LONG(a, b)      ((LONG)(((UINT16)(((DWORD_PTR)(a)) & 0xffff)) | ((ULONG)((UINT16)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define MAKE_LONG2(a, b)      ((LONG)(((UINT64)(((DWORD_PTR)(a)) & 0xffffff)) | ((ULONG)((UINT64)(((DWORD_PTR)(b)) & 0xff))) << 24))
NTSYSAPI SERVICE_DESCRIPTOR_ENTRY KeServiceDescriptorTable;

// 这个函数是已导出为声明的函数，想要使用需要自己声明
PCCH PsGetProcessImageFileName(PEPROCESS Process);

PPEB PsGetProcessPeb(PEPROCESS proc);

// 创建设备
NTSTATUS CreateDevice(IN PDRIVER_OBJECT	DriverObject);


// 获取驱动数量
NTSTATUS MyGetDriverNumber(PDRIVER_OBJECT DriverObject, PIRP Irp);
// 遍历驱动
NTSTATUS MyQueryDriver(PDRIVER_OBJECT DriverObject, PIRP Irp, ULONG outputSize);
// 隐藏驱动
NTSTATUS MyHideDriver(PDEVICE_OBJECT pDevice, PIRP Irp);

// 获取进程数量
NTSTATUS MyGetProcessNumber(PIRP Irp);
// 遍历进程
NTSTATUS MyQueryProcess(PIRP Irp, ULONG outputSize);
// 隐藏进程
NTSTATUS MyHindProcess(PIRP Irp);
// 结束进程
NTSTATUS MyDelProcess(PIRP Irp);
// 获取线程数量
NTSTATUS MyGetThreadNumber(PIRP Irp);
// 遍历线程
NTSTATUS MyQueryThread(PIRP Irp);

// 获取模块数量
NTSTATUS MyGetModuleNumber(PIRP Irp);
// 遍历模块
NTSTATUS MyQueryModule(PIRP Irp);
// 获取第一个文件
NTSTATUS FindFirstFile(const WCHAR* pszPath, HANDLE* phFile, FILE_BOTH_DIR_INFORMATION* pFileInfo, ULONG nInfoSize);
// 获取下一个文件
NTSTATUS FindNextFile(HANDLE hFile, FILE_BOTH_DIR_INFORMATION* pFileInfo, ULONG nInfoSize);
// 获取文件数量
NTSTATUS MyGetFileNumber(PIRP Irp);
// 遍历文件
NTSTATUS MyQueryFile(PIRP Irp);
// 删除文件
NTSTATUS DelFile(PIRP Irp);


// 获取注册表项目数量
NTSTATUS MyGetRegNumber(PIRP Irp);
// 遍历注册表
NTSTATUS MyQueryReg(PIRP Irp);
// 添加注册表子项
NTSTATUS MyCreateReg(PIRP Irp);
// 删除注册表子项
NTSTATUS MyDelReg(PIRP Irp);


// 遍历IDT表
NTSTATUS MyQueryIDT(PIRP Irp);

// 遍历GDT表
NTSTATUS MyQueryGDT(PIRP Irp);
// 获取GDT表中的元素个数
NTSTATUS MyGetGDTNumber(PIRP Irp);
// 遍历SSDT表
NTSTATUS MyQuerySSDT(PIRP Irp);

// 默认派遣函数
NTSTATUS CommonDispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);								// 保存的是具体消息的内容
// 自定义派遣函数
NTSTATUS IoCtlDispatchRoutine(IN PDEVICE_OBJECT	DeviceObject, IN PIRP Irp);


// 反调试
NTSTATUS FanTiaoShi(PIRP Irp);

// Sysenter-Hook

// 开启hook
VOID InstallSysenterHook();
// 关闭hook
VOID UninstallSysenterHook();


// 对象HOOk


// 打开文件获取文件句柄
NTSTATUS CreateFile(PHANDLE FileHandle, PCWCH pFilePath, ACCESS_MASK Access, ULONG CreateDisposition, BOOLEAN IsFile);
NTSTATUS MyOpenProcedure(IN ULONG Unknown, IN OB_OPEN_REASON OpenReason, IN PEPROCESS Process OPTIONAL, IN PVOID Object, IN ACCESS_MASK GrantedAccess, IN ULONG HandleCount);

// 文件对象HOOK
VOID OnObjecFileHook();
// 进程对象HOOK
VOID OnObjectProcessHook();

// 内核重载

// 内核重载 开始
void KernelReload(PIRP Irp);
// 卸载内核钩子
void UnHook();