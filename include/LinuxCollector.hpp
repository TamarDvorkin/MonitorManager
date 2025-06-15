#ifndef _LinuxCollector_HPP_
#define _LinuxCollector_HPP_

#include "ICollector.hpp" //collecting data
#include <string>

namespace CollectDAta
{
class LinuxCollector : ICollector
{
public:
    LinuxCollector();
    ~LinuxCollector()  noexcept override = default; 
    std::string CollectData() override;


private:
//private members and helper methods to collect data about:
//cpu, memory, disk space


};
}

#endif