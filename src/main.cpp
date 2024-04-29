#include <iostream>

#include "Speculator.h"
using namespace std;

#define BUFSIZE 4

int main() {

    char* file_name = "/path/to/file";

    int value;

    int fd = open(file_name, O_RDOLY); 
    

    while((n=read(fd, value, BUFSIZE)) > 0){
        std::cout<<"File read!"<<std::endl;
    }
    
    return 0;
}