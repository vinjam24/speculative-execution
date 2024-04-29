#include "SpeculatorObject.h"

template<typename T1, typename T2>
SpeculatorObject<T1, T2>::SpeculatorObject() {}

int SpeculatorObject::get(T1 key){
    return kernelObjects[key];
}

void SpeculatorObject::setX(T1 key, T2 value){
    kernelObjects[key] = value;
}