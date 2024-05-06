#include "Syscall.h"
#include "SpeculatorObject.h"
#include "Speculator.h"
#include "Globals.h"
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <iostream>
#include <vector>
#include <cstdarg>
void thread_function(pid_t pid, pid_t parent_pid, int file_descriptor, char* buffer, int buffer_size, int pipe_value){
    // Create Speculation in the detached thread
    int spec_id = speculator->create_speculation(pid, parent_pid, file_descriptor, buffer_size, pipe_value);
    std::vector<char> actual_buffer(buffer_size);
    ssize_t bytes_read;

    if((read(file_descriptor, actual_buffer.data(), buffer_size)) > 0){
        std::cout<<"Actual Reading"<<std::endl;
    }else{
        std::cout<<"Error"<<std::endl;
    }
    speculator->validate_speculation(actual_buffer, buffer_size, spec_id);    
    return;
}

using namespace std;

int speculative_print(const char* output_string, ...){
    va_list args;
    int count_size = 0;
    va_start(args, output_string);
    if(speculator->buffer_IO(output_string, &args) == -1){
        vprintf(output_string, args);
    }

    va_end(args);
    
    return count_size;
}

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
        read(pipe_fds[0], buffer, buffer_size);
        close(pipe_fds[0]);
        return 1;

    } else if (pid > 0) {
        // 2. Create a speculative Object and save speculative state
        pid_t actual_pid = getpid();

        int spec_id = speculator->create_speculation(actual_pid, pid, file_descriptor, buffer_size, pipe_fds[1]);

        pid_t async_proc_pid = fork();
        if(async_proc_pid > 0){
            char* cached_buffer = cache[file_descriptor];
            for(int i=0; i<buffer_size; i++){
                buffer[i] = cached_buffer[i];
            }

        }else if(async_proc_pid == 0){

            close(pipe_fds[0]);
            std::vector<char> actual_buffer(buffer_size);
            ssize_t bytes_read;

            if((read(file_descriptor, actual_buffer.data(), buffer_size)) > 0){
                std::cout<<"Actual Reading"<<std::endl;
            }else{
                std::cout<<"Error"<<std::endl;
            }
            speculator->validate_speculation(actual_buffer, buffer_size, spec_id);    
            kill(getpid(), SIGTERM);

        }
        
        
        return 1;


    }
}

int write_speculatively(char* file_name, const char* buffer, int buffer_size){
    pid_t pid = getpid(); 
    cout << "kojjaaa\n";
    speculator->write_speculatively(file_name, buffer, buffer_size, pid);
    return 0;
}
