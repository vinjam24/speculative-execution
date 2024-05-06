#ifndef SPECULATOR_H
#define SPECULATOR_H

#include "SpeculatorObject.h"
#include "UndoLog.h"
#include <map>
#include "Queue.h"
#include <sys/types.h>
#include <vector>
#include <cstdarg>
#include <string>

#define MAP_MAX_SIZE 1000

class Speculator{
    private:
        SpeculatorObject* speculator_object;
        Queue IO_buffer_queue;

        struct FileNameUL {
            char* file_name;
            UndoLog* undo_log;
        };

        struct PidUL {
            pid_t pid;
            UndoLog* undo_log;
        };

        struct SpecIDSpec{
            int spec_id;
            SpeculatorObject* spec;
        };

        FileNameUL file_to_undo_log_map[MAP_MAX_SIZE];
        PidUL process_to_undo_log_map[MAP_MAX_SIZE];
        SpecIDSpec id_to_speculation[MAP_MAX_SIZE];
        
        void propogate_buffer();
        void discard_buffer();

    public:

        // Store all speculations somewhere; 
        Speculator();
        int spec_counter; 
        int file_ul_cnt;
        int pid_ul_cnt;
        int id_spec_cnt;
        int create_speculation(pid_t ppid, pid_t pid, int file_descriptor, int buff, int pipe);
        void revert_kernel_undo_logs(SpeculatorObject* speculator_object);
        void validate_speculation(std::vector<char> buffer, int buffer_size, int spec_id);
        void commit_speculation(int spec_id);
        void fail_speculation(int spec_id);
        int buffer_IO(const char* output_value, va_list* args);
        int write_speculatively(char* file_name, const char* content, int buffer_size, pid_t pid); 
        void delete_undo_log_entries(SpeculatorObject* speculator_object);
        SpeculatorObject* findWith(int spec_id);
        UndoLog* findProcessUndoWith(int p_id);
        UndoLog* findFileUndoWith(char* file_name);
        
    
};


#endif