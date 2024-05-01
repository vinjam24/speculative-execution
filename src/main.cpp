#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#include "Speculator.h"
#include "Globals.h"
#include "Syscall.h"

#define BUFSIZE 4

int main() {


    const char* file_name = "/path/to/file";

    // Initialize Cache to Speculate values from
    char cached_value[BUFSIZE];
    for(int i=0;i<BUFSIZE;i++){
        cached_value[i] = 'a';
    }

    // Main execution
    char value[BUFSIZE];
    int fd = open(file_name, O_RDONLY); 
    cache[fd] = cached_value;
    speculator = new Speculator();

    speculative_read(fd, value, BUFSIZE);

    for(int i=0;i<100;i++){
        sleep(1);
        std::cout<<i<<std::endl;
    }
    
    close(fd);
    
    return 0;
}