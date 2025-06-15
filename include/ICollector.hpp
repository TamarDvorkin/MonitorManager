#ifndef _ICollector_HPP_
#define _ICollector_HPP_
#include <string>


namespace CollectDAta
{
class ICollector
{
public:
    ICollector(); //ctor
    virtual ~ICollector() = 0;
    std::string virtual CollectData() = 0;


private:
//no members, pure interface class


};
}

#endif