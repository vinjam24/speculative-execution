#include "Speculator.h"
#include "Globals.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "SpeculatorObject.h"


Speculator::Speculator() : IO_buffer_queue(){
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

void Speculator::create_speculation(pid_t ppid, pid_t pid, int file_descriptor, int buffer_size, int pipe){

    Speculator::speculator_object.is_speculative = true;
    Speculator::speculator_object.child_pid = pid;
    Speculator::speculator_object.parent_pid = ppid;
    Speculator::speculator_object.cache_object = cache[file_descriptor];
    Speculator::speculator_object.buffer_size = buffer_size;
    Speculator::speculator_object.pipe_fd = pipe;
    std::cout<<"Created Speculation!"<<std::endl;
}

void Speculator::validate_speculation(std::vector<char> actual_buffer, int buffer_size){
    bool fail_flag = false;
    for(int i=0;i<buffer_size;i++){
        if(actual_buffer[i] != speculator_object.cache_object[i]){
            speculator_object.cache_object[i] = actual_buffer[i];
            fail_flag = true;
        }
    }
    fail_flag ? fail_speculation() : commit_speculation();

    return;
}

void Speculator::commit_speculation(){
    std::cout<<"Started Commit"<<std::endl;    
    Speculator::propogate_buffer();

    kill(speculator_object.child_pid, SIGKILL);


    Speculator::speculator_object.is_speculative = false;

    std::cout<<"Commit Speculation!"<<std::endl;    
    return;
}

void Speculator::fail_speculation(){

    ssize_t bytes_written = write(speculator_object.pipe_fd, speculator_object.cache_object, speculator_object.buffer_size);
    close(speculator_object.pipe_fd);
    
    // Put child process back on run queue
    kill(speculator_object.child_pid, SIGUSR1);
    std::cout<<"Fail Speculation!"<<std::endl;

    Speculator::discard_buffer();
    Speculator::speculator_object.is_speculative = false;
    kill(speculator_object.parent_pid, SIGKILL);
    return;
}

int Speculator::buffer_IO(const char *output_value, va_list* args)
{   

    if(!Speculator::speculator_object.is_speculative){
        return -1;
    }

    std::pair<const char*, va_list*> p = std::make_pair(output_value, args);

    Speculator::IO_buffer_queue.enqueue(p);
    std::cout<<"hey "<<std::string(output_value)<<std::endl;


    
    return 0;
}
