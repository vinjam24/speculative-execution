#ifndef CACHE_H
#define CACHE_H
using namespace std;
#include <unordered_map>
#include <iostream>

template<typename KeyType = string, typename ValueType>
class Cache {
private:
    struct Node {
        ValueType value;
        Node* prev;
        Node* next;
    };

    unordered_map<KeyType, Node*> cacheMap;
    Node* head;
    Node* tail;
    size_t capacity;

    void moveToHead(Node* node);

public:
    Cache(size_t cap);

    ~Cache();

    void put(KeyType key, ValueType value);

    bool get(KeyType key, ValueType& value);
};

#endif // CACHE_H
