#include "stdafx.h"
#include "cpuload.h"

CCpuLoad::CCpuLoad()
{
	m_Perf = NULL;
	m_hModule = GetModuleHandle("NTDLL.DLL");
	if(m_hModule)
	{
		m_pQueryInfo = 
			(NTQUERYSYSTEMINFORMATION)
			GetProcAddress(m_hModule, "NtQuerySystemInformation");

		if(m_pQueryInfo)
		{
			double user = 0, kernel = 0;
			(*m_pQueryInfo)(SystemBasicInformation, 
				(PDWORD)&m_SysInfo, sizeof(m_SysInfo), 0 );

			GetKernelPerf(&user,&kernel);

		}
	}

};

CCpuLoad::~CCpuLoad()
{
	FreeLibrary(m_hModule);
};

void CCpuLoad::GetKernelPerf(double* user, double* kernel)
{
	int ret;

	ULONG i;
	__int64 idle_time = 0;
	__int64 kernel_time = 0;
	__int64 whole_time = 0;
	ULONG RequiredLength;

	if(! m_pQueryInfo ) return;

	m_Perf = new SYSTEM_PERFORMANCE_INFORMATION[m_SysInfo.ActiveProcessors];
	RequiredLength = sizeof SYSTEM_PERFORMANCE_INFORMATION * m_SysInfo.ActiveProcessors;
	ret = (*m_pQueryInfo)(SystemProcessorTimes,
		(PDWORD)m_Perf, RequiredLength, 0);

	if(! ret )
	{
		for( i = 0; i < m_SysInfo.ActiveProcessors; i++)
		{
			idle_time += m_Perf[i].IdleTime.QuadPart;
			kernel_time += m_Perf[i].KernelTime.QuadPart - m_Perf[i].IdleTime.QuadPart;
			whole_time += m_Perf[i].KernelTime.QuadPart + m_Perf[i].UserTime.QuadPart;
		}

		if( whole_time - latest_whole_time )
		{
			*user = 100 - (( (double)(idle_time - latest_idle_time) ) / ( (double)(whole_time - latest_whole_time) )) * 100;
			*kernel = (( (double)(kernel_time - latest_kernel_time) ) / ( (double)(whole_time - latest_whole_time) )) * 100;
		}

		latest_idle_time = idle_time;
		latest_kernel_time = kernel_time;
		latest_whole_time = whole_time;
	}

	delete [] m_Perf;
	return;
}

