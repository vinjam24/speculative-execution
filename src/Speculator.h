#ifndef SPECULATOR_H
#define SPECULATOR_H

#include "SpeculatorObject.h"
#include<sys/types.h>

class Speculator{
    private:
        SpeculatorObject<char*, char*> *cache_objects;
        SpeculatorObject<char*, pid_t> *child_processes;
    public:
        Speculator();
        void create_speculation(pid_t pid, int file_descriptor);
        void validate_speculation(char* buffer, int buffer_size);
        void commit_speculation();
        void fail_speculation();
    
};


#endif