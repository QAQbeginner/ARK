#pragma once
#include<iostream>
#include"MyStruct.h"
#include"CommunicationInfo.h"
#include<strsafe.h>

class CDriverTool
{
public:
	// 加载驱动，建立0环和3环之间的联系
	static VOID ConnectDriver(HANDLE &hDevice);


	// 初始化驱动列表
	static VOID InitDriverList(CListCtrl& DriverList);
	// 遍历驱动
	static VOID UserQueryDriver(HANDLE Device, CListCtrl& DriverList);
	// 隐藏驱动
	static VOID UserHideDriver(HANDLE Device, PWCHAR DriverPath);

	// 初始化线程列表
	static VOID InitProcessList(CListCtrl& ProcessList);
	// 遍历进程
	static VOID UserQueryProcess(HANDLE Device, CListCtrl& ProcessList);
	// 隐藏进程
	static VOID HideProcess(HANDLE Device, PULONG Pid);
	// 结束进程
	static VOID DelProcess(HANDLE Device, PULONG Pid);
	// 初始化线程列表
	static VOID InitThreadList(CListCtrl& ThreadList);
	// 初始化模块列表
	static VOID InitModuleLst(CListCtrl& ModuleList);
	// 遍历线程
	static VOID UserUueryThread(HANDLE Device, CListCtrl& ThreadList, PULONG PID);
	// 遍历模块
	static VOID UserQueryModule(HANDLE Device, CListCtrl& ModuleList, PULONG PID);
	
	
	// 初始化文件列表
	static VOID InitFileList(CListCtrl& FileList);
	// 遍历文件
	static VOID UserQueryFile(HANDLE Device, CListCtrl& FileList, PWCHAR FilePath, ULONG FileLength);
	// 删除文件
	static VOID UserDelFile(HANDLE Device, PWCHAR pFilePath, ULONG uFileDirLength);


	// 初始化注册表列表
	static VOID InitRegList(CListCtrl& RegList);
	// 遍历注册表（根目录）
	static VOID UserQueryReg(HANDLE Device, CTreeCtrl& RegTree, PWCHAR pRegKeyName, ULONG RegKeyNameLength, HTREEITEM hItem);
	// 遍历注册表2
	static VOID UserQueryChildReg(HANDLE Device, CTreeCtrl& RegTree, PWCHAR pRegKeyName, ULONG RegKeyNameLength, HTREEITEM hItem, CString Path, CListCtrl& RegList);
	// 添加注册表子项
	static VOID UserCreateReg(HANDLE Device, PWCHAR pRegKeyName, ULONG uRegKeyNameLength);
	// 删除注册表子项
	static VOID UserDelReg(HANDLE Device, PWCHAR pRegKeyName, ULONG uRegKeyNameLength);


	// 初始化IDT表
	static VOID InitIDTList(CListCtrl& IDTList);
	// 遍历IDT表
	static VOID UserQueryIDT(HANDLE Device, CListCtrl& IDTList);
	// 初始化GDT表
	static VOID InitGDTList(CListCtrl& GDTList);
	// 遍历GDT表
	static VOID UserQueryGDT(HANDLE Device, CListCtrl& GDTList);
	// 初始化SSDT表
	static VOID InitSSDTList(CListCtrl& SSDTList);
	// 遍历SSDT表
	static VOID UserQuerySSDT(HANDLE Device, CListCtrl& SSDTList);

	// SYSENTER Hook,保护自身程序不被调试
	static VOID SYSENTER_HOOK(HANDLE Device);

	// 反调试
	static VOID FanTiao(HANDLE Device);

	// 对象HOOK，让指定文件无法创建
	static VOID StopFile(HANDLE Device, PWCHAR StopFileName, ULONG StopFileNameLength);
	// 对象HOOK，让指定进程无法创建
	static VOID StopProcess(HANDLE Device, PWCHAR StopProcessName, ULONG StopProcessNameLength);
	// 关闭HOOK
	static VOID CloseHook(HANDLE Device);

	// 开启内核重载
	static VOID OpenNeiHe(HANDLE Device, CListCtrl& SSDTList);
	// 关闭内核重载
	static VOID CloseNeiHe(HANDLE Device);
};

