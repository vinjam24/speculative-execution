#ifndef SYSCALL_H
#define SYSCALL_H

int speculative_read(int file_descriptor, char* buffer, int buffer_size);

int speculative_print(const char* output_value, ...);

#endif