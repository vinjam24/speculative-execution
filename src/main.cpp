#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include "Speculator.h"
#include "Globals.h"
#include "Syscall.h"

#define BUFSIZE 4


using namespace std;
int main() {
    
    const char* file_name = "./files/example.txt";
    char* write_spec_file_name = "./files/example1.txt";
    const char* content = "bhav";

    

    // Initialize Cache to Speculate values from
    char cached_value[BUFSIZE];
    for(int i=0;i<BUFSIZE;i++){
        cached_value[i] = 'a';
    }

    // Initialize Speculator
    // int shmid = shmget(IPC_PRIVATE, sizeof(Speculator), IPC_CREAT | 0666);
    // Speculator* speculator = (Speculator*)shmat(shmid, NULL, 0);
    // std::cout<<speculator->speculative
    // Main execution
    initializeGlobalSharedObject();
    

    char value[BUFSIZE];
    int fd = open(file_name, O_RDWR);
    int koj = open(write_spec_file_name, O_RDONLY);

    cache[fd] = cached_value;

    speculative_read(fd, value, BUFSIZE);
    
 
    
    close(fd);
    // write_speculatively(write_spec_file_name, content, BUFSIZE);
    for(int i=0;i<BUFSIZE;i++){
        speculative_print("%c\n", value[i]);
    }
    sleep(5);

    return 0;
}