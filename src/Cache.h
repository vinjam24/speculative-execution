#ifndef CACHE_H
#define CACHE_H
#include <unordered_map>
#include <string>

// template<typename K, typename V>
class Cache{
    private:
        int x;
        std::unordered_map<std::string, std::string> mp;
    
    public:
        Cache(size_t capacity);
        ~Cache();
        void setX(int x);
        int getX();
};

#endif