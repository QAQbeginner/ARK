#include"ARKFunc.h"


PPEB PsGetProcessPeb(PEPROCESS proc)
{
	PPEB res = (PPEB)(*(ULONG*)((ULONG)proc + 0x1a8));
	return res;
}
// 创建设备
NTSTATUS CreateDevice(IN PDRIVER_OBJECT	DriverObject)
{
	// 设备对象的名称，要求这个名字必须符合格式 \\Device\\xxx，表示当前是设备对象
	UNICODE_STRING DeviceName = { 0 };
	RtlInitUnicodeString(&DeviceName, L"\\Device\\king");

	// 创建一个设备对象，并指定设备对象的基本属性
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS Status = IoCreateDevice(
		DriverObject,					// *设备所属的驱动对象，创建成功后会被添加到驱动的 DeviceObject 链表中
		sizeof(DEVICE_EXTENSION),		// 设备扩展区域的大小，则快内存会被 DeviceExtension 指向
		&DeviceName,					// *设备对象的名称，仅内核可见
		FILE_DEVICE_UNKNOWN,			// 表示这是一个硬件无关的设备对象
		0,								// 设备属性暂时不用
		TRUE,							// 表示当前设备对象是否是独占的
		&DeviceObject);					// *返回创建的设备对象

	// 判断上面的设备对象是否创建成功，如果失败立即返回
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("设备对象创建失败\n");
		return Status;
	}

	// 设备名称仅内核可见，想要 R3 操作，必须绑定到一个符号链接名
	UNICODE_STRING SymbolName = { 0 };
	RtlInitUnicodeString(&SymbolName, L"\\??\\king");
	Status = IoCreateSymbolicLink(&SymbolName, &DeviceName);

	// 如果符号链接名出错，设备将无法和 R3 通信，意味着此设备没有意义
	if (!NT_SUCCESS(Status))
	{
		IoDeleteDevice(DeviceObject);
		DbgPrint("符号链接创建失败\n");
		return Status;
	}

	// 将设备对象的基本信息保存到设备对象的扩展内存中
	PDEVICE_EXTENSION Extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	Extension->Self = DeviceObject;
	Extension->SymbolName = SymbolName;
	Extension->DeviceName = DeviceName;

	// 初始化成功以后，清除设备表示正在初始化的标志位
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	// 创建成功后，可以通过设置 Flags 指定读写的方式，缓冲区读写方式常用于对速度
	//	要求不高，对传输长度要求不大的情况。实际在传输内容的过程中，IO管理器会创建
	//	一个新的缓冲区，将内容拷贝到内核层，或拷贝到用户层
	DeviceObject->Flags |= DO_BUFFERED_IO;

	return Status;
}


// 获取驱动数量
NTSTATUS MyGetDriverNumber(PDRIVER_OBJECT DriverObject, PIRP Irp)
{
	// 定义结构体
	PLDR_DATA_TABLE_ENTRY Current = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	PLDR_DATA_TABLE_ENTRY Items = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	UINT32 DriverNum = 0;
	do {
		// 进行遍历
		DriverNum++;
		Items = (PLDR_DATA_TABLE_ENTRY)Items->InLoadOrderLinks.Flink;
	} while (Current != Items);
	Irp->IoStatus.Information = DriverNum;
	return STATUS_SUCCESS;
}
// 遍历驱动
NTSTATUS MyQueryDriver(PDRIVER_OBJECT DriverObject, PIRP Irp, ULONG outputSize)
{
	//获取驱动链
	PLDR_DATA_TABLE_ENTRY pLdr = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	PLIST_ENTRY pTemp = &pLdr->InLoadOrderLinks;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == NULL)
		return STATUS_SUCCESS;
	RtlFillMemory(pOutBuff, outputSize, 0);
	PDRIVER_INFO pOutDriverInfo = (PDRIVER_INFO)pOutBuff;
	do
	{
		PLDR_DATA_TABLE_ENTRY pDriverInfo = (PLDR_DATA_TABLE_ENTRY)pTemp;
		pTemp = pTemp->Blink;
		if (pDriverInfo->DllBase != 0)
		{
			//获取驱动名
			RtlCopyMemory(pOutDriverInfo->driverName, pDriverInfo->BaseDllName.Buffer, pDriverInfo->BaseDllName.Length);
			//获取驱动路径
			RtlCopyMemory(pOutDriverInfo->driverPath, pDriverInfo->FullDllName.Buffer, pDriverInfo->FullDllName.Length);
			//获取驱动基址
			pOutDriverInfo->DllBase = pDriverInfo->DllBase;
			//获取驱动大小
			pOutDriverInfo->SizeOfImage = pDriverInfo->SizeOfImage;
		}
		pOutDriverInfo++;

	} while (pTemp != &pLdr->InLoadOrderLinks);

	return STATUS_SUCCESS;
}
// 隐藏驱动
NTSTATUS MyHideDriver(PDEVICE_OBJECT pDevice,PIRP Irp)
{
	//获取驱动链
	PLDR_DATA_TABLE_ENTRY pLdr = (PLDR_DATA_TABLE_ENTRY)pDevice->DriverObject->DriverSection;
	PLIST_ENTRY pTemp = &pLdr->InLoadOrderLinks;
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//初始化字符串,获取要隐藏的驱动名
	UNICODE_STRING pHideDriverName = { 0 };
	RtlInitUnicodeString(&pHideDriverName, (PWCHAR)pInputBuff);

	do
	{
		PLDR_DATA_TABLE_ENTRY pDriverInfo = (PLDR_DATA_TABLE_ENTRY)pTemp;
		if (RtlCompareUnicodeString(&pDriverInfo->BaseDllName, &pHideDriverName, FALSE) == 0)
		{
			//修改Flink和Blink指针，以跳过我们要隐藏的驱动
			//使要隐藏的驱动的下一个驱动指向的前一个驱动为我们要隐藏驱动的前一个驱动
			pTemp->Blink->Flink = pTemp->Flink;
			//使要隐藏的驱动的前一个驱动指向的下一个驱动为我们要隐藏驱动的下一个驱动
			pTemp->Flink->Blink = pTemp->Blink;
			pTemp->Flink = (PLIST_ENTRY)&pTemp->Flink;
			pTemp->Blink = (PLIST_ENTRY)&pTemp->Blink;

			break;
		}
		pTemp = pTemp->Blink;

	} while (pTemp != &pLdr->InLoadOrderLinks);
	return STATUS_SUCCESS;
}


