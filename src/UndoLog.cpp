#include "UndoLog.h"
#include "SpeculatorObject.h"

UndoLog::UndoLog(char* prev_state, SpeculatorObject* speculator_object){
    this->add_to_undo_log(speculator_object, prev_state);
}

void UndoLog::add_to_undo_log(SpeculatorObject* speculator_object, char* prev_state){
    Entry entry;
    entry.prev_state = prev_state;
    entry.speculator_object = speculator_object;
    this->entries.push_back(entry);
}