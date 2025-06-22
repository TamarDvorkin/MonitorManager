#include "../include/LinuxCollector.hpp" //TODO:: set makefile to read the files from include folder
#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <algorithm>

namespace CollectData
{
    
    #ifdef __linux__

        LinuxCollector :: LinuxCollector()
        {
            std::string dummyResult = CollectData();
            m_isFirstRead =  true; 
        }

        std::string LinuxCollector::CollectData()
        {

        }
    

        double LinuxCollector::CollectCpuPercantageData()
        {}
        double LinuxCollector::CollectAvailableMemoryBytesData()
        {}
        LinuxCollector::DiskIOData LinuxCollector::CollectDiskIoBytesPerSecData()
        {
   

        }


        LinuxCollector::RawCpuTimes LinuxCollector::GetRawCpuTimes()
        {
            RawCpuTimes currentTimes;
            std::ifstream statFile("/proc/stat");
            if(!statFile.is_open())
            {
                std::cerr << "LinuxCollector: FAiled to opme /proc/stat" << std::endl;
            }

            std::string line;
            std::getline(statFile,line);
            statFile.close();
            std::stringstream ss(line); //parse line 
            std::string cpuLabel;
            ss >>cpuLabel;
            if("cpu" != cpuLabel)
            {
                std::cerr <<"LinuxCollector: unexpected format in /c=proc/stat" <<std::endl;
                return currentTimes;
;
            }
            //parsing cumu;ative cpu rimes values into struct RawCpuTimes
            ss >> currentTimes.user >> currentTimes.nice >> currentTimes.system >>currentTimes.idle>>
            currentTimes.iowait >> currentTimes.irq >>currentTimes.softirq >>currentTimes.steal >>
            currentTimes.guest >>currentTimes.guest_nice;

            return currentTimes

        }
        void LinuxCollector::GetRawMemoryStats(unsigned long long& totalMemKB, unsigned long long greeMemKB, unsigned long long& buffersKB, unsigned long long& cachedK)
        {}
        void LinuxCollector::GetRawDiskStats(unsigned long long& readCompelted ,unsigned long long& sectorsRead ,unsigned long long& writeCompleted ,unsigned long long& sectorsWritten)
        {}




    #endif
}