#include "Syscall.h"
#include "SpeculatorObject.h"
#include "Speculator.h"
#include "Globals.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <iostream>
#include<vector>

void thread_function(pid_t pid, int file_descriptor, char* buffer, int buffer_size, int pipe_value){

    // Create Speculation in the detached thread
    speculator->create_speculation(pid, file_descriptor, buffer_size, pipe_value);
    std::vector<char> actual_buffer(buffer_size);
    ssize_t bytes_read;
    if((read(file_descriptor, actual_buffer.data(), buffer_size)) > 0){
        std::cout<<"Actual Reading"<<std::endl;
    }else{
        std::cout<<"Error"<<std::endl;
    }
    speculator->validate_speculation(actual_buffer, buffer_size);    
    return;
}

using namespace std;

int speculative_read(int file_descriptor, char* buffer, int buffer_size){

    // 1. Create child process
    int pipe_fds[2];
    pipe(pipe_fds);
    pid_t pid = fork();

    if(pid == 0){
        // 1.1 Save kernel Objects in Child Process
        signal(SIGUSR1, [](int signum){
            return;
        });
        pause();
        close(pipe_fds[1]);
        char check[4];

        std::cout<<read(pipe_fds[0], check, 4)<<std::endl;
        std::cout<<"Hell! "<<buffer_size<<std::endl;
        for(int i=0;i<buffer_size;i++){
            std::cout<<"Hi "<<std::endl;
            std::cout<<buffer[i]<<std::endl;
        }

        return 1;

    } else if (pid > 0) {
        // 2. Create a speculative Object and save speculative state
        
        char* cached_buffer = cache[file_descriptor];
        for(int i=0; i<buffer_size; i++){
            buffer[i] = cached_buffer[i];
        }
        // 3. Spawn a thread to perform actual read operation
        std::thread t1(thread_function, pid, file_descriptor, buffer, buffer_size, pipe_fds[1]);
        t1.detach();
        return 1;


    }
}