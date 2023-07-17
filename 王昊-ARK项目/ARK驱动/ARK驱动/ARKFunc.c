#include"ARKFunc.h"


PPEB PsGetProcessPeb(PEPROCESS proc)
{
	PPEB res = (PPEB)(*(ULONG*)((ULONG)proc + 0x1a8));
	return res;
}
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
NTSTATUS MyGetDriverNumber(PDRIVER_OBJECT DriverObject, PIRP Irp)
{
	// ����ṹ��
	PLDR_DATA_TABLE_ENTRY Current = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	PLDR_DATA_TABLE_ENTRY Items = (PLDR_DATA_TABLE_ENTRY)(DriverObject->DriverSection);
	UINT32 DriverNum = 0;
	do {
		// ���б���
		DriverNum++;
		Items = (PLDR_DATA_TABLE_ENTRY)Items->InLoadOrderLinks.Flink;
	} while (Current != Items);
	Irp->IoStatus.Information = DriverNum;
	return STATUS_SUCCESS;
}
// ��������
NTSTATUS MyQueryDriver(PDRIVER_OBJECT DriverObject, PIRP Irp, ULONG outputSize)
{
	//��ȡ������
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
			//��ȡ������
			RtlCopyMemory(pOutDriverInfo->driverName, pDriverInfo->BaseDllName.Buffer, pDriverInfo->BaseDllName.Length);
			//��ȡ����·��
			RtlCopyMemory(pOutDriverInfo->driverPath, pDriverInfo->FullDllName.Buffer, pDriverInfo->FullDllName.Length);
			//��ȡ������ַ
			pOutDriverInfo->DllBase = pDriverInfo->DllBase;
			//��ȡ������С
			pOutDriverInfo->SizeOfImage = pDriverInfo->SizeOfImage;
		}
		pOutDriverInfo++;

	} while (pTemp != &pLdr->InLoadOrderLinks);

	return STATUS_SUCCESS;
}
// ��������
NTSTATUS MyHideDriver(PDEVICE_OBJECT pDevice,PIRP Irp)
{
	//��ȡ������
	PLDR_DATA_TABLE_ENTRY pLdr = (PLDR_DATA_TABLE_ENTRY)pDevice->DriverObject->DriverSection;
	PLIST_ENTRY pTemp = &pLdr->InLoadOrderLinks;
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//��ʼ���ַ���,��ȡҪ���ص�������
	UNICODE_STRING pHideDriverName = { 0 };
	RtlInitUnicodeString(&pHideDriverName, (PWCHAR)pInputBuff);

	do
	{
		PLDR_DATA_TABLE_ENTRY pDriverInfo = (PLDR_DATA_TABLE_ENTRY)pTemp;
		if (RtlCompareUnicodeString(&pDriverInfo->BaseDllName, &pHideDriverName, FALSE) == 0)
		{
			//�޸�Flink��Blinkָ�룬����������Ҫ���ص�����
			//ʹҪ���ص���������һ������ָ���ǰһ������Ϊ����Ҫ����������ǰһ������
			pTemp->Blink->Flink = pTemp->Flink;
			//ʹҪ���ص�������ǰһ������ָ�����һ������Ϊ����Ҫ������������һ������
			pTemp->Flink->Blink = pTemp->Blink;
			pTemp->Flink = (PLIST_ENTRY)&pTemp->Flink;
			pTemp->Blink = (PLIST_ENTRY)&pTemp->Blink;

			break;
		}
		pTemp = pTemp->Blink;

	} while (pTemp != &pLdr->InLoadOrderLinks);
	return STATUS_SUCCESS;
}