// 获取进程数量
NTSTATUS MyGetProcessNumber(PIRP Irp)
{
	// 定义进程和线程的结构体指针，用于接收找到的内容
	PEPROCESS Process;
	ULONG nCount = 0;
	//获取当前进程的EPROCESS
	PEPROCESS proc = PsGetCurrentProcess();
	//获取活动进程链
	PLIST_ENTRY pTemp = (PLIST_ENTRY)((ULONG)proc + 0xb8);
	do
	{
		pTemp = pTemp->Blink;
		nCount++;
	} while (pTemp != (PLIST_ENTRY)((ULONG)proc + 0xb8));
	Irp->IoStatus.Information = nCount;
	return STATUS_SUCCESS;
}
// 遍历进程
NTSTATUS MyQueryProcess(PIRP Irp, ULONG outputSize)
{
	// 定义进程和线程的结构体指针，用于接收找到的内容
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	RtlFillMemory(pOutBuff, outputSize, 0);
	PPROCESS_INFO pOutProcessInfo = (PPROCESS_INFO)pOutBuff;
	if (pOutProcessInfo == NULL)
		return STATUS_SUCCESS;
	//获取当前进程的EPROCESS
	PEPROCESS proc = PsGetCurrentProcess();
	//获取活动进程链
	PLIST_ENTRY pTemp = (PLIST_ENTRY)((ULONG)proc + 0xb8);

	//将输出缓冲区全置0
	RtlFillMemory(pOutBuff, outputSize, 0);

	do
	{
		//循环获取下一块进程块
		PEPROCESS pProcessInfo = (PEPROCESS)((ULONG)pTemp - 0xb8);
		pTemp = pTemp->Blink;

		//保存进程路径字符串结构体地址
		PUNICODE_STRING pName = (PUNICODE_STRING)(*(ULONG*)((ULONG)pProcessInfo + 0x1ec));

		//获取PEPROCESS中SectionObject的值
		//当此值为空时说明此进程已经作废
		PVOID SecObj = (PVOID)(*(ULONG*)((ULONG)pProcessInfo + 0x128));
		if (pName != 0 && SecObj != NULL)
		{
			//获取进程ID
			pOutProcessInfo->PID = (PVOID)(*(ULONG*)((ULONG)pProcessInfo + 0xb4));
			//获取EPROCESS
			pOutProcessInfo->EProcess = (PVOID)pProcessInfo;
			//获取进程路径
			RtlCopyMemory(pOutProcessInfo->ProcessName, pName->Buffer, pName->Length);
		}
		pOutProcessInfo++;

	} while (pTemp != (PLIST_ENTRY)((ULONG)proc + 0xb8));
	return STATUS_SUCCESS;
}
// 隐藏进程
NTSTATUS MyHindProcess(PIRP Irp)
{
	//获取当前进程的EPROCESS
	PEPROCESS proc = PsGetCurrentProcess();
	//获取活动进程链
	PLIST_ENTRY pTemp = (PLIST_ENTRY)((ULONG)proc + 0xb8);
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	do
	{
		//循环获取下一块进程块
		PEPROCESS pProcessInfo = (PEPROCESS)((ULONG)pTemp - 0xb8);

		//获取PEPROCESS中SectionObject的值
		//当此值为空时说明此进程已经作废
		PVOID SecObj = (PVOID)(*(ULONG*)((ULONG)pProcessInfo + 0x128));
		if (SecObj != NULL)
		{
			//遍历到的进程id
			ULONG pid = *(ULONG*)((ULONG)pProcessInfo + 0xb4);
			//要隐藏的进程id
			ULONG fid = *(ULONG*)pInputBuff;
			//比较进程id
			if (pid == fid)
			{
				//修改Flink和Blink指针，以跳过我们要隐藏的驱动
				//使要隐藏的驱动的下一个驱动指向的前一个驱动为我们要隐藏驱动的前一个驱动
				pTemp->Blink->Flink = pTemp->Flink;
				//使要隐藏的驱动的前一个驱动指向的下一个驱动为我们要隐藏驱动的下一个驱动
				pTemp->Flink->Blink = pTemp->Blink;
				pTemp->Flink = (PLIST_ENTRY)&pTemp->Flink;
				pTemp->Blink = (PLIST_ENTRY)&pTemp->Blink;
				break;
			}
		}
		pTemp = pTemp->Blink;

	} while (pTemp != (PLIST_ENTRY)((ULONG)proc + 0xb8));
	return STATUS_SUCCESS;
}
// 结束进程
NTSTATUS MyDelProcess(PIRP Irp)
{
	HANDLE hProcess = NULL;
	CLIENT_ID ClientId = { 0 };
	OBJECT_ATTRIBUTES objAttribut = { sizeof(OBJECT_ATTRIBUTES) };
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//要结束的进程的id
	ClientId.UniqueProcess = *(HANDLE*)pInputBuff;
	ClientId.UniqueThread = 0;
	//打开进程，如果句柄有效，则结束进程
	ZwOpenProcess(
		&hProcess,	//返回打开的句柄
		1,			//访问权限
		&objAttribut,	//对象属性
		&ClientId	//进程ID结构
	);
	if (hProcess)
	{
		//结束进程
		ZwTerminateProcess(hProcess, 0);
		//关闭句柄
		ZwClose(hProcess);
	}

	return STATUS_SUCCESS;
}
// 获取线程数量
NTSTATUS MyGetThreadNumber(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//获取要查看模块所在进程的进程块EPROCESS
	PULONG PID = (PULONG)pInputBuff;
	ULONG nCount = 0;
	PETHREAD Thread;

	// 通过暴力枚举可能使用的所有 id，来查找对应的进程或线程
	for (ULONG id = 0; id < 5000; id += 4)
	{
		// 如果不是进程，还有可能是线程，不可能进程和线程的 id 相同
		if (id != 0)
		{
			if (NT_SUCCESS(PsLookupThreadByThreadId(UlongToHandle(id), &Thread)))
			{
				// 通过线程结构体找到所属的进程，并输出基本信息
				PEPROCESS Process2 = IoThreadToProcess(Thread);
				if (PsGetProcessId(Process2) == ULongToHandle(*PID))
					nCount++;
				// PsLookupProcessByProcessId 会增加引用计数，用完要减去
				ObDereferenceObject(Thread);
			}
		}
	}

	Irp->IoStatus.Information = nCount;

	return STATUS_SUCCESS;
}
// 遍历线程
NTSTATUS MyQueryThread(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//获取要查看模块所在进程的进程块EPROCESS
	PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	//将输出缓冲区全置0
	RtlFillMemory(pOutBuff, outputSize, 0);

	PTHREAD_INFO pOutThreadInfo = (PTHREAD_INFO)pOutBuff;
	if (pOutThreadInfo == NULL)
		return STATUS_SUCCESS;
	//获取要查看模块所在进程的进程块EPROCESS
	PULONG PID = (PULONG)pInputBuff;
	ULONG nCount = 0;
	PETHREAD Thread;

	// 通过暴力枚举可能使用的所有 id，来查找对应的进程或线程
	for (ULONG id = 0; id < 5000; id += 4)
	{
		if (id != 0)
		{
			// 如果不是进程，还有可能是线程，不可能进程和线程的 id 相同
			if (NT_SUCCESS(PsLookupThreadByThreadId(UlongToHandle(id), &Thread)))
			{
				// 通过线程结构体找到所属的进程，并输出基本信息
				PEPROCESS Process2 = IoThreadToProcess(Thread);
				if (PsGetProcessId(Process2) == ULongToHandle(*PID))
				{
					pOutThreadInfo->Tid = id;
					pOutThreadInfo++;
				}
				// PsLookupProcessByProcessId 会增加引用计数，用完要减去
				ObDereferenceObject(Thread);
			}
		}
	}

	Irp->IoStatus.Information = nCount;

	return STATUS_SUCCESS;
}
// 获取模块数量
NTSTATUS MyGetModuleNumber(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//获取要查看模块所在进程的进程块EPROCESS
	PEPROCESS proc = (PEPROCESS)(*(ULONG*)pInputBuff);
	ULONG nCount = 0;
	//进程挂靠
	KeAttachProcess(proc);

	//获取PEB地址
	PPEB peb = PsGetProcessPeb(proc);

	//获取模块链表
	PLIST_ENTRY pTemp = (PLIST_ENTRY)(*(ULONG*)((ULONG)peb + 0xc) + 0xc);

	do
	{
		pTemp = pTemp->Flink;
		nCount++;
	} while (pTemp != (PLIST_ENTRY)(*(ULONG*)((ULONG)peb + 0xc) + 0xc));

	Irp->IoStatus.Information = nCount;
	//解除挂靠
	KeDetachProcess();

	return STATUS_SUCCESS;
}
// 遍历模块
NTSTATUS MyQueryModule(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == NULL)
		return STATUS_SUCCESS;
	//获取要查看模块所在进程的进程块EPROCESS
	PEPROCESS proc = (PEPROCESS)(*(ULONG*)pInputBuff);

	//进程挂靠
	KeAttachProcess(proc);

	//获取PEB地址
	PPEB peb = PsGetProcessPeb(proc);

	//获取模块链表
	PLIST_ENTRY pTemp = (PLIST_ENTRY)(*(ULONG*)((ULONG)peb + 0xc) + 0xc);

	//将输出缓冲区全置0
	RtlFillMemory(pOutBuff, outputSize, 0);

	PMODULE_INFO pOutModuleInfo = (PMODULE_INFO)pOutBuff;

	do
	{
		//循环获取下一块模块
		PLDR_DATA_TABLE_ENTRY pModuleInfo = (PLDR_DATA_TABLE_ENTRY)pTemp;
		pTemp = pTemp->Flink;

		if (pModuleInfo->FullDllName.Buffer != 0)
		{
			//获取模块路径
			RtlCopyMemory(pOutModuleInfo->wcModuleFullPath, pModuleInfo->FullDllName.Buffer, pModuleInfo->FullDllName.Length);
			//获取模块基址
			pOutModuleInfo->DllBase = pModuleInfo->DllBase;
			//获取模块大小
			pOutModuleInfo->SizeOfImage = pModuleInfo->SizeOfImage;
		}

		pOutModuleInfo++;

	} while (pTemp != (PLIST_ENTRY)(*(ULONG*)((ULONG)peb + 0xc) + 0xc));

	//解除挂靠
	KeDetachProcess();

	return STATUS_SUCCESS;
}



