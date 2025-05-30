#ifndef _OSFactoryCollector_HPP_
#define _OSFactoryCollector_HPP_

#include <map>
#include <functional>
#include <string>
#include "IFactory.hpp"
#include "ICollector.hpp"

class OSFactoryCollector :public IFactory // a factory for creating collectors
{
public:

    OSFactoryCollector(); //ctor  //TODO:check if ctor should be explicit
    ~OSFactoryCollector()  noexcept override = default; //default- compiler generates, no special clean up//rull of 3
    OSFactoryCollector(const OSFactoryCollector &other_); //=delete if you want to disable
    OSFactoryCollector& operator = (const OSFactoryCollector &other_); // =delete if you want to disable
    ICollector* CreateCollector(const std::string& osType) override;
    

private:
//something to decide which os is it 
    std::map<std::string, std::function<ICollector*()>> m_osCollectorsMap;
    ICollector* CreateLinuxCollector();
    ICollector* CreateWindowsCollector();

    //about collecting os data :estions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

};

#endif