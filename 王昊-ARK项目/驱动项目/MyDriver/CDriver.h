#pragma once
#include"MyStruct.h"

// ����������ѵ���Ϊ�����ĺ�������Ҫʹ����Ҫ�Լ�����
PCCH PsGetProcessImageFileName(PEPROCESS Process);
PEB* PsGetProcessPeb(PEPROCESS Process);

// ʹ�� CTL_CODE ���Դ���һ��������(�豸����, ��ʶ��, IO��ʽ, Ȩ��)
// ��ȡ��������
#define	CTL_GetDriverNum CTL_CODE(0x8001, 0x800, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
// ��������
#define	CTL_QueryDriver CTL_CODE(0x8001, 0x801, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
// ����ö�ٽ����߳���Ϣ
#define	CTL_QueryProcessAndThread CTL_CODE(0x8001, 0x802, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

// �����豸
NTSTATUS CreateDevice(IN PDRIVER_OBJECT	DriverObject)
{
	// �豸��������ƣ�Ҫ��������ֱ�����ϸ�ʽ \\Device\\xxx����ʾ��ǰ���豸����
	UNICODE_STRING DeviceName = { 0 };
	RtlInitUnicodeString(&DeviceName, L"\\Device\\king");

	// ����һ���豸���󣬲�ָ���豸����Ļ�������
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS Status = IoCreateDevice(
		DriverObject,					// *�豸�������������󣬴����ɹ���ᱻ��ӵ������� DeviceObject ������
		sizeof(DEVICE_EXTENSION),		// �豸��չ����Ĵ�С������ڴ�ᱻ DeviceExtension ָ��
		&DeviceName,					// *�豸��������ƣ����ں˿ɼ�
		FILE_DEVICE_UNKNOWN,			// ��ʾ����һ��Ӳ���޹ص��豸����
		0,								// �豸������ʱ����
		TRUE,							// ��ʾ��ǰ�豸�����Ƿ��Ƕ�ռ��
		&DeviceObject);					// *���ش������豸����

	// �ж�������豸�����Ƿ񴴽��ɹ������ʧ����������
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("�豸���󴴽�ʧ��\n");
		return Status;
	}

	// �豸���ƽ��ں˿ɼ�����Ҫ R3 ����������󶨵�һ������������
	UNICODE_STRING SymbolName = { 0 };
	RtlInitUnicodeString(&SymbolName, L"\\??\\king");
	Status = IoCreateSymbolicLink(&SymbolName, &DeviceName);

	// ������������������豸���޷��� R3 ͨ�ţ���ζ�Ŵ��豸û������
	if (!NT_SUCCESS(Status))
	{
		IoDeleteDevice(DeviceObject);
		DbgPrint("�������Ӵ���ʧ��\n");
		return Status;
	}

	// ���豸����Ļ�����Ϣ���浽�豸�������չ�ڴ���
	PDEVICE_EXTENSION Extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	Extension->Self = DeviceObject;
	Extension->SymbolName = SymbolName;
	Extension->DeviceName = DeviceName;

	// ��ʼ���ɹ��Ժ�����豸��ʾ���ڳ�ʼ���ı�־λ
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	// �����ɹ��󣬿���ͨ������ Flags ָ����д�ķ�ʽ����������д��ʽ�����ڶ��ٶ�
	//	Ҫ�󲻸ߣ��Դ��䳤��Ҫ�󲻴�������ʵ���ڴ������ݵĹ����У�IO�������ᴴ��
	//	һ���µĻ������������ݿ������ں˲㣬�򿽱����û���
	DeviceObject->Flags |= DO_BUFFERED_IO;

	return Status;
}


// ��ȡ��������
UINT32 MyGetDriverNumber(PDRIVER_OBJECT DriverObject)
{
	// ����ṹ��
	PLDR_DATA_TABLE_ENTRY Current = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	PLDR_DATA_TABLE_ENTRY Items = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	UINT32 DriverNum=0;
	do {
		// ���б������
		DriverNum++;
		Items = (PLDR_DATA_TABLE_ENTRY)Items->InLoadOrderLinks.Flink;
	} while (Current != Items);
	return DriverNum;
}

// ��������
NTSTATUS MyQueryDriver(PDRIVER_OBJECT DriverObject, PWCHAR Buffer)
{
	// ����ṹ��
	PLDR_DATA_TABLE_ENTRY Current = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	PLDR_DATA_TABLE_ENTRY Items = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	do {
		// ���б������
		RtlCopyMemory(Buffer, (PVOID)Items->BaseDllName.Buffer, Items->BaseDllName.Length);
		Items = (PLDR_DATA_TABLE_ENTRY)Items->InLoadOrderLinks.Flink;
		Buffer = (char*)Buffer + 0x100;
	} while (Current != Items);
	return STATUS_SUCCESS;
}