// 获取第一个文件
NTSTATUS FindFirstFile(const WCHAR* pszPath, HANDLE* phFile, FILE_BOTH_DIR_INFORMATION* pFileInfo, ULONG nInfoSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	// 1. 打开文件夹,得到文件夹的文件句柄
	HANDLE hFile = NULL;				//保存文件句柄
	OBJECT_ATTRIBUTES oa = { 0 };
	UNICODE_STRING path;	//保存文件路径
	RtlInitUnicodeString(&path, pszPath);

	InitializeObjectAttributes(
		&oa,/*要初始化的对象属性结构体*/
		&path,/*文件路径*/
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,/*属性:路径不区分大小写,打开的句柄是内核句柄*/
		NULL,
		NULL);

	IO_STATUS_BLOCK isb = { 0 };		//保存函数的操作结果
	status = ZwCreateFile(
		&hFile,/*输出的文件句柄*/
		GENERIC_READ,
		&oa,/*对象属性,需要提前将文件夹路径初始化进去*/
		&isb,
		NULL,/*文件预分配大小*/
		FILE_ATTRIBUTE_NORMAL,/*文件属性*/
		FILE_SHARE_READ,/*共享方式*/
		FILE_OPEN,/*创建描述: 存在则打开*/
		FILE_DIRECTORY_FILE,/*创建选项: 目录文件*/
		NULL,
		0);

	if (!NT_SUCCESS(isb.Status)) {
		return isb.Status;
	}

	// 2. 通过文件夹的文件句柄查询文件夹下的文件信息.
	status = ZwQueryDirectoryFile(
		hFile,
		NULL,/*用于异步IO*/
		NULL,
		NULL,
		&isb,
		pFileInfo,/*保存文件信息的缓冲区*/
		nInfoSize,/*缓冲区的字节数.*/
		FileBothDirectoryInformation,/*要获取的信息的类型*/
		TRUE,/*是否只返回一个文件信息*/
		NULL,/*用于过滤文件的表达式: *.txt*/
		TRUE/*是否重新开始扫描,TRUE从目录中的第一个条目开始,FALSE从上次呼叫开始恢复扫描*/
	);
	if (!NT_SUCCESS(isb.Status)) {
		return isb.Status;
	}
	// 传出文件句柄
	*phFile = hFile;
	return STATUS_SUCCESS;
}
// 获取下一个文件
NTSTATUS FindNextFile(HANDLE hFile, FILE_BOTH_DIR_INFORMATION* pFileInfo, ULONG nInfoSize)
{
	IO_STATUS_BLOCK isb = { 0 };	//保存函数的操作结果
	ZwQueryDirectoryFile(
		hFile,
		NULL,/*用于异步IO*/
		NULL,
		NULL,
		&isb,
		pFileInfo,/*保存文件信息的缓冲区*/
		nInfoSize,/*缓冲区的字节数.*/
		FileBothDirectoryInformation,/*要获取的信息的类型*/
		TRUE,/*是否只返回一个文件信息*/
		NULL,/*用于过滤文件的表达式: *.txt*/
		FALSE/*是否重新开始扫描,TRUE从目录中的第一个条目开始,FALSE从上次呼叫开始恢复扫描*/
	);
	return isb.Status;
}
// 获取文件数量
NTSTATUS MyGetFileNumber(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	UNICODE_STRING ustrFolder = { 0 };
	WCHAR strSymbol[0x512] = L"\\??\\";
	WCHAR strSymbolLast[2] = L"\\";
	//将路径组装为链接符号名
	wcscat_s(strSymbol, 0x512, (PWCHAR)pInputBuff);
	wcscat_s(strSymbol, 0x512, strSymbolLast);
	RtlInitUnicodeString(&ustrFolder, strSymbol);
	ULONG nCount = 0;

	HANDLE hFile = NULL;
	//后面加上266*2是用来存放文件名的
	char buff[sizeof(FILE_BOTH_DIR_INFORMATION) + 266 * 2];
	FILE_BOTH_DIR_INFORMATION* pFileInfo = (FILE_BOTH_DIR_INFORMATION*)buff;

	NTSTATUS status = FindFirstFile(
		ustrFolder.Buffer,
		&hFile,
		pFileInfo,
		sizeof(buff));
	if (!NT_SUCCESS(status)) {
		DbgPrint("查找第一个文件失败:0x%08X\n", status);
		return status;
	}
	do
	{
		// DbgPrint("文件名: %ls\n", pFileInfo->FileName);
		nCount++;
	} while (STATUS_SUCCESS == FindNextFile(hFile, pFileInfo, sizeof(buff)));

	Irp->IoStatus.Information = nCount;

	return STATUS_SUCCESS;

}
// 遍历文件
NTSTATUS MyQueryFile(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	UNICODE_STRING ustrFolder = { 0 };
	WCHAR strSymbol[0x512] = L"\\??\\";
	WCHAR strSymbolLast[2] = L"\\";
	//将路径组装为链接符号名
	wcscat_s(strSymbol, 0x512, (PWCHAR)pInputBuff);
	wcscat_s(strSymbol, 0x512, strSymbolLast);
	RtlInitUnicodeString(&ustrFolder, strSymbol);
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == NULL)
		return STATUS_SUCCESS;
	PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;

	//将输出缓冲区全置0
	RtlFillMemory(pOutBuff, outputSize, 0);

	PFILE_INFO pOutFileInfo = (PFILE_INFO)pOutBuff;

	HANDLE hFile = NULL;
	//后面加上266*2是用来存放文件名的
	char buff[sizeof(FILE_BOTH_DIR_INFORMATION) + 266 * 2];
	FILE_BOTH_DIR_INFORMATION* pFileInfo = (FILE_BOTH_DIR_INFORMATION*)buff;

	NTSTATUS status = FindFirstFile(
		ustrFolder.Buffer,
		&hFile,
		pFileInfo,
		sizeof(buff));
	if (!NT_SUCCESS(status)) {
		DbgPrint("查找第一个文件失败:0x%08X\n", status);
		return status;
	}

	do
	{
		// DbgPrint("文件名: %ls\n", pFileInfo->FileName);
		//文件名
		RtlCopyMemory(pOutFileInfo->wcFileName, pFileInfo->FileName, pFileInfo->FileNameLength);
		//创建时间
		pOutFileInfo->CreateTime = pFileInfo->CreationTime;
		//修改时间
		pOutFileInfo->ChangeTime = pFileInfo->ChangeTime;
		//文件大小
		pOutFileInfo->Size = pFileInfo->EndOfFile.QuadPart;

		//文件是目录还是文件
		if (pFileInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			pOutFileInfo->FileOrDirectory = 0;
		}
		else
		{
			pOutFileInfo->FileOrDirectory = 1;
		}
		pOutFileInfo++;
	} while (STATUS_SUCCESS == FindNextFile(hFile, pFileInfo, sizeof(buff)));

	return STATUS_SUCCESS;
}
// 删除文件
NTSTATUS DelFile(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	UNICODE_STRING ustrFolder = { 0 };
	WCHAR strSymbol[0x512] = L"\\??\\";

	//将路径组装为链接符号名
	wcscat_s(strSymbol, 0x512, (PWCHAR)pInputBuff);
	RtlInitUnicodeString(&ustrFolder, strSymbol);

	//初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES oa = { 0 };

	InitializeObjectAttributes(
		&oa,/*要初始化的对象属性结构体*/
		&ustrFolder,/*文件路径*/
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,/*属性:路径不区分大小写,打开的句柄是内核句柄*/
		NULL,
		NULL);

	//删除指定文件
	ZwDeleteFile(&oa);

	return STATUS_SUCCESS;
}



