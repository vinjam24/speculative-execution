#include "SpeculatorObject.h"

template<typename T1, typename T2>
SpeculatorObject<T1, T2>::SpeculatorObject() {}

template<typename T1, typename T2>
T2 SpeculatorObject<T1, T2>::get(T1 key){
    return kernelObjects[key];
}

template<typename T1, typename T2>
void SpeculatorObject<T1, T2>::set(T1 key, T2 value){
    kernelObjects[key] = value;
}