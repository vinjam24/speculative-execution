#ifndef UNDOLOG_H
#define UNDOLOG_H

#include "SpeculatorObject.h"
class UndoLog{
    private:
    struct Entry {
        char* prev_state;
        SpeculatorObject* speculator_object;
    };
    std::vector<Entry> entries;

    public:
    
    UndoLog(char* prev_state, SpeculatorObject* speculator_object);
    ~UndoLog();
    void add_to_undo_log(SpeculatorObject* speculator_object, char* prev_state);

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