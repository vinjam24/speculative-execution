#include "Speculator.h"
#include "Globals.h"
#include "SpeculatorObject.h"
#include "UndoLog.h"
#include <cstring>
#include <iostream>
#include <ostream>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

/*TODO: Change Speculator::speculator_object */


Speculator::Speculator() {
  Speculator::spec_counter = 0;
  speculator_object = new SpeculatorObject;
  this->file_ul_cnt = 0;
  this->pid_ul_cnt = 0;
  this->id_spec_cnt = 0;
}

void Speculator::propogate_buffer(){

    while(!Speculator::IO_buffer_queue.isEmpty()){
        std::cout<<"hey "<<IO_buffer_queue.size()<<std::endl;

        std::pair<const char*, va_list*> buffer_object = Speculator::IO_buffer_queue.dequeue();
        const char* output_string = buffer_object.first;
        va_list* args = buffer_object.second;
        std::cout<<"hey "<<output_string<<" "<<buffer_object.second<<std::endl;

        // vprintf(output_string, *args);
    }
    return;
}

void Speculator::discard_buffer(){
    while(!Speculator::IO_buffer_queue.isEmpty()){
        Speculator::IO_buffer_queue.dequeue();
    }
    return;
}

int Speculator::create_speculation(pid_t ppid, pid_t pid, int file_descriptor,
                                    int buffer_size, int pipe) {
  SpeculatorObject* spec = new SpeculatorObject;

  spec->child_pid = pid;
  spec->is_speculative = true;
  spec->cache_object = cache[file_descriptor];
  spec->buffer_size = buffer_size;
  spec->pipe_fd = pipe;
  spec->parent_pid = ppid;
  this->id_to_speculation[this->id_spec_cnt].spec = spec;
  this->id_to_speculation[this->id_spec_cnt++].spec_id = this->spec_counter++;
  /*TODO: Remove below*/

  speculator_object->is_speculative = true;
  speculator_object->child_pid = pid;
  speculator_object->cache_object = cache[file_descriptor];
  speculator_object->buffer_size = buffer_size;
  speculator_object->pipe_fd = pipe;
  speculator_object->parent_pid = ppid;

  process_to_undo_log_map[this->pid_ul_cnt].undo_log = new UndoLog(pid, spec);
  process_to_undo_log_map[this->pid_ul_cnt].pid =ppid;

  spec->undologs.push_back(process_to_undo_log_map[this->pid_ul_cnt++].undo_log);

  std::cout << "Created Speculation!" << std::endl;
  return Speculator::spec_counter++;
}

void Speculator::validate_speculation(std::vector<char> actual_buffer, int buffer_size, int spec_id){
    bool fail_flag = false;
    for(int i=0;i<buffer_size;i++){
        if(actual_buffer[i] != speculator_object->cache_object[i]){
            speculator_object->cache_object[i] = actual_buffer[i];
            fail_flag = true;
        }
    }
    fail_flag ? fail_speculation(spec_id) : commit_speculation(spec_id);
    return;
}

SpeculatorObject* Speculator::findWith(int spec_id){
  int spec_idx;
  SpeculatorObject* speculator_object = nullptr;
  // Find speculator object with the given spec_id;
  for(int i =0; this->spec_counter; i++) {
    if(this->id_to_speculation[i].spec_id == spec_id){
      speculator_object = this->id_to_speculation[i].spec;
      break;
    }
  }
  return speculator_object;
}

void Speculator::commit_speculation(int spec_id) {
  
  
  // SpeculatorObject* speculator_object = id_to_speculation[spec_id];
  SpeculatorObject* speculator_object = this->findWith(spec_id);
  // Remove entries in undologs with the given speculator;

  Speculator::propogate_buffer();
  Speculator::delete_undo_log_entries(speculator_object);

  kill(speculator_object->child_pid, SIGKILL);

  std::cout << "Commit Speculation!" << std::endl;
  return;
}


void Speculator::revert_kernel_undo_logs(SpeculatorObject* speculator_object){
  std::vector<UndoLog*> undos = speculator_object->undologs;
  for(int i = 0 ; i < undos.size(); i++){
    undos[i]->revert_till(speculator_object);
  }
}

