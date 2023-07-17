#include"ARKFunc.h"
// 卸载函数
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

	//  创建设备对象后，可以通过驱动对象设置的 IRP 派遣函数进行消息响应，消息
	//	会由 R3 发出，由指定的设备对象接收到。同一驱动下所有的设备对象都使用
	//	相同的 IRP 派遣函数，如果不提供指定的功能，将无法通信
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