// 获取注册表项目数量
NTSTATUS MyGetRegNumber(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;

	// 1. 打开此路径.得到路径对应的句柄.
	NTSTATUS status;
	HANDLE hKey;
	OBJECT_ATTRIBUTES objAtt = { 0 };
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, (PWCHAR)pInputBuff);
	InitializeObjectAttributes(&objAtt,
		&name,
		OBJ_CASE_INSENSITIVE,
		0,
		0);
	//打开注册表
	status = ZwOpenKey(&hKey,
		GENERIC_ALL,
		&objAtt);
	if (status != STATUS_SUCCESS) {
		return status;
	}
	//查询VALUE的大小
	ULONG size = 0;
	ZwQueryKey(hKey,
		KeyFullInformation,
		0,
		0,
		&size);
	KEY_FULL_INFORMATION* pKeyInfo = (KEY_FULL_INFORMATION*)
		ExAllocatePoolWithTag(PagedPool, size, 'gnaw');
	if (pKeyInfo == NULL)
		return STATUS_SUCCESS;
	ZwQueryKey(hKey,
		KeyFullInformation,
		pKeyInfo,
		size,
		&size);

	Irp->IoStatus.Information = pKeyInfo->SubKeys + pKeyInfo->Values;

	ExFreePoolWithTag(pKeyInfo, 'gnaw');
	ZwClose(hKey);
	return STATUS_SUCCESS;
}
// 遍历注册表
NTSTATUS MyQueryReg(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == NULL)
		return STATUS_SUCCESS;
	if (pOutBuff == 0)
	{
		DbgPrint("获取Mdl为空");
		return STATUS_SUCCESS;
	}
	// 1. 打开此路径.得到路径对应的句柄.
	NTSTATUS status;
	HANDLE hKey;
	OBJECT_ATTRIBUTES objAtt = { 0 };
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, (PWCHAR)pInputBuff);
	InitializeObjectAttributes(&objAtt,
		&name,
		OBJ_CASE_INSENSITIVE,
		0,
		0);
	//打开注册表
	status = ZwOpenKey(&hKey,
		GENERIC_ALL,
		&objAtt);
	if (status != STATUS_SUCCESS) {
		return status;
	}
	//查询VALUE的大小
	ULONG size = 0;
	ZwQueryKey(hKey,
		KeyFullInformation,
		0,
		0,
		&size);
	KEY_FULL_INFORMATION* pKeyInfo = (KEY_FULL_INFORMATION*)
		ExAllocatePoolWithTag(PagedPool, size, 'gnaw');
	ZwQueryKey(hKey,
		KeyFullInformation,
		pKeyInfo,
		size,
		&size);

	//将输出缓冲区全置0
	RtlFillMemory(pOutBuff, outputSize, 0);

	PREG_INFO pReg = (PREG_INFO)pOutBuff;
	ULONG nCount = 0;
	//遍历子项的名字
	for (ULONG i = 0; i < pKeyInfo->SubKeys; i++)
	{
		pReg[nCount].Type = 0;
		ZwEnumerateKey(hKey, i, KeyBasicInformation, NULL, 0, &size);
		PKEY_BASIC_INFORMATION pKeyBaseInfo = (PKEY_BASIC_INFORMATION)ExAllocatePoolWithTag(PagedPool, size, 'oah');
		ZwEnumerateKey(hKey, i, KeyBasicInformation, pKeyBaseInfo, size, &size);
		RtlCopyMemory(pReg[nCount].KeyName, pKeyBaseInfo->Name, pKeyBaseInfo->NameLength);
		ExFreePoolWithTag(pKeyBaseInfo, 'oah');
		nCount++;
	}
	for (ULONG i = 0; i < pKeyInfo->Values; i++)
	{
		pReg[nCount].Type = 1;
		//查询单个VALUE的大小
		ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, NULL, 0, &size);
		PKEY_VALUE_FULL_INFORMATION pValueInfo = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, size, 'uin');
		if (pValueInfo == NULL)
			return STATUS_SUCCESS;
		//查询单个VALUE的详情
		ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, pValueInfo, size, &size);
		//获取到值的名字
		RtlCopyMemory(pReg[nCount].ValueName, pValueInfo->Name, pValueInfo->NameLength);
		//获取值的类型
		pReg[nCount].ValueType = pValueInfo->Type;
		//获取值的数据
		RtlCopyMemory(pReg[nCount].Value, (PVOID)((ULONG)pValueInfo + pValueInfo->DataOffset), pValueInfo->DataLength);
		//获取值的长度
		pReg[nCount].ValueLength = pValueInfo->DataLength;
		ExFreePoolWithTag(pValueInfo, 'uin');
		nCount++;
	}
	ExFreePoolWithTag(pKeyInfo, 'gnaw');
	ZwClose(hKey);
	return STATUS_SUCCESS;
}
// 添加注册表子项
NTSTATUS MyCreateReg(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	// 1. 打开此路径.得到路径对应的句柄.
	NTSTATUS status;
	HANDLE hKey;
	OBJECT_ATTRIBUTES objAtt = { 0 };
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, (PWCHAR)pInputBuff);
	InitializeObjectAttributes(&objAtt,
		&name,
		OBJ_CASE_INSENSITIVE,
		0,
		0);
	//创建键
	status = ZwCreateKey(&hKey,
		KEY_ALL_ACCESS,
		&objAtt,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		NULL);
	if (NT_SUCCESS(status)) {
		ZwClose(hKey);
	}

	return STATUS_SUCCESS;
}
// 删除注册表子项
NTSTATUS MyDelReg(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	// 1. 打开此路径.得到路径对应的句柄.
	NTSTATUS status;
	HANDLE hKey;
	OBJECT_ATTRIBUTES objAtt = { 0 };
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, (PWCHAR)pInputBuff);
	InitializeObjectAttributes(&objAtt,
		&name,
		OBJ_CASE_INSENSITIVE,
		0,
		0);
	//打开键
	status = ZwOpenKey(&hKey,
		KEY_ALL_ACCESS,
		&objAtt);
	if (NT_SUCCESS(status)) {
		//删除键
		status = ZwDeleteKey(hKey);
		ZwClose(hKey);
	}
	return STATUS_SUCCESS;
}



// 遍历IDT表
NTSTATUS MyQueryIDT(PIRP Irp)
{
	SIDT_INFO sidtInfo = { 0,0,0 };
	PIDT_ENTRY pIDTEntry = NULL;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == 0)
	{
		DbgPrint("获取MDL失败\n");
		return STATUS_SUCCESS;
	}
	//获取IDTR寄存器的值
	__asm sidt sidtInfo;

	//获取IDT表数组首地址
	pIDTEntry = (PIDT_ENTRY)MAKE_LONG(sidtInfo.uLowIdtBase, sidtInfo.uHighIdtBase);

	PIDT_INFO pOutIdtInfo = (PIDT_INFO)pOutBuff;
	if (pOutIdtInfo == 0)
	{
		DbgPrint("获取指针失败\n");
		return STATUS_SUCCESS;
	}
	//获取IDT信息
	for (ULONG i = 0; i < 0x100; i++)
	{
		//中断地址
		pOutIdtInfo->pFunction = MAKE_LONG(pIDTEntry[i].uOffsetLow, pIDTEntry[i].uOffsetHigh);
		//段选择子
		pOutIdtInfo->Selector = pIDTEntry[i].uSelector;
		//类型
		pOutIdtInfo->GateType = pIDTEntry[i].GateType;
		//特权等级
		pOutIdtInfo->Dpl = pIDTEntry[i].DPL;
		pOutIdtInfo++;
	}
	return STATUS_SUCCESS;
}

// 获取GDT表个数
NTSTATUS MyGetGDTNumber(PIRP Irp)
{
	SGDT_INFO sgdtInfo = { 0,0,0 };
	PGDT_ENTER pGdtEntry = NULL;
	//获取GDTR寄存器的值
	__asm sgdt sgdtInfo;
	//获取GDT表数组首地址
	pGdtEntry = (PGDT_ENTER)MAKE_LONG(sgdtInfo.uLowGdtBase, sgdtInfo.uHighGdtBase);
	//获取GDT表数组个数
	ULONG gdtCount = sgdtInfo.uGdtLimit / 8;

	ULONG nCount = 0;

	//获取GDT信息
	for (ULONG i = 0; i < gdtCount; i++)
	{
		//如果段无效，则不遍历
		if (pGdtEntry[i].P == 0)
		{
			continue;
		}
		nCount++;
	}

	Irp->IoStatus.Information = nCount;

	return STATUS_SUCCESS;
}
// 遍历GDT表
NTSTATUS MyQueryGDT(PIRP Irp)
{
	SGDT_INFO sgdtInfo = { 0,0,0 };
	PGDT_ENTER pGdtEntry = NULL;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	//获取GDTR寄存器的值
	__asm sgdt sgdtInfo;
	//获取GDT表数组首地址
	pGdtEntry = (PGDT_ENTER)MAKE_LONG(sgdtInfo.uLowGdtBase, sgdtInfo.uHighGdtBase);
	//获取GDT表数组个数
	ULONG gdtCount = sgdtInfo.uGdtLimit / 8;

	PGDT_INFO pOutGdtInfo = (PGDT_INFO)pOutBuff;

	//获取GDT信息
	for (ULONG i = 0; i < gdtCount; i++)
	{
		//如果段无效，则不遍历
		if (pGdtEntry[i].P == 0)
		{
			continue;
		}
		//段基址
		pOutGdtInfo->BaseAddr = MAKE_LONG2(pGdtEntry[i].base0_23, pGdtEntry[i].base24_31);
		//段特权等级
		pOutGdtInfo->Dpl = (ULONG)pGdtEntry[i].DPL;
		//段类型
		pOutGdtInfo->GateType = (ULONG)pGdtEntry[i].Type;
		//段粒度
		pOutGdtInfo->Grain = (ULONG)pGdtEntry[i].G;
		//段限长
		pOutGdtInfo->Limit = MAKE_LONG(pGdtEntry[i].Limit0_15, pGdtEntry[i].Limit16_19);
		pOutGdtInfo++;
	}
	return STATUS_SUCCESS;
}
// 遍历SSDT表
NTSTATUS MyQuerySSDT(PIRP Irp)
{
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == NULL)
		return STATUS_SUCCESS;
	PSSDT_INFO pOutSsdtInfo = (PSSDT_INFO)pOutBuff;

	for (ULONG i = 0; i < KeServiceDescriptorTable.NumberOfServices; i++)
	{
		//函数地址
		pOutSsdtInfo->uFuntionAddr = (ULONG)KeServiceDescriptorTable.ServiceTableBase[i];
		//调用号
		pOutSsdtInfo->uIndex = i;
		pOutSsdtInfo++;
	}
	return STATUS_SUCCESS;
}


