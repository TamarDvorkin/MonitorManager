#ifndef _LinuxCollector_HPP_
#define _LinuxCollector_HPP_

#include "ICollector.hpp" //collecting data

class LinuxCollector : ICollector
{
public:
    LinuxCollector();
    ~LinuxCollector()  noexcept override = default; 
    void CollectData() override;


private:


};

#endif