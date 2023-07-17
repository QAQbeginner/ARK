#pragma once
#include"MyStruct.h"

// 这个函数是已导出为声明的函数，想要使用需要自己声明
PCCH PsGetProcessImageFileName(PEPROCESS Process);
PEB* PsGetProcessPeb(PEPROCESS Process);

// 使用 CTL_CODE 可以创建一个控制码(设备类型, 标识符, IO方式, 权限)
// 获取驱动数量
#define	CTL_GetDriverNum CTL_CODE(0x8001, 0x800, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
// 遍历驱动
#define	CTL_QueryDriver CTL_CODE(0x8001, 0x801, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
// 暴力枚举进程线程信息
#define	CTL_QueryProcessAndThread CTL_CODE(0x8001, 0x802, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

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
UINT32 MyGetDriverNumber(PDRIVER_OBJECT DriverObject)
{
	// 定义结构体
	PLDR_DATA_TABLE_ENTRY Current = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	PLDR_DATA_TABLE_ENTRY Items = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	UINT32 DriverNum=0;
	do {
		// 进行遍历输出
		DriverNum++;
		Items = (PLDR_DATA_TABLE_ENTRY)Items->InLoadOrderLinks.Flink;
	} while (Current != Items);
	return DriverNum;
}

// 遍历驱动
NTSTATUS MyQueryDriver(PDRIVER_OBJECT DriverObject, PWCHAR Buffer)
{
	// 定义结构体
	PLDR_DATA_TABLE_ENTRY Current = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	PLDR_DATA_TABLE_ENTRY Items = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	do {
		// 进行遍历输出
		RtlCopyMemory(Buffer, (PVOID)Items->BaseDllName.Buffer, Items->BaseDllName.Length);
		Items = (PLDR_DATA_TABLE_ENTRY)Items->InLoadOrderLinks.Flink;
		Buffer = (char*)Buffer + 0x100;
	} while (Current != Items);
	return STATUS_SUCCESS;
}


// 暴力枚举获取进程线程信息
NTSTATUS MyQueryProcessAndThreaInfo(PVOID ProcessBuffer)
{
	// 定义进程和线程的结构体指针，用于接收找到的内容
	PEPROCESS Process;
	PETHREAD Thread;

	// 通过暴力枚举可能使用的所有 id，来查找对应的进程或线程
	for (ULONG id = 0; id < 5000; id += 4)
	{
		// 如果通过 id 找到了进程，就输出进程相关的信息
		if (NT_SUCCESS(PsLookupProcessByProcessId(UlongToHandle(id), &Process)))
		{
			// 从指定的进程对象中获取名称并输出
			UNICODE_STRING ProcessInfo;
			ProcessInfo.MaximumLength = 0x30;
			RtlUnicodeStringPrintf(&ProcessInfo, L"[%08X][Process]: %s", id, PsGetProcessImageFileName(Process));
			RtlCopyMemory(ProcessBuffer, ProcessInfo.Buffer, sizeof(ProcessInfo.Buffer));
			// PsLookupProcessByProcessId 会增加引用计数，用完要减去
			ProcessBuffer = (char*)ProcessBuffer + 0x30;
			ObDereferenceObject(Process);
		}
		// 如果不是进程，还有可能是线程，不可能进程和线程的 id 相同
		else if (NT_SUCCESS(PsLookupThreadByThreadId(UlongToHandle(id), &Thread)))
		{
			// 通过线程结构体找到所属的进程，并输出基本信息
			PEPROCESS Process2 = IoThreadToProcess(Thread);
			UNICODE_STRING ThreadInfo;
			ThreadInfo.MaximumLength = 0x30;
			RtlUnicodeStringPrintf(&ThreadInfo, L"[%08X][Thread]: %d", id, PsGetProcessId(Process2));
			RtlCopyMemory(ProcessBuffer, ThreadInfo.Buffer, sizeof(ThreadInfo.Buffer));
			ProcessBuffer = (char*)ProcessBuffer + 0x30;
			// PsLookupProcessByProcessId 会增加引用计数，用完要减去
			ObDereferenceObject(Thread);
		}
	}

	return STATUS_SUCCESS;
}

// 遍历模块 
NTSTATUS MyQueryModuleInfo(SIZE_T Pid)
{
	PVOID Peb = NULL;
	struct _LIST_ENTRY pLdrHeader;
	NTSTATUS status;
	// 进程块
	PEPROCESS pProc = NULL;
	status = PsLookupProcessByProcessId((HANDLE)Pid, &pProc);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("获取进程EPROCESS失败\n");
		return status;
	}
	_try
	{
		// 获取PEB
		Peb = PsGetProcessPeb(pProc);
		// 当前线程切换到新进程对象
		KeAttachProcess(pProc);
		// 获取LDR链
		pLdrHeader = ((PPEB)Peb)->Ldr->InLoadOrderModuleList;
		// 获取链表开始
		struct _LIST_ENTRY* pTemp = pLdrHeader.Flink;
		struct _LIST_ENTRY* pNext = pLdrHeader.Flink;
		do
		{
			// 获取模块信息
			struct _LDR_DATA_TABLE_ENTRY pLdrTable =
				*(PLDR_DATA_TABLE_ENTRY)(pNext->Flink);
			//获取模块信息
			DbgPrint("ExeName=%wZ\n", &pLdrTable.BaseDllName);
			DbgPrint("基址=%08x \n", pLdrTable.DllBase);

			pNext = pNext->Flink;

		} while (pNext != pTemp);
	
	}
	_except(EXCEPTION_EXECUTE_HANDLER){}

	// 将线程从Process进程转回到原来的进程
	KeDetachProcess();
	// 递减以此引用计数
	ObDereferenceObject(pProc);
	return 0;
}

// 默认派遣函数
NTSTATUS CommonDispatchRoutine(
	IN	PDEVICE_OBJECT	DeviceObject,						// 接收到消息的设备对象
	IN	PIRP			Irp)								// 保存的是具体消息的内容
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



// 自定义派遣函数
NTSTATUS IoCtlDispatchRoutine(
	IN	PDEVICE_OBJECT	DeviceObject,
	IN	PIRP			Irp)
{
	DbgBreakPoint();
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG IoControlCode = Stack->Parameters.DeviceIoControl.IoControlCode;
	// ULONG InputBufferLength = Stack->Parameters.DeviceIoControl.InputBufferLength;
	// ULONG OutputBufferLength = Stack->Parameters.DeviceIoControl.OutputBufferLength;

	Irp->IoStatus.Status = STATUS_SUCCESS;

	switch (IoControlCode)
	{
	// 获取驱动数量
	case CTL_GetDriverNum:  
	{
		PVOID DriverNum = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		RtlCopyMemory(DriverNum, "Success",8);
		Irp->IoStatus.Information = MyGetDriverNumber(DeviceObject->DriverObject);
		break;
	}
	// 遍历驱动
	case CTL_QueryDriver:
	{
		PWCHAR Buffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		MyQueryDriver(DeviceObject->DriverObject,Buffer);
		break;
	}
	case CTL_QueryProcessAndThread:
	{
		PVOID Buffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		MyQueryProcessAndThreaInfo(Buffer);
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