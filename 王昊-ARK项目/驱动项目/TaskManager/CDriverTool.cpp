#include "pch.h"
#include "CDriverTool.h"

// 加载驱动，建立0环和3环之间的联系【没写】
VOID CDriverTool::ConnectDriver(HANDLE &hDevice)
{
	hDevice = CreateFile(L"\\\\.\\king", GENERIC_ALL, NULL,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice == NULL)
	{
		MessageBox(0,L"打开设备失败", 0, 0);
		return ;
	}
	return;
}


// 初始化驱动列表
VOID CDriverTool::InitDriverList(CListCtrl& DriverList)
{
	/*
	 设置进程列表风格
	*/
	DWORD oldstyle = DriverList.GetExtendedStyle();
	DriverList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	DriverList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	DriverList.InsertColumn(0, L"驱动名", 0, Width/2);
	DriverList.InsertColumn(1, L"驱动路径", 0, Width/2);
}
// 遍历驱动，并保存值列表
VOID CDriverTool::UserQueryDriver(HANDLE Device,CListCtrl& DriverList)
{
	DWORD DriverNum = 0;
	DeviceIoControl(Device, CTL_GetDriverNum, 0, sizeof(DWORD), NULL, 0, &DriverNum, NULL);
	PVOID DriverInfo = malloc(sizeof(DRIVER_INFO) * DriverNum);
	DWORD size = 0;
	DeviceIoControl(
		Device,		//打开的设备句柄
		CTL_QueryDriver,	//控制码
		NULL,		//输入缓冲区
		0,			//输入缓冲区大小
		DriverInfo,		//输出缓冲区
		sizeof(DRIVER_INFO) * DriverNum,	//输出缓冲区的大小
		&size,		//实际传输的字节数
		NULL		//是否是OVERLAPPED操作
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
		//驱动名
		DriverList.InsertItem(j, buffTemp[i].driverName);
		// 驱动路径
		DriverList.SetItemText(j, 1, buffTemp[i].driverPath);
	}
	//释放空间
	free(DriverInfo);
}
// 隐藏驱动
VOID CDriverTool::UserHideDriver(HANDLE Device, PWCHAR DriverPath)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_HideDriver, DriverPath, MAX_PATH, NULL, 0, &size, NULL);
}


