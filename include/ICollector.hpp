#ifndef _ICollector_HPP_
#define _ICollector_HPP_

class ICollector
{
public:
    ICollector(); //ctor
    virtual ~ICollector() = 0;
    void virtual CollectData() = 0;


private:
//no members, pure interface class


};

#endif