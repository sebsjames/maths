// Can you use a sm::vec as a key to an std::unordered_map?
//
#include <string>
#include <unordered_map>
#include <iostream>

import sm.vec;

int main()
{
    int rtn = 0;

    // Some keys
    sm::vec<int, 2> k1 = {1,2};
    sm::vec<int, 2> k2 = {1,3};
    sm::vec<int, 2> k3 = {2,3};
    sm::vec<int, 2> k4 = {2,4};

    // With unordered_map, you use vec's hash function as the third argument:
    std::unordered_map<sm::vec<int, 2>, std::string, sm::vec<int, 2>::hash> themap;

    themap[k1] = std::string("value1");
    themap[k2] = std::string("value2");
    themap[k3] = std::string("value3");
    themap[k4] = std::string("value4");

    std::cout << "Map size is " << themap.size() << std::endl;
    if (themap.size() != 4) { --rtn; }

    std::cout << "map with k1 returns " << themap[k1] << " (should be \"value1\")" << std::endl;
    if (themap[k1] != std::string("value1")) { --rtn; }
    std::cout << "map with k2 returns " << themap[k2] << " (should be \"value2\")" << std::endl;
    if (themap[k2] != std::string("value2")) { --rtn; }
    std::cout << "map with k3 returns " << themap[k3] << " (should be \"value3\")" << std::endl;
    if (themap[k3] != std::string("value3")) { --rtn; }
    std::cout << "map with k4 returns " << themap[k4] << " (should be \"value4\")" << std::endl;
    if (themap[k4] != std::string("value4")) { --rtn; }

    return rtn;
}
