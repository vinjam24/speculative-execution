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

using namespace std;

int speculative_read(int file_descriptor, char* buffer, int buffer_size){

    // 1. Create child process
    pid_t pid = fork();

    if(pid == 0){
        // 1.1 Save kernel Objects in Child Process
        signal(SIGUSR1, [](int signum){
            return;
        });
        pause();

    } else if (pid > 0) {
        // 2. Create a speculative Object and save speculative state
        speculator->create_speculation(pid, file_descriptor);
        char* cached_buffer = cache[file_descriptor];
        for(int i=0; i<buffer_size; i++){
            buffer[i] = cached_buffer[i];
        }

        // 3. Spawn a thread to perform actual read operation
        std::thread t1([](int file_descriptor, int buffer_size, Speculator* speculator){
            char actual_buffer[buffer_size];
            int n;
            while((read(file_descriptor, actual_buffer, buffer_size)) > 0){
                std::cout<<"Actual Reading"<<std::endl;
            }
            speculator->validate_speculation(actual_buffer, buffer_size);
            
            
        }, file_descriptor, buffer_size, speculator);

        return 1;


    }
}