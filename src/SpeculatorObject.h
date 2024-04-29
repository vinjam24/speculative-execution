#ifndef SPECULATOROBJECT_H
#define SPECULATOROBJECT_H

template <typename T1, typename T2>
class SpeculatorObject{
    private:
        Map<T1, T2> kernelObjects;
    
    public:
        SpeculatorObject();
        void set(T1 key, T2 value);
        int get(T1 key);
};

#endif