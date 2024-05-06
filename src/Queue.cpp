#include <iostream>
#include "Queue.h"

using namespace std;

Queue::Queue() : frontIndex(0), rearIndex(0) {}

bool Queue::isEmpty() const {
    if(frontIndex == 0 && rearIndex == 0){
        return true;
    }
    return false;
}

bool Queue::isFull() const {
    return (rearIndex + 1) % MAX_SIZE == frontIndex;
}

int Queue::size() const {
    return (rearIndex - frontIndex);
}

void Queue::enqueue(std::pair<const char*, va_list*> value) {

    if (isFull()) {
        cout << "Queue is full. Cannot enqueue." << endl;
        return;
    }
    if (isEmpty()) {
        frontIndex = rearIndex = 1;
    } else {

        rearIndex = (rearIndex + 1) % MAX_SIZE;
    }

    data[rearIndex] = value;

}

std::pair<const char*, va_list*> Queue::dequeue() {
    
    std::pair<const char*, va_list*> dequeuedValue = data[frontIndex];
    if (frontIndex == rearIndex) {
        frontIndex = rearIndex = 0;
    } else {
        frontIndex = (frontIndex + 1) % MAX_SIZE;
    }
    return dequeuedValue;
}
