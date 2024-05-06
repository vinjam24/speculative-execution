#ifndef SYSCALL_H
#define SYSCALL_H

int speculative_read(int file_descriptor, char* buffer, int buffer_size);

int speculative_print(const char* output_value, ...);

int write_speculatively(char* file_name, const char* buffer, int buffer_size);

#endif