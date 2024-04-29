#include <iostream>
#include <string>
#include "Speculator.h"
#include "Cache.h"
using namespace std;

int main() {
    Speculator speculator;
    speculator.create_speculation();
    Cache stringCache(100);
    // stringCache.put("key1", "value1");
    // stringCache.put("key2", "value2");

    // std::string value;
    // if (stringCache.get("key1", value)) {
    //     std::cout << "Value of key 'key1': " << value << std::endl;
    // }
    return 0;
}