// 反调试
NTSTATUS FanTiaoShi(PIRP Irp)
{
	NTSTATUS status;
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	ULONG Pid = *(PULONG)pInputBuff;
	PEPROCESS pProc = NULL;
	status = PsLookupProcessByProcessId((HANDLE)Pid, &pProc);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("反调试失败\n");
		return status;
	}
	PFLAG2 a=(PFLAG2)((ULONG)pProc + 0x26C);
	a->ProtectedProcess = 1;
	ObDereferenceObject(pProc);
	return status;
}


ULONG MyPid = 0;
ULONG_PTR OldKiFastCallEntry = 0;
// Sysenter-Hook
VOID _declspec(naked) MyKiFastCallEntry()
{
	// 1. 假设要过滤的是函数 ZwOpenProcess，此时调用号是 0xBE
	//	[edx + 0x00]: 返回地址1 cs
	//	[edx + 0x04]: 返回地址2 eip
	//	[edx + 0x08]: 参数1		ProcessHandle(句柄)
	//	[edx + 0x0C]: 参数2		DesiredAccess(权限)
	//	[edx + 0x10]: 参数3		ObjectAttributes(属性)
	//	[edx + 0x14]: 参数4		ClientId(PID)
	__asm
	{
		pushad;						// 保存所有的寄存器
		cmp eax, 0xBE;				// 比对调用号，确定是否是关注的函数
		jne tag_End;				// 如果不是关注的函数，就执行原有函数

	// 开始 HOOK
		mov eax, [edx + 0x14];		// 获取到进程的 &ClientID
		mov eax, [eax]				// * 获取 ClientID.ProcessId
		cmp MyPid, eax;				// 判断是否是保护的 pid
		jne tag_End;				// 如果不是就调用原函数
		mov[edx + 0x0C], 0;			// 如果是就修改权限为 0

	tag_End:
		popad;						// 恢复原有的寄存器
		jmp OldKiFastCallEntry;		// 跳转到原始函数
	}
}
// 开启hook
VOID InstallSysenterHook()
{
	__asm
	{
		// 从 msr 0x176 中获取到原有的函数地址
		mov ecx, 0x176
		rdmsr
		mov OldKiFastCallEntry, eax
	}

	__asm
	{
		mov ecx, 0x176
		mov eax, MyKiFastCallEntry
		wrmsr
	}
}
// 关闭hook
VOID UninstallSysenterHook()
{
	__asm
	{
		mov ecx, 0x176
		mov eax, OldKiFastCallEntry
		wrmsr
	}
}


// 对象Hook
typedef POBJECT_TYPE(*pfnObGetObjectType)(PVOID Object);
ULONG* HookAddress = NULL;				// 函数所在地址
OB_OPEN_METHOD HookFunction = NULL;		// 旧的函数
UNICODE_STRING StopFileName = { 0 };
UNICODE_STRING StopProcessName = { 0 };
// 打开文件获取文件句柄
NTSTATUS CreateFile(PHANDLE FileHandle, PCWCH pFilePath, ACCESS_MASK Access, ULONG CreateDisposition, BOOLEAN IsFile)
{
	// 保存文件或目录句柄的变量
	HANDLE FileHandle2 = NULL;
	IO_STATUS_BLOCK IoStatusBlock = { 0 };


	// 初始化文件或目录对应的路径名称，这个名称必须是 \\??\\ 开头的
	UNICODE_STRING FilePath = { 0 };
	RtlInitUnicodeString(&FilePath, pFilePath);

	// 操作任何一个具名对象，都需要使用 OBJECT_ATTRIBUTES 结构体
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	InitializeObjectAttributes(
		&ObjectAttributes,			// 需要进行初始化的对象属性结构体
		&FilePath,					// 对应的具名对象的名字
		OBJ_CASE_INSENSITIVE,		// 对象属性，这里表示对象不区分大小写
		NULL, NULL);				// 对象所在的目录，对象的安全描述符

	// 该函数接收一个布尔值表示操作的是文件还是目录，这里将它转换为标志位
	ULONG CreateOption = IsFile ? FILE_NON_DIRECTORY_FILE : FILE_DIRECTORY_FILE;

	// 默认的内核操作都是异步的，如果想要进行同步操作，必须指定下面的标志位
	CreateOption |= FILE_SYNCHRONOUS_IO_NONALERT;

	// 此函数可以用于打开一个文件或目录，如果成功得到句柄，失败返回错误原因
	NTSTATUS Status = ZwCreateFile(
		&FileHandle2,				// 如果函数执行成功，则返回文件句柄
		Access,						// 以何种权限打开指定的文件或目录 GENERIC_ALL
		&ObjectAttributes,			// 对象属性结构体，主要用于提供文件的路径
		&IoStatusBlock,				// 保存当前 IO 操作的执行结果
		0,							// 如果文件是新创建的，这个参数指定新文件的大小
		FILE_ATTRIBUTE_NORMAL,		// 文件的属性，通常使用 FILE_ATTRIBUTE_NORMAL
		FILE_SHARE_VALID_FLAGS,		// 文件的共享方式，这里表示共享读写删除
		CreateDisposition,			// 文件的创建标志，通常是 FILE_OPEN_IF
		CreateOption,				// 一组标志位，这里我们用来说明打开的是什么
		NULL, 0);					// 扩展的缓冲区以及大小，文档要求为 NULL

	// 判断文件是否打开成功，如果成功设置得到的句柄，否则直接返回错误码
	if (NT_SUCCESS(Status))
	{
		*FileHandle = FileHandle2;
	}
	return Status;
}

NTSTATUS MyOpenProcedure(IN ULONG Unknown,IN OB_OPEN_REASON OpenReason,IN PEPROCESS Process OPTIONAL,IN PVOID Object,IN ACCESS_MASK GrantedAccess,IN ULONG HandleCount)
{
	// 这里 HOOK 的是文件，Object 表示的就是被 HOOK 的对象
	PFILE_OBJECT FileObject = Object;

	if (ObCreateHandle == OpenReason)
	{
		DbgPrint("创建了 %wZ\n", &FileObject->FileName);
		// 如果想要拦截文件操作，不能做完全匹配，可以从指定的路径中
		//	查找需要拦截的文件名，如果找到了，返回失败，否则放行
	}
	else if (ObOpenHandle == OpenReason)
	{
		// DbgPrint("打开了 %wZ\n", &FileObject->FileName);
		PWCHAR Buffer = ExAllocatePoolWithTag(NonPagedPoolNx, 0x1000, 'tset');
		RtlFillMemory(Buffer, 0, 0x1000);
		RtlCopyMemory(Buffer, FileObject->FileName.Buffer, FileObject->FileName.Length);
		if (wcsstr(Buffer, L"123.txt"))
		{
			DbgPrint("打开失败 %wZ\n", &FileObject->FileName);
			ExFreePoolWithTag(Buffer, 'tset');
			return STATUS_UNSUCCESSFUL;
		}
		ExFreePoolWithTag(Buffer, 'tset');
	}
	return HookFunction ? HookFunction(Unknown, OpenReason, Process, Object, GrantedAccess, HandleCount) : STATUS_SUCCESS;
}
NTSTATUS MyOpenProcedure1(IN ULONG Unknown, IN OB_OPEN_REASON OpenReason, IN PEPROCESS Process OPTIONAL, IN PVOID Object, IN ACCESS_MASK GrantedAccess, IN ULONG HandleCount)
{
	// 这里 HOOK 的是文件，Object 表示的就是被 HOOK 的对象
	PEPROCESS ProcessObject = Object;
	CHAR* arr = (CHAR*)((char*)ProcessObject + 0x16c);
	// 0x16C
	if (strstr(arr,"calc.exe") != 0)
	{
		return STATUS_UNSUCCESSFUL;
	}
	
	return HookFunction ? HookFunction(Unknown, OpenReason, Process, Object, GrantedAccess, HandleCount) : STATUS_SUCCESS;
}


