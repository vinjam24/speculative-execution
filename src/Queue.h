#ifndef QUEUE_H
#define QUEUE_H
#include <utility>
#include <cstdarg>
#include <string>

#define MAX_SIZE 1000

class Queue {
private:
    int frontIndex; // Index of the front element
    int rearIndex; // Index of the rear element
    std::pair<const char*, va_list*> data[MAX_SIZE]; // Array to store elements
public:
    Queue(); // Constructor
    int size() const;
    bool isEmpty() const; // Check if the queue is empty
    bool isFull() const; // Check if the queue is full
    void enqueue(std::pair<const char*, va_list*> value); // Add an element to the rear of the queue
    std::pair<const char*, va_list*> dequeue(); // Remove and return the front element of the queue
};

#endif
