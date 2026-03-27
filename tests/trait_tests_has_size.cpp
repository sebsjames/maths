#include <iostream>
#include <complex>
#include <deque>
#include <vector>

import sm.trait_tests;
import sm.vec;

int main()
{
    int rtn = 0;

    if constexpr (sm::has_size_const_method<sm::vec<float>>::value == true) {
        std::cout << "vec<float> has size()\n";
    } else {
        std::cout << "vec<float> has NO size()\n";
        --rtn;
    }

    if constexpr (sm::has_size_const_method<std::deque<int>>::value == true) {
        std::cout << "deque<int> has size()\n";
    } else {
        std::cout << "deque<int> has NO size()\n";
        --rtn;
    }

    if constexpr (sm::has_size_const_method<std::string>::value == true) {
        std::cout << "std::string has size()\n";
    } else {
        std::cout << "std::string has NO size()\n";
        --rtn;
    }

    if constexpr (sm::has_size_const_method<std::remove_reference_t<std::complex<float>>>::value == true) {
        std::cout << "std::complex<float> has size()\n";
        --rtn;
    } else {
        std::cout << "std::complex<float> has NO size()\n";
    }

    return rtn;
}