// ��ȡ��������
NTSTATUS MyGetProcessNumber(PIRP Irp)
{
	// ������̺��̵߳Ľṹ��ָ�룬���ڽ����ҵ�������
	PEPROCESS Process;
	ULONG nCount = 0;
	//��ȡ��ǰ���̵�EPROCESS
	PEPROCESS proc = PsGetCurrentProcess();
	//��ȡ�������
	PLIST_ENTRY pTemp = (PLIST_ENTRY)((ULONG)proc + 0xb8);
	do
	{
		pTemp = pTemp->Blink;
		nCount++;
	} while (pTemp != (PLIST_ENTRY)((ULONG)proc + 0xb8));
	Irp->IoStatus.Information = nCount;
	return STATUS_SUCCESS;
}
// ��������
NTSTATUS MyQueryProcess(PIRP Irp, ULONG outputSize)
{
	// ������̺��̵߳Ľṹ��ָ�룬���ڽ����ҵ�������
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	RtlFillMemory(pOutBuff, outputSize, 0);
	PPROCESS_INFO pOutProcessInfo = (PPROCESS_INFO)pOutBuff;
	if (pOutProcessInfo == NULL)
		return STATUS_SUCCESS;
	//��ȡ��ǰ���̵�EPROCESS
	PEPROCESS proc = PsGetCurrentProcess();
	//��ȡ�������
	PLIST_ENTRY pTemp = (PLIST_ENTRY)((ULONG)proc + 0xb8);

	//�����������ȫ��0
	RtlFillMemory(pOutBuff, outputSize, 0);

	do
	{
		//ѭ����ȡ��һ����̿�
		PEPROCESS pProcessInfo = (PEPROCESS)((ULONG)pTemp - 0xb8);
		pTemp = pTemp->Blink;

		//�������·���ַ����ṹ���ַ
		PUNICODE_STRING pName = (PUNICODE_STRING)(*(ULONG*)((ULONG)pProcessInfo + 0x1ec));

		//��ȡPEPROCESS��SectionObject��ֵ
		//����ֵΪ��ʱ˵���˽����Ѿ�����
		PVOID SecObj = (PVOID)(*(ULONG*)((ULONG)pProcessInfo + 0x128));
		if (pName != 0 && SecObj != NULL)
		{
			//��ȡ����ID
			pOutProcessInfo->PID = (PVOID)(*(ULONG*)((ULONG)pProcessInfo + 0xb4));
			//��ȡEPROCESS
			pOutProcessInfo->EProcess = (PVOID)pProcessInfo;
			//��ȡ����·��
			RtlCopyMemory(pOutProcessInfo->ProcessName, pName->Buffer, pName->Length);
		}
		pOutProcessInfo++;

	} while (pTemp != (PLIST_ENTRY)((ULONG)proc + 0xb8));
	return STATUS_SUCCESS;
}
// ���ؽ���
NTSTATUS MyHindProcess(PIRP Irp)
{
	//��ȡ��ǰ���̵�EPROCESS
	PEPROCESS proc = PsGetCurrentProcess();
	//��ȡ�������
	PLIST_ENTRY pTemp = (PLIST_ENTRY)((ULONG)proc + 0xb8);
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	do
	{
		//ѭ����ȡ��һ����̿�
		PEPROCESS pProcessInfo = (PEPROCESS)((ULONG)pTemp - 0xb8);

		//��ȡPEPROCESS��SectionObject��ֵ
		//����ֵΪ��ʱ˵���˽����Ѿ�����
		PVOID SecObj = (PVOID)(*(ULONG*)((ULONG)pProcessInfo + 0x128));
		if (SecObj != NULL)
		{
			//�������Ľ���id
			ULONG pid = *(ULONG*)((ULONG)pProcessInfo + 0xb4);
			//Ҫ���صĽ���id
			ULONG fid = *(ULONG*)pInputBuff;
			//�ȽϽ���id
			if (pid == fid)
			{
				//�޸�Flink��Blinkָ�룬����������Ҫ���ص�����
				//ʹҪ���ص���������һ������ָ���ǰһ������Ϊ����Ҫ����������ǰһ������
				pTemp->Blink->Flink = pTemp->Flink;
				//ʹҪ���ص�������ǰһ������ָ�����һ������Ϊ����Ҫ������������һ������
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
// ��������
NTSTATUS MyDelProcess(PIRP Irp)
{
	HANDLE hProcess = NULL;
	CLIENT_ID ClientId = { 0 };
	OBJECT_ATTRIBUTES objAttribut = { sizeof(OBJECT_ATTRIBUTES) };
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//Ҫ�����Ľ��̵�id
	ClientId.UniqueProcess = *(HANDLE*)pInputBuff;
	ClientId.UniqueThread = 0;
	//�򿪽��̣���������Ч�����������
	ZwOpenProcess(
		&hProcess,	//���ش򿪵ľ��
		1,			//����Ȩ��
		&objAttribut,	//��������
		&ClientId	//����ID�ṹ
	);
	if (hProcess)
	{
		//��������
		ZwTerminateProcess(hProcess, 0);
		//�رվ��
		ZwClose(hProcess);
	}

	return STATUS_SUCCESS;
}
// ��ȡ�߳�����
NTSTATUS MyGetThreadNumber(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//��ȡҪ�鿴ģ�����ڽ��̵Ľ��̿�EPROCESS
	PULONG PID = (PULONG)pInputBuff;
	ULONG nCount = 0;
	PETHREAD Thread;

	// ͨ������ö�ٿ���ʹ�õ����� id�������Ҷ�Ӧ�Ľ��̻��߳�
	for (ULONG id = 0; id < 5000; id += 4)
	{
		// ������ǽ��̣����п������̣߳������ܽ��̺��̵߳� id ��ͬ
		if (id != 0)
		{
			if (NT_SUCCESS(PsLookupThreadByThreadId(UlongToHandle(id), &Thread)))
			{
				// ͨ���߳̽ṹ���ҵ������Ľ��̣������������Ϣ
				PEPROCESS Process2 = IoThreadToProcess(Thread);
				if (PsGetProcessId(Process2) == ULongToHandle(*PID))
					nCount++;
				// PsLookupProcessByProcessId ���������ü���������Ҫ��ȥ
				ObDereferenceObject(Thread);
			}
		}
	}

	Irp->IoStatus.Information = nCount;

	return STATUS_SUCCESS;
}
// �����߳�
NTSTATUS MyQueryThread(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//��ȡҪ�鿴ģ�����ڽ��̵Ľ��̿�EPROCESS
	PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	//�����������ȫ��0
	RtlFillMemory(pOutBuff, outputSize, 0);

	PTHREAD_INFO pOutThreadInfo = (PTHREAD_INFO)pOutBuff;
	if (pOutThreadInfo == NULL)
		return STATUS_SUCCESS;
	//��ȡҪ�鿴ģ�����ڽ��̵Ľ��̿�EPROCESS
	PULONG PID = (PULONG)pInputBuff;
	ULONG nCount = 0;
	PETHREAD Thread;

	// ͨ������ö�ٿ���ʹ�õ����� id�������Ҷ�Ӧ�Ľ��̻��߳�
	for (ULONG id = 0; id < 5000; id += 4)
	{
		if (id != 0)
		{
			// ������ǽ��̣����п������̣߳������ܽ��̺��̵߳� id ��ͬ
			if (NT_SUCCESS(PsLookupThreadByThreadId(UlongToHandle(id), &Thread)))
			{
				// ͨ���߳̽ṹ���ҵ������Ľ��̣������������Ϣ
				PEPROCESS Process2 = IoThreadToProcess(Thread);
				if (PsGetProcessId(Process2) == ULongToHandle(*PID))
				{
					pOutThreadInfo->Tid = id;
					pOutThreadInfo++;
				}
				// PsLookupProcessByProcessId ���������ü���������Ҫ��ȥ
				ObDereferenceObject(Thread);
			}
		}
	}

	Irp->IoStatus.Information = nCount;

	return STATUS_SUCCESS;
}
// ��ȡģ������
NTSTATUS MyGetModuleNumber(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	//��ȡҪ�鿴ģ�����ڽ��̵Ľ��̿�EPROCESS
	PEPROCESS proc = (PEPROCESS)(*(ULONG*)pInputBuff);
	ULONG nCount = 0;
	//���̹ҿ�
	KeAttachProcess(proc);

	//��ȡPEB��ַ
	PPEB peb = PsGetProcessPeb(proc);

	//��ȡģ������
	PLIST_ENTRY pTemp = (PLIST_ENTRY)(*(ULONG*)((ULONG)peb + 0xc) + 0xc);

	do
	{
		pTemp = pTemp->Flink;
		nCount++;
	} while (pTemp != (PLIST_ENTRY)(*(ULONG*)((ULONG)peb + 0xc) + 0xc));

	Irp->IoStatus.Information = nCount;
	//����ҿ�
	KeDetachProcess();

	return STATUS_SUCCESS;
}
// ����ģ��
NTSTATUS MyQueryModule(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == NULL)
		return STATUS_SUCCESS;
	//��ȡҪ�鿴ģ�����ڽ��̵Ľ��̿�EPROCESS
	PEPROCESS proc = (PEPROCESS)(*(ULONG*)pInputBuff);

	//���̹ҿ�
	KeAttachProcess(proc);

	//��ȡPEB��ַ
	PPEB peb = PsGetProcessPeb(proc);

	//��ȡģ������
	PLIST_ENTRY pTemp = (PLIST_ENTRY)(*(ULONG*)((ULONG)peb + 0xc) + 0xc);

	//�����������ȫ��0
	RtlFillMemory(pOutBuff, outputSize, 0);

	PMODULE_INFO pOutModuleInfo = (PMODULE_INFO)pOutBuff;

	do
	{
		//ѭ����ȡ��һ��ģ��
		PLDR_DATA_TABLE_ENTRY pModuleInfo = (PLDR_DATA_TABLE_ENTRY)pTemp;
		pTemp = pTemp->Flink;

		if (pModuleInfo->FullDllName.Buffer != 0)
		{
			//��ȡģ��·��
			RtlCopyMemory(pOutModuleInfo->wcModuleFullPath, pModuleInfo->FullDllName.Buffer, pModuleInfo->FullDllName.Length);
			//��ȡģ���ַ
			pOutModuleInfo->DllBase = pModuleInfo->DllBase;
			//��ȡģ���С
			pOutModuleInfo->SizeOfImage = pModuleInfo->SizeOfImage;
		}

		pOutModuleInfo++;

	} while (pTemp != (PLIST_ENTRY)(*(ULONG*)((ULONG)peb + 0xc) + 0xc));

	//����ҿ�
	KeDetachProcess();

	return STATUS_SUCCESS;
}



// ��ȡ��һ���ļ�
NTSTATUS FindFirstFile(const WCHAR* pszPath, HANDLE* phFile, FILE_BOTH_DIR_INFORMATION* pFileInfo, ULONG nInfoSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	// 1. ���ļ���,�õ��ļ��е��ļ����
	HANDLE hFile = NULL;				//�����ļ����
	OBJECT_ATTRIBUTES oa = { 0 };
	UNICODE_STRING path;	//�����ļ�·��
	RtlInitUnicodeString(&path, pszPath);

	InitializeObjectAttributes(
		&oa,/*Ҫ��ʼ���Ķ������Խṹ��*/
		&path,/*�ļ�·��*/
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,/*����:·�������ִ�Сд,�򿪵ľ�����ں˾��*/
		NULL,
		NULL);

	IO_STATUS_BLOCK isb = { 0 };		//���溯���Ĳ������
	status = ZwCreateFile(
		&hFile,/*������ļ����*/
		GENERIC_READ,
		&oa,/*��������,��Ҫ��ǰ���ļ���·����ʼ����ȥ*/
		&isb,
		NULL,/*�ļ�Ԥ�����С*/
		FILE_ATTRIBUTE_NORMAL,/*�ļ�����*/
		FILE_SHARE_READ,/*����ʽ*/
		FILE_OPEN,/*��������: �������*/
		FILE_DIRECTORY_FILE,/*����ѡ��: Ŀ¼�ļ�*/
		NULL,
		0);

	if (!NT_SUCCESS(isb.Status)) {
		return isb.Status;
	}

	// 2. ͨ���ļ��е��ļ������ѯ�ļ����µ��ļ���Ϣ.
	status = ZwQueryDirectoryFile(
		hFile,
		NULL,/*�����첽IO*/
		NULL,
		NULL,
		&isb,
		pFileInfo,/*�����ļ���Ϣ�Ļ�����*/
		nInfoSize,/*���������ֽ���.*/
		FileBothDirectoryInformation,/*Ҫ��ȡ����Ϣ������*/
		TRUE,/*�Ƿ�ֻ����һ���ļ���Ϣ*/
		NULL,/*���ڹ����ļ��ı��ʽ: *.txt*/
		TRUE/*�Ƿ����¿�ʼɨ��,TRUE��Ŀ¼�еĵ�һ����Ŀ��ʼ,FALSE���ϴκ��п�ʼ�ָ�ɨ��*/
	);
	if (!NT_SUCCESS(isb.Status)) {
		return isb.Status;
	}
	// �����ļ����
	*phFile = hFile;
	return STATUS_SUCCESS;
}
// ��ȡ��һ���ļ�
NTSTATUS FindNextFile(HANDLE hFile, FILE_BOTH_DIR_INFORMATION* pFileInfo, ULONG nInfoSize)
{
	IO_STATUS_BLOCK isb = { 0 };	//���溯���Ĳ������
	ZwQueryDirectoryFile(
		hFile,
		NULL,/*�����첽IO*/
		NULL,
		NULL,
		&isb,
		pFileInfo,/*�����ļ���Ϣ�Ļ�����*/
		nInfoSize,/*���������ֽ���.*/
		FileBothDirectoryInformation,/*Ҫ��ȡ����Ϣ������*/
		TRUE,/*�Ƿ�ֻ����һ���ļ���Ϣ*/
		NULL,/*���ڹ����ļ��ı��ʽ: *.txt*/
		FALSE/*�Ƿ����¿�ʼɨ��,TRUE��Ŀ¼�еĵ�һ����Ŀ��ʼ,FALSE���ϴκ��п�ʼ�ָ�ɨ��*/
	);
	return isb.Status;
}
// ��ȡ�ļ�����
NTSTATUS MyGetFileNumber(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	UNICODE_STRING ustrFolder = { 0 };
	WCHAR strSymbol[0x512] = L"\\??\\";
	WCHAR strSymbolLast[2] = L"\\";
	//��·����װΪ���ӷ�����
	wcscat_s(strSymbol, 0x512, (PWCHAR)pInputBuff);
	wcscat_s(strSymbol, 0x512, strSymbolLast);
	RtlInitUnicodeString(&ustrFolder, strSymbol);
	ULONG nCount = 0;

	HANDLE hFile = NULL;
	//�������266*2����������ļ�����
	char buff[sizeof(FILE_BOTH_DIR_INFORMATION) + 266 * 2];
	FILE_BOTH_DIR_INFORMATION* pFileInfo = (FILE_BOTH_DIR_INFORMATION*)buff;

	NTSTATUS status = FindFirstFile(
		ustrFolder.Buffer,
		&hFile,
		pFileInfo,
		sizeof(buff));
	if (!NT_SUCCESS(status)) {
		DbgPrint("���ҵ�һ���ļ�ʧ��:0x%08X\n", status);
		return status;
	}
	do
	{
		// DbgPrint("�ļ���: %ls\n", pFileInfo->FileName);
		nCount++;
	} while (STATUS_SUCCESS == FindNextFile(hFile, pFileInfo, sizeof(buff)));

	Irp->IoStatus.Information = nCount;

	return STATUS_SUCCESS;

}
// �����ļ�
NTSTATUS MyQueryFile(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	UNICODE_STRING ustrFolder = { 0 };
	WCHAR strSymbol[0x512] = L"\\??\\";
	WCHAR strSymbolLast[2] = L"\\";
	//��·����װΪ���ӷ�����
	wcscat_s(strSymbol, 0x512, (PWCHAR)pInputBuff);
	wcscat_s(strSymbol, 0x512, strSymbolLast);
	RtlInitUnicodeString(&ustrFolder, strSymbol);
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == NULL)
		return STATUS_SUCCESS;
	PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;

	//�����������ȫ��0
	RtlFillMemory(pOutBuff, outputSize, 0);

	PFILE_INFO pOutFileInfo = (PFILE_INFO)pOutBuff;

	HANDLE hFile = NULL;
	//�������266*2����������ļ�����
	char buff[sizeof(FILE_BOTH_DIR_INFORMATION) + 266 * 2];
	FILE_BOTH_DIR_INFORMATION* pFileInfo = (FILE_BOTH_DIR_INFORMATION*)buff;

	NTSTATUS status = FindFirstFile(
		ustrFolder.Buffer,
		&hFile,
		pFileInfo,
		sizeof(buff));
	if (!NT_SUCCESS(status)) {
		DbgPrint("���ҵ�һ���ļ�ʧ��:0x%08X\n", status);
		return status;
	}

	do
	{
		// DbgPrint("�ļ���: %ls\n", pFileInfo->FileName);
		//�ļ���
		RtlCopyMemory(pOutFileInfo->wcFileName, pFileInfo->FileName, pFileInfo->FileNameLength);
		//����ʱ��
		pOutFileInfo->CreateTime = pFileInfo->CreationTime;
		//�޸�ʱ��
		pOutFileInfo->ChangeTime = pFileInfo->ChangeTime;
		//�ļ���С
		pOutFileInfo->Size = pFileInfo->EndOfFile.QuadPart;

		//�ļ���Ŀ¼�����ļ�
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
// ɾ���ļ�
NTSTATUS DelFile(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	UNICODE_STRING ustrFolder = { 0 };
	WCHAR strSymbol[0x512] = L"\\??\\";

	//��·����װΪ���ӷ�����
	wcscat_s(strSymbol, 0x512, (PWCHAR)pInputBuff);
	RtlInitUnicodeString(&ustrFolder, strSymbol);

	//��ʼ��OBJECT_ATTRIBUTES������
	OBJECT_ATTRIBUTES oa = { 0 };

	InitializeObjectAttributes(
		&oa,/*Ҫ��ʼ���Ķ������Խṹ��*/
		&ustrFolder,/*�ļ�·��*/
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,/*����:·�������ִ�Сд,�򿪵ľ�����ں˾��*/
		NULL,
		NULL);

	//ɾ��ָ���ļ�
	ZwDeleteFile(&oa);

	return STATUS_SUCCESS;
}



// ��ȡע�����Ŀ����
NTSTATUS MyGetRegNumber(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;

	// 1. �򿪴�·��.�õ�·����Ӧ�ľ��.
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
	//��ע���
	status = ZwOpenKey(&hKey,
		GENERIC_ALL,
		&objAtt);
	if (status != STATUS_SUCCESS) {
		return status;
	}
	//��ѯVALUE�Ĵ�С
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
// ����ע���
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
		DbgPrint("��ȡMdlΪ��");
		return STATUS_SUCCESS;
	}
	// 1. �򿪴�·��.�õ�·����Ӧ�ľ��.
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
	//��ע���
	status = ZwOpenKey(&hKey,
		GENERIC_ALL,
		&objAtt);
	if (status != STATUS_SUCCESS) {
		return status;
	}
	//��ѯVALUE�Ĵ�С
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

	//�����������ȫ��0
	RtlFillMemory(pOutBuff, outputSize, 0);

	PREG_INFO pReg = (PREG_INFO)pOutBuff;
	ULONG nCount = 0;
	//�������������
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
		//��ѯ����VALUE�Ĵ�С
		ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, NULL, 0, &size);
		PKEY_VALUE_FULL_INFORMATION pValueInfo = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, size, 'uin');
		if (pValueInfo == NULL)
			return STATUS_SUCCESS;
		//��ѯ����VALUE������
		ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, pValueInfo, size, &size);
		//��ȡ��ֵ������
		RtlCopyMemory(pReg[nCount].ValueName, pValueInfo->Name, pValueInfo->NameLength);
		//��ȡֵ������
		pReg[nCount].ValueType = pValueInfo->Type;
		//��ȡֵ������
		RtlCopyMemory(pReg[nCount].Value, (PVOID)((ULONG)pValueInfo + pValueInfo->DataOffset), pValueInfo->DataLength);
		//��ȡֵ�ĳ���
		pReg[nCount].ValueLength = pValueInfo->DataLength;
		ExFreePoolWithTag(pValueInfo, 'uin');
		nCount++;
	}
	ExFreePoolWithTag(pKeyInfo, 'gnaw');
	ZwClose(hKey);
	return STATUS_SUCCESS;
}
// ���ע�������
NTSTATUS MyCreateReg(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	// 1. �򿪴�·��.�õ�·����Ӧ�ľ��.
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
	//������
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
// ɾ��ע�������
NTSTATUS MyDelReg(PIRP Irp)
{
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	// 1. �򿪴�·��.�õ�·����Ӧ�ľ��.
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
	//�򿪼�
	status = ZwOpenKey(&hKey,
		KEY_ALL_ACCESS,
		&objAtt);
	if (NT_SUCCESS(status)) {
		//ɾ����
		status = ZwDeleteKey(hKey);
		ZwClose(hKey);
	}
	return STATUS_SUCCESS;
}



// ����IDT��
NTSTATUS MyQueryIDT(PIRP Irp)
{
	SIDT_INFO sidtInfo = { 0,0,0 };
	PIDT_ENTRY pIDTEntry = NULL;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == 0)
	{
		DbgPrint("��ȡMDLʧ��\n");
		return STATUS_SUCCESS;
	}
	//��ȡIDTR�Ĵ�����ֵ
	__asm sidt sidtInfo;

	//��ȡIDT�������׵�ַ
	pIDTEntry = (PIDT_ENTRY)MAKE_LONG(sidtInfo.uLowIdtBase, sidtInfo.uHighIdtBase);

	PIDT_INFO pOutIdtInfo = (PIDT_INFO)pOutBuff;
	if (pOutIdtInfo == 0)
	{
		DbgPrint("��ȡָ��ʧ��\n");
		return STATUS_SUCCESS;
	}
	//��ȡIDT��Ϣ
	for (ULONG i = 0; i < 0x100; i++)
	{
		//�жϵ�ַ
		pOutIdtInfo->pFunction = MAKE_LONG(pIDTEntry[i].uOffsetLow, pIDTEntry[i].uOffsetHigh);
		//��ѡ����
		pOutIdtInfo->Selector = pIDTEntry[i].uSelector;
		//����
		pOutIdtInfo->GateType = pIDTEntry[i].GateType;
		//��Ȩ�ȼ�
		pOutIdtInfo->Dpl = pIDTEntry[i].DPL;
		pOutIdtInfo++;
	}
	return STATUS_SUCCESS;
}