void Speculator::delete_undo_log_entries(SpeculatorObject* speculator_object){
  int i = 0;
  std::vector<UndoLog*> undologs = speculator_object->undologs;
  for(i = 0; i < undologs.size(); i++){
    UndoLog* ul =  undologs[i];
    ul->remove_entry(speculator_object);
  }
}

void Speculator::fail_speculation(int spec_id) {

  ssize_t bytes_written =
      write(speculator_object->pipe_fd, speculator_object->cache_object,
            speculator_object->buffer_size);
  close(speculator_object->pipe_fd);
  SpeculatorObject* speculator_object = this->findWith(spec_id);
  // revert state of kernel objects using undo log
  Speculator::revert_kernel_undo_logs(speculator_object);


  // Put child process back on run queue
  //change here to revert process
  /*TODO*/
  kill(speculator_object->child_pid, SIGUSR1);
  std::cout << "Fail Speculation!" << std::endl;

    Speculator::discard_buffer();
    Speculator::speculator_object->is_speculative = false;
    return;
}

int Speculator::buffer_IO(const char *output_value, va_list* args)
{   
    if(!Speculator::speculator_object->is_speculative){
        return -1;
    }

    Speculator::speculator_object->IO_buffer_queue.push(std::make_pair(output_value, args));
    return 0;
}

UndoLog* Speculator::findProcessUndoWith(int p_id){
  UndoLog* undo = nullptr;
  int undo_idx = -1;
  for(int i =0 ; i < this->pid_ul_cnt; i++){
    if(this->process_to_undo_log_map[i].pid == p_id){
      undo = this->process_to_undo_log_map[i].undo_log;
      break;
    }
  }
  return undo;
}

UndoLog* Speculator::findFileUndoWith(char* file_name){
  int undo_idx = -1;
  for(int i = 0; i < this->file_ul_cnt; i++){
    if(strcmp(this->file_to_undo_log_map[i].file_name,file_name) ==0 ){
      return this->file_to_undo_log_map[i].undo_log;
    }
  }

  return nullptr;
}

int Speculator::write_speculatively(char* file_name, const char *content,
                                    int buffer_size, pid_t pid) {
/*TODO*/                      
  // if (!Speculator::speculator_object->is_speculative) {
  //   return -1;
  // }

  std::cout<< "writing speculatively\n";
  
  std::cout << pid << std::endl;
  UndoLog* undo_tmp = this->findProcessUndoWith(pid);
  SpeculatorObject* speculator_object = undo_tmp->entries[undo_tmp->entry_cnt].speculator_object;

  char prev_state[buffer_size+1];
  int fd = open(file_name, O_RDWR);
  // Adding just one speculation
  read(fd, prev_state, buffer_size);
  if(this->findFileUndoWith(file_name) == nullptr){
  // if (Speculator::file_to_undo_log_map.find(file_name) ==
      // Speculator::file_to_undo_log_map.end()) {

        // If there is no undo log corresponding to the file
        UndoLog* undo = new UndoLog(prev_state, speculator_object, file_name);
        file_to_undo_log_map[this->file_ul_cnt].undo_log = undo;
        strcpy(file_to_undo_log_map[this->file_ul_cnt].file_name, file_name);
  }
  else {
    UndoLog* undo = this->findFileUndoWith(file_name);
    UndoLog* process_undo = this->findProcessUndoWith(pid);
    auto entries = this->findProcessUndoWith(pid)->entries;
    for(int i = 0; i < process_undo->entry_cnt; i++){
      auto entry = process_undo->entries[i];
      undo->add_to_undo_log(entry.speculator_object, entry.prev_state);
    // for(auto entry: this->process_to_undo_log_map[pid]->entries) {
      // undo->add_to_undo_log(entries.speculator_object, entry.prev_state);
    }
    //Add to process undo log
    UndoLog *process_ul = this->findProcessUndoWith(speculator_object->parent_pid);
    int n = undo->entry_cnt;
    /* TODO */
    // for(int i = n-2; i >=0; i--){
    //   if(!process_ul->speculation_exists(undo->entries[i])){
    //     // create checkpoint and add it to 
    //   }
    // }
    //process_ul->add_dependencies(Speculator::speculator_object);
  }


  if (ftruncate(fd, 0) == -1) {
        std::cerr << "Failed to truncate file\n";
        close(fd);
        return 0;
  }
  write(fd, content, buffer_size);
  close(fd);
  return 0;

  // Where to store the undo log
}