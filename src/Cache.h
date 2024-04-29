#ifndef CACHE_H
#define CACHE_H
#include <unordered_map>
#include <string>


template<typename K, typename V>
class Cache{
    private:

        struct Node {
            V val;
            Node* prev;
            Node* next;
        };
        size_t capacity;
        int x;
        std::unordered_map<K, Node*> mp;
        Node* head;
        Node* tail;
        void moveToHead(Node* node);
    
    public:

        Cache(size_t cap);
        ~Cache();
        
        void put(K key, const V value);
        V get(K key);

        
        void setX(int x);
        int getX();
};

#endif