#include "../include/WindowsCollector.hpp" //TODO:: set makefile to read the files from include folder
#include <iostream> //log errors
#include <iomanip> //std::fied r setpercision, formatting data
#include <sstream> //format to txt file with std::stringstream

namespace CollectData
{

#ifdef  _WIN32


//implement FileTimeToUNLONGLONG before ctor, because 
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

std::string WindowsCollector:: CollectData()  
{
    //call about microsoft build in collecting data functions :
    //https://learn.microsoft.com/en-us/windows/win32/api/pdh/nf-pdh-pdhgetformattedcountervalue
    //https://learn.microsoft.com/en-us/windows/win32/perfctrs/collecting-performance-data
    std::string result;
    result <<std::fixed <<std::setprecision(4);
    auto current_time =  std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = current_time - m_lastCollectTime;
    double timeDelta = elapsedSeconds.count();
    m_lastCollectTime = current_time;
    
    //collect cpu usage data
    double cpu_usage = CollectCpuPercantageData();
    result << "cpu_percentage is : " <<cpu_usage << "%" << std::endl;

    //collect memory data
    double available_mem_bytes = CollectAvailableMemoryBytesData();
    result << "memory available bytes : " <<available_mem_bytes  << std::endl;

    //collect disk IO data
    DiskIOData disk_data = CollectDiskIoBytesPerSecData();
    result << "disk read bytes per sec : " <<disk_data.readBytesPerSec  << std::endl;
    result << "disk write bytes per sec : " <<disk_data.writeBytesPerSec  << std::endl;


     //finish collecting data with flag
    m_isFirstRead =  false;
    return result.str(); //result of cpu, memory and disk data
}

bool WindowsCollector::InitializePdhCounters()
{
    if(!SetUpCpuQuery() ||  !SetUpMemoryQuery() ||  !SetUpDiskQuery() || !PerformInitialPhdCollect())
    {return false;}
    
    CaptureInitialCpuTimes();
    m_lastCollectTime =  std::chrono::steady_clock::now();

    return true;

}


double WindowsCollector::CollectCpuPercantageData()
{
    double cpu_usage = 0.0;
    PDH_FMT_COUNTERVALUE cpu_value;
    PDH_STATUS status = PdhCollectQueryData(m_cpuQuery.get());
    if(ERROR_SUCCESS == status)
    {
        status = PdhGetFormattedCounterValue(m_cpuTotalCounter, PDH_FMT_DOUBLE, nullptr, &cpu_value);   //formatted cpu usage
        if(ERROR_SUCCESS == status)
        {
            cpu_usage = cpu_value.doubleValue;
        }
        else
        {
            std:cerr<<"WindowsCollector::PdhGetFormattedCounterValue for cpu usage failed with "<< std::hex << status <<std::endl;
        }
    }
    else
    {
        std:cerr<<"WindowsCollector::PdhCollectQueryData for cpu usage failed with "<< std::hex << status <<std::endl;
        
    }

    return cpu_usage;
}

double WindowsCollector::CollectAvailableMemoryBytesData()
{
    double available_memory_in_bytes = 0.0;
    PDH_FMT_COUNTERVALUE mem_value;
    PDH_STATUS status = PdhCollectQueryData(m_memoryQuery.get());
    if(ERROR_SUCCESS == status)
    {
        status = PdhGetFormattedCounterValue(m_meoryAvailableCounter, PDH_FMT_DOUBLE, nullptr, &mem_value);   //formatted cpu usage
        if(ERROR_SUCCESS == status)
        {
            available_memory_in_bytes = mem_value.doubleValue;
        }
        else
        {
            std:cerr<<"WindowsCollector::PdhGetFormattedCounterValue for memory available bytes failed with "<< std::hex << status <<std::endl;
        }
    }
    else
    {
        std:cerr<<"WindowsCollector::PdhCollectQueryData for memory failed with "<< std::hex << status <<std::endl;
        
    }

    return available_memory_in_bytes;

}

DiskIOData WindowsCollector::CollectDiskIoBytesPerSecData()
{
    DiskIOData data = {0.0, 0.0}; //read and write data
    PDH_FMT_COUNTERVALUE disk_read_val; disk_write_val;
    PDH_STATUS status = PdhCollectQueryData(m_diskQuery.get());
    if(ERROR_SUCCESS == data)
    {
        //get formatted data for disk read bytes
        status = PdhGetFormattedCounterValue(m_diskReadByteSecCounter, PDH_FMT_DOUBLE, nullptr, &disk_read_val);   //formatted cpu usage
        if(ERROR_SUCCESS == data)
        {
            data.readBytesPerSec = disk_read_val.doubleValue;
        }
        else
        {
            std:cerr<<"WindowsCollector::PdhGetFormattedCounterValue disk read bytes failed with "<< std::hex << status <<std::endl;
        }
        //get formatted data for disk read bytes
        status = PdhGetFormattedCounterValue(m_DskWriteByteSecCoun, PDH_FMT_DOUBLE, nullptr, &disk_write_val);   //formatted cpu usage
        if(ERROR_SUCCESS == data)
        {
            data.writeBytesPerSec = disk_write_val.doubleValue;
        }
        else
        {
            std:cerr<<"WindowsCollector::PdhGetFormattedCounterValue for disk write bytes failed with "<< std::hex << status <<std::endl;
        }
    }
    else
    {
        std:cerr<<"WindowsCollector::PdhCollectQueryData for disk failed with "<< std::hex << status <<std::endl;
        
    }

    return data;

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
}