// ��ȡGDT�����
NTSTATUS MyGetGDTNumber(PIRP Irp)
{
	SGDT_INFO sgdtInfo = { 0,0,0 };
	PGDT_ENTER pGdtEntry = NULL;
	//��ȡGDTR�Ĵ�����ֵ
	__asm sgdt sgdtInfo;
	//��ȡGDT�������׵�ַ
	pGdtEntry = (PGDT_ENTER)MAKE_LONG(sgdtInfo.uLowGdtBase, sgdtInfo.uHighGdtBase);
	//��ȡGDT���������
	ULONG gdtCount = sgdtInfo.uGdtLimit / 8;

	ULONG nCount = 0;

	//��ȡGDT��Ϣ
	for (ULONG i = 0; i < gdtCount; i++)
	{
		//�������Ч���򲻱���
		if (pGdtEntry[i].P == 0)
		{
			continue;
		}
		nCount++;
	}

	Irp->IoStatus.Information = nCount;

	return STATUS_SUCCESS;
}
// ����GDT��
NTSTATUS MyQueryGDT(PIRP Irp)
{
	SGDT_INFO sgdtInfo = { 0,0,0 };
	PGDT_ENTER pGdtEntry = NULL;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	//��ȡGDTR�Ĵ�����ֵ
	__asm sgdt sgdtInfo;
	//��ȡGDT�������׵�ַ
	pGdtEntry = (PGDT_ENTER)MAKE_LONG(sgdtInfo.uLowGdtBase, sgdtInfo.uHighGdtBase);
	//��ȡGDT���������
	ULONG gdtCount = sgdtInfo.uGdtLimit / 8;

	PGDT_INFO pOutGdtInfo = (PGDT_INFO)pOutBuff;

	//��ȡGDT��Ϣ
	for (ULONG i = 0; i < gdtCount; i++)
	{
		//�������Ч���򲻱���
		if (pGdtEntry[i].P == 0)
		{
			continue;
		}
		//�λ�ַ
		pOutGdtInfo->BaseAddr = MAKE_LONG2(pGdtEntry[i].base0_23, pGdtEntry[i].base24_31);
		//����Ȩ�ȼ�
		pOutGdtInfo->Dpl = (ULONG)pGdtEntry[i].DPL;
		//������
		pOutGdtInfo->GateType = (ULONG)pGdtEntry[i].Type;
		//������
		pOutGdtInfo->Grain = (ULONG)pGdtEntry[i].G;
		//���޳�
		pOutGdtInfo->Limit = MAKE_LONG(pGdtEntry[i].Limit0_15, pGdtEntry[i].Limit16_19);
		pOutGdtInfo++;
	}
	return STATUS_SUCCESS;
}
// ����SSDT��
NTSTATUS MyQuerySSDT(PIRP Irp)
{
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	if (pOutBuff == NULL)
		return STATUS_SUCCESS;
	PSSDT_INFO pOutSsdtInfo = (PSSDT_INFO)pOutBuff;

	for (ULONG i = 0; i < KeServiceDescriptorTable.NumberOfServices; i++)
	{
		//������ַ
		pOutSsdtInfo->uFuntionAddr = (ULONG)KeServiceDescriptorTable.ServiceTableBase[i];
		//���ú�
		pOutSsdtInfo->uIndex = i;
		pOutSsdtInfo++;
	}
	return STATUS_SUCCESS;
}


