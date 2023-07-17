#pragma once
#include<ntddk.h>
#include<ntstrsafe.h>
#include <ntimage.h>

typedef struct _DEVICE_EXTENSION
{
    PDEVICE_OBJECT Self;
    UNICODE_STRING DeviceName;
    UNICODE_STRING SymbolName;
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
    struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x8
    struct _LIST_ENTRY InInitializationOrderLinks;                          //0x10
    VOID* DllBase;                                                          //0x18
    VOID* EntryPoint;                                                       //0x1c
    ULONG SizeOfImage;                                                      //0x20
    struct _UNICODE_STRING FullDllName;                                     //0x24
    struct _UNICODE_STRING BaseDllName;                                     //0x2c
    ULONG Flags;                                                            //0x34
    USHORT LoadCount;                                                       //0x38
    USHORT TlsIndex;                                                        //0x3a
    union
    {
        struct _LIST_ENTRY HashLinks;                                       //0x3c
        struct
        {
            VOID* SectionPointer;                                           //0x3c
            ULONG CheckSum;                                                 //0x40
        }u1;
    }u11;
    union
    {
        ULONG TimeDateStamp;                                                //0x44
        VOID* LoadedImports;                                                //0x44
    }u2;
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x48
    VOID* PatchInformation;                                                 //0x4c
    struct _LIST_ENTRY ForwarderLinks;                                      //0x50
    struct _LIST_ENTRY ServiceTagLinks;                                     //0x58
    struct _LIST_ENTRY StaticLinks;                                         //0x60
    VOID* ContextInformation;                                               //0x68
    ULONG OriginalBase;                                                     //0x6c
    union _LARGE_INTEGER LoadTime;                                          //0x70
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB
{
    UCHAR InheritedAddressSpace;                                            //0x0
    UCHAR ReadImageFileExecOptions;                                         //0x1
    UCHAR BeingDebugged;                                                    //0x2
    union
    {
        UCHAR BitField;                                                     //0x3
        struct
        {
            UCHAR ImageUsesLargePages : 1;                                    //0x3
            UCHAR IsProtectedProcess : 1;                                     //0x3
            UCHAR IsLegacyProcess : 1;                                        //0x3
            UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
            UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
            UCHAR SpareBits : 3;                                              //0x3
        }u1;
    }u3;
    VOID* Mutant;                                                           //0x4
    VOID* ImageBaseAddress;                                                 //0x8
    struct _PEB_LDR_DATA* Ldr;                                              //0xc
    struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;                 //0x10
    VOID* SubSystemData;                                                    //0x14
    VOID* ProcessHeap;                                                      //0x18
    struct _RTL_CRITICAL_SECTION* FastPebLock;                              //0x1c
    VOID* AtlThunkSListPtr;                                                 //0x20
    VOID* IFEOKey;                                                          //0x24
    union
    {
        ULONG CrossProcessFlags;                                            //0x28
        struct
        {
            ULONG ProcessInJob : 1;                                           //0x28
            ULONG ProcessInitializing : 1;                                    //0x28
            ULONG ProcessUsingVEH : 1;                                        //0x28
            ULONG ProcessUsingVCH : 1;                                        //0x28
            ULONG ProcessUsingFTH : 1;                                        //0x28
            ULONG ReservedBits0 : 27;                                         //0x28
        }u2;
    }u4;
    union
    {
        VOID* KernelCallbackTable;                                          //0x2c
        VOID* UserSharedInfoPtr;                                            //0x2c
    }u5;
    ULONG SystemReserved[1];                                                //0x30
    ULONG AtlThunkSListPtr32;                                               //0x34
    VOID* ApiSetMap;                                                        //0x38
    ULONG TlsExpansionCounter;                                              //0x3c
    VOID* TlsBitmap;                                                        //0x40
    ULONG TlsBitmapBits[2];                                                 //0x44
    VOID* ReadOnlySharedMemoryBase;                                         //0x4c
    VOID* HotpatchInformation;                                              //0x50
    VOID** ReadOnlyStaticServerData;                                        //0x54
    VOID* AnsiCodePageData;                                                 //0x58
    VOID* OemCodePageData;                                                  //0x5c
    VOID* UnicodeCaseTableData;                                             //0x60
    ULONG NumberOfProcessors;                                               //0x64
    ULONG NtGlobalFlag;                                                     //0x68
    union _LARGE_INTEGER CriticalSectionTimeout;                            //0x70
    ULONG HeapSegmentReserve;                                               //0x78
    ULONG HeapSegmentCommit;                                                //0x7c
    ULONG HeapDeCommitTotalFreeThreshold;                                   //0x80
    ULONG HeapDeCommitFreeBlockThreshold;                                   //0x84
    ULONG NumberOfHeaps;                                                    //0x88
    ULONG MaximumNumberOfHeaps;                                             //0x8c
    VOID** ProcessHeaps;                                                    //0x90
    VOID* GdiSharedHandleTable;                                             //0x94
    VOID* ProcessStarterHelper;                                             //0x98
    ULONG GdiDCAttributeList;                                               //0x9c
    struct _RTL_CRITICAL_SECTION* LoaderLock;                               //0xa0
    ULONG OSMajorVersion;                                                   //0xa4
    ULONG OSMinorVersion;                                                   //0xa8
    USHORT OSBuildNumber;                                                   //0xac
    USHORT OSCSDVersion;                                                    //0xae
    ULONG OSPlatformId;                                                     //0xb0
    ULONG ImageSubsystem;                                                   //0xb4
    ULONG ImageSubsystemMajorVersion;                                       //0xb8
    ULONG ImageSubsystemMinorVersion;                                       //0xbc
    ULONG ActiveProcessAffinityMask;                                        //0xc0
    ULONG GdiHandleBuffer[34];                                              //0xc4
    VOID(*PostProcessInitRoutine)();                                       //0x14c
    VOID* TlsExpansionBitmap;                                               //0x150
    ULONG TlsExpansionBitmapBits[32];                                       //0x154
    ULONG SessionId;                                                        //0x1d4
    union _ULARGE_INTEGER AppCompatFlags;                                   //0x1d8
    union _ULARGE_INTEGER AppCompatFlagsUser;                               //0x1e0
    VOID* pShimData;                                                        //0x1e8
    VOID* AppCompatInfo;                                                    //0x1ec
    struct _UNICODE_STRING CSDVersion;                                      //0x1f0
    struct _ACTIVATION_CONTEXT_DATA* ActivationContextData;                 //0x1f8
    struct _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap;                //0x1fc
    struct _ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData;    //0x200
    struct _ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap;                 //0x204
    ULONG MinimumStackCommit;                                               //0x208
    struct _FLS_CALLBACK_INFO* FlsCallback;                                 //0x20c
    struct _LIST_ENTRY FlsListHead;                                         //0x210
    VOID* FlsBitmap;                                                        //0x218
    ULONG FlsBitmapBits[4];                                                 //0x21c
    ULONG FlsHighIndex;                                                     //0x22c
    VOID* WerRegistrationData;                                              //0x230
    VOID* WerShipAssertPtr;                                                 //0x234
    VOID* pContextData;                                                     //0x238
    VOID* pImageHeaderHash;                                                 //0x23c
    union
    {
        ULONG TracingFlags;                                                 //0x240
        struct
        {
            ULONG HeapTracingEnabled : 1;                                     //0x240
            ULONG CritSecTracingEnabled : 1;                                  //0x240
            ULONG SpareTracingBits : 30;                                      //0x240
        }u6;
    }u7;
}PEB, * PPEB;

typedef struct _PEB_LDR_DATA
{
    ULONG Length;                                                           //0x0
    UCHAR Initialized;                                                      //0x4
    VOID* SsHandle;                                                         //0x8
    struct _LIST_ENTRY InLoadOrderModuleList;                               //0xc
    struct _LIST_ENTRY InMemoryOrderModuleList;                             //0x14
    struct _LIST_ENTRY InInitializationOrderModuleList;                     //0x1c
    VOID* EntryInProgress;                                                  //0x24
    UCHAR ShutdownInProgress;                                               //0x28
    VOID* ShutdownThreadId;                                                 //0x2c
}PEB_LDR_DATA, * PPEB_LDR_DATA;

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
    PVOID PID;
    WCHAR ProcessName[0x100];
    PVOID EProcess;
}PROCESS_INFO,*PPROCESS_INFO;
// 线程信息结构体
typedef struct _THREAD_INFO
{
    ULONG Tid;		    // 线程ID
    ULONG BasePriority;	//静态优先级
}THREAD_INFO, * PTHREAD_INFO;
// 模块信息结构体
typedef struct _MODULE_INFO
{
    WCHAR wcModuleFullPath[260];	//模块路径
    PVOID DllBase;		//基地址
    ULONG SizeOfImage;	//大小
}MODULE_INFO, * PMODULE_INFO;
//文件信息结构体
typedef struct _FILEINFO
{
    char FileOrDirectory;	    //一个字节来保存是文件还是目录,0表示目录，1表示文件
    WCHAR wcFileName[260];	    //文件名
    LONGLONG Size;				//文件大小
    LARGE_INTEGER CreateTime;	//创建时间
    LARGE_INTEGER ChangeTime;	//修改时间00
}FILE_INFO, * PFILE_INFO;
//注册表信息结构体
typedef struct _REG_INFO
{
    ULONG Type;				// 类型，0 为子项，1为值
    WCHAR KeyName[256];		// 项名
    WCHAR ValueName[256];   // 值的名字
    ULONG ValueType;		// 值的类型
    UCHAR Value[256];	    // 值
    ULONG ValueLength;		//值的长度
}REG_INFO, * PREG_INFO;
//IDT信息结构体
typedef struct _IDT_INFO
{
    ULONG pFunction;		//处理函数的地址
    ULONG Selector;			//段选择子
    ULONG ParamCount;		//参数个数
    ULONG Dpl;				//段特权级
    ULONG GateType;			//类型
}IDT_INFO, * PIDT_INFO;
// idtr寄存器指向这个结构体
typedef struct _SIDT_INFO
{
    UINT16 uIdtLimit; // IDT 范围
    UINT16 uLowIdtBase; // IDT低基址
    UINT16 uHighIdtBase; // IDT高基址
}SIDT_INFO,*PSIDT_INFO;
// 描述符的结构体
typedef struct _IDT_ENTRY
{
    UINT16 uOffsetLow;   // 处理程序低地址偏移
    UINT16 uSelector;
    UINT8 uReserved;
    UINT8 GateType : 4;
    UINT8 StorageSegment : 1;
    UINT8 DPL : 2;
    UINT8 Present : 1;
    UINT16 uOffsetHigh;
}IDT_ENTRY,*PIDT_ENTRY;
// gdtr寄存器指向这个结构体
typedef struct _SGDT_INFO
{
    UINT16 uGdtLimit;   // GDT范围
    UINT16 uLowGdtBase; // GDT低地址
    UINT16 uHighGdtBase; // GDT高地址
}SGDT_INFO,*PSGDT_INFO;
// GDT段描述符
typedef struct _GDT_ENTRY
{
    UINT64 Limit0_15 : 16;	//段限长低地址偏移
    UINT64 base0_23 : 24;	//段基地址低地址偏移
    UINT64 Type : 4;		//段类型
    UINT64 S : 1;			//描述符类型（0系统段，1数据段或代码段）
    UINT64 DPL : 2;			//特权等级
    UINT64 P : 1;			//段是否有效
    UINT64 Limit16_19 : 4;	//段限长高地址偏移
    UINT64 AVL : 1;			//可供系统软件使用
    UINT64 L : 1;
    UINT64 D_B : 1;			//默认操作大小（0=16位段，1=32位段）
    UINT64 G : 1;			//粒度
    UINT64 base24_31 : 8;	//段基地址高地址偏移
}GDT_ENTER, * PGDT_ENTER;
//GDT信息结构体
typedef struct _GDT_INFO
{
    ULONG BaseAddr;	//段基址
    ULONG Limit;	//段限长
    ULONG Grain;	//段粒度
    ULONG Dpl;		//特权等级
    ULONG GateType;	//类型
}GDT_INFO, * PGDT_INFO;
// 系统描述服务表结构体
typedef struct _SERVICE_DESCRIPTOR_ENTRY
{
    ULONG* ServiceTableBase;        // 服务表基址
    ULONG* ServiceCounterTableBase; // 计数表基址
    ULONG NumberOfServices;         // 表中项的个数
    UCHAR* ParamTableBase;          // 参数表基址
} SERVICE_DESCRIPTOR_ENTRY, * PSERVICE_DESCRIPTOR_ENTRY;
// SSDT信息结构体
typedef struct _SSDT_INFO
{
    ULONG uIndex;	//回调号
    ULONG uFuntionAddr;	//函数地址
}SSDT_INFO, *PSSDT_INFO;
// 反调试所需要结构体
typedef struct _FLAG2
{
    ULONG JobNotReallyActive : 1;                                     //0x26c
    ULONG AccountingFolded : 1;                                       //0x26c
    ULONG NewProcessReported : 1;                                     //0x26c
    ULONG ExitProcessReported : 1;                                    //0x26c
    ULONG ReportCommitChanges : 1;                                    //0x26c
    ULONG LastReportMemory : 1;                                       //0x26c
    ULONG ReportPhysicalPageChanges : 1;                              //0x26c
    ULONG HandleTableRundown : 1;                                     //0x26c
    ULONG NeedsHandleRundown : 1;                                     //0x26c
    ULONG RefTraceEnabled : 1;                                        //0x26c
    ULONG NumaAware : 1;                                              //0x26c
    ULONG ProtectedProcess : 1;                                       //0x26c
    ULONG DefaultPagePriority : 3;                                    //0x26c
    ULONG PrimaryTokenFrozen : 1;                                     //0x26c
    ULONG ProcessVerifierTarget : 1;                                  //0x26c
    ULONG StackRandomizationDisabled : 1;                             //0x26c
    ULONG AffinityPermanent : 1;                                      //0x26c
    ULONG AffinityUpdateEnable : 1;                                   //0x26c
    ULONG PropagateNode : 1;                                          //0x26c
    ULONG ExplicitAffinity : 1;                                       //0x26c
}FLAG2,*PFLAG2;


