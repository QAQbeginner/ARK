#include"ARKFunc.h"
// ж�غ���
VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	PDEVICE_OBJECT DeviceObject = DriverObject->DeviceObject;
	while (DeviceObject)
	{ 
		PDEVICE_OBJECT TempObject = DeviceObject->NextDevice;

		PDEVICE_EXTENSION Extesion = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
		IoDeleteSymbolicLink(&Extesion->SymbolName);
		IoDeleteDevice(DeviceObject);

		DeviceObject = TempObject;
	}
}
PDRIVER_OBJECT g_pDriver = NULL;
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject,PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DriverObject->DriverUnload = DriverUnload;

	//  �����豸����󣬿���ͨ�������������õ� IRP ��ǲ����������Ϣ��Ӧ����Ϣ
	//	���� R3 ��������ָ�����豸������յ���ͬһ���������е��豸����ʹ��
	//	��ͬ�� IRP ��ǲ������������ṩָ���Ĺ��ܣ����޷�ͨ��
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = CommonDispatchRoutine;
	}

	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoCtlDispatchRoutine;

	// DbgBreakPoint();
	g_pDriver = DriverObject;
	CreateDevice(DriverObject);

	return STATUS_SUCCESS;
}