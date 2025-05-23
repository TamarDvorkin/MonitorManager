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

struct PdhQuetDeleter
{
    void operator()(PDH_HQUERY queryHandle) const
    {
        if(nullptr!= queryHandle)// PDH handles are typically NULL or 0 if not successfully opened.
        {
            PdhCloseQuert(queryHandle);//cleans all counters added to this query
        }

    }

};


class WindowsCollector : ICollector
{
public: //TODO:: CHECK FUNCTIONS I NEED to collect data on both os
    WindowsCollector(); //initialize PDH queries
    ~WindowsCollector() noexcept override = default; 
    void CollectData() override;



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
std:chrono::steady_clock::time_pint m_lastCollectTime;// Timestamp of the previous data collection

//absolute values collected at previus sample, needed for calculatinh CPU percentage over time
ULONGLOBG m_prevCpuIdleTime = 0;
ULONGLOBG m_prevCpuKernelTime = 0;
ULONGLOBG m_prevCpuUserTime = 0;
ULONGLOBG m_prevCpuTotalTime = 0;

ULONGLOBG m_prevDiskReadBytes = 0; //for calculatin disk isage rates
ULONGLOBG m_prevDiskWriteBytes = 0;
bool InitializePdhCounters();
void GetRawCpuTimes(ULONGLOBG& idle, ULONGLOBG& kernel, ULONGLOBG& user, ULONGLOBG& total);


};
#endif //_WINN32


#endif //WindowsCollector