// ������
NTSTATUS FanTiaoShi(PIRP Irp)
{
	NTSTATUS status;
	PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
	ULONG Pid = *(PULONG)pInputBuff;
	PEPROCESS pProc = NULL;
	status = PsLookupProcessByProcessId((HANDLE)Pid, &pProc);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("������ʧ��\n");
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
	// 1. ����Ҫ���˵��Ǻ��� ZwOpenProcess����ʱ���ú��� 0xBE
	//	[edx + 0x00]: ���ص�ַ1 cs
	//	[edx + 0x04]: ���ص�ַ2 eip
	//	[edx + 0x08]: ����1		ProcessHandle(���)
	//	[edx + 0x0C]: ����2		DesiredAccess(Ȩ��)
	//	[edx + 0x10]: ����3		ObjectAttributes(����)
	//	[edx + 0x14]: ����4		ClientId(PID)
	__asm
	{
		pushad;						// �������еļĴ���
		cmp eax, 0xBE;				// �ȶԵ��úţ�ȷ���Ƿ��ǹ�ע�ĺ���
		jne tag_End;				// ������ǹ�ע�ĺ�������ִ��ԭ�к���

	// ��ʼ HOOK
		mov eax, [edx + 0x14];		// ��ȡ�����̵� &ClientID
		mov eax, [eax]				// * ��ȡ ClientID.ProcessId
		cmp MyPid, eax;				// �ж��Ƿ��Ǳ����� pid
		jne tag_End;				// ������Ǿ͵���ԭ����
		mov[edx + 0x0C], 0;			// ����Ǿ��޸�Ȩ��Ϊ 0

	tag_End:
		popad;						// �ָ�ԭ�еļĴ���
		jmp OldKiFastCallEntry;		// ��ת��ԭʼ����
	}
}
// ����hook
VOID InstallSysenterHook()
{
	__asm
	{
		// �� msr 0x176 �л�ȡ��ԭ�еĺ�����ַ
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
// �ر�hook
VOID UninstallSysenterHook()
{
	__asm
	{
		mov ecx, 0x176
		mov eax, OldKiFastCallEntry
		wrmsr
	}
}


// ����Hook
typedef POBJECT_TYPE(*pfnObGetObjectType)(PVOID Object);
ULONG* HookAddress = NULL;				// �������ڵ�ַ
OB_OPEN_METHOD HookFunction = NULL;		// �ɵĺ���
UNICODE_STRING StopFileName = { 0 };
UNICODE_STRING StopProcessName = { 0 };
// ���ļ���ȡ�ļ����
NTSTATUS CreateFile(PHANDLE FileHandle, PCWCH pFilePath, ACCESS_MASK Access, ULONG CreateDisposition, BOOLEAN IsFile)
{
	// �����ļ���Ŀ¼����ı���
	HANDLE FileHandle2 = NULL;
	IO_STATUS_BLOCK IoStatusBlock = { 0 };


	// ��ʼ���ļ���Ŀ¼��Ӧ��·�����ƣ�������Ʊ����� \\??\\ ��ͷ��
	UNICODE_STRING FilePath = { 0 };
	RtlInitUnicodeString(&FilePath, pFilePath);

	// �����κ�һ���������󣬶���Ҫʹ�� OBJECT_ATTRIBUTES �ṹ��
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	InitializeObjectAttributes(
		&ObjectAttributes,			// ��Ҫ���г�ʼ���Ķ������Խṹ��
		&FilePath,					// ��Ӧ�ľ������������
		OBJ_CASE_INSENSITIVE,		// �������ԣ������ʾ�������ִ�Сд
		NULL, NULL);				// �������ڵ�Ŀ¼������İ�ȫ������

	// �ú�������һ������ֵ��ʾ���������ļ�����Ŀ¼�����ｫ��ת��Ϊ��־λ
	ULONG CreateOption = IsFile ? FILE_NON_DIRECTORY_FILE : FILE_DIRECTORY_FILE;

	// Ĭ�ϵ��ں˲��������첽�ģ������Ҫ����ͬ������������ָ������ı�־λ
	CreateOption |= FILE_SYNCHRONOUS_IO_NONALERT;

	// �˺����������ڴ�һ���ļ���Ŀ¼������ɹ��õ������ʧ�ܷ��ش���ԭ��
	NTSTATUS Status = ZwCreateFile(
		&FileHandle2,				// �������ִ�гɹ����򷵻��ļ����
		Access,						// �Ժ���Ȩ�޴�ָ�����ļ���Ŀ¼ GENERIC_ALL
		&ObjectAttributes,			// �������Խṹ�壬��Ҫ�����ṩ�ļ���·��
		&IoStatusBlock,				// ���浱ǰ IO ������ִ�н��
		0,							// ����ļ����´����ģ��������ָ�����ļ��Ĵ�С
		FILE_ATTRIBUTE_NORMAL,		// �ļ������ԣ�ͨ��ʹ�� FILE_ATTRIBUTE_NORMAL
		FILE_SHARE_VALID_FLAGS,		// �ļ��Ĺ���ʽ�������ʾ�����дɾ��
		CreateDisposition,			// �ļ��Ĵ�����־��ͨ���� FILE_OPEN_IF
		CreateOption,				// һ���־λ��������������˵���򿪵���ʲô
		NULL, 0);					// ��չ�Ļ������Լ���С���ĵ�Ҫ��Ϊ NULL

	// �ж��ļ��Ƿ�򿪳ɹ�������ɹ����õõ��ľ��������ֱ�ӷ��ش�����
	if (NT_SUCCESS(Status))
	{
		*FileHandle = FileHandle2;
	}
	return Status;
}

NTSTATUS MyOpenProcedure(IN ULONG Unknown,IN OB_OPEN_REASON OpenReason,IN PEPROCESS Process OPTIONAL,IN PVOID Object,IN ACCESS_MASK GrantedAccess,IN ULONG HandleCount)
{
	// ���� HOOK �����ļ���Object ��ʾ�ľ��Ǳ� HOOK �Ķ���
	PFILE_OBJECT FileObject = Object;

	if (ObCreateHandle == OpenReason)
	{
		DbgPrint("������ %wZ\n", &FileObject->FileName);
		// �����Ҫ�����ļ���������������ȫƥ�䣬���Դ�ָ����·����
		//	������Ҫ���ص��ļ���������ҵ��ˣ�����ʧ�ܣ��������
	}
	else if (ObOpenHandle == OpenReason)
	{
		// DbgPrint("���� %wZ\n", &FileObject->FileName);
		PWCHAR Buffer = ExAllocatePoolWithTag(NonPagedPoolNx, 0x1000, 'tset');
		RtlFillMemory(Buffer, 0, 0x1000);
		RtlCopyMemory(Buffer, FileObject->FileName.Buffer, FileObject->FileName.Length);
		if (wcsstr(Buffer, L"123.txt"))
		{
			DbgPrint("��ʧ�� %wZ\n", &FileObject->FileName);
			ExFreePoolWithTag(Buffer, 'tset');
			return STATUS_UNSUCCESSFUL;
		}
		ExFreePoolWithTag(Buffer, 'tset');
	}
	return HookFunction ? HookFunction(Unknown, OpenReason, Process, Object, GrantedAccess, HandleCount) : STATUS_SUCCESS;
}
NTSTATUS MyOpenProcedure1(IN ULONG Unknown, IN OB_OPEN_REASON OpenReason, IN PEPROCESS Process OPTIONAL, IN PVOID Object, IN ACCESS_MASK GrantedAccess, IN ULONG HandleCount)
{
	// ���� HOOK �����ļ���Object ��ʾ�ľ��Ǳ� HOOK �Ķ���
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
	// �Ӳ���ϵͳ�ĵ�����У��ҵ�ָ���� ObGetObjectType ������ַ��������
	UNICODE_STRING RoutineName = RTL_CONSTANT_STRING(L"ObGetObjectType");
	return (pfnObGetObjectType)MmGetSystemRoutineAddress(&RoutineName);
}
// �����ļ�����HOOK
VOID OnObjecFileHook()
{
	HANDLE FileHandle = NULL;
	PFILE_OBJECT FileObject = NULL;
	NTSTATUS Status = STATUS_SUCCESS;

	// 1. ��ȡһ����Ҫ HOOK ����������Ӧ��[�ں˶���ṹ��]
	Status = CreateFile(&FileHandle, L"\\??\\D:\\demo.txt", GENERIC_READ, FILE_OPEN_IF, TRUE);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("�ļ���ʧ��\n");
		return;
	}

	// 2. �������ȡ���ľ��ת����һ���ں˶���ṹ��
	Status = ObReferenceObjectByHandle(FileHandle, FILE_ALL_ACCESS, NULL, KernelMode, &FileObject, NULL);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("�����ȡʧ��\n");
		return;
	}

	// 3. �Ӳ���ϵͳ ssdt �л�ȡ ObGetObjectType �ĵ�ַ
	pfnObGetObjectType ObGetObjectType = GetObjectTypeAddress();
	if (ObGetObjectType == NULL)
	{
		DbgPrint("ObGetObjectType ��ȡʧ��\n");
		return;
	}

	// 4. ͨ������ṹ�壬�ҵ���������Ӧ�� OBJECT_TYPE �ṹ��
	POBJECT_TYPE ObjectType = ObGetObjectType(FileObject);
	if (ObjectType == NULL)
	{
		DbgPrint("�������ͻ�ȡʧ��\n");
		return;
	}

	// 5. �ҵ� _OBJECT_TYPE_INITIALIZER ����Ҫ Hook �ĺ������ڵ�λ��
	HookAddress = &ObjectType->TypeInfo.OpenProcedure;
	HookFunction = (OB_OPEN_METHOD)ObjectType->TypeInfo.OpenProcedure;

	// 6. ��Ŀ�꺯���滻���û��Զ���ĺ�����ִ�����ز���
	InterlockedExchange((LONG*)HookAddress, (LONG)MyOpenProcedure);
	ObDereferenceObject(FileObject);
	ZwClose(FileHandle);
}

