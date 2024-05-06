#ifndef UNDOLOG_H
#define UNDOLOG_H

#include "SpeculatorObject.h"
#include <vector>
class UndoLog{
    private:
    struct Entry {
        char* prev_state;
        SpeculatorObject* speculator_object;
    };

    struct ProcessEntry {
        pid_t checkpoint_id;
        SpeculatorObject* speculator_object;
    };

    

    public:
    std::vector<Entry> entries;
    char* file_name;
    std::vector<ProcessEntry> p_entries;
    UndoLog(char* prev_state, SpeculatorObject* speculator_object, char* file_name);
    UndoLog(pid_t id, SpeculatorObject* speculator_object);
    ~UndoLog();
    void add_to_undo_log(SpeculatorObject* speculator_object, char* prev_state);
    void add_to_undo_log(SpeculatorObject* speculator_object, pid_t id);
    void remove_entry(SpeculatorObject* speculator_object);
    void add_dependencies(SpeculatorObject* speculator_object);
    bool speculation_exists(Entry entry);
    void revert_till(SpeculatorObject* speculator_object);

    // Undolog should contain process checkpoint reference. (Each process is associated with a undolog)
    // Undo log should also support for kernel objects.
    // Each entry is linked to the speculation and it stores how to revert

    /*
    
    In general, Speculator propagates dependencies from a process P 
    to an object X whenever P modifies X and P depends on speculations 
    that X does not.The entry in X’s undo log for that operation is 
    associated with all speculations on which P depended but X did not.
    */



};

#endif