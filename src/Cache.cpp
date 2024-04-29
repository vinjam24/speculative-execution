#include "Cache.h"

template <typename KeyType, typename ValueType>
Cache<KeyType, ValueType>::Cache(size_t cap) : capacity(cap), head(nullptr), tail(nullptr) {}

template <typename KeyType, typename ValueType>
Cache<KeyType, ValueType>::~Cache()
{
    for (auto &it : cacheMap)
    {
        delete it.second;
    }
}

template <typename KeyType, typename ValueType>
void Cache<KeyType, ValueType>::moveToHead(Node *node)
{
    if (node == head)
        return;

    if (node == tail)
    {
        tail = tail->prev;
        tail->next = nullptr;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    node->next = head;
    node->prev = nullptr;
    head->prev = node;
    head = node;
}

template <typename KeyType, typename ValueType>
void Cache<KeyType, ValueType>::put(KeyType key, ValueType value)
{
    auto it = cacheMap.find(key);
    if (it != cacheMap.end())
    {
        Node *node = it->second;
        node->value = value;
        moveToHead(node);
    }
    else
    {
        Node *newNode = new Node;
        newNode->value = value;
        newNode->prev = nullptr;
        newNode->next = head;
        if (head)
            head->prev = newNode;
        head = newNode;
        if (!tail)
            tail = newNode;
        cacheMap[key] = newNode;

        if (cacheMap.size() > capacity)
        {
            Node *temp = tail;
            tail = tail->prev;
            tail->next = nullptr;
            cacheMap.erase(cacheMap.find(temp->value));
            delete temp;
        }
    }
}

template <typename KeyType, typename ValueType>
bool Cache<KeyType, ValueType>::get(KeyType key, ValueType &value)
{
    auto it = cacheMap.find(key);
    if (it != cacheMap.end())
    {
        value = it->second->value;
        moveToHead(it->second);
        return true;
    }
    return false;
}