// ���̶���HOOK
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
		DbgPrint("���̴�ʧ��\n");
		return;
	}

	// 2. �������ȡ���ľ��ת����һ���ں˶���ṹ��
	Status = ObReferenceObjectByHandle(ProcessHandle, FILE_ALL_ACCESS, NULL, KernelMode, &ProcessObject, NULL);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("�����ȡʧ��\n");
		return;
	}

	////��ȡ��ǰ���̵�EPROCESS
	//PEPROCESS ProcessObject = PsGetCurrentProcess();
	// 3. �Ӳ���ϵͳ ssdt �л�ȡ ObGetObjectType �ĵ�ַ
	pfnObGetObjectType ObGetObjectType = GetObjectTypeAddress();
	if (ObGetObjectType == NULL)
	{
		DbgPrint("ObGetObjectType ��ȡʧ��\n");
		return;
	}
	// 4. ͨ������ṹ�壬�ҵ���������Ӧ�� OBJECT_TYPE �ṹ��
	POBJECT_TYPE ObjectType = ObGetObjectType(ProcessObject);
	if (ObjectType == NULL)
	{
		DbgPrint("�������ͻ�ȡʧ��\n");
		return;
	}
	// 5. �ҵ� _OBJECT_TYPE_INITIALIZER ����Ҫ Hook �ĺ������ڵ�λ��
	HookAddress = &ObjectType->TypeInfo.OpenProcedure;
	HookFunction = (OB_OPEN_METHOD)ObjectType->TypeInfo.OpenProcedure;
	// 6. ��Ŀ�꺯���滻���û��Զ���ĺ�����ִ�����ز���
	InterlockedExchange((LONG*)HookAddress, (LONG)MyOpenProcedure1);
	ObDereferenceObject(ProcessObject);
	ZwClose(ProcessHandle);
}

