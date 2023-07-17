#include "pch.h"
#include "CDriverTool.h"

// ��������������0����3��֮�����ϵ��ûд��
VOID CDriverTool::ConnectDriver(HANDLE &hDevice)
{
	hDevice = CreateFile(L"\\\\.\\king", GENERIC_ALL, NULL,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice == NULL)
	{
		MessageBox(0,L"���豸ʧ��", 0, 0);
		return ;
	}
	return;
}


// ��ʼ�������б�
VOID CDriverTool::InitDriverList(CListCtrl& DriverList)
{
	/*
	 ���ý����б���
	*/
	DWORD oldstyle = DriverList.GetExtendedStyle();
	DriverList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	DriverList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	DriverList.InsertColumn(0, L"������", 0, Width/2);
	DriverList.InsertColumn(1, L"����·��", 0, Width/2);
}
// ����������������ֵ�б�
VOID CDriverTool::UserQueryDriver(HANDLE Device,CListCtrl& DriverList)
{
	DWORD DriverNum = 0;
	DeviceIoControl(Device, CTL_GetDriverNum, 0, sizeof(DWORD), NULL, 0, &DriverNum, NULL);
	PVOID DriverInfo = malloc(sizeof(DRIVER_INFO) * DriverNum);
	DWORD size = 0;
	DeviceIoControl(
		Device,		//�򿪵��豸���
		CTL_QueryDriver,	//������
		NULL,		//���뻺����
		0,			//���뻺������С
		DriverInfo,		//���������
		sizeof(DRIVER_INFO) * DriverNum,	//����������Ĵ�С
		&size,		//ʵ�ʴ�����ֽ���
		NULL		//�Ƿ���OVERLAPPED����
	);

	PDRIVER_INFO buffTemp = (PDRIVER_INFO)DriverInfo;
	int j = 0;
	for (DWORD i = 0; i < DriverNum; i++,j++)
	{
		if (buffTemp[i].SizeOfImage == 0)
		{
			j--;
			continue;
		}
		//������
		DriverList.InsertItem(j, buffTemp[i].driverName);
		// ����·��
		DriverList.SetItemText(j, 1, buffTemp[i].driverPath);
	}
	//�ͷſռ�
	free(DriverInfo);
}
// ��������
VOID CDriverTool::UserHideDriver(HANDLE Device, PWCHAR DriverPath)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_HideDriver, DriverPath, MAX_PATH, NULL, 0, &size, NULL);
}


