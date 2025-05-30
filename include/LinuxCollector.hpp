#ifndef _LinuxCollector_HPP_
#define _LinuxCollector_HPP_

#include "ICollector.hpp" //collecting data
#include <string>

class LinuxCollector : ICollector
{
public:
    LinuxCollector();
    ~LinuxCollector()  noexcept override = default; 
    std::string CollectData() override;


private:


};

#endif