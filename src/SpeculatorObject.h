#ifndef SPECULATOROBJECT_H
#define SPECULATOROBJECT_H

#include <map>

template <typename T1, typename T2>
class SpeculatorObject{
    private:
    public:
        std::map<T1, T2> kernelObjects;
        SpeculatorObject();
        void set(T1 key, T2 value);
        T2 get(T1 key);
};

#endif