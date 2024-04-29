#ifndef SYSCALL_H
#define SYSCALL_H

class Syscall{
    public:
        int speculative_read(int file_descriptor, char* buffer, int buffer_size);
};


#endif