// ��ʼ�������б�
VOID CDriverTool::InitProcessList(CListCtrl& ProcessList)
{
	/*
	 ���ý����б���
	*/
	DWORD oldstyle = ProcessList.GetExtendedStyle();
	ProcessList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	ProcessList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	ProcessList.InsertColumn(0, L"����ID", 0, Width/10);
	ProcessList.InsertColumn(1, L"��������", 0, 7*Width/10);
	ProcessList.InsertColumn(2, L"EPROCESS", 0, 2*Width/10);
}
// ��������
VOID CDriverTool::UserQueryProcess(HANDLE Device, CListCtrl& ProcessList)
{
	DWORD ProcessNum;
	DeviceIoControl(Device, CTL_GetProcessNum, 0, sizeof(DWORD), NULL, 0, &ProcessNum, NULL);
	PVOID ProcessInfo = malloc(sizeof(PROCESS_INFO) * ProcessNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryProcess, NULL, 0, ProcessInfo, sizeof(PROCESS_INFO) * ProcessNum, &size, NULL);
	PPROCESS_INFO buffTemp = (PPROCESS_INFO)ProcessInfo;
	DWORD j = 0;
	for (DWORD i = 0; i < ProcessNum; i++)
	{
		if (buffTemp[i].PID != 0)
		{
			CStringA Info;
			Info.Format("%d", buffTemp[i].PID);
			// ����id
			ProcessList.InsertItem(j, (CString)Info);
			//������
			CString Info1;
			Info1.Format(L"%s", buffTemp[i].ProcessName);
			ProcessList.SetItemText(j, 1, (CString)Info1);
			// ���̿�
			Info.Format("0x%08X", buffTemp[i].EProcess);
			ProcessList.SetItemText(j, 2, (CString)Info);
			j++;
		}
	}
	//�ͷſռ�
	free(ProcessInfo);
}
// ���ؽ���
VOID CDriverTool::HideProcess(HANDLE Device,PULONG Pid)
{
	DWORD size = 0;

	DeviceIoControl(
		Device,		//�򿪵��豸���
		CTL_HideProcess,	//������
		Pid,		//���뻺����
		sizeof(ULONG),			//���뻺������С
		NULL,		//���������
		0,			//����������Ĵ�С
		&size,		//ʵ�ʴ�����ֽ���
		NULL		//�Ƿ���OVERLAPPED����
	);
}
// ��������
VOID CDriverTool::DelProcess(HANDLE Device, PULONG Pid)
{
	DWORD size = 0;

	DeviceIoControl(
		Device,		//�򿪵��豸���
		CTL_DelProcess,	//������
		Pid,		//���뻺����
		sizeof(ULONG),			//���뻺������С
		NULL,		//���������
		0,			//����������Ĵ�С
		&size,		//ʵ�ʴ�����ֽ���
		NULL		//�Ƿ���OVERLAPPED����
	);
}
// ��ʼ���߳��б�
VOID CDriverTool::InitThreadList(CListCtrl& ThreadList)
{
	/*
	 ���ý����б���
	*/
	DWORD oldstyle = ThreadList.GetExtendedStyle();
	ThreadList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	ThreadList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	ThreadList.InsertColumn(0, L"�߳�ID", 0, Width / 2);
	ThreadList.InsertColumn(1, L"�߳����ȼ�", 0, Width / 2);
}
// �����߳�
VOID CDriverTool::UserUueryThread(HANDLE Device, CListCtrl& ThreadList, PULONG Pid)
{
	DWORD ThreadNum;
	DeviceIoControl(Device, CTL_GetThreadNum, Pid, sizeof(DWORD), NULL, 0, &ThreadNum, NULL);
	if (ThreadNum == 0)
		return;
	PVOID ThreadInfo = malloc(sizeof(THREAD_INFO) * ThreadNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryThread, Pid, sizeof(DWORD), ThreadInfo, sizeof(THREAD_INFO) * ThreadNum, &size, NULL);
	PTHREAD_INFO buffTemp = (PTHREAD_INFO)ThreadInfo;
	for (DWORD i = 0; i < ThreadNum; i++)
	{
		CStringA Info;
		Info.Format("%d", buffTemp[i].Tid);
		// �߳�id
		ThreadList.InsertItem(i, (CString)Info);
		//���ȼ�
		//Info.Format("%d", buffTemp[i].BasePriority);
		//ThreadList.SetItemText(i, 1, (CString)Info);
	}
	//�ͷſռ�
	free(ThreadInfo);
}
// ��ʼ��ģ���б�
VOID CDriverTool::InitModuleLst(CListCtrl& ModuleList)
{
	/*
	 ���ý����б���
	*/
	DWORD oldstyle = ModuleList.GetExtendedStyle();
	ModuleList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	ModuleList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	ModuleList.InsertColumn(0, L"ģ��·��", 0, Width / 3);
	ModuleList.InsertColumn(1, L"ģ����ػ�ַ", 0, Width / 3);
	ModuleList.InsertColumn(2, L"��С", 0, Width / 3);
}
// ����ģ��
VOID CDriverTool::UserQueryModule(HANDLE Device, CListCtrl& ModuleList, PULONG pEprocess)
{
	// ��ȡ����
	DWORD ModuleNum = 0;
	DeviceIoControl(Device, CTL_GetModuleNum, pEprocess, sizeof(DWORD), NULL, 0, &ModuleNum, NULL);
	// ����ռ�
	PVOID ModuleInfo = malloc(sizeof(MODULE_INFO) * ModuleNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryModule, pEprocess, sizeof(DWORD), ModuleInfo, sizeof(MODULE_INFO) * ModuleNum, &size, NULL);
	PMODULE_INFO buffTemp = (PMODULE_INFO)ModuleInfo;
	for (DWORD i = 0; i < ModuleNum; i++)
	{
		CString Info;
		Info.Format(L"%s", buffTemp[i].wcModuleFullPath);
		// ·��
		ModuleList.InsertItem(i, (CString)Info);
		// ģ�����ַ
		Info.Format(L"0x%08X", buffTemp[i].DllBase);
		ModuleList.SetItemText(i, 1, (CString)Info);
		// ��С
		Info.Format(L"0x%08X", buffTemp[i].SizeOfImage);
		ModuleList.SetItemText(i, 2, (CString)Info);
	}
	//�ͷſռ�
	free(ModuleInfo);
}


