#include "Syscall.h"
#include "SpeculativeObject.h"
#include "Speculator.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <thread>

Syscall::Syscall() {};


Syscall::int speculative_read(int file_descriptor, char* buffer, int buffer_size){

    
    // 1. Create child process
    pid_t pid = fork();

    if(pid == 0){
        // 1.1 Save kernel Objects in Child Process
        if (setpriority(PRIO_PROCESS, 0, 20) == -1) {
            return 1;
        }

    } else if (pid > 0) {
        // 2. Create a speculative Object and save speculative state
        Speculator *speculator = new Speculator();

        speculator->create_speculation();

        // 3. Spawn a thread to perform actual read operation
        char* actual_buffer;
        std::thread t1([](file_descriptor, buffer_size){
            while((n = read(file_descriptor, actual_buffer, buffer_size)) > 0){
                std::cout<<"Actual Reading"<<std::endl;
            }
        }, file_descriptor, buffer_size);

        

        // 4. Commit/fail speculation based on thread's output
        if(actual_buffer != )



    }

    

}