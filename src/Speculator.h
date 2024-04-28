#ifndef SPECULATOR_H
#define SPECULATOR_H

#include "SpeculatorObject.h"

class Speculator{
    public:
        Speculator();
        void create_speculation();
        void commit_speculation();
        void fail_speculation();
    
};


#endif