#include<windows.h>
#include<atlstr.h>
// ��ȡ��������
#define	CTL_GetDriverNum CTL_CODE(0x8001, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// ��������
#define	CTL_QueryDriver CTL_CODE(0x8001, 0x801, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// ��ȡ��������
#define	CTL_GetProcessNum CTL_CODE(0x8001, 0x802, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// ��������
#define	CTL_QueryProcess CTL_CODE(0x8001, 0x803, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
//�����������������Ϣ
typedef struct _DRIVER_INFO
{
	PVOID DllBase;
	WCHAR driverName[0x100];
	ULONG SizeOfImage;
	WCHAR driverPath[0x100];
}DRIVER_INFO, * PDRIVER_INFO;
// ����������Ľ�����Ϣ
typedef struct _PROCESS_INFO
{
	DWORD PID;
	CHAR ProcessName[0x100];
}PROCESS_INFO, * PPROCESS_INFO;
int main()
{
	HANDLE Device = CreateFile(L"\\\\.\\king", GENERIC_ALL, NULL,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	
}