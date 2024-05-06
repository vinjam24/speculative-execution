#ifndef SPECULATOR_H
#define SPECULATOR_H

#include "SpeculatorObject.h"
#include "Queue.h"
#include <sys/types.h>
#include <vector>
#include <cstdarg>
#include <string>



class Speculator{
    private:
        SpeculatorObject speculator_object;
        Queue IO_buffer_queue;
        void propogate_buffer();
        void discard_buffer();

    public:

        // Store all speculations somewhere; 
        Speculator();
        void create_speculation(pid_t ppid, pid_t pid, int file_descriptor, int buff, int pipe);
        void validate_speculation(std::vector<char> buffer, int buffer_size);
        void commit_speculation();
        void fail_speculation();
        int buffer_IO(const char* output_value, va_list* args);
    
};


#endif