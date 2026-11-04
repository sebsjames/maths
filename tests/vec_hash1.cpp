#include <cstdlib>
#include <iostream>

import sm.vec;

int main()
{
    int rtn = 0;

    sm::vec<int, 2>::hash hsh;
    auto k = sm::vec<int, 2>{0, 0};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {0, 1};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {0, 2};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {0, 3};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {0, 4};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {0, 5};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {1, 1};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {1, 2};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {1, 3};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {1, 4};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {1, 5};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {28942374, 289048597};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    k = {289048597, 28942374};
    std::cout << "vec hash of " << k << " = " << hsh(k) << std::endl;

    if (hsh(sm::vec<int, 2>{0, 0}) == hsh(sm::vec<int, 2>{1, 1})) { --rtn; }

    return rtn;
}
