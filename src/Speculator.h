#ifndef SPECULATOR_H
#define SPECULATOR_H

#include "SpeculatorObject.h"
#include "UndoLog.h"
#include <map>
#include <sys/types.h>
#include <vector>
#include <cstdarg>


class Speculator{
    private:
        SpeculatorObject* speculator_object;
        void propogate_buffer();
        void discard_buffer();
        std::map<char*, UndoLog*> file_to_undo_log_map;

    public:
        // Store all speculations somewhere; 
        Speculator();
        void create_speculation(pid_t pid, int file_descriptor, int buff, int pipe);
        void validate_speculation(std::vector<char> buffer, int buffer_size);
        void commit_speculation();
        void fail_speculation();
        int buffer_IO(const char* output_value, va_list* args);
        int write_speculatively(char* file_name, const char* content, int buffer_size); 
    
};


#endif