// 初始化进程列表
VOID CDriverTool::InitProcessList(CListCtrl& ProcessList)
{
	/*
	 设置进程列表风格
	*/
	DWORD oldstyle = ProcessList.GetExtendedStyle();
	ProcessList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	ProcessList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	ProcessList.InsertColumn(0, L"进程ID", 0, Width/10);
	ProcessList.InsertColumn(1, L"进程名称", 0, 7*Width/10);
	ProcessList.InsertColumn(2, L"EPROCESS", 0, 2*Width/10);
}
// 遍历进程
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
			// 进程id
			ProcessList.InsertItem(j, (CString)Info);
			//进程名
			CString Info1;
			Info1.Format(L"%s", buffTemp[i].ProcessName);
			ProcessList.SetItemText(j, 1, (CString)Info1);
			// 进程块
			Info.Format("0x%08X", buffTemp[i].EProcess);
			ProcessList.SetItemText(j, 2, (CString)Info);
			j++;
		}
	}
	//释放空间
	free(ProcessInfo);
}
// 隐藏进程
VOID CDriverTool::HideProcess(HANDLE Device,PULONG Pid)
{
	DWORD size = 0;

	DeviceIoControl(
		Device,		//打开的设备句柄
		CTL_HideProcess,	//控制码
		Pid,		//输入缓冲区
		sizeof(ULONG),			//输入缓冲区大小
		NULL,		//输出缓冲区
		0,			//输出缓冲区的大小
		&size,		//实际传输的字节数
		NULL		//是否是OVERLAPPED操作
	);
}
// 结束进程
VOID CDriverTool::DelProcess(HANDLE Device, PULONG Pid)
{
	DWORD size = 0;

	DeviceIoControl(
		Device,		//打开的设备句柄
		CTL_DelProcess,	//控制码
		Pid,		//输入缓冲区
		sizeof(ULONG),			//输入缓冲区大小
		NULL,		//输出缓冲区
		0,			//输出缓冲区的大小
		&size,		//实际传输的字节数
		NULL		//是否是OVERLAPPED操作
	);
}
// 初始化线程列表
VOID CDriverTool::InitThreadList(CListCtrl& ThreadList)
{
	/*
	 设置进程列表风格
	*/
	DWORD oldstyle = ThreadList.GetExtendedStyle();
	ThreadList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	ThreadList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	ThreadList.InsertColumn(0, L"线程ID", 0, Width / 2);
	ThreadList.InsertColumn(1, L"线程优先级", 0, Width / 2);
}
// 遍历线程
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
		// 线程id
		ThreadList.InsertItem(i, (CString)Info);
		//优先级
		//Info.Format("%d", buffTemp[i].BasePriority);
		//ThreadList.SetItemText(i, 1, (CString)Info);
	}
	//释放空间
	free(ThreadInfo);
}
// 初始化模块列表
VOID CDriverTool::InitModuleLst(CListCtrl& ModuleList)
{
	/*
	 设置进程列表风格
	*/
	DWORD oldstyle = ModuleList.GetExtendedStyle();
	ModuleList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	ModuleList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	ModuleList.InsertColumn(0, L"模块路径", 0, Width / 3);
	ModuleList.InsertColumn(1, L"模块加载基址", 0, Width / 3);
	ModuleList.InsertColumn(2, L"大小", 0, Width / 3);
}
// 遍历模块
VOID CDriverTool::UserQueryModule(HANDLE Device, CListCtrl& ModuleList, PULONG pEprocess)
{
	// 获取数量
	DWORD ModuleNum = 0;
	DeviceIoControl(Device, CTL_GetModuleNum, pEprocess, sizeof(DWORD), NULL, 0, &ModuleNum, NULL);
	// 申请空间
	PVOID ModuleInfo = malloc(sizeof(MODULE_INFO) * ModuleNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryModule, pEprocess, sizeof(DWORD), ModuleInfo, sizeof(MODULE_INFO) * ModuleNum, &size, NULL);
	PMODULE_INFO buffTemp = (PMODULE_INFO)ModuleInfo;
	for (DWORD i = 0; i < ModuleNum; i++)
	{
		CString Info;
		Info.Format(L"%s", buffTemp[i].wcModuleFullPath);
		// 路径
		ModuleList.InsertItem(i, (CString)Info);
		// 模块基地址
		Info.Format(L"0x%08X", buffTemp[i].DllBase);
		ModuleList.SetItemText(i, 1, (CString)Info);
		// 大小
		Info.Format(L"0x%08X", buffTemp[i].SizeOfImage);
		ModuleList.SetItemText(i, 2, (CString)Info);
	}
	//释放空间
	free(ModuleInfo);
}