pfnObGetObjectType GetObjectTypeAddress()
{
	// 从操作系统的导入表中，找到指定的 ObGetObjectType 函数地址，并返回
	UNICODE_STRING RoutineName = RTL_CONSTANT_STRING(L"ObGetObjectType");
	return (pfnObGetObjectType)MmGetSystemRoutineAddress(&RoutineName);
}
// 开启文件对象HOOK
VOID OnObjecFileHook()
{
	HANDLE FileHandle = NULL;
	PFILE_OBJECT FileObject = NULL;
	NTSTATUS Status = STATUS_SUCCESS;

	// 1. 获取一个想要 HOOK 的类型所对应的[内核对象结构体]
	Status = CreateFile(&FileHandle, L"\\??\\D:\\demo.txt", GENERIC_READ, FILE_OPEN_IF, TRUE);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("文件打开失败\n");
		return;
	}

	// 2. 将上面获取到的句柄转换成一个内核对象结构体
	Status = ObReferenceObjectByHandle(FileHandle, FILE_ALL_ACCESS, NULL, KernelMode, &FileObject, NULL);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("对象获取失败\n");
		return;
	}

	// 3. 从操作系统 ssdt 中获取 ObGetObjectType 的地址
	pfnObGetObjectType ObGetObjectType = GetObjectTypeAddress();
	if (ObGetObjectType == NULL)
	{
		DbgPrint("ObGetObjectType 获取失败\n");
		return;
	}

	// 4. 通过对象结构体，找到对象所对应的 OBJECT_TYPE 结构体
	POBJECT_TYPE ObjectType = ObGetObjectType(FileObject);
	if (ObjectType == NULL)
	{
		DbgPrint("对象类型获取失败\n");
		return;
	}

	// 5. 找到 _OBJECT_TYPE_INITIALIZER 中需要 Hook 的函数所在的位置
	HookAddress = &ObjectType->TypeInfo.OpenProcedure;
	HookFunction = (OB_OPEN_METHOD)ObjectType->TypeInfo.OpenProcedure;

	// 6. 将目标函数替换成用户自定义的函数，执行拦截操作
	InterlockedExchange((LONG*)HookAddress, (LONG)MyOpenProcedure);
	ObDereferenceObject(FileObject);
	ZwClose(FileHandle);
}

// 进程对象HOOK
VOID OnObjectProcessHook()
{
	HANDLE ProcessHandle = NULL;
	PEPROCESS ProcessObject = NULL;
	NTSTATUS Status = STATUS_SUCCESS;
	CLIENT_ID Client;
	HANDLE dwPid=PsGetCurrentProcessId();
	Client.UniqueProcess = (HANDLE)dwPid;
	Client.UniqueThread = 0;
	OBJECT_ATTRIBUTES Objectattributes = { 0 };
	InitializeObjectAttributes(&Objectattributes, 0, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
	Status = ZwOpenProcess(&ProcessHandle, PROCESS_ALL_ACCESS, &Objectattributes, &Client);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("进程打开失败\n");
		return;
	}

	// 2. 将上面获取到的句柄转换成一个内核对象结构体
	Status = ObReferenceObjectByHandle(ProcessHandle, FILE_ALL_ACCESS, NULL, KernelMode, &ProcessObject, NULL);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("对象获取失败\n");
		return;
	}

	////获取当前进程的EPROCESS
	//PEPROCESS ProcessObject = PsGetCurrentProcess();
	// 3. 从操作系统 ssdt 中获取 ObGetObjectType 的地址
	pfnObGetObjectType ObGetObjectType = GetObjectTypeAddress();
	if (ObGetObjectType == NULL)
	{
		DbgPrint("ObGetObjectType 获取失败\n");
		return;
	}
	// 4. 通过对象结构体，找到对象所对应的 OBJECT_TYPE 结构体
	POBJECT_TYPE ObjectType = ObGetObjectType(ProcessObject);
	if (ObjectType == NULL)
	{
		DbgPrint("对象类型获取失败\n");
		return;
	}
	// 5. 找到 _OBJECT_TYPE_INITIALIZER 中需要 Hook 的函数所在的位置
	HookAddress = &ObjectType->TypeInfo.OpenProcedure;
	HookFunction = (OB_OPEN_METHOD)ObjectType->TypeInfo.OpenProcedure;
	// 6. 将目标函数替换成用户自定义的函数，执行拦截操作
	InterlockedExchange((LONG*)HookAddress, (LONG)MyOpenProcedure1);
	ObDereferenceObject(ProcessObject);
	ZwClose(ProcessHandle);
}

// 关闭HOOK
VOID OffObjectHook()
{
	InterlockedExchange((LONG*)HookAddress, (LONG)HookFunction);
}


