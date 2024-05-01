#include "Speculator.h"
#include "Globals.h"
#include <iostream>
#include <signal.h>


Speculator::Speculator() {}

void Speculator::create_speculation(pid_t pid, int file_descriptor){
    char* pid_key = "PID";
    char* cache_object_key = "return_value";
    child_processes->kernelObjects[pid_key] = pid;
    cache_objects->kernelObjects[cache_object_key] = cache[file_descriptor];
    std::cout<<"Created Speculation!"<<std::endl;
}

void Speculator::validate_speculation(char* actual_buffer, int buffer_size){
    for(int i=0;i<buffer_size;i++){
        if(actual_buffer[i] != cache_objects->kernelObjects["return_value"][i]){
            Speculator::fail_speculation();
        }
    }
    Speculator::commit_speculation();

}

void Speculator::commit_speculation(){
    kill(child_processes->kernelObjects["PID"], SIGKILL);
    std::cout<<"Commit Speculation!"<<std::endl;
}

void Speculator::fail_speculation(){
    kill(child_processes->kernelObjects["PID"], SIGUSR1);
    std::cout<<"Fail Speculation!"<<std::endl;
}