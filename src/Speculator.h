#ifndef SPECULATOR_H
#define SPECULATOR_H

#include "SpeculatorObject.h"
#include "SpeculatorObject.cpp"
#include<sys/types.h>
#include<vector>

class Speculator{
    private:
        pid_t child_process;
    public:
        // Store all speculations somewhere; 
        char* buffer_value;
        char* cache_object;
        int pipe_value;
        int buffer_size;
        Speculator();
        void create_speculation(pid_t pid, int file_descriptor, int buff, int pipe);
        void validate_speculation(std::vector<char> buffer, int buffer_size);
        void commit_speculation();
        void fail_speculation();
    
};


#endif