// ����ö�ٻ�ȡ�����߳���Ϣ
NTSTATUS MyQueryProcessAndThreaInfo(PVOID ProcessBuffer)
{
	// ������̺��̵߳Ľṹ��ָ�룬���ڽ����ҵ�������
	PEPROCESS Process;
	PETHREAD Thread;

	// ͨ������ö�ٿ���ʹ�õ����� id�������Ҷ�Ӧ�Ľ��̻��߳�
	for (ULONG id = 0; id < 5000; id += 4)
	{
		// ���ͨ�� id �ҵ��˽��̣������������ص���Ϣ
		if (NT_SUCCESS(PsLookupProcessByProcessId(UlongToHandle(id), &Process)))
		{
			// ��ָ���Ľ��̶����л�ȡ���Ʋ����
			UNICODE_STRING ProcessInfo;
			ProcessInfo.MaximumLength = 0x30;
			RtlUnicodeStringPrintf(&ProcessInfo, L"[%08X][Process]: %s", id, PsGetProcessImageFileName(Process));
			RtlCopyMemory(ProcessBuffer, ProcessInfo.Buffer, sizeof(ProcessInfo.Buffer));
			// PsLookupProcessByProcessId ���������ü���������Ҫ��ȥ
			ProcessBuffer = (char*)ProcessBuffer + 0x30;
			ObDereferenceObject(Process);
		}
		// ������ǽ��̣����п������̣߳������ܽ��̺��̵߳� id ��ͬ
		else if (NT_SUCCESS(PsLookupThreadByThreadId(UlongToHandle(id), &Thread)))
		{
			// ͨ���߳̽ṹ���ҵ������Ľ��̣������������Ϣ
			PEPROCESS Process2 = IoThreadToProcess(Thread);
			UNICODE_STRING ThreadInfo;
			ThreadInfo.MaximumLength = 0x30;
			RtlUnicodeStringPrintf(&ThreadInfo, L"[%08X][Thread]: %d", id, PsGetProcessId(Process2));
			RtlCopyMemory(ProcessBuffer, ThreadInfo.Buffer, sizeof(ThreadInfo.Buffer));
			ProcessBuffer = (char*)ProcessBuffer + 0x30;
			// PsLookupProcessByProcessId ���������ü���������Ҫ��ȥ
			ObDereferenceObject(Thread);
		}
	}

	return STATUS_SUCCESS;
}

// ����ģ�� 
NTSTATUS MyQueryModuleInfo(SIZE_T Pid)
{
	PVOID Peb = NULL;
	struct _LIST_ENTRY pLdrHeader;
	NTSTATUS status;
	// ���̿�
	PEPROCESS pProc = NULL;
	status = PsLookupProcessByProcessId((HANDLE)Pid, &pProc);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("��ȡ����EPROCESSʧ��\n");
		return status;
	}
	_try
	{
		// ��ȡPEB
		Peb = PsGetProcessPeb(pProc);
		// ��ǰ�߳��л����½��̶���
		KeAttachProcess(pProc);
		// ��ȡLDR��
		pLdrHeader = ((PPEB)Peb)->Ldr->InLoadOrderModuleList;
		// ��ȡ����ʼ
		struct _LIST_ENTRY* pTemp = pLdrHeader.Flink;
		struct _LIST_ENTRY* pNext = pLdrHeader.Flink;
		do
		{
			// ��ȡģ����Ϣ
			struct _LDR_DATA_TABLE_ENTRY pLdrTable =
				*(PLDR_DATA_TABLE_ENTRY)(pNext->Flink);
			//��ȡģ����Ϣ
			DbgPrint("ExeName=%wZ\n", &pLdrTable.BaseDllName);
			DbgPrint("��ַ=%08x \n", pLdrTable.DllBase);

			pNext = pNext->Flink;

		} while (pNext != pTemp);
	
	}
	_except(EXCEPTION_EXECUTE_HANDLER){}

	// ���̴߳�Process����ת�ص�ԭ���Ľ���
	KeDetachProcess();
	// �ݼ��Դ����ü���
	ObDereferenceObject(pProc);
	return 0;
}

// Ĭ����ǲ����
NTSTATUS CommonDispatchRoutine(
	IN	PDEVICE_OBJECT	DeviceObject,						// ���յ���Ϣ���豸����
	IN	PIRP			Irp)								// ������Ǿ�����Ϣ������
{
	UNREFERENCED_PARAMETER(DeviceObject);

	// 1. ������Ϣ�Ĵ���״̬�����ɹ���ʧ�ܣ����ʧ���ˣ���Ϊʲôʧ�� [GetLastError]
	Irp->IoStatus.Status = STATUS_SUCCESS;

	// 2. ������Ϣ������ֽڳ��ȣ�����д�ĳ��� [Bytes]
	Irp->IoStatus.Information = 0;

	// 3. һ��������ɣ���Ҫ�������󷽣���������� [����]
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// 4. ���� IO ��������ǰ�� IRP �Ƿ���ɹ�
	return STATUS_SUCCESS;
}



// �Զ�����ǲ����
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
	// ��ȡ��������
	case CTL_GetDriverNum:  
	{
		PVOID DriverNum = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		RtlCopyMemory(DriverNum, "Success",8);
		Irp->IoStatus.Information = MyGetDriverNumber(DeviceObject->DriverObject);
		break;
	}
	// ��������
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