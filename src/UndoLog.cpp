#include "UndoLog.h"
#include "SpeculatorObject.h"
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

UndoLog::UndoLog(char* prev_state, SpeculatorObject* speculator_object, char* file_name){
    this->file_name = file_name;
    this->add_to_undo_log(speculator_object, prev_state);
}

void UndoLog::add_to_undo_log(SpeculatorObject* speculator_object, char* prev_state){
    Entry entry;
    entry.prev_state = prev_state;
    entry.speculator_object = speculator_object;
    this->entries.push_back(entry);
    speculator_object->undologs.push_back(this);
}

void UndoLog::add_to_undo_log(SpeculatorObject *speculator_object, pid_t id){
    ProcessEntry p_entry;
    p_entry.speculator_object = speculator_object;
    this->p_entries.push_back(p_entry);
    speculator_object->undologs.push_back(this);
}

UndoLog::UndoLog(pid_t id, SpeculatorObject* speculator_object){
    this->add_to_undo_log(speculator_object, id);
}

void UndoLog::remove_entry(SpeculatorObject *speculator_object){
    int i = 0;
    for(auto it=this->entries.begin(); it != this->entries.end(); it++){
        if(it->speculator_object == speculator_object){
            this->entries.erase(it);
        }
    }

    for(auto it=this->p_entries.begin(); it != this->p_entries.end(); it++){
        if(it->speculator_object == speculator_object){
            this->p_entries.erase(it);
        }
    }
}

bool UndoLog::speculation_exists(Entry entry) {
    bool ans = false;
    for(auto it= this->entries.begin(); it!= this->entries.end(); it++){
        if(it->speculator_object == entry.speculator_object){
            ans = true;
            break;
        }
    }

    return ans;
}

void UndoLog::revert_till(SpeculatorObject *speculator_object){
    std::vector<Entry> new_entries;
    for(auto it = this->entries.begin(); it!= this->entries.end(); it++){
        if(it->speculator_object == speculator_object){
            int fd = open(file_name, O_RDONLY);
            write(fd, it->prev_state, speculator_object->buffer_size);
            break;
        }
        else {
            new_entries.push_back(*it);
        }
    }
    this->entries.swap(new_entries);
}

void UndoLog::add_dependencies(SpeculatorObject *speculator_object){
    /* TODO */
}