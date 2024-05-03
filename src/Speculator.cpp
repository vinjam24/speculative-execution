#include "Speculator.h"
#include "Globals.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include<vector>
#include "SpeculatorObject.h"


Speculator::Speculator() {}

void Speculator::create_speculation(pid_t pid, int file_descriptor, int buff, int pipe){
    child_process = pid;
    cache_object = cache[file_descriptor];
    buffer_size = buff;
    pipe_value = pipe;
    std::cout<<"Created Speculation!"<<std::endl;
}

void Speculator::validate_speculation(std::vector<char> actual_buffer, int buffer_size){
    bool fail_flag = false;
    for(int i=0;i<buffer_size;i++){
        if(actual_buffer[i] != cache_object[i]){
            cache_object[i] = actual_buffer[i];
            fail_flag = true;
            std::cout<<actual_buffer[i]<<" "<<cache_object[i]<<std::endl;
        }
    }
    fail_flag ? fail_speculation() : commit_speculation();
    return;
}

void Speculator::commit_speculation(){
    kill(child_process, SIGKILL);
    std::cout<<"Commit Speculation!"<<std::endl;
    return;
}

void Speculator::fail_speculation(){

    ssize_t bytes_written = write(pipe_value, cache_object, buffer_size);
    // Put child process back on run queue
    kill(child_process, SIGUSR1);
    std::cout<<"Fail Speculation!"<<std::endl;
    return;
}