#include <iostream>
#include <string>
#include "Speculator.h"
#include "Cache.h"
#include "Cache.cpp"
using namespace std;

int main() {
    Speculator speculator;
    speculator.create_speculation();
    Cache<std::string,std::string> stringCache(100);
    stringCache.put("key1", "value1");
    stringCache.put("key2", "value2");
    stringCache.put("key2", "value2");

    std::string value = stringCache.get("key1");
    stringCache.put("key1", "value3");
    std::string val2 = stringCache.get("key1");
    return 0;
}