extern PDRIVER_OBJECT g_pDriver;
// 默认派遣函数
NTSTATUS CommonDispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)								// 保存的是具体消息的内容
{
	UNREFERENCED_PARAMETER(DeviceObject);

	// 1. 设置消息的处理状态，即成功或失败，如果失败了，是为什么失败 [GetLastError]
	Irp->IoStatus.Status = STATUS_SUCCESS;

	// 2. 设置消息处理的字节长度，即读写的长度 [Bytes]
	Irp->IoStatus.Information = 0;

	// 3. 一旦请求完成，需要告诉请求方，请求结束了 [返回]
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// 4. 告诉 IO 管理器当前的 IRP 是否处理成功
	return STATUS_SUCCESS;
}
// 自定义派遣函数吗
NTSTATUS IoCtlDispatchRoutine(IN PDEVICE_OBJECT	DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG IoControlCode = Stack->Parameters.DeviceIoControl.IoControlCode;

	Irp->IoStatus.Status = STATUS_SUCCESS;
	switch (IoControlCode)
	{
		// 获取驱动数量
	case CTL_GetDriverNum:
	{
		// DbgBreakPoint();
		MyGetDriverNumber(DeviceObject->DriverObject, Irp);
		break;
	}
	// 遍历驱动
	case CTL_QueryDriver:
	{
		//获取当前IRP栈
		PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
		ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
		MyQueryDriver(DeviceObject->DriverObject, Irp, outputSize);
		break;
	}
	case CTL_HideDriver:
	{
		MyHideDriver(DeviceObject, Irp);
		break;
	}
	// 获取进程数量
	case CTL_GetProcessNum:
	{
		MyGetProcessNumber(Irp);
		break;
	}
	// 遍历进程
	case CTL_QueryProcess:
	{
		//获取当前IRP栈
		PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
		ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
		MyQueryProcess(Irp, outputSize);
		break;
	}
	// 隐藏进程
	case CTL_HideProcess:
	{
		MyHindProcess(Irp);
		break;
	}
	// 结束进程
	case CTL_DelProcess:
	{
		MyDelProcess(Irp);
		break;
	}
	// 获取线程数量
	case CTL_GetThreadNum:
	{
		MyGetThreadNumber(Irp);
		break;
	}
	// 遍历线程
	case CTL_QueryThread:
	{
		MyQueryThread(Irp);
		break;
	}
	// 获取模块数量
	case CTL_GetModuleNum:
	{
		MyGetModuleNumber(Irp);
		break;
	}
	// 遍历模块
	case CTL_QueryModule:
	{
		MyQueryModule(Irp);
		break;
	}
	// 获取文件数量
	case CTL_GetFileNum:
	{
		MyGetFileNumber(Irp);
		break;
	}
	// 遍历文件
	case CTL_QueryFile:
	{
		MyQueryFile(Irp);
		break;
	}
	// 删除文件
	case CTL_DelFile:
	{
		DelFile(Irp);
		break;
	}
	// 获取注册表数量
	case CTL_GetRegNum:
	{
		MyGetRegNumber(Irp);
		break;
	}
	// 遍历注册表
	case CTL_QueryReg:
	{
		MyQueryReg(Irp);
		break;
	}
	// 创建注册表子项
	case CTL_CreateReg:
	{
		MyCreateReg(Irp);
		break;
	}
	// 删除注册表子项
	case CTL_DelReg:
	{
		MyDelReg(Irp);
		break;
	}
	// 遍历IDT表
	case CTL_QueryIDT:
	{
		MyQueryIDT(Irp);
		break;
	}
	// 获取GDT表元素个数
	case CTL_GetGDTNum:
	{
		MyGetGDTNumber(Irp);
		break;
	}
	// 遍历GDT表
	case CTL_QueryGDT:
	{
		MyQueryGDT(Irp);
		break;
	}
	// 获取SSDT表函数个数
	case CTL_GetSSDTNum:
	{
		Irp->IoStatus.Information = KeServiceDescriptorTable.NumberOfServices;
		break;
	}
	// 遍历SSDT
	case CTL_QuerySSDT:
	{
		MyQuerySSDT(Irp);
		break;
	}
	// 保护自身不被调试
	case CTL_ProtectDriver:
	{
		PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
		MyPid = *(PULONG)pInputBuff;
		InstallSysenterHook();
		break;
	}
	// 反调试
	case CTL_FanTiao:
	{
		FanTiaoShi(Irp);
		break;
	}
	// 让指定文件无法创建
	case CTL_StopFile:
	{
		PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
		RtlInitUnicodeString(&StopFileName, (PWCHAR)pInputBuff);
		OnObjecFileHook();
		break;
	}
	// 让指定进程无法创建
	case CTL_StopProcess:
	{
		PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
		RtlInitUnicodeString(&StopProcessName, (PWCHAR)pInputBuff);
		OnObjectProcessHook();
		break;
	}
	// 关闭HOOK
	case CTL_CloseHook:
	{
		OffObjectHook();
		break;
	}
	case CTL_NeiHeChongZai:
	{
		// 重载内核
		KernelReload(Irp);
		break;
	}
	case CTL_CloseNeiHeChongZai:
	{
		UnHook();
		break;
	}
	case CTL_CancleProtectDriver:
	{
		UninstallSysenterHook();
		break;
	}
	default:
	{
		Irp->IoStatus.Information = 0;
		Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
		break;
	}
	}

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


// 内核重载
PSSDTEntry g_pNewSSDT;//新的SSDT
ULONG g_JmpPoint;
PUCHAR pHookPoint;

// 打开文件
HANDLE KernelCreateFile(IN PUNICODE_STRING pstrFile,IN BOOLEAN bIsDir)  // 文件路径符号链接 // 是否为文件夹
{
	HANDLE          hFile = NULL;
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess =
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	ULONG           ulCreateOpt =
		FILE_SYNCHRONOUS_IO_NONALERT;
	// 1. 初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes =
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // 返回初始化完毕的结构体
		pstrFile,      // 文件对象名称
		ulAttributes,  // 对象属性
		NULL, NULL);   // 一般为NULL
	// 2. 创建文件对象
	ulCreateOpt |= bIsDir ?
		FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
	Status = ZwCreateFile(
		&hFile,                // 返回文件句柄
		GENERIC_ALL,           // 文件操作描述
		&objAttrib,            // OBJECT_ATTRIBUTES
		&StatusBlock,          // 接受函数的操作结果
		0,                     // 初始文件大小
		FILE_ATTRIBUTE_NORMAL, // 新建文件的属性
		ulShareAccess,         // 文件共享方式
		FILE_OPEN_IF,          // 文件存在则打开不存在则创建
		ulCreateOpt,           // 打开操作的附加标志位
		NULL,                  // 扩展属性区
		0);                   // 扩展属性区长度
	if (!NT_SUCCESS(Status))
		return (HANDLE)-1;
	return hFile;
}

// 获取文件大小
ULONG64 KernelGetFileSize(IN HANDLE hfile)
{
	// 查询文件状态
	IO_STATUS_BLOCK           StatusBlock = { 0 };
	FILE_STANDARD_INFORMATION fsi = { 0 };
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	Status = ZwQueryInformationFile(
		hfile,        // 文件句柄
		&StatusBlock, // 接受函数的操作结果
		&fsi,         // 根据最后一个参数的类型输出相关信息
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation);
	if (!NT_SUCCESS(Status))
		return 0;
	return fsi.EndOfFile.QuadPart;
}

// 读取文件
ULONG64 KernelReadFile(IN HANDLE hfile,IN PLARGE_INTEGER Offset,IN ULONG ulLength,OUT PVOID pBuffer)  
{
	// 1. 读取文件
	IO_STATUS_BLOCK StatusBlock = { 0 };
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	Status = ZwReadFile(
		hfile,        // 文件句柄
		NULL,         // 信号状态(一般为NULL)
		NULL, NULL,   // 保留
		&StatusBlock, // 接受函数的操作结果
		pBuffer,      // 保存读取数据的缓存
		ulLength,     // 想要读取的长度
		Offset,       // 读取的起始偏移
		NULL);        // 一般为NULL
	if (!NT_SUCCESS(Status))  return 0;
	// 2. 返回实际读取的长度
	return StatusBlock.Information;
}

// 搜索内存特征
void* SearchMemory(char* buf, int BufLenth, char* Mem, int MaxLenth)
{
	int MemIndex = 0;
	int BufIndex = 0;
	for (MemIndex = 0; MemIndex < MaxLenth; MemIndex++)
	{
		BufIndex = 0;
		if (Mem[MemIndex] == buf[BufIndex] || buf[BufIndex] == '?')
		{
			int MemIndexTemp = MemIndex;
			do
			{
				MemIndexTemp++;
				BufIndex++;
			} while ((Mem[MemIndexTemp] == buf[BufIndex] || buf[BufIndex] == '?') && BufIndex < BufLenth);
			if (BufIndex == BufLenth)
			{
				return Mem + MemIndex;
			}

		}
	}
	return 0;
}

// 关闭页保护
void OffProtected()
{
	__asm { //关闭内存保护
		cli;
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}

}
// 开启页保护
void OnProtected()
{
	__asm { //恢复内存保护
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
		sti;
	}

}

// 通过名称获取模块地址
ULONG32 MyGetModuleHandle(PUNICODE_STRING pModuleName)
{
	PLDR_DATA_TABLE_ENTRY1 pLdr =
		(PLDR_DATA_TABLE_ENTRY1)g_pDriver->DriverSection;
	LIST_ENTRY* pTemp = &pLdr->InLoadOrderLinks;
	do
	{
		PLDR_DATA_TABLE_ENTRY1 pDriverInfo =
			(PLDR_DATA_TABLE_ENTRY1)pTemp;
		if (RtlCompareUnicodeString(pModuleName, &pDriverInfo->BaseDllName, FALSE) == 0)
		{
			return pDriverInfo->DllBase;
		}
		pTemp = pTemp->Blink;
	} while (pTemp != &pLdr->InLoadOrderLinks);
	return 0;
}

//windows根据不同的环境，会加载不同的内核文件
//单核，开了PAE  
//单核，没开PAE
//多核，开了PAE
//多核，没开PAE

// 读取内核模块到内存中
void ReadKernelToBuf(PWCHAR pPath, PUCHAR* pBuf)
{
	//-----------------------------------------
	UNICODE_STRING pKernelPath;  //内核文件路径
	HANDLE hFile = 0;            //内核文件句柄
	LARGE_INTEGER Offset = { 0 };//读取的偏移值
	//-----------------------------------------
	//1 打开文件
	RtlInitUnicodeString(
		&pKernelPath,
		pPath);
	hFile = KernelCreateFile(&pKernelPath, FALSE);
	//2 获取文件大小
	ULONG64 ulFileSize = KernelGetFileSize(hFile);
	*pBuf = ExAllocatePool(NonPagedPool, ulFileSize);
	RtlZeroMemory(*pBuf, ulFileSize);
	//3 读取文件到内存
	KernelReadFile(hFile, &Offset, ulFileSize, *pBuf);
}

