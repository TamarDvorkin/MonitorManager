#ifndef _WindowsCollector_HPP_
#define _WindowsCollector_HPP_

#include <string>
#include <memory> //std::uniquue_ptr
#include <map>
#include <functional> //std::function
#include <chrono>     //time calculations
//windows specific includes
#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib") //link with pdh.lib

#include "ICollector.hpp"

namespace CollectDAta
{
struct PdhQuetDeleter //PDH: Performance Data Helper.
{
    void operator()(PDH_HQUERY queryHandle) const
    {
        if(nullptr!= queryHandle)// PDH handles are typically NULL or 0 if not successfully opened.
        {
            PdhCloseQuery(queryHandle);//cleans all counters added to this query
        }

    }

};


class WindowsCollector : ICollector
{
public: //TODO:: CHECK FUNCTIONS I NEED to collect data on both os
    WindowsCollector(); //initialize PDH queries // call initializePdhCounters
    ~WindowsCollector() noexcept override = default; 
    std::string CollectData() override;



private:
//smart pointers clean by calling PhdCloseQuery when object destroyed
std::unique_ptr<void, PdhQuetDeleter> m_cpuQuery;
std::unique_ptr<void, PdhQuetDeleter> m_memoryQuery;
std::unique_ptr<void, PdhQuetDeleter> m_diskQuery;

/*--- PDH- PERFORMANCE, DATA HELPER, Counter Handles (Raw, managed by their parent Query) --- These handles represent specific performance counters within a query.
They don't need unique_ptr because PdhCloseQuery (called by the PdhQueryDeleter)
automatically removes all counters associated with that query.*/
PDH_HCOUNTER m_cpuTotalCounter=NULL;
PDH_HCOUNTER m_meoryAvailableCounter=NULL;
PDH_HCOUNTER m_diskReadByteSecCounter=NULL;// Counter for disk read bytes per second
PDH_HCOUNTER m_DskWriteByteSecCoun=NULL;

bool m_isFirstRead= true;
std::chrono::steady_clock::time_point m_lastCollectTime;// Timestamp of the previous data collection

//absolute values collected at previus sample, needed for calculatinh CPU percentage over time
ULONGLONG m_prevCpuIdleTime = 0;
ULONGLONG m_prevCpuKernelTime = 0;
ULONGLONG m_prevCpuUserTime = 0;
ULONGLONG m_prevCpuTotalTime = 0;

ULONGLONG m_prevDiskReadBytes = 0; //for calculatin disk isage rates
ULONGLONG m_prevDiskWriteBytes = 0;

/////////////////////////////PRIVATE METHODS TO CALCULATE SYSTEM USAGE ///////////////////////////
bool InitializePdhCounters(); // call PdhOpenQuery
/////////helper functions for InitializePdhCounters////////
bool SetUpCpuQuery();
bool SetUpMemoryQuery();
bool SetUpDiskQuery();
bool PerformInitialPhdCollect();
bool CaptureInitialCpuTimes();
//////////////////////////////////////////////////////////
/////////helper functions for CollectDAta() Mehotd////////
double CollectCpuPercantageData();
double CollectAvailableMemoryBytesData();
DiskIOData CollectDiskIoBytesPerSecData();



///helper functions to get raw CPU times and convert FILETIME to UNLONGLONG
void GetRawCpuTimes(ULONGLONG& idle, ULONGLONG kernel, ULONGLONG& user, ULONGLONG& total);
ULONGLONG FileTimeToUNLONGLONG(const FIILETIME& ft);

};
}
#endif //_WINN32


#endif //WindowsCollector