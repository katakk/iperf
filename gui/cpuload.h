#include "stdafx.h"
#include "winternl.h"

typedef int (FAR WINAPI * NTQUERYSYSTEMINFORMATION) (DWORD, PDWORD, int, ULONG *);

typedef struct _SYSTEM_BASIC_INFORMATION
{
	ULONG Unknown;
	ULONG MaximumIncrement;
	ULONG PhysicalPageSize;
	ULONG NumberOfPhysicalPages;
	ULONG LowestPhysicalPage;
	ULONG HighestPhysicalPage;
	ULONG AllocationGranularity;
	ULONG LowestUserAddress;
	ULONG HighestUserAddress;
	ULONG ActiveProcessors;
	UCHAR NumberProcessors;
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
     LARGE_INTEGER IdleTime;
     LARGE_INTEGER KernelTime;
     LARGE_INTEGER UserTime;
     LARGE_INTEGER DpcTime;
     LARGE_INTEGER InterruptTime;
     ULONG InterruptCount;
} SYSTEM_PERFORMANCE_INFORMATION, *PSYSTEM_PERFORMANCE_INFORMATION;

typedef struct _SYSTEM_TIME_INFORMATION
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG uCurrentTimeZoneId;
	DWORD dwReserved;
} SYSTEM_TIME_INFORMATION, *PSYSTEM_TIME_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation = 0,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemProcessesAndThreadsInformation = 5,
	SystemProcessorTimes = 8,
	SystemPagefileInformation = 18,
} SYSTEM_INFORMATION_CLASS;

class CCpuLoad
{
	HMODULE m_hModule;
	NTQUERYSYSTEMINFORMATION m_pQueryInfo;
	SYSTEM_BASIC_INFORMATION m_SysInfo;
	SYSTEM_PERFORMANCE_INFORMATION *m_Perf;
	__int64 latest_idle_time, latest_kernel_time, latest_whole_time;

public:
	CCpuLoad();
	virtual ~CCpuLoad();
	void GetKernelPerf(double* user, double* kernel);
};