// 对象HOOK所需要结构体

// 要 HOOK 的函数保存的位置
typedef struct _OBJECT_TYPE_INITIALIZER {
    USHORT Length;
    UCHAR ObjectTypeFlags;
    UCHAR CaseInsensitive;
    UCHAR UnnamedObjectsOnly;
    UCHAR  UseDefaultObject;
    UCHAR  SecurityRequired;
    UCHAR MaintainHandleCount;
    UCHAR MaintainTypeList;
    UCHAR SupportsObjectCallbacks;
    UCHAR CacheAligned;
    ULONG ObjectTypeCode;
    BOOLEAN InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    BOOLEAN   ValidAccessMask;
    BOOLEAN   RetainAccess;
    POOL_TYPE PoolType;
    BOOLEAN DefaultPagedPoolCharge;
    BOOLEAN DefaultNonPagedPoolCharge;
    PVOID DumpProcedure;
    ULONG OpenProcedure;
    PVOID CloseProcedure;
    PVOID DeleteProcedure;
    ULONG ParseProcedure;
    ULONG SecurityProcedure;
    ULONG QueryNameProcedure;
    UCHAR OkayToCloseProcedure;
} OBJECT_TYPE_INITIALIZER, * POBJECT_TYPE_INITIALIZER;
// ObGetObjectType 的返回值
typedef struct _OBJECT_TYPE {
    LIST_ENTRY TypeList;
    UNICODE_STRING Name;
    PVOID DefaultObject;
    ULONG Index;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    OBJECT_TYPE_INITIALIZER TypeInfo;
    ULONG TypeLock;
    ULONG Key;
    LIST_ENTRY   CallbackList;
} OBJECT_TYPE, * POBJECT_TYPE;
// 文件的打开方式
typedef enum _OB_OPEN_REASON {
    ObCreateHandle,
    ObOpenHandle,
    ObDuplicateHandle,
    ObInheritHandle,
    ObMaxO3penReason
} OB_OPEN_REASON;
// 需要 HOOK 的函数的原型
typedef NTSTATUS(*OB_OPEN_METHOD)(
    IN ULONG Unknown,
    IN OB_OPEN_REASON OpenReason,
    IN PEPROCESS Process OPTIONAL,
    IN PVOID Object,
    IN ACCESS_MASK GrantedAccess,
    IN ULONG HandleCount);



// 内核重载需要的结构体
typedef struct _ServiceDesriptorEntry
{
    ULONG* ServiceTableBase;        // 服务表基址
    ULONG* ServiceCounterTableBase; // 计数表基址
    ULONG NumberOfServices;         // 表中项的个数
    UCHAR* ParamTableBase;          // 参数表基址
}SSDTEntry, * PSSDTEntry;

typedef struct _LDR_DATA_TABLE_ENTRY1 {
    LIST_ENTRY InLoadOrderLinks;    //双向链表
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union {
        LIST_ENTRY HashLinks;
        struct {
            PVOID SectionPointer;
            ULONG CheckSum;
        }u1;
    }u5;
    union {
        struct {
            ULONG TimeDateStamp;
        }u2;
        struct {
            PVOID LoadedImports;
        }u3;
    }u6;
} LDR_DATA_TABLE_ENTRY1, * PLDR_DATA_TABLE_ENTRY1;



