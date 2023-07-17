#pragma once
#include"TongXun.h"
#include"ARKStruct.h"

#define MAKE_LONG(a, b)      ((LONG)(((UINT16)(((DWORD_PTR)(a)) & 0xffff)) | ((ULONG)((UINT16)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define MAKE_LONG2(a, b)      ((LONG)(((UINT64)(((DWORD_PTR)(a)) & 0xffffff)) | ((ULONG)((UINT64)(((DWORD_PTR)(b)) & 0xff))) << 24))
NTSYSAPI SERVICE_DESCRIPTOR_ENTRY KeServiceDescriptorTable;

// ����������ѵ���Ϊ�����ĺ�������Ҫʹ����Ҫ�Լ�����
PCCH PsGetProcessImageFileName(PEPROCESS Process);

PPEB PsGetProcessPeb(PEPROCESS proc);

// �����豸
NTSTATUS CreateDevice(IN PDRIVER_OBJECT	DriverObject);


// ��ȡ��������
NTSTATUS MyGetDriverNumber(PDRIVER_OBJECT DriverObject, PIRP Irp);
// ��������
NTSTATUS MyQueryDriver(PDRIVER_OBJECT DriverObject, PIRP Irp, ULONG outputSize);
// ��������
NTSTATUS MyHideDriver(PDEVICE_OBJECT pDevice, PIRP Irp);

// ��ȡ��������
NTSTATUS MyGetProcessNumber(PIRP Irp);
// ��������
NTSTATUS MyQueryProcess(PIRP Irp, ULONG outputSize);
// ���ؽ���
NTSTATUS MyHindProcess(PIRP Irp);
// ��������
NTSTATUS MyDelProcess(PIRP Irp);
// ��ȡ�߳�����
NTSTATUS MyGetThreadNumber(PIRP Irp);
// �����߳�
NTSTATUS MyQueryThread(PIRP Irp);

// ��ȡģ������
NTSTATUS MyGetModuleNumber(PIRP Irp);
// ����ģ��
NTSTATUS MyQueryModule(PIRP Irp);
// ��ȡ��һ���ļ�
NTSTATUS FindFirstFile(const WCHAR* pszPath, HANDLE* phFile, FILE_BOTH_DIR_INFORMATION* pFileInfo, ULONG nInfoSize);
// ��ȡ��һ���ļ�
NTSTATUS FindNextFile(HANDLE hFile, FILE_BOTH_DIR_INFORMATION* pFileInfo, ULONG nInfoSize);
// ��ȡ�ļ�����
NTSTATUS MyGetFileNumber(PIRP Irp);
// �����ļ�
NTSTATUS MyQueryFile(PIRP Irp);
// ɾ���ļ�
NTSTATUS DelFile(PIRP Irp);


// ��ȡע�����Ŀ����
NTSTATUS MyGetRegNumber(PIRP Irp);
// ����ע���
NTSTATUS MyQueryReg(PIRP Irp);
// ���ע�������
NTSTATUS MyCreateReg(PIRP Irp);
// ɾ��ע�������
NTSTATUS MyDelReg(PIRP Irp);


// ����IDT��
NTSTATUS MyQueryIDT(PIRP Irp);

// ����GDT��
NTSTATUS MyQueryGDT(PIRP Irp);
// ��ȡGDT���е�Ԫ�ظ���
NTSTATUS MyGetGDTNumber(PIRP Irp);
// ����SSDT��
NTSTATUS MyQuerySSDT(PIRP Irp);

// Ĭ����ǲ����
NTSTATUS CommonDispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);								// ������Ǿ�����Ϣ������
// �Զ�����ǲ����
NTSTATUS IoCtlDispatchRoutine(IN PDEVICE_OBJECT	DeviceObject, IN PIRP Irp);


// ������
NTSTATUS FanTiaoShi(PIRP Irp);

// Sysenter-Hook

// ����hook
VOID InstallSysenterHook();
// �ر�hook
VOID UninstallSysenterHook();


// ����HOOk


// ���ļ���ȡ�ļ����
NTSTATUS CreateFile(PHANDLE FileHandle, PCWCH pFilePath, ACCESS_MASK Access, ULONG CreateDisposition, BOOLEAN IsFile);
NTSTATUS MyOpenProcedure(IN ULONG Unknown, IN OB_OPEN_REASON OpenReason, IN PEPROCESS Process OPTIONAL, IN PVOID Object, IN ACCESS_MASK GrantedAccess, IN ULONG HandleCount);

// �ļ�����HOOK
VOID OnObjecFileHook();
// ���̶���HOOK
VOID OnObjectProcessHook();

// �ں�����

// �ں����� ��ʼ
void KernelReload(PIRP Irp);
// ж���ں˹���
void UnHook();