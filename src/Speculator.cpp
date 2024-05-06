#include "Speculator.h"
#include "Globals.h"
#include "SpeculatorObject.h"
#include "UndoLog.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

Speculator::Speculator() { speculator_object = new SpeculatorObject; }

void Speculator::propogate_buffer() {
  while (!Speculator::speculator_object->IO_buffer_queue.empty()) {
    std::pair<const char *, va_list *> buffer_object =
        Speculator::speculator_object->IO_buffer_queue.front();
    const char *output_string = buffer_object.first;
    va_list *args = buffer_object.second;
    vprintf(output_string, *args);
    Speculator::speculator_object->IO_buffer_queue.pop();
  }
  return;
}

void Speculator::discard_buffer() {
  while (!Speculator::speculator_object->IO_buffer_queue.empty()) {
    Speculator::speculator_object->IO_buffer_queue.pop();
  }
  return;
}

void Speculator::create_speculation(pid_t pid, pid_t parent_pid, int file_descriptor,
                                    int buffer_size, int pipe) {
  speculator_object->is_speculative = true;
  speculator_object->child_pid = pid;
  speculator_object->cache_object = cache[file_descriptor];
  speculator_object->buffer_size = buffer_size;
  speculator_object->pipe_fd = pipe;
  speculator_object->parent_pid = parent_pid;

  process_to_undo_log_map[parent_pid] = new UndoLog(pid, Speculator::speculator_object);
  Speculator::speculator_object->undologs.push_back(process_to_undo_log_map[parent_pid]);

  std::cout << "Created Speculation!" << std::endl;
}

void Speculator::validate_speculation(std::vector<char> actual_buffer,
                                      int buffer_size) {
  bool fail_flag = false;
  for (int i = 0; i < buffer_size; i++) {
    if (actual_buffer[i] != speculator_object->cache_object[i]) {
      speculator_object->cache_object[i] = actual_buffer[i];
      fail_flag = true;
    }
  }
  fail_flag ? fail_speculation() : commit_speculation();
  return;
}

void Speculator::commit_speculation() {

  kill(speculator_object->child_pid, SIGKILL);

  // Remove entries in undologs with the given speculator;

  Speculator::propogate_buffer();
  Speculator::delete_undo_log_entries(Speculator::speculator_object);
  Speculator::speculator_object->is_speculative = false;

  std::cout << "Commit Speculation!" << std::endl;
  return;
}

void Speculator::fail_speculation() {

  ssize_t bytes_written =
      write(speculator_object->pipe_fd, speculator_object->cache_object,
            speculator_object->buffer_size);
  close(speculator_object->pipe_fd);

  // revert state of kernel objects using undo log
  Speculator::revert_kernel_undo_logs(Speculator::speculator_object);


  // Put child process back on run queue
  //change here to revert process
  /*TODO*/
  kill(speculator_object->child_pid, SIGUSR1);
  std::cout << "Fail Speculation!" << std::endl;

  Speculator::discard_buffer();
  Speculator::speculator_object->is_speculative = false;
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

int Speculator::buffer_IO(const char *output_value, va_list *args) {
  if (!Speculator::speculator_object->is_speculative) {
    return -1;
  }

  Speculator::speculator_object->IO_buffer_queue.push(
      std::make_pair(output_value, args));
  return 0;
}

int Speculator::write_speculatively(char* file_name, const char *content,
                                    int buffer_size) {
/*TODO*/                      
  // if (!Speculator::speculator_object->is_speculative) {
  //   return -1;
  // }
  char prev_state[buffer_size+1];
  int fd = open(file_name, O_RDWR);
  // Adding just one speculation
  read(fd, prev_state, buffer_size);
  if (Speculator::file_to_undo_log_map.find(file_name) ==
      Speculator::file_to_undo_log_map.end()) {

        // If there is no undo log corresponding to the file
        UndoLog* undo = new UndoLog(prev_state, Speculator::speculator_object, file_name);
        file_to_undo_log_map[file_name] = undo;
  }
  else {
    UndoLog* undo = file_to_undo_log_map[file_name];
    undo->add_to_undo_log(Speculator::speculator_object, prev_state);
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