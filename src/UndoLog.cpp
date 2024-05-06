#include "UndoLog.h"
#include "SpeculatorObject.h"
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

UndoLog::UndoLog(char* prev_state, SpeculatorObject* speculator_object, char* file_name){
    this->file_name = file_name;
    this->add_to_undo_log(speculator_object, prev_state);
    this->entry_cnt = 0;
    this->p_entry_cnt = 0;
}

void UndoLog::add_to_undo_log(SpeculatorObject* speculator_object, char* prev_state){
    for(auto ent : this->entries){
        if(ent.speculator_object == speculator_object) {
            return;
        }
    }
    Entry entry;
    strcpy(entry.prev_state, prev_state);
    entry.speculator_object = speculator_object;
    this->entries[this->entry_cnt++] = entry;
    speculator_object->undologs.push_back(this);
}

void UndoLog::add_to_undo_log(SpeculatorObject *speculator_object, pid_t id){
    ProcessEntry p_entry;
    p_entry.speculator_object = speculator_object;
    this->p_entries[this->p_entry_cnt++]= (p_entry);
    speculator_object->undologs.push_back(this);
}

UndoLog::UndoLog(pid_t id, SpeculatorObject* speculator_object){
    this->add_to_undo_log(speculator_object, id);
}

void UndoLog::remove_entry(SpeculatorObject *speculator_object){
    int i = 0;

    int entry_idx = -1;
    int p_entry_idx = -1;

    for(; i < this->entry_cnt; i++){
        if(this->entries[i].speculator_object == speculator_object){
            entry_idx = i;
            break;
        }
    }

    for(i=0; i < this->p_entry_cnt; i++){
        if(this->p_entries[i].speculator_object == speculator_object){
            p_entry_idx = i;
            break;
        }
    }

    for(i = entry_idx; i < this->entry_cnt; i++){
        this->entries[i] = this->entries[i+1];
    }

    for(i = p_entry_idx; i < this->p_entry_cnt; i++){
        this->p_entries[i] = this->p_entries[i+1];
    }

}

bool UndoLog::speculation_exists(Entry entry) {
    bool ans = false;

    for(int i =0; i < this->entry_cnt; i++){
        Entry ent = this->entries[i];
        if(ent.speculator_object == entry.speculator_object){
            ans = true;
            break;
        }
    }
    return ans;
}

void UndoLog::revert_till(SpeculatorObject *speculator_object){
    std::vector<Entry> new_entries;

    int idx = -1;

    for(int i = 0; i < this->entry_cnt; i++){
        Entry entry = this->entries[i];
        
        if(entry.speculator_object == speculator_object) {
            int fd = open(file_name, O_RDONLY);
            write(fd, entry.prev_state, speculator_object->buffer_size);
            idx = i;
            
            break;
        }
    }

    if(idx == -1) return;
    this->entry_cnt = idx;
    for(int i = idx; i < MAX_SIZE; i++){
        this->entries[i] = *(new Entry());
    }

}

void UndoLog::add_dependencies(SpeculatorObject *speculator_object){
    /* TODO */
}