// ��ʼ���ļ��б�
VOID CDriverTool::InitFileList(CListCtrl& FileList)
{
	/*
	 ���ý����б���
	*/
	DWORD oldstyle = FileList.GetExtendedStyle();
	FileList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	FileList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	FileList.InsertColumn(0, L"�ļ�����", 0, 2*Width / 10);
	FileList.InsertColumn(1, L"�ļ���", 0, 3*Width / 10);
	FileList.InsertColumn(2, L"����ʱ��", 0, 2*Width / 10);
	FileList.InsertColumn(3, L"�޸�ʱ��", 0, 2*Width / 10);
}
// �����ļ�
VOID CDriverTool::UserQueryFile(HANDLE Device, CListCtrl& FileList, PWCHAR FilePath,ULONG FileLength)
{
	// ��ȡ����h
	DWORD FileNum = 0;
	DeviceIoControl(Device, CTL_GetFileNum, FilePath, FileLength, NULL, 0, &FileNum, NULL);
	// ����ռ�
	PVOID FileInfo = malloc(sizeof(FILE_INFO) * FileNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryFile, FilePath, FileLength, FileInfo, sizeof(FILE_INFO) * FileNum, &size, NULL);
	PFILE_INFO buffTemp = (PFILE_INFO)FileInfo;
	for (DWORD i = 0; i < FileNum; i++)
	{
		CString Info;
		if(buffTemp[i].FileOrDirectory==0)
			Info.Format(L"[Ŀ¼]");
		else
			Info.Format(L"[�ļ�]");
		// ����
		FileList.InsertItem(i, (CString)Info);
		// �ļ�·��
		Info.Format(L"%s", buffTemp[i].wcFileName);
		FileList.SetItemText(i, 1, (CString)Info);
		// �ļ�����ʱ��
		FILETIME ft = { 0 };
		SYSTEMTIME st = { 0 };
		FileTimeToLocalFileTime((PFILETIME)&buffTemp[i].CreateTime, &ft);
		FileTimeToSystemTime(&ft, &st);
		Info.Format(L"%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		FileList.SetItemText(i, 2, Info);
		// �ļ��޸�ʱ��
		FileTimeToLocalFileTime((PFILETIME)&buffTemp[i].ChangeTime, &ft);
		FileTimeToSystemTime(&ft, &st);
		Info.Format(L"%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		FileList.SetItemText(i, 3, Info);
	}
	//�ͷſռ�
	free(FileInfo);
}
// ɾ���ļ�
VOID CDriverTool::UserDelFile(HANDLE Device, PWCHAR pFilePath, ULONG uFileDirLength)
{
	DWORD size = 0;

	DeviceIoControl(
		Device,		//�򿪵��豸���
		CTL_DelFile,	//������
		pFilePath,		//���뻺����
		uFileDirLength,	//���뻺������С
		NULL,		//���������
		0,			//����������Ĵ�С
		&size,		//ʵ�ʴ�����ֽ���
		NULL		//�Ƿ���OVERLAPPED����
	);
}


// ��ʼ��ע����б�
VOID CDriverTool::InitRegList(CListCtrl& RegList)
{
	/*
	 ���ý����б���
	*/
	DWORD oldstyle = RegList.GetExtendedStyle();
	RegList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	RegList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	RegList.InsertColumn(0, L"����", 0, 4 * Width / 10);
	RegList.InsertColumn(1, L"����", 0, 3 * Width / 10);
	RegList.InsertColumn(2, L"ע�������", 0, 3 * Width / 10);
}
// ����ע���
VOID CDriverTool::UserQueryReg(HANDLE Device, CTreeCtrl& RegTree,PWCHAR pRegKeyName,ULONG RegKeyNameLength, HTREEITEM hItem)
{
	// ��ȡ����
	DWORD RegNum = 0;
	DeviceIoControl(Device, CTL_GetRegNum, pRegKeyName, RegKeyNameLength, NULL, 0, &RegNum, NULL);
	// ����ռ�
	PVOID RegInfo = malloc(sizeof(REG_INFO) * RegNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryReg, pRegKeyName, RegKeyNameLength, RegInfo, sizeof(REG_INFO) * RegNum, &size, NULL);
	PREG_INFO buffTemp = (PREG_INFO)RegInfo;
	
	//������Ϣ��Tree�ؼ�
	for (DWORD i = 0; i < RegNum; i++)
	{
		//����
		if (buffTemp[i].Type == 0)
		{
			CString KeyName = buffTemp[i].KeyName;
			CString Path;
			if (KeyName == L"MACHINE")
			{
				Path = L"\\Registry\\Machine";
				KeyName.Format(L"%s", L"HKEY_LOCAL_MACHINE");
			}
			else if (KeyName == L"USER")
			{
				Path = L"\\Registry\\user";
				KeyName.Format(L"%s", L"HKEY_USERS");
			}
			//������ϵͳ�̷����õ�����
			HTREEITEM hItem2 = RegTree.InsertItem(KeyName, hItem);
			wchar_t* pBuff = _wcsdup(Path.GetBuffer());
			RegTree.SetItemData(hItem2, (DWORD_PTR)pBuff);
		}
	}
	//�ͷſռ�
	free(RegInfo);
}
// ����ע���2
VOID CDriverTool::UserQueryChildReg(HANDLE Device, CTreeCtrl& RegTree, PWCHAR pRegKeyName, ULONG RegKeyNameLength, HTREEITEM hItem,CString Path, CListCtrl& RegList)
{
	// ��ȡ����
	DWORD RegNum = 0;
	DeviceIoControl(Device, CTL_GetRegNum, pRegKeyName, RegKeyNameLength, NULL, 0, &RegNum, NULL);
	if (RegNum == 0)
	{
		MessageBox(0, L"��ȡ����Ϊ0", L"��ȡ����Ϊ0", 0);
		return;
	}
	// ����ռ�
	PVOID RegInfo = malloc(sizeof(REG_INFO) * RegNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryReg, pRegKeyName, RegKeyNameLength, RegInfo, sizeof(REG_INFO) * RegNum, &size, NULL);
	PREG_INFO buffTemp = (PREG_INFO)RegInfo;


	int nIndex = 0;
	//������Ϣ��Tree�ؼ�
	for (DWORD i = 0; i < RegNum; i++)
	{
		//�����û���ӽڵ�
		if (buffTemp[i].Type == 0)
		{
			//������ϵͳ�̷����õ�����
			HTREEITEM hItem2 = RegTree.InsertItem(buffTemp[i].KeyName, hItem);
			//ƴ��·��
			WCHAR szFullPath[MAX_PATH] = { 0 };
			StringCbPrintf(szFullPath, MAX_PATH, L"%s\\%s", Path, buffTemp[i].KeyName);
			wchar_t* pBuff = _wcsdup(szFullPath);
			RegTree.SetItemData(hItem2, (DWORD_PTR)pBuff);
		}
		else if (buffTemp[i].Type == 1)
		{
			CString buffer;
			CString buffer2;

			RegList.InsertItem(nIndex, _T(""));

			//ֵ��
			buffer = buffTemp[i].ValueName;
			if (buffer == L"")
			{
				buffer = L"Ĭ��";
			}
			RegList.SetItemText(nIndex, 0, buffer);

			switch (buffTemp[i].ValueType)
			{
				//��ʾ��NULL��β���ַ���
			case REG_SZ:
			{
				buffer = L"REG_SZ";
				buffer2.Format(L"%s", buffTemp[i].Value);
				break;
			}
			//��ʾ����������
			case REG_BINARY:
			{
				buffer = L"REG_BINARY";
				buffer2 = L"";
				CString temp;
				for (ULONG j = 0; j < buffTemp[i].ValueLength; ++j)
				{
					temp.Format(L"%02X", (unsigned char)buffTemp[i].Value[j]);
					buffer2 += temp;
					buffer2 += L" ";
				}
				break;
			}
			case REG_DWORD:
			{
				buffer = L"REG_DWORD";
				buffer2.Format(L"0x%08x(%d)", *(DWORD*)buffTemp[i].Value, *(DWORD*)buffTemp[i].Value);
				break;
			}
			case REG_MULTI_SZ:
			{
				buffer = L"REG_MULTI_SZ";
				buffer2.Format(L"%s", buffTemp[i].Value);
				break;
			}
			case REG_EXPAND_SZ:
			{
				buffer = L"REG_EXPAND_SZ";
				buffer2.Format(L"%s", buffTemp[i].Value);
				break;
			}
			default:
				buffer.Format(L"%d", buffTemp[i].ValueType);
				buffer2 = L"";
				CString temp;
				for (int j = 0; j < buffTemp[i].ValueLength; ++j)
				{
					temp.Format(L"%02X", (unsigned char)buffTemp[i].Value[j]);
					buffer2 += temp;
					buffer2 += L" ";
				}
				break;
			}
			RegList.SetItemText(nIndex, 1, buffer);	// ��
			RegList.SetItemText(nIndex, 2, buffer2);	// ����
			nIndex++;
		}
	}

	//�ͷſռ�
	free(RegInfo);
}
// ���ע�������
VOID CDriverTool::UserCreateReg(HANDLE Device, PWCHAR pRegKeyName, ULONG uRegKeyNameLength)
{
	DWORD size = 0;
	DeviceIoControl(
		Device,		//�򿪵��豸���
		CTL_CreateReg,	//������
		pRegKeyName,		//���뻺����
		uRegKeyNameLength,			//���뻺������С
		NULL,		//���������
		0,			//����������Ĵ�С
		&size,		//ʵ�ʴ�����ֽ���
		NULL		//�Ƿ���OVERLAPPED����
	);
}
// ɾ��ע�������
VOID CDriverTool::UserDelReg(HANDLE Device, PWCHAR pRegKeyName, ULONG uRegKeyNameLength)
{
	DWORD size = 0;
	DeviceIoControl(
		Device,		//�򿪵��豸���
		CTL_DelReg,	//������
		pRegKeyName,		//���뻺����
		uRegKeyNameLength,			//���뻺������С
		NULL,		//���������
		0,			//����������Ĵ�С
		&size,		//ʵ�ʴ�����ֽ���
		NULL		//�Ƿ���OVERLAPPED����
	);
}


// ��ʼ��IDT��
VOID CDriverTool::InitIDTList(CListCtrl& IDTList)
{
	/*
	 ���ý����б���
	*/
	DWORD oldstyle = IDTList.GetExtendedStyle();
	IDTList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	IDTList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	IDTList.InsertColumn(0, L"�жϵ�ַ", 0, 4 * Width / 10);
	IDTList.InsertColumn(1, L"��ѡ����", 0, 2 * Width / 10);
	IDTList.InsertColumn(2, L"����", 0, 2 * Width / 10);
	IDTList.InsertColumn(3, L"��Ȩ�ȼ�", 0, 2 * Width / 10);
}
// ����IDT��
VOID CDriverTool::UserQueryIDT(HANDLE Device, CListCtrl& IDTList)
{
	PVOID IDTInfo = malloc(sizeof(IDT_INFO) * 0x100);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryIDT, 0, 0, IDTInfo, sizeof(IDT_INFO) * 0x100, &size, NULL);
	PIDT_INFO buffTemp = (PIDT_INFO)IDTInfo;
	for (int i = 0; i < 0x100; i++)
	{
		CStringA Info;
		// �жϵ�ַ
		Info.Format("%08X", buffTemp[i].pFunction);
		IDTList.InsertItem(i, (CString)Info);
		// ��ѡ����
		Info.Format("%d", buffTemp[i].Selector);
		IDTList.SetItemText(i, 1,(CString)Info);
		// ����
		Info.Format("%d", buffTemp[i].GateType);
		IDTList.SetItemText(i, 2, (CString)Info);
		// ��Ȩ�ȼ�
		Info.Format("%d", buffTemp[i].Dpl);
		IDTList.SetItemText(i, 3, (CString)Info);
	}
	//�ͷſռ�
	free(IDTInfo);
}
// ��ʼ��GDT��
VOID CDriverTool::InitGDTList(CListCtrl& GDTList)
{
	/*
	 ���ý����б���
	*/
	DWORD oldstyle = GDTList.GetExtendedStyle();
	GDTList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	GDTList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	GDTList.InsertColumn(0, L"��ַ", 0, 4 * Width / 10);
	GDTList.InsertColumn(1, L"������", 0, 2 * Width / 10);
	GDTList.InsertColumn(2, L"����", 0, 2 * Width / 10);
	GDTList.InsertColumn(3, L"��Ȩ�ȼ�", 0, 2 * Width / 10);
}
// ����GDT��
VOID CDriverTool::UserQueryGDT(HANDLE Device, CListCtrl& GDTList)
{
	// ��ȡ����h
	DWORD GDTNum = 0;
	DeviceIoControl(Device, CTL_GetGDTNum, 0, NULL, NULL, 0, &GDTNum, NULL);


	PVOID GDTInfo = malloc(sizeof(GDT_INFO) * GDTNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryGDT, 0, 0, GDTInfo, sizeof(GDT_INFO) * GDTNum, &size, NULL);
	PGDT_INFO buffTemp = (PGDT_INFO)GDTInfo;
	for (int i = 0; i < GDTNum; i++)
	{
		CStringA Info;
		// ����ַ
		Info.Format("%08X", buffTemp[i].BaseAddr);
		GDTList.InsertItem(i, (CString)Info);
		// ������
		Info.Format("%d", buffTemp[i].Grain);
		GDTList.SetItemText(i, 1, (CString)Info);
		// ����
		Info.Format("%d", buffTemp[i].GateType);
		GDTList.SetItemText(i, 2, (CString)Info);
		// ��Ȩ�ȼ�
		Info.Format("%d", buffTemp[i].Dpl);
		GDTList.SetItemText(i, 3, (CString)Info);
	}
	//�ͷſռ�
	free(GDTInfo);
}
// ��ʼ��SSDT��
VOID CDriverTool::InitSSDTList(CListCtrl& SSDTList)
{

	DWORD oldstyle = SSDTList.GetExtendedStyle();
	SSDTList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	SSDTList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 �����б�ͷ
	*/
	SSDTList.InsertColumn(0, L"���", 0, Width / 5);
	SSDTList.InsertColumn(1, L"�Ϻ�����ַ", 0, 2 * Width / 5);
	SSDTList.InsertColumn(2, L"�º�����ַ", 0, 2 * Width / 5);
}
// ����SSDT��
VOID CDriverTool::UserQuerySSDT(HANDLE Device, CListCtrl& SSDTList)
{
	// ��ȡ����h
	DWORD SSDTNum = 0;
	DeviceIoControl(Device, CTL_GetSSDTNum, 0, NULL, NULL, 0, &SSDTNum, NULL);

	PVOID SSDTInfo = malloc(sizeof(SSDT_INFO) * SSDTNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QuerySSDT, 0, 0, SSDTInfo, sizeof(SSDT_INFO) * SSDTNum, &size, NULL);
	PSSDT_INFO buffTemp = (PSSDT_INFO)SSDTInfo;
	for (int i = 0; i < SSDTNum; i++)
	{
		CStringA Info;
		// ���
		Info.Format("0x%08X", buffTemp[i].uIndex);
		SSDTList.InsertItem(i, (CString)Info);
		// ��ַ
		Info.Format("0x%08x", buffTemp[i].uFuntionAddr);
		SSDTList.SetItemText(i, 1, (CString)Info);
	}
	//�ͷſռ�
	free(SSDTInfo);
}

// SYSENTER Hook,����������򲻱�����
VOID CDriverTool::SYSENTER_HOOK(HANDLE Device)
{
	// ��ȡ��ǰ����ID
	ULONG PID=_getpid();
	DWORD size = 0;
	DeviceIoControl(Device, CTL_ProtectDriver,&PID, sizeof(ULONG), NULL, 0, &size, NULL);
}
// ������
VOID CDriverTool::FanTiao(HANDLE Device)
{
	// ��ȡ��ǰ����ID
	ULONG PID = _getpid();
	DWORD size = 0;
	DeviceIoControl(Device, CTL_FanTiao, &PID, sizeof(ULONG), NULL, 0, &size, NULL);
}

// ����HOOK����ָ���ļ��޷�����
VOID CDriverTool::StopFile(HANDLE Device, PWCHAR StopFileName, ULONG StopFileNameLength)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_StopFile, StopFileName, StopFileNameLength, NULL, 0, &size, NULL);
}
// ����HOOK����ָ�������޷�����
VOID CDriverTool::StopProcess(HANDLE Device, PWCHAR StopProcessName, ULONG StopProcessNameLength)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_StopProcess, StopProcessName, StopProcessNameLength, NULL, 0, &size, NULL);
}
// �ر�HOOK
VOID CDriverTool::CloseHook(HANDLE Device)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_CloseHook, 0, 0, NULL, 0, &size, NULL);
}

// �����ں�����
VOID CDriverTool::OpenNeiHe(HANDLE Device, CListCtrl& SSDTList)
{
	DWORD size = 0;
	// ��ȡ����h
	DWORD SSDTNum = 0;
	DeviceIoControl(Device, CTL_GetSSDTNum, 0, NULL, NULL, 0, &SSDTNum, NULL);
	PVOID NewSSDTInfo = malloc(sizeof(ULONG) * SSDTNum);
	DeviceIoControl(Device, CTL_NeiHeChongZai, 0, 0, NewSSDTInfo, sizeof(ULONG) * SSDTNum, &size, NULL);
	PULONG buffTemp = (PULONG)NewSSDTInfo;
	for (int i = 0; i < SSDTNum; i++)
	{
		CStringA Info;
		// ��ַ
		Info.Format("0x%08x", buffTemp[i]);
		SSDTList.SetItemText(i, 2, (CString)Info);
	}
	//�ͷſռ�
	free(NewSSDTInfo);
}
// �ر��ں�����
VOID CDriverTool::CloseNeiHe(HANDLE Device)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_CloseNeiHeChongZai, 0, 0, NULL, 0, &size, NULL);
}