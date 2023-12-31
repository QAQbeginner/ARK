#pragma once
#include <ntifs.h> 

// 获取驱动数量
#define	CTL_GetDriverNum CTL_CODE(0x8001, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历驱动
#define	CTL_QueryDriver CTL_CODE(0x8001, 0x801, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 获取进程数量
#define	CTL_GetProcessNum CTL_CODE(0x8001, 0x802, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历进程
#define	CTL_QueryProcess CTL_CODE(0x8001, 0x803, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 隐藏进程
#define	CTL_HideProcess CTL_CODE(0x8001, 0x804, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 结束进程
#define	CTL_DelProcess CTL_CODE(0x8001, 0x805, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 获取线程数量
#define	CTL_GetThreadNum CTL_CODE(0x8001, 0x806, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历线程
#define	CTL_QueryThread CTL_CODE(0x8001, 0x807, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 获取模块数量
#define	CTL_GetModuleNum CTL_CODE(0x8001, 0x808, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历模块
#define	CTL_QueryModule CTL_CODE(0x8001, 0x809, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 获取文件数量
#define	CTL_GetFileNum CTL_CODE(0x8001, 0x810, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历文件
#define	CTL_QueryFile CTL_CODE(0x8001, 0x811, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 删除文件
#define	CTL_DelFile CTL_CODE(0x8001, 0x812, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 获取注册表数量
#define	CTL_GetRegNum CTL_CODE(0x8001, 0x813, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历注册表
#define	CTL_QueryReg CTL_CODE(0x8001, 0x814, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 创建注册表子项
#define	CTL_CreateReg CTL_CODE(0x8001, 0x815, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 删除注册表子项
#define	CTL_DelReg CTL_CODE(0x8001, 0x816, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历IDT表
#define	CTL_QueryIDT CTL_CODE(0x8001, 0x817, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历GDT表
#define CTL_QueryGDT CTL_CODE(0x8001, 0x818, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 获取GDT表的数量
#define CTL_GetGDTNum CTL_CODE(0x8001, 0x819, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 获取SSDT表的数量
#define CTL_GetSSDTNum CTL_CODE(0x8001, 0x820, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 遍历SSDT表
#define CTL_QuerySSDT CTL_CODE(0x8001, 0x821, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 隐藏驱动
#define CTL_HideDriver CTL_CODE(0x8001, 0x822, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 保护自身不被结束
#define CTL_ProtectDriver CTL_CODE(0x8001, 0x823, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 反调试
#define CTL_FanTiao CTL_CODE(0x8001, 0x824, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 让指定文件无法创建
#define CTL_StopFile CTL_CODE(0x8001, 0x825, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 让指定进程无法创建
#define CTL_StopProcess CTL_CODE(0x8001, 0x826, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 关闭HOOK
#define CTL_CloseHook CTL_CODE(0x8001, 0x827, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 开启内核重载
#define CTL_NeiHeChongZai CTL_CODE(0x8001, 0x828, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 关闭内核重载
#define CTL_CloseNeiHeChongZai CTL_CODE(0x8001, 0x829, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
// 取消保护自身不被保护
#define CTL_CancleProtectDriver CTL_CODE(0x8001, 0x830, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
