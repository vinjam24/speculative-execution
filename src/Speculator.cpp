#include "Speculator.h"
#include "Globals.h"
#include "SpeculatorObject.h"
#include "UndoLog.h"
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
  Speculator::id_to_speculation[Speculator::spec_counter] = spec;

  /*TODO: Remove below*/

  speculator_object->is_speculative = true;
  speculator_object->child_pid = pid;
  speculator_object->cache_object = cache[file_descriptor];
  speculator_object->buffer_size = buffer_size;
  speculator_object->pipe_fd = pipe;
  speculator_object->parent_pid = ppid;

  process_to_undo_log_map[ppid] = new UndoLog(pid, spec);
  spec->undologs.push_back(process_to_undo_log_map[ppid]);

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

void Speculator::commit_speculation(int spec_id) {

  
  SpeculatorObject* speculator_object = id_to_speculation[spec_id];

  // Remove entries in undologs with the given speculator;

  Speculator::propogate_buffer();
  Speculator::delete_undo_log_entries(speculator_object);

  kill(speculator_object->child_pid, SIGKILL);

  std::cout << "Commit Speculation!" << std::endl;
  return;
}

void Speculator::fail_speculation(int spec_id) {

  ssize_t bytes_written =
      write(speculator_object->pipe_fd, speculator_object->cache_object,
            speculator_object->buffer_size);
  close(speculator_object->pipe_fd);
  SpeculatorObject* speculator_object = id_to_speculation[spec_id];
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

int Speculator::write_speculatively(char* file_name, const char *content,
                                    int buffer_size, pid_t pid) {
/*TODO*/                      
  // if (!Speculator::speculator_object->is_speculative) {
  //   return -1;
  // }

  std::cout<< "writing speculatively\n";
  
  std::cout << pid << std::endl;
  SpeculatorObject* speculator_object = this->process_to_undo_log_map[pid]->entries.back().speculator_object;

  char prev_state[buffer_size+1];
  int fd = open(file_name, O_RDWR);
  // Adding just one speculation
  read(fd, prev_state, buffer_size);
  if (Speculator::file_to_undo_log_map.find(file_name) ==
      Speculator::file_to_undo_log_map.end()) {

        // If there is no undo log corresponding to the file
        UndoLog* undo = new UndoLog(prev_state, speculator_object, file_name);
        file_to_undo_log_map[file_name] = undo;
  }
  else {
    UndoLog* undo = file_to_undo_log_map[file_name];
    for(auto entry: this->process_to_undo_log_map[pid]->entries) {
      undo->add_to_undo_log(entry.speculator_object, entry.prev_state);
    }
    //Add to process undo log
    UndoLog *process_ul = process_to_undo_log_map[speculator_object->parent_pid];
    int n = undo->entries.size();
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