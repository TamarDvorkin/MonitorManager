#ifndef _IFactory_HPP_
#define _IFactory_HPP_

#include "ICollector.hpp"
#include "string"

class IFactory
{
public:
    IFactory(); //ctor
    virtual ~IFactory() = 0;
    virtual ICollector* CreateCollector(const std::string& osType) = 0; //TODO:: pure virtual?yes but i should ce=reate collector, not factory(heritace)
    

private:
//no members, pure interface class

};

#endif