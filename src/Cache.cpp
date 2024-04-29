#include "Cache.h"
#include <stdexcept>
#include <unordered_map>
#include <iostream>
using namespace std;
template<typename K, typename V>
Cache<K,V>::Cache(size_t cap) : capacity(cap), head(nullptr), tail(nullptr) {}

template<typename K, typename V>
int Cache<K,V>::getX(){
    return x;
}

template<typename K, typename V>
void Cache<K,V>::setX(int value){
    x = value;
}

template<typename K, typename V>
Cache<K,V>::~Cache(){}

template<typename K, typename V>
void Cache<K,V>::moveToHead(Node* node){
    if(node == head){
        return;
    }
    if(node == tail) {
        tail = tail->prev;
        tail->next = nullptr;
    }
    else{
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    head = node;

}

template<typename K, typename V>
void Cache<K,V>::put(K key, V value){
    
    typename std::unordered_map<K, Node*>::const_iterator it = mp.find(key);

    if (it == mp.end()){
        Node* newNode = new Node;

        newNode->val = value;
        newNode->prev = nullptr;
        newNode->next = head;

        if(head) {
            head->prev = newNode;
        }
        head = newNode;

        if(!tail){
            tail = newNode;
        }

        mp[key] = newNode;
    }
    else {
        Node *node = it->second;
        node->val = value;
        moveToHead(node);
    }

    cout << "Added " << key << "->" << value << endl;
}

template<typename K, typename V>
V Cache<K, V>::get(K key){
    typename std::unordered_map<K, Node*>::const_iterator it = mp.find(key);
    if(it == mp.end()){
        // What to do in the case of cache-miss? 
        cout << "Cache miss for key : " << key << endl;
        throw runtime_error("Cache Miss");
    }
    else {
        moveToHead(it->second);
        cout << "Value of cache with " << key << " key is: " << it->second->val << endl;
        return it->second->val;
    }
}