// 展开内核PE文件
void ZKKernel(PUCHAR* pZkBUf, PUCHAR buf)
{
	//1 获得DOS头，继而获得NT头，再获得扩展头
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)buf;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + buf);
	ULONG uZkSize = pNt->OptionalHeader.SizeOfImage;

	//2 申请空间
	*pZkBUf = ExAllocatePool(NonPagedPool, uZkSize);
	RtlZeroMemory(*pZkBUf, uZkSize);
	//3 开始展开
	//3.1 先拷贝头部
	memcpy(*pZkBUf, buf, pNt->OptionalHeader.SizeOfHeaders);
	//3.2再拷贝区段
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	for (int i = 0; i < pNt->FileHeader.NumberOfSections; i++)
	{
		memcpy(
			*pZkBUf + pSection[i].VirtualAddress,//本区段内存中的起始位置
			buf + pSection[i].PointerToRawData,  //本区段在文件中的位置
			pSection[i].Misc.VirtualSize         //本区段的大小
		);
	}
}
// 修复新内核重定位
void FixReloc(PUCHAR ZkBuf, PUCHAR OldBase)
{
	typedef struct _TYPE {
		USHORT Offset : 12;
		USHORT Type : 4;
	}TYPE, * PTYPE;

	//1 获得DOS头，继而获得NT头，再获得扩展头
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)ZkBuf;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + ZkBuf);
	//2 获得重定位表
	PIMAGE_DATA_DIRECTORY pRelocDir = (pNt->OptionalHeader.DataDirectory + 5);
	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)
		(pRelocDir->VirtualAddress + ZkBuf);

	//2.5 得到一个老内核与默认基址间的一个差值
	ULONG uOffset = (ULONG)OldBase - pNt->OptionalHeader.ImageBase;

	//3 开始修复重定位
	while (pReloc->SizeOfBlock != 0)
	{
		ULONG uCount = (pReloc->SizeOfBlock - 8) / 2;//本0x1000内，有多少需要重定位的地方
		ULONG uBaseRva = pReloc->VirtualAddress;     //本0x1000的起始位置
		PTYPE pType = (PTYPE)(pReloc + 1);
		for (int i = 0; i < uCount; i++)
		{
			if (pType->Type == 3)
			{
				PULONG pRelocPoint = (uBaseRva + pType->Offset + ZkBuf);
				//重定位后的地址 - 新基址 = 没重定位的地址 - 默认基址
				//所以：重定位后的地址 = 新基址 - 默认基址 + 没重定位的地址
				*pRelocPoint = uOffset + *pRelocPoint;
			}
			pType++;
		}
		pReloc = (PIMAGE_BASE_RELOCATION)((ULONG)pReloc + pReloc->SizeOfBlock);
	}

}
// 修复旧SSDT表
void FixSSDT(PUCHAR pZKBuf, PUCHAR OldBase, PIRP Irp)
{

	//新内核某位置1 - 新内核基址 = 老内核某位置1 - 老内核基址；
	//新内核某位置1 = 新内核基址 - 老内核基址 + 老内核某位置1;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	PULONG AddrArr = (PULONG)pOutBuff;

	LONG Offset = (ULONG)pZKBuf - (ULONG)OldBase;
	//1 得到新内核中的SSDT
	g_pNewSSDT = (PSSDTEntry)((LONG)&KeServiceDescriptorTable + Offset);

	//2 填充系统服务个数
	g_pNewSSDT->NumberOfServices = KeServiceDescriptorTable.NumberOfServices;

	//3 填充SSDT表
	g_pNewSSDT->ServiceTableBase = (ULONG*)((PUCHAR)KeServiceDescriptorTable.ServiceTableBase + Offset);
	//让所有的SSDT中保存的函数地址，都指向新内核
	for (int i = 0; i < g_pNewSSDT->NumberOfServices; i++)
	{
		AddrArr[i] = g_pNewSSDT->ServiceTableBase[i];
		g_pNewSSDT->ServiceTableBase[i] = g_pNewSSDT->ServiceTableBase[i] + Offset;
	}
	//4 填充参数表
	g_pNewSSDT->ParamTableBase = (PULONG)((PUCHAR)KeServiceDescriptorTable.ParamTableBase + Offset);
	memcpy(g_pNewSSDT->ParamTableBase,
		KeServiceDescriptorTable.ParamTableBase,
		g_pNewSSDT->NumberOfServices
	);
}
// 获取KiFastCallEntry函数
ULONG GetKiFastCallEntry()
{
	ULONG uAddress = 0;
	_asm
	{
		push eax;
		push ecx;
		mov ecx, 0x176;
		rdmsr;
		mov uAddress, eax;
		pop ecx;
		pop eax;
	}
	return uAddress;
}

// hook中过滤函数
ULONG FilterFun(ULONG SSdtBase, PULONG OldFun, ULONG Id)
{
	//如果相等，说明调用的是SSDT中的函数
	if (SSdtBase == (ULONG)KeServiceDescriptorTable.ServiceTableBase)
	{
		//使用思路：
		//假如进程是OD，并且函数调用是190号，就走新内核中的函数，这样通过hookOpenProcess就无法拦住OD了。
		return g_pNewSSDT->ServiceTableBase[Id];
	}
	return OldFun;

}

// inline Hook的回调函数
_declspec(naked)void MyHookFun()
{
	//eax 里面是调用号，edx里面是老函数地址，edi里面是SSDT基址
	_asm {
		pushad;
		pushfd;
		push eax;		//调用号
		push edx;		//原始函数地址
		push edi;		//SSDT基址

		call FilterFun;	// 自己的过滤函数，获取最真实函数地址  由于前面压入3个参数和pushfd,pushad ebx刚好在栈中的【esp+0x18]
		mov dword ptr ds : [esp + 0x18] , eax; // 【esp+0x18】 ebx的值，替换服务函数地址
		popfd;
		popad;			  // 恢复通用寄存器，ebx被替换成新的函数地址了
						  // 执行原始hook的5个字节
		sub     esp, ecx;
		shr     ecx, 2;
		jmp     g_JmpPoint;	// 跳转回原来函数地址
	}
}

// hook 目标的5个字节
UCHAR Old_Code[5] = { 0 };

// hookKiFastCallEntry函数
void OnHookKiFastCallEntry()
{
	// KiFastCallEntry中特征值
	char buf[] = { 0x2b, 0xe1, 0xc1, 0xe9, 0x02 };
	// 获取KiFastCallEntry函数地址
	ULONG KiFastCallEntryAdd = GetKiFastCallEntry();
	// 找到hook点
	pHookPoint = SearchMemory(buf, 5, (char*)KiFastCallEntryAdd, 0x200);
	// 绕过前5个字节，应为被hook替换了
	g_JmpPoint = (ULONG)(pHookPoint + 5);
	// 备份旧的5个字节
	memcpy(Old_Code, pHookPoint, 5);
	// 关闭页保护
	OffProtected();
	// jmp xxxxxx
	// 写入跳转目标地址   目标地址-指令所在-5
	pHookPoint[0] = 0xE9;
	*(ULONG*)(&pHookPoint[1]) = (ULONG)MyHookFun - (ULONG)pHookPoint - 5;
	// 开启页保护
	OnProtected();
}

// 内核重载开始
void KernelReload(PIRP Irp)
{
	PUCHAR pBuf = NULL;
	PUCHAR pZKBuf = NULL;
	UNICODE_STRING KernelName;

	//1 首先把内核文件读取到内存里 (默认开启PAE)
	ReadKernelToBuf(L"\\??\\C:\\Windows\\System32\\ntkrnlpa.exe", &pBuf);

	//2 把读到内存中的内核给展开成0x1000对齐
	ZKKernel(&pZKBuf, pBuf);
	ExFreePool(pBuf);

	//3 修复新内核的重定位 ，虽然开启PAE（ntkrnlpa.exe）,但是显示的名称 ntoskrnl.exe
	RtlInitUnicodeString(&KernelName, L"ntoskrnl.exe");
	ULONG32 uBase = MyGetModuleHandle(&KernelName);

	FixReloc(pZKBuf, (PUCHAR)uBase);

	//4 修复新的SSDT表
	FixSSDT(pZKBuf, (PUCHAR)uBase,Irp);

	//5 Hook掉KiFastCallEntry，在自己的Hook函数中判断应该走新内核还是老内核
	OnHookKiFastCallEntry();

}

// 卸载内核钩子
void UnHook()
{
	OffProtected();
	memcpy(pHookPoint, Old_Code, 5);
	OnProtected();
}