// �ر�HOOK
VOID OffObjectHook()
{
	InterlockedExchange((LONG*)HookAddress, (LONG)HookFunction);
}


extern PDRIVER_OBJECT g_pDriver;
// Ĭ����ǲ����
NTSTATUS CommonDispatchRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)								// ������Ǿ�����Ϣ������
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
// �Զ�����ǲ������
NTSTATUS IoCtlDispatchRoutine(IN PDEVICE_OBJECT	DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG IoControlCode = Stack->Parameters.DeviceIoControl.IoControlCode;

	Irp->IoStatus.Status = STATUS_SUCCESS;
	switch (IoControlCode)
	{
		// ��ȡ��������
	case CTL_GetDriverNum:
	{
		// DbgBreakPoint();
		MyGetDriverNumber(DeviceObject->DriverObject, Irp);
		break;
	}
	// ��������
	case CTL_QueryDriver:
	{
		//��ȡ��ǰIRPջ
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
	// ��ȡ��������
	case CTL_GetProcessNum:
	{
		MyGetProcessNumber(Irp);
		break;
	}
	// ��������
	case CTL_QueryProcess:
	{
		//��ȡ��ǰIRPջ
		PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);
		ULONG outputSize = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
		MyQueryProcess(Irp, outputSize);
		break;
	}
	// ���ؽ���
	case CTL_HideProcess:
	{
		MyHindProcess(Irp);
		break;
	}
	// ��������
	case CTL_DelProcess:
	{
		MyDelProcess(Irp);
		break;
	}
	// ��ȡ�߳�����
	case CTL_GetThreadNum:
	{
		MyGetThreadNumber(Irp);
		break;
	}
	// �����߳�
	case CTL_QueryThread:
	{
		MyQueryThread(Irp);
		break;
	}
	// ��ȡģ������
	case CTL_GetModuleNum:
	{
		MyGetModuleNumber(Irp);
		break;
	}
	// ����ģ��
	case CTL_QueryModule:
	{
		MyQueryModule(Irp);
		break;
	}
	// ��ȡ�ļ�����
	case CTL_GetFileNum:
	{
		MyGetFileNumber(Irp);
		break;
	}
	// �����ļ�
	case CTL_QueryFile:
	{
		MyQueryFile(Irp);
		break;
	}
	// ɾ���ļ�
	case CTL_DelFile:
	{
		DelFile(Irp);
		break;
	}
	// ��ȡע�������
	case CTL_GetRegNum:
	{
		MyGetRegNumber(Irp);
		break;
	}
	// ����ע���
	case CTL_QueryReg:
	{
		MyQueryReg(Irp);
		break;
	}
	// ����ע�������
	case CTL_CreateReg:
	{
		MyCreateReg(Irp);
		break;
	}
	// ɾ��ע�������
	case CTL_DelReg:
	{
		MyDelReg(Irp);
		break;
	}
	// ����IDT��
	case CTL_QueryIDT:
	{
		MyQueryIDT(Irp);
		break;
	}
	// ��ȡGDT��Ԫ�ظ���
	case CTL_GetGDTNum:
	{
		MyGetGDTNumber(Irp);
		break;
	}
	// ����GDT��
	case CTL_QueryGDT:
	{
		MyQueryGDT(Irp);
		break;
	}
	// ��ȡSSDT��������
	case CTL_GetSSDTNum:
	{
		Irp->IoStatus.Information = KeServiceDescriptorTable.NumberOfServices;
		break;
	}
	// ����SSDT
	case CTL_QuerySSDT:
	{
		MyQuerySSDT(Irp);
		break;
	}
	// ��������������
	case CTL_ProtectDriver:
	{
		PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
		MyPid = *(PULONG)pInputBuff;
		InstallSysenterHook();
		break;
	}
	// ������
	case CTL_FanTiao:
	{
		FanTiaoShi(Irp);
		break;
	}
	// ��ָ���ļ��޷�����
	case CTL_StopFile:
	{
		PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
		RtlInitUnicodeString(&StopFileName, (PWCHAR)pInputBuff);
		OnObjecFileHook();
		break;
	}
	// ��ָ�������޷�����
	case CTL_StopProcess:
	{
		PVOID pInputBuff = Irp->AssociatedIrp.SystemBuffer;
		RtlInitUnicodeString(&StopProcessName, (PWCHAR)pInputBuff);
		OnObjectProcessHook();
		break;
	}
	// �ر�HOOK
	case CTL_CloseHook:
	{
		OffObjectHook();
		break;
	}
	case CTL_NeiHeChongZai:
	{
		// �����ں�
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


// �ں�����
PSSDTEntry g_pNewSSDT;//�µ�SSDT
ULONG g_JmpPoint;
PUCHAR pHookPoint;

// ���ļ�
HANDLE KernelCreateFile(IN PUNICODE_STRING pstrFile,IN BOOLEAN bIsDir)  // �ļ�·���������� // �Ƿ�Ϊ�ļ���
{
	HANDLE          hFile = NULL;
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess =
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	ULONG           ulCreateOpt =
		FILE_SYNCHRONOUS_IO_NONALERT;
	// 1. ��ʼ��OBJECT_ATTRIBUTES������
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes =
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // ���س�ʼ����ϵĽṹ��
		pstrFile,      // �ļ���������
		ulAttributes,  // ��������
		NULL, NULL);   // һ��ΪNULL
	// 2. �����ļ�����
	ulCreateOpt |= bIsDir ?
		FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
	Status = ZwCreateFile(
		&hFile,                // �����ļ����
		GENERIC_ALL,           // �ļ���������
		&objAttrib,            // OBJECT_ATTRIBUTES
		&StatusBlock,          // ���ܺ����Ĳ������
		0,                     // ��ʼ�ļ���С
		FILE_ATTRIBUTE_NORMAL, // �½��ļ�������
		ulShareAccess,         // �ļ�����ʽ
		FILE_OPEN_IF,          // �ļ�������򿪲������򴴽�
		ulCreateOpt,           // �򿪲����ĸ��ӱ�־λ
		NULL,                  // ��չ������
		0);                   // ��չ����������
	if (!NT_SUCCESS(Status))
		return (HANDLE)-1;
	return hFile;
}

// ��ȡ�ļ���С
ULONG64 KernelGetFileSize(IN HANDLE hfile)
{
	// ��ѯ�ļ�״̬
	IO_STATUS_BLOCK           StatusBlock = { 0 };
	FILE_STANDARD_INFORMATION fsi = { 0 };
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	Status = ZwQueryInformationFile(
		hfile,        // �ļ����
		&StatusBlock, // ���ܺ����Ĳ������
		&fsi,         // �������һ��������������������Ϣ
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation);
	if (!NT_SUCCESS(Status))
		return 0;
	return fsi.EndOfFile.QuadPart;
}

// ��ȡ�ļ�
ULONG64 KernelReadFile(IN HANDLE hfile,IN PLARGE_INTEGER Offset,IN ULONG ulLength,OUT PVOID pBuffer)  
{
	// 1. ��ȡ�ļ�
	IO_STATUS_BLOCK StatusBlock = { 0 };
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	Status = ZwReadFile(
		hfile,        // �ļ����
		NULL,         // �ź�״̬(һ��ΪNULL)
		NULL, NULL,   // ����
		&StatusBlock, // ���ܺ����Ĳ������
		pBuffer,      // �����ȡ���ݵĻ���
		ulLength,     // ��Ҫ��ȡ�ĳ���
		Offset,       // ��ȡ����ʼƫ��
		NULL);        // һ��ΪNULL
	if (!NT_SUCCESS(Status))  return 0;
	// 2. ����ʵ�ʶ�ȡ�ĳ���
	return StatusBlock.Information;
}

// �����ڴ�����
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

// �ر�ҳ����
void OffProtected()
{
	__asm { //�ر��ڴ汣��
		cli;
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}

}
// ����ҳ����
void OnProtected()
{
	__asm { //�ָ��ڴ汣��
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
		sti;
	}

}

// ͨ�����ƻ�ȡģ���ַ
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

//windows���ݲ�ͬ�Ļ���������ز�ͬ���ں��ļ�
//���ˣ�����PAE  
//���ˣ�û��PAE
//��ˣ�����PAE
//��ˣ�û��PAE

// ��ȡ�ں�ģ�鵽�ڴ���
void ReadKernelToBuf(PWCHAR pPath, PUCHAR* pBuf)
{
	//-----------------------------------------
	UNICODE_STRING pKernelPath;  //�ں��ļ�·��
	HANDLE hFile = 0;            //�ں��ļ����
	LARGE_INTEGER Offset = { 0 };//��ȡ��ƫ��ֵ
	//-----------------------------------------
	//1 ���ļ�
	RtlInitUnicodeString(
		&pKernelPath,
		pPath);
	hFile = KernelCreateFile(&pKernelPath, FALSE);
	//2 ��ȡ�ļ���С
	ULONG64 ulFileSize = KernelGetFileSize(hFile);
	*pBuf = ExAllocatePool(NonPagedPool, ulFileSize);
	RtlZeroMemory(*pBuf, ulFileSize);
	//3 ��ȡ�ļ����ڴ�
	KernelReadFile(hFile, &Offset, ulFileSize, *pBuf);
}

// չ���ں�PE�ļ�
void ZKKernel(PUCHAR* pZkBUf, PUCHAR buf)
{
	//1 ���DOSͷ���̶����NTͷ���ٻ����չͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)buf;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + buf);
	ULONG uZkSize = pNt->OptionalHeader.SizeOfImage;

	//2 ����ռ�
	*pZkBUf = ExAllocatePool(NonPagedPool, uZkSize);
	RtlZeroMemory(*pZkBUf, uZkSize);
	//3 ��ʼչ��
	//3.1 �ȿ���ͷ��
	memcpy(*pZkBUf, buf, pNt->OptionalHeader.SizeOfHeaders);
	//3.2�ٿ�������
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	for (int i = 0; i < pNt->FileHeader.NumberOfSections; i++)
	{
		memcpy(
			*pZkBUf + pSection[i].VirtualAddress,//�������ڴ��е���ʼλ��
			buf + pSection[i].PointerToRawData,  //���������ļ��е�λ��
			pSection[i].Misc.VirtualSize         //�����εĴ�С
		);
	}
}
// �޸����ں��ض�λ
void FixReloc(PUCHAR ZkBuf, PUCHAR OldBase)
{
	typedef struct _TYPE {
		USHORT Offset : 12;
		USHORT Type : 4;
	}TYPE, * PTYPE;

	//1 ���DOSͷ���̶����NTͷ���ٻ����չͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)ZkBuf;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + ZkBuf);
	//2 ����ض�λ��
	PIMAGE_DATA_DIRECTORY pRelocDir = (pNt->OptionalHeader.DataDirectory + 5);
	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)
		(pRelocDir->VirtualAddress + ZkBuf);

	//2.5 �õ�һ�����ں���Ĭ�ϻ�ַ���һ����ֵ
	ULONG uOffset = (ULONG)OldBase - pNt->OptionalHeader.ImageBase;

	//3 ��ʼ�޸��ض�λ
	while (pReloc->SizeOfBlock != 0)
	{
		ULONG uCount = (pReloc->SizeOfBlock - 8) / 2;//��0x1000�ڣ��ж�����Ҫ�ض�λ�ĵط�
		ULONG uBaseRva = pReloc->VirtualAddress;     //��0x1000����ʼλ��
		PTYPE pType = (PTYPE)(pReloc + 1);
		for (int i = 0; i < uCount; i++)
		{
			if (pType->Type == 3)
			{
				PULONG pRelocPoint = (uBaseRva + pType->Offset + ZkBuf);
				//�ض�λ��ĵ�ַ - �»�ַ = û�ض�λ�ĵ�ַ - Ĭ�ϻ�ַ
				//���ԣ��ض�λ��ĵ�ַ = �»�ַ - Ĭ�ϻ�ַ + û�ض�λ�ĵ�ַ
				*pRelocPoint = uOffset + *pRelocPoint;
			}
			pType++;
		}
		pReloc = (PIMAGE_BASE_RELOCATION)((ULONG)pReloc + pReloc->SizeOfBlock);
	}

}
// �޸���SSDT��
void FixSSDT(PUCHAR pZKBuf, PUCHAR OldBase, PIRP Irp)
{

	//���ں�ĳλ��1 - ���ں˻�ַ = ���ں�ĳλ��1 - ���ں˻�ַ��
	//���ں�ĳλ��1 = ���ں˻�ַ - ���ں˻�ַ + ���ں�ĳλ��1;
	PVOID pOutBuff = NULL;
	pOutBuff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	PULONG AddrArr = (PULONG)pOutBuff;

	LONG Offset = (ULONG)pZKBuf - (ULONG)OldBase;
	//1 �õ����ں��е�SSDT
	g_pNewSSDT = (PSSDTEntry)((LONG)&KeServiceDescriptorTable + Offset);

	//2 ���ϵͳ�������
	g_pNewSSDT->NumberOfServices = KeServiceDescriptorTable.NumberOfServices;

	//3 ���SSDT��
	g_pNewSSDT->ServiceTableBase = (ULONG*)((PUCHAR)KeServiceDescriptorTable.ServiceTableBase + Offset);
	//�����е�SSDT�б���ĺ�����ַ����ָ�����ں�
	for (int i = 0; i < g_pNewSSDT->NumberOfServices; i++)
	{
		AddrArr[i] = g_pNewSSDT->ServiceTableBase[i];
		g_pNewSSDT->ServiceTableBase[i] = g_pNewSSDT->ServiceTableBase[i] + Offset;
	}
	//4 ��������
	g_pNewSSDT->ParamTableBase = (PULONG)((PUCHAR)KeServiceDescriptorTable.ParamTableBase + Offset);
	memcpy(g_pNewSSDT->ParamTableBase,
		KeServiceDescriptorTable.ParamTableBase,
		g_pNewSSDT->NumberOfServices
	);
}
// ��ȡKiFastCallEntry����
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

// hook�й��˺���
ULONG FilterFun(ULONG SSdtBase, PULONG OldFun, ULONG Id)
{
	//�����ȣ�˵�����õ���SSDT�еĺ���
	if (SSdtBase == (ULONG)KeServiceDescriptorTable.ServiceTableBase)
	{
		//ʹ��˼·��
		//���������OD�����Һ���������190�ţ��������ں��еĺ���������ͨ��hookOpenProcess���޷���סOD�ˡ�
		return g_pNewSSDT->ServiceTableBase[Id];
	}
	return OldFun;

}

// inline Hook�Ļص�����
_declspec(naked)void MyHookFun()
{
	//eax �����ǵ��úţ�edx�������Ϻ�����ַ��edi������SSDT��ַ
	_asm {
		pushad;
		pushfd;
		push eax;		//���ú�
		push edx;		//ԭʼ������ַ
		push edi;		//SSDT��ַ

		call FilterFun;	// �Լ��Ĺ��˺�������ȡ����ʵ������ַ  ����ǰ��ѹ��3��������pushfd,pushad ebx�պ���ջ�еġ�esp+0x18]
		mov dword ptr ds : [esp + 0x18] , eax; // ��esp+0x18�� ebx��ֵ���滻��������ַ
		popfd;
		popad;			  // �ָ�ͨ�üĴ�����ebx���滻���µĺ�����ַ��
						  // ִ��ԭʼhook��5���ֽ�
		sub     esp, ecx;
		shr     ecx, 2;
		jmp     g_JmpPoint;	// ��ת��ԭ��������ַ
	}
}

// hook Ŀ���5���ֽ�
UCHAR Old_Code[5] = { 0 };

// hookKiFastCallEntry����
void OnHookKiFastCallEntry()
{
	// KiFastCallEntry������ֵ
	char buf[] = { 0x2b, 0xe1, 0xc1, 0xe9, 0x02 };
	// ��ȡKiFastCallEntry������ַ
	ULONG KiFastCallEntryAdd = GetKiFastCallEntry();
	// �ҵ�hook��
	pHookPoint = SearchMemory(buf, 5, (char*)KiFastCallEntryAdd, 0x200);
	// �ƹ�ǰ5���ֽڣ�ӦΪ��hook�滻��
	g_JmpPoint = (ULONG)(pHookPoint + 5);
	// ���ݾɵ�5���ֽ�
	memcpy(Old_Code, pHookPoint, 5);
	// �ر�ҳ����
	OffProtected();
	// jmp xxxxxx
	// д����תĿ���ַ   Ŀ���ַ-ָ������-5
	pHookPoint[0] = 0xE9;
	*(ULONG*)(&pHookPoint[1]) = (ULONG)MyHookFun - (ULONG)pHookPoint - 5;
	// ����ҳ����
	OnProtected();
}

// �ں����ؿ�ʼ
void KernelReload(PIRP Irp)
{
	PUCHAR pBuf = NULL;
	PUCHAR pZKBuf = NULL;
	UNICODE_STRING KernelName;

	//1 ���Ȱ��ں��ļ���ȡ���ڴ��� (Ĭ�Ͽ���PAE)
	ReadKernelToBuf(L"\\??\\C:\\Windows\\System32\\ntkrnlpa.exe", &pBuf);

	//2 �Ѷ����ڴ��е��ں˸�չ����0x1000����
	ZKKernel(&pZKBuf, pBuf);
	ExFreePool(pBuf);

	//3 �޸����ں˵��ض�λ ����Ȼ����PAE��ntkrnlpa.exe��,������ʾ������ ntoskrnl.exe
	RtlInitUnicodeString(&KernelName, L"ntoskrnl.exe");
	ULONG32 uBase = MyGetModuleHandle(&KernelName);

	FixReloc(pZKBuf, (PUCHAR)uBase);

	//4 �޸��µ�SSDT��
	FixSSDT(pZKBuf, (PUCHAR)uBase,Irp);

	//5 Hook��KiFastCallEntry�����Լ���Hook�������ж�Ӧ�������ں˻������ں�
	OnHookKiFastCallEntry();

}

// ж���ں˹���
void UnHook()
{
	OffProtected();
	memcpy(pHookPoint, Old_Code, 5);
	OnProtected();
}
