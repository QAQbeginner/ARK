#include<windows.h>
#include<atlstr.h>
// 获取驱动数量
#define	CTL_GetDriverNum CTL_CODE(0x8001, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历驱动
#define	CTL_QueryDriver CTL_CODE(0x8001, 0x801, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 获取进程数量
#define	CTL_GetProcessNum CTL_CODE(0x8001, 0x802, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历进程
#define	CTL_QueryProcess CTL_CODE(0x8001, 0x803, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
//保存遍历到的驱动信息
typedef struct _DRIVER_INFO
{
	PVOID DllBase;
	WCHAR driverName[0x100];
	ULONG SizeOfImage;
	WCHAR driverPath[0x100];
}DRIVER_INFO, * PDRIVER_INFO;
// 保存遍历到的进程信息
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