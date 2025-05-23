
#ifndef _MONITOR_HPP_
#define _MONITOR_HPP_

#include <stdlib.h>
#include <map>
#include <string>
#include <memory>
#include "IFactory.hpp"


class Monitor
{
public:
    Monitor(IFactory factory); //ctor 
    ~Monitor() noexcept = default; //rull of 3
    Monitor(const Monitor &other_); //=delete if you want to disable
    Monitor& operator = (const Monitor &other_); // =delete if you want to disable
    void CollectData();
    void WriteToSharedMem();

private:
    std::shared_ptr<IFactory> m_factory;  
    std::map <std::string, std::string> m_systemData; //map of cpu_usage, string
    //TODO:
    //shared?should factory be reachable from more places?or unique?

    //TODO: lock (mutex) on the shared memory when write?

    //TODO::ahould i think about thread pool for each thread to monitor something else,
    //like thread for cpu, thread for memory, thread for disk? 

};

#endif