#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#include "Speculator.h"
#include "Globals.h"
#include "Syscall.h"

#define BUFSIZE 4

using namespace std;
int main() {


    const char* file_name = "./files/example.txt";
    const char* write_spec_file_name = "./files/example1.txt";
    const char* content = "pook";


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

    write_speculatively(write_spec_file_name, content, BUFSIZE);



    for(int i=0;i<BUFSIZE;i++){
        sleep(1);
        speculative_print("%c\n", value[i]);
    }
    
    close(fd);
    
    return 0;
}