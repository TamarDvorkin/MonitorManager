#include "WindowsCollector.hpp"
#include <iostream> //log errors
#include <iomanip> //std::fied r setpercision, formatting data
#include <sstream> //format to txt file with std::stringstream


#ifdef  _WIN32

//implement FileTimeToUNLONGLONG before ctor, becuase 
//all other methods depend on it
ULONGLONG WindowsCollector::FileTimeToUNLONGLONG(const FIILETIME& ft)
{
    return static_cast<ULONGLONG>(ft.dwHighDateTime) << 32 | ft.dwLowDateTime;
}

WindowsCollector::WindowsCollector()
{
    InitializePdhCounters();
    std::string initialResult = CollectData();
    m_isFirstRead = true;
}

WindowsCollector::~WindowsCollector() //TODO::
{

}

std::string WindowsCollector:: CollectData()  //TODO::
{

}

bool WindowsCollector::InitializePdhCounters()
{
    if(!SetUpCpuQuery() ||  !SetUpMemoryQuery() ||  !SetUpDiskQuery() || !PerformInitialPhdCollect())
    {return false;}
    
    CaptureInitialCpuTimes();
    m_lastCollectTime =  std::chrono::steady_clock::now();

    return true;

}

void WindowsCollector::GetRawCpuTimes(ULONGLONG& idle, ULONGLONG& kernel, ULONGLONG& user, ULONGLONG& total)
{
    FILETIME ftIdle, ftKernel,ftUser;
    if(GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
    {
        idle =  FileTimeToUNLONGLONG(ftIdle);
        kernel = FileTimeToUNLONGLONG(ftKernel);
        total = FileTimeToUNLONGLONG(ftUser) ;
    }
    else
    {
        std::cerr << "WindowsCollector: GetSystemTimes failed." << std::endl;
        idle =  kernel = total = 0;
    }

}

bool WindowsCollector::SetUpCpuQuery()
{    
    PDH_STATUS status;
    PDH_HQUERY rawCpuQueryHandle =  nullptr;
    status = PdhOpenQuery(nullptr, 0 &rawCpuQueryHandle);  //calling phd microsoft API
    if(ERROR_SUCCESS!= status)
    {
        std::cerr << "WindowsCollector: PdhAddCounter for CPU failed with " << std::hex << status << std::endl;
        return false;
    }
    cpuQuery_ =  std::unique_ptr<void, PdhQueryDeleter>(rawCpuQueryHandle);
    status = PdhAddCounter(puQuery_.get(), TEXT("\\Processor(_Total)\\% Processor Time"), 0, &m_cpuTotalCounter); 

    if (status != ERROR_SUCCESS)
    {
        std::cerr << "WindowsCollector: PdhAddCounter for % Processor Time failed with " << std::hex << status << std::endl;
        return false;
    }
    return true;

}

bool WindowsCollector::SetUpMemoryQuery()
{
    PDH_STATUS status;
    PDH_HQUERY rawMemQueryHandle =  nullptr;
    status = PdhOpenQuery(nullptr, 0 &rawMemQueryHandle);  //calling phd microsoft API
    if(ERROR_SUCCESS!= status)
    {
        std::cerr << "WindowsCollector: PdhOpenQuery for Memory failed with " << std::hex << status << std::endl;
        return false;
    }

    memoryQuery_ = std::unique_ptr<void, PdhQueryDeleter>(rawMemQueryHandle); // Manage handle
    status = PdhAddCounter(memoryQuery_.get(), TEXT("\\Memory\\Available Bytes"), 0, &m_meoryAvailableCounter); // Add "Available Bytes" counter);

    if(ERROR_SUCCESS!= status)
    {
        std::cerr << "WindowsCollector: PdhAddCounter for Available Bytes failed with " << std::hex << status << std::endl;
        return false;
    }
    return true;

}

bool WindowsCollector::SetUpDiskQuery()
{
    PDH_STATUS status;
    PDH_HQUERY rawDiskQueryHandle =  nullptr;
    status = PdhOpenQuery(nullptr, 0 &rawDiskQueryHandle);  //calling phd microsoft API
    if(ERROR_SUCCESS!= status)
    {
        std::cerr << "WindowsCollector: PdhOpenQuery for Disk failed with " << std::hex << status << std::endl;
        return false;
    }

    diskQuery_ = std::unique_ptr<void, PdhQueryDeleter>(rawDiskQueryHandle);
    status =  PdhAddCounter(diskQuery_.get(), TEXT("\\PhysicalDisk(_Total)\\Disk Read Bytes/sec"), 0, &m_diskReadByteSecCounter); // Add Disk Read counter)

    if(ERROR_SUCCESS!= status)
    {
        std::cerr << "WindowsCollector: dhAddCounter for Disk Write Bytes/sec failed with " << std::hex << status << std::endl;
        return false;
    }
    return true;
}

bool WindowsCollector::PerformInitialPhdCollect()
{
    PDH_STATUS status;
    status = PdhCollectQueryData(cpuQuery_.get());  //calling phd microsoft API
    if(ERROR_SUCCESS!= status && PDH_NO_DATA!= status)
    {
        std::cerr << "WindowsCollector: Initial PdhCollectQueryData for CPU failed with " << std::hex << status << std::endl;
        return false;
    }

    return true;
    
}

bool WindowsCollector::CaptureInitialCpuTimes()
{
    ULONGLONG currentIdle, currentKernel, currentUser;
    getRawSystemTimes(currentIdle, currentKernel, currentUser);

    m_prevCpuIdleTime = currentIdle;
    m_prevCpuKernelTime = currentKernel;
    m_prevCpuUserTime = currentUser;
    m_prevCpuTotalTime= currentIdle + currentKernel + currentUser; 
}





#endif _WIN32