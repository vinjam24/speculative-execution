#ifndef SPECULATOROBJECT_H
#define SPECULATOROBJECT_H

#include<sys/types.h>
#include <queue>
#include <utility>
#include <cstdarg>
#include <vector>

class UndoLog;

typedef struct {
    pid_t parent_pid;
    pid_t child_pid;
    char* buffer_value;
    char* cache_object;
    int pipe_fd;
    int buffer_size;
    bool is_speculative;
    std::queue<std::pair<const char*, va_list*> > IO_buffer_queue;
    // Speculator object should contain references to UndoLogs
    std::vector<UndoLog*> undologs;


} SpeculatorObject;


#endif