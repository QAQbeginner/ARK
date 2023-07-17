// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
// 原函数的目标地址
DWORD OldPath = 0;

// 保存旧地址的全局变量
UCHAR OldCode[5] = { 0 };
// 要跳转的代码数组
UCHAR NewCode[4] = { 0 };

// 保存以前的页属性
DWORD OldProtect;

// 保存ProcessId
DWORD MyProcessId;

// 获取物理内存中的PID信息
VOID ReseveMsg()
{
	// 1. 打开一个已经创建好的文件映射内核对象
	HANDLE Mapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"InjectName");
	PDWORD Buffer = (PDWORD)MapViewOfFile(Mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DWORD));
	MyProcessId = *Buffer;
	// 关闭句柄，防止句柄泄露
	CloseHandle(Mapping);
	// 将虚拟内存和物理内存的关联解除
	UnmapViewOfFile(Buffer);
}

// 自己的openprocess函数
HANDLE WINAPI MyOpenProcess(_In_ DWORD dwDesiredAccess, _In_ BOOL bInheritHandle, _In_ DWORD dwProcessId)
{

	HANDLE Ret = NULL;
	OffHook();
	if (dwProcessId != MyProcessId)
	{
		Ret = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	}
	else
	{
		MessageBox(0, L"该进程已被保护", 0, 0);
	}
	OnHook();
	return Ret;
}

// 初始化Hook功能函数
VOID HookInit()
{
	// 获取openprocess的原函数地址
	OldPath = (DWORD)OpenProcess;
	*(DWORD*)NewCode = (DWORD)MyOpenProcess - OldPath - 5;
}

// 开启Hook功能函数
VOID OnHook()
{
	MessageBox(0, L"保护成功", L"QAQ^-^", 0);
	// 先将原代码保存至全局变量
	char* pAddr = (char*)OldPath;
	OldCode[0] = pAddr[0];
	OldCode[1] = pAddr[1];
	OldCode[2] = pAddr[2];
	OldCode[3] = pAddr[3];
	OldCode[4] = pAddr[4];
	// 将新的代码赋值给原地址
	VirtualProtect(pAddr, 5, PAGE_EXECUTE_READWRITE, &OldProtect);
	pAddr[0] = 0xE9;
	pAddr[1] = NewCode[0];
	pAddr[2] = NewCode[1];
	pAddr[3] = NewCode[2];
	pAddr[4] = NewCode[3];
	VirtualProtect(pAddr, 5, OldProtect, &OldProtect);
}

// 关闭Hook功能函数
VOID OffHook()
{
	// 先将原代码保存至全局变量
	char* pAddr = (char*)OldPath;
	VirtualProtect(pAddr, 5, PAGE_EXECUTE_READWRITE, &OldProtect);
	pAddr[0] = OldCode[0];
	pAddr[1] = OldCode[1];
	pAddr[2] = OldCode[2];
	pAddr[3] = OldCode[3];
	pAddr[4] = OldCode[4];
	VirtualProtect(pAddr, 5, OldProtect, &OldProtect);
}