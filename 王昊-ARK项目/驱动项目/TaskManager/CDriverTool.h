#pragma once
#include<iostream>
#include"MyStruct.h"
#include"CommunicationInfo.h"
#include<strsafe.h>

class CDriverTool
{
public:
	// ��������������0����3��֮�����ϵ
	static VOID ConnectDriver(HANDLE &hDevice);


	// ��ʼ�������б�
	static VOID InitDriverList(CListCtrl& DriverList);
	// ��������
	static VOID UserQueryDriver(HANDLE Device, CListCtrl& DriverList);
	// ��������
	static VOID UserHideDriver(HANDLE Device, PWCHAR DriverPath);

	// ��ʼ���߳��б�
	static VOID InitProcessList(CListCtrl& ProcessList);
	// ��������
	static VOID UserQueryProcess(HANDLE Device, CListCtrl& ProcessList);
	// ���ؽ���
	static VOID HideProcess(HANDLE Device, PULONG Pid);
	// ��������
	static VOID DelProcess(HANDLE Device, PULONG Pid);
	// ��ʼ���߳��б�
	static VOID InitThreadList(CListCtrl& ThreadList);
	// ��ʼ��ģ���б�
	static VOID InitModuleLst(CListCtrl& ModuleList);
	// �����߳�
	static VOID UserUueryThread(HANDLE Device, CListCtrl& ThreadList, PULONG PID);
	// ����ģ��
	static VOID UserQueryModule(HANDLE Device, CListCtrl& ModuleList, PULONG PID);
	
	
	// ��ʼ���ļ��б�
	static VOID InitFileList(CListCtrl& FileList);
	// �����ļ�
	static VOID UserQueryFile(HANDLE Device, CListCtrl& FileList, PWCHAR FilePath, ULONG FileLength);
	// ɾ���ļ�
	static VOID UserDelFile(HANDLE Device, PWCHAR pFilePath, ULONG uFileDirLength);


	// ��ʼ��ע����б�
	static VOID InitRegList(CListCtrl& RegList);
	// ����ע�����Ŀ¼��
	static VOID UserQueryReg(HANDLE Device, CTreeCtrl& RegTree, PWCHAR pRegKeyName, ULONG RegKeyNameLength, HTREEITEM hItem);
	// ����ע���2
	static VOID UserQueryChildReg(HANDLE Device, CTreeCtrl& RegTree, PWCHAR pRegKeyName, ULONG RegKeyNameLength, HTREEITEM hItem, CString Path, CListCtrl& RegList);
	// ���ע�������
	static VOID UserCreateReg(HANDLE Device, PWCHAR pRegKeyName, ULONG uRegKeyNameLength);
	// ɾ��ע�������
	static VOID UserDelReg(HANDLE Device, PWCHAR pRegKeyName, ULONG uRegKeyNameLength);


	// ��ʼ��IDT��
	static VOID InitIDTList(CListCtrl& IDTList);
	// ����IDT��
	static VOID UserQueryIDT(HANDLE Device, CListCtrl& IDTList);
	// ��ʼ��GDT��
	static VOID InitGDTList(CListCtrl& GDTList);
	// ����GDT��
	static VOID UserQueryGDT(HANDLE Device, CListCtrl& GDTList);
	// ��ʼ��SSDT��
	static VOID InitSSDTList(CListCtrl& SSDTList);
	// ����SSDT��
	static VOID UserQuerySSDT(HANDLE Device, CListCtrl& SSDTList);

	// SYSENTER Hook,����������򲻱�����
	static VOID SYSENTER_HOOK(HANDLE Device);

	// ������
	static VOID FanTiao(HANDLE Device);

	// ����HOOK����ָ���ļ��޷�����
	static VOID StopFile(HANDLE Device, PWCHAR StopFileName, ULONG StopFileNameLength);
	// ����HOOK����ָ�������޷�����
	static VOID StopProcess(HANDLE Device, PWCHAR StopProcessName, ULONG StopProcessNameLength);
	// �ر�HOOK
	static VOID CloseHook(HANDLE Device);

	// �����ں�����
	static VOID OpenNeiHe(HANDLE Device, CListCtrl& SSDTList);
	// �ر��ں�����
	static VOID CloseNeiHe(HANDLE Device);
};

