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

void Speculator::create_speculation(pid_t pid, int file_descriptor,
                                    int buffer_size, int pipe) {
  speculator_object->is_speculative = true;
  speculator_object->child_pid = pid;
  speculator_object->cache_object = cache[file_descriptor];
  speculator_object->buffer_size = buffer_size;
  speculator_object->pipe_fd = pipe;
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

  Speculator::propogate_buffer();
  Speculator::speculator_object->is_speculative = false;

  std::cout << "Commit Speculation!" << std::endl;
  return;
}

void Speculator::fail_speculation() {

  ssize_t bytes_written =
      write(speculator_object->pipe_fd, speculator_object->cache_object,
            speculator_object->buffer_size);
  close(speculator_object->pipe_fd);

  // Put child process back on run queue
  kill(speculator_object->child_pid, SIGUSR1);
  std::cout << "Fail Speculation!" << std::endl;

  Speculator::discard_buffer();
  Speculator::speculator_object->is_speculative = false;
  return;
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
        UndoLog* undo = new UndoLog(prev_state, Speculator::speculator_object);
        file_to_undo_log_map[file_name] = undo;
  }
  else {
    UndoLog* undo = file_to_undo_log_map[file_name];
    undo->add_to_undo_log(Speculator::speculator_object, prev_state);
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