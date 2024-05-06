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
        void propogate_buffer(SpeculatorObject* speculator_object);
        void discard_buffer(SpeculatorObject* speculator_object);
        std::map<char*, UndoLog*> file_to_undo_log_map;
        std::map<pid_t, UndoLog*> process_to_undo_log_map;
        std::map<int, SpeculatorObject*> id_to_speculation;

    public:
        // Store all speculations somewhere; 
        Speculator();
        int spec_counter; 
        int create_speculation(pid_t pid, pid_t parent_pid, int file_descriptor, int buff, int pipe);
        void revert_kernel_undo_logs(SpeculatorObject* speculator_object);
        void validate_speculation(std::vector<char> buffer, int buffer_size, int spec_id);
        void commit_speculation(int spec_id);
        void fail_speculation(int spec_id);
        int buffer_IO(const char* output_value, va_list* args);
        int write_speculatively(char* file_name, const char* content, int buffer_size, pid_t pid); 
        void delete_undo_log_entries(SpeculatorObject* speculator_object);
    
};


#endif