// 初始化文件列表
VOID CDriverTool::InitFileList(CListCtrl& FileList)
{
	/*
	 设置进程列表风格
	*/
	DWORD oldstyle = FileList.GetExtendedStyle();
	FileList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	FileList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	FileList.InsertColumn(0, L"文件属性", 0, 2*Width / 10);
	FileList.InsertColumn(1, L"文件名", 0, 3*Width / 10);
	FileList.InsertColumn(2, L"创建时间", 0, 2*Width / 10);
	FileList.InsertColumn(3, L"修改时间", 0, 2*Width / 10);
}
// 遍历文件
VOID CDriverTool::UserQueryFile(HANDLE Device, CListCtrl& FileList, PWCHAR FilePath,ULONG FileLength)
{
	// 获取数量h
	DWORD FileNum = 0;
	DeviceIoControl(Device, CTL_GetFileNum, FilePath, FileLength, NULL, 0, &FileNum, NULL);
	// 申请空间
	PVOID FileInfo = malloc(sizeof(FILE_INFO) * FileNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryFile, FilePath, FileLength, FileInfo, sizeof(FILE_INFO) * FileNum, &size, NULL);
	PFILE_INFO buffTemp = (PFILE_INFO)FileInfo;
	for (DWORD i = 0; i < FileNum; i++)
	{
		CString Info;
		if(buffTemp[i].FileOrDirectory==0)
			Info.Format(L"[目录]");
		else
			Info.Format(L"[文件]");
		// 属性
		FileList.InsertItem(i, (CString)Info);
		// 文件路径
		Info.Format(L"%s", buffTemp[i].wcFileName);
		FileList.SetItemText(i, 1, (CString)Info);
		// 文件创建时间
		FILETIME ft = { 0 };
		SYSTEMTIME st = { 0 };
		FileTimeToLocalFileTime((PFILETIME)&buffTemp[i].CreateTime, &ft);
		FileTimeToSystemTime(&ft, &st);
		Info.Format(L"%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		FileList.SetItemText(i, 2, Info);
		// 文件修改时间
		FileTimeToLocalFileTime((PFILETIME)&buffTemp[i].ChangeTime, &ft);
		FileTimeToSystemTime(&ft, &st);
		Info.Format(L"%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		FileList.SetItemText(i, 3, Info);
	}
	//释放空间
	free(FileInfo);
}
// 删除文件
VOID CDriverTool::UserDelFile(HANDLE Device, PWCHAR pFilePath, ULONG uFileDirLength)
{
	DWORD size = 0;

	DeviceIoControl(
		Device,		//打开的设备句柄
		CTL_DelFile,	//控制码
		pFilePath,		//输入缓冲区
		uFileDirLength,	//输入缓冲区大小
		NULL,		//输出缓冲区
		0,			//输出缓冲区的大小
		&size,		//实际传输的字节数
		NULL		//是否是OVERLAPPED操作
	);
}


// 初始化注册表列表
VOID CDriverTool::InitRegList(CListCtrl& RegList)
{
	/*
	 设置进程列表风格
	*/
	DWORD oldstyle = RegList.GetExtendedStyle();
	RegList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	RegList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	RegList.InsertColumn(0, L"名称", 0, 4 * Width / 10);
	RegList.InsertColumn(1, L"类型", 0, 3 * Width / 10);
	RegList.InsertColumn(2, L"注册表数据", 0, 3 * Width / 10);
}
// 遍历注册表
VOID CDriverTool::UserQueryReg(HANDLE Device, CTreeCtrl& RegTree,PWCHAR pRegKeyName,ULONG RegKeyNameLength, HTREEITEM hItem)
{
	// 获取数量
	DWORD RegNum = 0;
	DeviceIoControl(Device, CTL_GetRegNum, pRegKeyName, RegKeyNameLength, NULL, 0, &RegNum, NULL);
	// 申请空间
	PVOID RegInfo = malloc(sizeof(REG_INFO) * RegNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryReg, pRegKeyName, RegKeyNameLength, RegInfo, sizeof(REG_INFO) * RegNum, &size, NULL);
	PREG_INFO buffTemp = (PREG_INFO)RegInfo;
	
	//插入信息到Tree控件
	for (DWORD i = 0; i < RegNum; i++)
	{
		//子项
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
			//将所有系统盘符设置到树中
			HTREEITEM hItem2 = RegTree.InsertItem(KeyName, hItem);
			wchar_t* pBuff = _wcsdup(Path.GetBuffer());
			RegTree.SetItemData(hItem2, (DWORD_PTR)pBuff);
		}
	}
	//释放空间
	free(RegInfo);
}
// 遍历注册表2
VOID CDriverTool::UserQueryChildReg(HANDLE Device, CTreeCtrl& RegTree, PWCHAR pRegKeyName, ULONG RegKeyNameLength, HTREEITEM hItem,CString Path, CListCtrl& RegList)
{
	// 获取数量
	DWORD RegNum = 0;
	DeviceIoControl(Device, CTL_GetRegNum, pRegKeyName, RegKeyNameLength, NULL, 0, &RegNum, NULL);
	if (RegNum == 0)
	{
		MessageBox(0, L"获取数量为0", L"获取数量为0", 0);
		return;
	}
	// 申请空间
	PVOID RegInfo = malloc(sizeof(REG_INFO) * RegNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryReg, pRegKeyName, RegKeyNameLength, RegInfo, sizeof(REG_INFO) * RegNum, &size, NULL);
	PREG_INFO buffTemp = (PREG_INFO)RegInfo;


	int nIndex = 0;
	//插入信息到Tree控件
	for (DWORD i = 0; i < RegNum; i++)
	{
		//子项并且没有子节点
		if (buffTemp[i].Type == 0)
		{
			//将所有系统盘符设置到树中
			HTREEITEM hItem2 = RegTree.InsertItem(buffTemp[i].KeyName, hItem);
			//拼接路径
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

			//值名
			buffer = buffTemp[i].ValueName;
			if (buffer == L"")
			{
				buffer = L"默认";
			}
			RegList.SetItemText(nIndex, 0, buffer);

			switch (buffTemp[i].ValueType)
			{
				//表示以NULL结尾的字符串
			case REG_SZ:
			{
				buffer = L"REG_SZ";
				buffer2.Format(L"%s", buffTemp[i].Value);
				break;
			}
			//表示二进制数据
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
			RegList.SetItemText(nIndex, 1, buffer);	// 文
			RegList.SetItemText(nIndex, 2, buffer2);	// 数据
			nIndex++;
		}
	}

	//释放空间
	free(RegInfo);
}
// 添加注册表子项
VOID CDriverTool::UserCreateReg(HANDLE Device, PWCHAR pRegKeyName, ULONG uRegKeyNameLength)
{
	DWORD size = 0;
	DeviceIoControl(
		Device,		//打开的设备句柄
		CTL_CreateReg,	//控制码
		pRegKeyName,		//输入缓冲区
		uRegKeyNameLength,			//输入缓冲区大小
		NULL,		//输出缓冲区
		0,			//输出缓冲区的大小
		&size,		//实际传输的字节数
		NULL		//是否是OVERLAPPED操作
	);
}
// 删除注册表子项
VOID CDriverTool::UserDelReg(HANDLE Device, PWCHAR pRegKeyName, ULONG uRegKeyNameLength)
{
	DWORD size = 0;
	DeviceIoControl(
		Device,		//打开的设备句柄
		CTL_DelReg,	//控制码
		pRegKeyName,		//输入缓冲区
		uRegKeyNameLength,			//输入缓冲区大小
		NULL,		//输出缓冲区
		0,			//输出缓冲区的大小
		&size,		//实际传输的字节数
		NULL		//是否是OVERLAPPED操作
	);
}


// 初始化IDT表
VOID CDriverTool::InitIDTList(CListCtrl& IDTList)
{
	/*
	 设置进程列表风格
	*/
	DWORD oldstyle = IDTList.GetExtendedStyle();
	IDTList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	IDTList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	IDTList.InsertColumn(0, L"中断地址", 0, 4 * Width / 10);
	IDTList.InsertColumn(1, L"段选择子", 0, 2 * Width / 10);
	IDTList.InsertColumn(2, L"类型", 0, 2 * Width / 10);
	IDTList.InsertColumn(3, L"特权等级", 0, 2 * Width / 10);
}
// 遍历IDT表
VOID CDriverTool::UserQueryIDT(HANDLE Device, CListCtrl& IDTList)
{
	PVOID IDTInfo = malloc(sizeof(IDT_INFO) * 0x100);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryIDT, 0, 0, IDTInfo, sizeof(IDT_INFO) * 0x100, &size, NULL);
	PIDT_INFO buffTemp = (PIDT_INFO)IDTInfo;
	for (int i = 0; i < 0x100; i++)
	{
		CStringA Info;
		// 中断地址
		Info.Format("%08X", buffTemp[i].pFunction);
		IDTList.InsertItem(i, (CString)Info);
		// 段选择子
		Info.Format("%d", buffTemp[i].Selector);
		IDTList.SetItemText(i, 1,(CString)Info);
		// 类型
		Info.Format("%d", buffTemp[i].GateType);
		IDTList.SetItemText(i, 2, (CString)Info);
		// 特权等级
		Info.Format("%d", buffTemp[i].Dpl);
		IDTList.SetItemText(i, 3, (CString)Info);
	}
	//释放空间
	free(IDTInfo);
}
// 初始化GDT表
VOID CDriverTool::InitGDTList(CListCtrl& GDTList)
{
	/*
	 设置进程列表风格
	*/
	DWORD oldstyle = GDTList.GetExtendedStyle();
	GDTList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	GDTList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	GDTList.InsertColumn(0, L"地址", 0, 4 * Width / 10);
	GDTList.InsertColumn(1, L"段粒度", 0, 2 * Width / 10);
	GDTList.InsertColumn(2, L"类型", 0, 2 * Width / 10);
	GDTList.InsertColumn(3, L"特权等级", 0, 2 * Width / 10);
}
// 遍历GDT表
VOID CDriverTool::UserQueryGDT(HANDLE Device, CListCtrl& GDTList)
{
	// 获取数量h
	DWORD GDTNum = 0;
	DeviceIoControl(Device, CTL_GetGDTNum, 0, NULL, NULL, 0, &GDTNum, NULL);


	PVOID GDTInfo = malloc(sizeof(GDT_INFO) * GDTNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QueryGDT, 0, 0, GDTInfo, sizeof(GDT_INFO) * GDTNum, &size, NULL);
	PGDT_INFO buffTemp = (PGDT_INFO)GDTInfo;
	for (int i = 0; i < GDTNum; i++)
	{
		CStringA Info;
		// 基地址
		Info.Format("%08X", buffTemp[i].BaseAddr);
		GDTList.InsertItem(i, (CString)Info);
		// 段粒度
		Info.Format("%d", buffTemp[i].Grain);
		GDTList.SetItemText(i, 1, (CString)Info);
		// 类型
		Info.Format("%d", buffTemp[i].GateType);
		GDTList.SetItemText(i, 2, (CString)Info);
		// 特权等级
		Info.Format("%d", buffTemp[i].Dpl);
		GDTList.SetItemText(i, 3, (CString)Info);
	}
	//释放空间
	free(GDTInfo);
}
// 初始化SSDT表
VOID CDriverTool::InitSSDTList(CListCtrl& SSDTList)
{

	DWORD oldstyle = SSDTList.GetExtendedStyle();
	SSDTList.SetExtendedStyle(oldstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rc;
	SSDTList.GetClientRect(rc);
	int Width = rc.Width();
	/*
	 插入列表头
	*/
	SSDTList.InsertColumn(0, L"序号", 0, Width / 5);
	SSDTList.InsertColumn(1, L"老函数地址", 0, 2 * Width / 5);
	SSDTList.InsertColumn(2, L"新函数地址", 0, 2 * Width / 5);
}
// 遍历SSDT表
VOID CDriverTool::UserQuerySSDT(HANDLE Device, CListCtrl& SSDTList)
{
	// 获取数量h
	DWORD SSDTNum = 0;
	DeviceIoControl(Device, CTL_GetSSDTNum, 0, NULL, NULL, 0, &SSDTNum, NULL);

	PVOID SSDTInfo = malloc(sizeof(SSDT_INFO) * SSDTNum);
	DWORD size = 0;
	DeviceIoControl(Device, CTL_QuerySSDT, 0, 0, SSDTInfo, sizeof(SSDT_INFO) * SSDTNum, &size, NULL);
	PSSDT_INFO buffTemp = (PSSDT_INFO)SSDTInfo;
	for (int i = 0; i < SSDTNum; i++)
	{
		CStringA Info;
		// 序号
		Info.Format("0x%08X", buffTemp[i].uIndex);
		SSDTList.InsertItem(i, (CString)Info);
		// 地址
		Info.Format("0x%08x", buffTemp[i].uFuntionAddr);
		SSDTList.SetItemText(i, 1, (CString)Info);
	}
	//释放空间
	free(SSDTInfo);
}

// SYSENTER Hook,保护自身程序不被调试
VOID CDriverTool::SYSENTER_HOOK(HANDLE Device)
{
	// 获取当前进程ID
	ULONG PID=_getpid();
	DWORD size = 0;
	DeviceIoControl(Device, CTL_ProtectDriver,&PID, sizeof(ULONG), NULL, 0, &size, NULL);
}
// 反调试
VOID CDriverTool::FanTiao(HANDLE Device)
{
	// 获取当前进程ID
	ULONG PID = _getpid();
	DWORD size = 0;
	DeviceIoControl(Device, CTL_FanTiao, &PID, sizeof(ULONG), NULL, 0, &size, NULL);
}

// 对象HOOK，让指定文件无法创建
VOID CDriverTool::StopFile(HANDLE Device, PWCHAR StopFileName, ULONG StopFileNameLength)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_StopFile, StopFileName, StopFileNameLength, NULL, 0, &size, NULL);
}
// 对象HOOK，让指定进程无法创建
VOID CDriverTool::StopProcess(HANDLE Device, PWCHAR StopProcessName, ULONG StopProcessNameLength)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_StopProcess, StopProcessName, StopProcessNameLength, NULL, 0, &size, NULL);
}
// 关闭HOOK
VOID CDriverTool::CloseHook(HANDLE Device)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_CloseHook, 0, 0, NULL, 0, &size, NULL);
}

// 开启内核重载
VOID CDriverTool::OpenNeiHe(HANDLE Device, CListCtrl& SSDTList)
{
	DWORD size = 0;
	// 获取数量h
	DWORD SSDTNum = 0;
	DeviceIoControl(Device, CTL_GetSSDTNum, 0, NULL, NULL, 0, &SSDTNum, NULL);
	PVOID NewSSDTInfo = malloc(sizeof(ULONG) * SSDTNum);
	DeviceIoControl(Device, CTL_NeiHeChongZai, 0, 0, NewSSDTInfo, sizeof(ULONG) * SSDTNum, &size, NULL);
	PULONG buffTemp = (PULONG)NewSSDTInfo;
	for (int i = 0; i < SSDTNum; i++)
	{
		CStringA Info;
		// 地址
		Info.Format("0x%08x", buffTemp[i]);
		SSDTList.SetItemText(i, 2, (CString)Info);
	}
	//释放空间
	free(NewSSDTInfo);
}
// 关闭内核重载
VOID CDriverTool::CloseNeiHe(HANDLE Device)
{
	DWORD size = 0;
	DeviceIoControl(Device, CTL_CloseNeiHeChongZai, 0, 0, NULL, 0, &size, NULL);
}