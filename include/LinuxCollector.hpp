#ifndef _LinuxCollector_HPP_
#define _LinuxCollector_HPP_

#include "ICollector.hpp" //collecting data
#include <string>
#include <fstream> //reading /proc
#include <sstream> //parsing lines form /proc
#include <chrono> //time 
namespace CollectData
{
class LinuxCollector : ICollector
{
public:
    LinuxCollector();
    ~LinuxCollector()  noexcept override= default; 
    std::string CollectData() override;


private:
//private members and helper methods to collect data about:
//cpu, memory, disk space
struct DiskIOData                   //nested struct DiskIOData - coupled to the internal class
{
    double readBytesPerSec;
    double writeBytesPerSec;
};

struct RawCpuTimes
{
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
    unsigned long long guest;
    unsigned long long guest_nice;


    RawCpuTimes():user(0), nice(0),system(0), idle(0), iowait(0), irq(0), softirq(0), steal(0),guest(0),guest_nice(0){}
    unsigned long long GetTotalJIffies() const;         //calculate total jiffies fro all components
};


bool m_isFirstRead= true;
std::chrono::steady_clock::time_point m_lastCollectTime;// Timestamp of the previous data collection

/*--- Disk Io data from /Proc/diskstats 
These members store the cumulative counts of rea\write and sectors from prevous read.*/
unsigned long long m_prevDiskRead = 0; //for calculatin disk isage rates
unsigned long long m_prevDiskWrite = 0;
unsigned long long m_prevDiskSectorsWrite = 0;
unsigned long long m_prevDiskRead = 0; 
unsigned long long m_cpuTotalCounter=NULL;
unsigned long long m_meoryAvailableCounter=NULL;
unsigned long long m_diskReadByteSecCounter=NULL;       // Counter for disk read bytes per second
unsigned long long m_DskWriteByteSecCoun=NULL;


//absolute values collected at previus sample, needed for calculatinh CPU percentage over time
unsigned long long m_prevCpuUserTime = 0;
unsigned long long m_prevCpuNiceTime = 0;               //time in user mode woth low priorty
unsigned long long m_prevCpuSystemTime = 0;
unsigned long long m_prevCpuIdleTime = 0;
unsigned long long m_prevCpuIoWait = 0;
unsigned long long m_prevCpuIrQ = 0;                    //time serving hardware interrupts
unsigned long long m_prevCpuSoftwareIrQ = 0;            //time serving software interrupts
unsigned long long m_prevCpuSteal = 0;                  //time stolen from virtual machine TODO:: check what it rally means
unsigned long long m_prevCpuGuest = 0;                  //time spent runing gest VM
unsigned long long m_prevCpuGuestNice = 0;



/////////////////////////////PRIVATE METHODS TO CALCULATE SYSTEM USAGE ///////////////////////////

//////////////////////////////////////////////////////////
/////////helper functions for CollectData() Mehotd////////

double CollectCpuPercantageData();
double CollectAvailableMemoryBytesData();
DiskIOData CollectDiskIoBytesPerSecData();



///helper functions to get raw CPU and disk data
RawCpuTimes GetRawCpuTimes();
void GetRawMemoryStats(unsigned long long& totalMemKB, unsigned long long greeMemKB, unsigned long long& buffersKB, unsigned long long& cachedK);
void GetRawDiskStats(unsigned long long& readCompelted ,unsigned long long& sectorsRead ,unsigned long long& writeCompleted ,unsigned long long& sectorsWritten);

};
}

#endif