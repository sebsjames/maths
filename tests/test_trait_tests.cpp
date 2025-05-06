#include <iostream>
#include <sj/trait_tests>

template <typename _S=float>
std::enable_if_t < sj::is_copyable_container<_S>::value, bool >
set_from (const _S& v)
{
        std::cout << "Type _S=" << typeid(_S).name() << " size " << sizeof (v) << " is a simple, copyable container" << std::endl;
        return true;
}
template <typename _S=float>
std::enable_if_t < !sj::is_copyable_container<_S>::value, bool >
set_from (const _S& v)
{
    std::cout << "Type _S=" << typeid(_S).name() << " size " << sizeof (v) << " isn't a container" << std::endl;
    return false;
}

template <typename _S=float>
std::enable_if_t < sj::is_copyable_fixedsize<_S>::value, bool >
set_from_fixed (const _S& v)
{
    std::cout << "Type _S=" << typeid(_S).name() << " size " << sizeof (v) << " is a fixed size, simple, copyable container" << std::endl;
    return true;
}
template <typename _S=float>
std::enable_if_t < !sj::is_copyable_fixedsize<_S>::value, bool >
set_from_fixed (const _S& v)
{
    std::cout << "Type _S=" << typeid(_S).name() << " size " << sizeof (v) << " is NOT a fixed size, simple, copyable container" << std::endl;
    return false;
}

template <typename _S=float>
std::enable_if_t < !sj::is_complex<_S>::value, bool >
complex_from (const _S& v)
{
    std::cout << "Type _S=" << typeid(_S).name() << " size " << sizeof (v) << " isn't a complex" << std::endl;
    return false;
}
template <typename _S=float>
std::enable_if_t < sj::is_complex<_S>::value, bool >
complex_from (const _S& v)
{
    std::cout << "Type _S=" << typeid(_S).name() << " size " << sizeof (v) << " is a complex" << std::endl;
    return true;
}

#include <sj/vec>
#include <sj/vvec>
#include <set>
#include <array>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <complex>

int main()
{
    int rtn = 0;

    /*
     * has_subtraction
     */
    std::cout << "float has subtraction? " << (sj::has_subtraction<float>::value ? "true" : "false") << std::endl;
    if (!sj::has_subtraction<float>::value
        || !sj::has_subtraction<double>::value
        || !sj::has_subtraction<int>::value
        || !sj::has_subtraction<unsigned int>::value) { --rtn; }
    std::cout << "vector has subtraction? " << (sj::has_subtraction<std::vector<float>>::value ? "true" : "false") << std::endl;
    if (sj::has_subtraction<std::vector<float>>::value) { --rtn; }
    std::cout << "vvec has subtraction? " << (sj::has_subtraction<sj::vvec<float>>::value ? "true" : "false") << std::endl;
    std::cout << "vec has subtraction? " << (sj::has_subtraction<sj::vec<float, 4>>::value ? "true" : "false") << std::endl;
    if (!sj::has_subtraction<sj::vvec<float>>::value) { --rtn; }
    if (!sj::has_subtraction<sj::vec<float, 17>>::value) { --rtn; }

    /*
     * has_resize
     */
    std::cout << "vvec has resize: " << (sj::has_resize_method<sj::vvec<float>>::value ? "true" : "false") << std::endl;
    if (sj::has_resize_method<sj::vvec<float>>::value == false) { --rtn; }
    std::cout << "float has resize: " << (sj::has_resize_method<float>::value ? "true" : "false") << std::endl;
    if (sj::has_resize_method<float>::value == true) { --rtn; }
    std::cout << "array has resize: " << (sj::has_resize_method<std::array<float, 3>>::value ? "true" : "false") << std::endl;
    if (sj::has_resize_method<sj::vec<float, 5>>::value == true) { --rtn; }

    float f = 0.0f;
    bool float_can = set_from (f);
    std::array<double, 10> c = {0.0};
    bool array_can = set_from (c);

    std::vector<double> c2 = {0.0};
    bool vector_can = set_from (c2);

    // I want false returned for std::map as this can't be set_from. So it's not JUST that map has
    // to have a LegacyInputIterator, because you can't std::copy(map::iterator, map::iterator,
    // vector::iterator). So leaving this FAILING for now.
    //std::map<int, double> c3;
    //bool map_can = set_from (c3);

    std::set<double> c4 = {};
    bool set_can = set_from (c4);

    std::complex<float> c5 = {};
    bool complex_can = complex_from (c5);

    float c6 = 0.0f;
    bool float_is_complex = complex_from (c6);

    if (float_can || !array_can || !vector_can || !set_can || !complex_can || float_is_complex) {
        std::cout << "Test failed\n";
        --rtn;
    }

    bool c_is_fixed = set_from_fixed (c);
    if (!c_is_fixed) { std::cout << "Fail on array<double>\n"; --rtn; }

    bool c2_is_fixed = set_from_fixed (c2);
    if (c2_is_fixed) { std::cout << "Fail on vector<double>\n"; --rtn; }


    std::cout << "array is fixed size? " << (sj::is_copyable_fixedsize<std::array<float, 2>>::value ? "true" : "false") << std::endl;
    if (sj::is_copyable_fixedsize<std::array<float, 2>>::value == false) { std::cout << "Fail on array<float>\n"; --rtn; }
    if constexpr (sj::is_copyable_fixedsize<std::array<float, 2>>::value == false) { std::cout << "Fail on constexpr array<float>\n"; --rtn; }
    // if constexpr (sj::is_copyable_fixedsize<std::array<float, 2>&>::value == false) { --rtn; } // fails to compile

    std::cout << "ZERO sized array is fixed size? " << (sj::is_copyable_fixedsize<std::array<int, 0>>::value ? "true" : "false") << std::endl;

      if (sj::is_copyable_fixedsize<std::array<int, 0>>::value == false) { std::cout << "Fail on array<int, 0>\n"; --rtn; }

    std::cout << "sj::vec is fixed size? " << (sj::is_copyable_fixedsize<sj::vec<double, 56>>::value ? "true" : "false") << std::endl;
    if (sj::is_copyable_fixedsize<sj::vec<double, 56>>::value == false) { std::cout << "Fail on sj::vec<double>\n"; --rtn; }
    // if (sj::is_copyable_fixedsize<sj::vec<double, 56>>::value == false) { --rtn; } // fails to compile
    if constexpr (sj::is_copyable_fixedsize<sj::vec<double, 56>>::value == false) { --rtn; }

    std::cout << "vector is fixed size? " << (sj::is_copyable_fixedsize<std::vector<double>&>::value ? "true" : "false") << std::endl;
    if (sj::is_copyable_fixedsize<std::vector<double>>::value == true) { --rtn; }
    if (sj::is_copyable_fixedsize<std::vector<double>&>::value == true) { --rtn; }
    if constexpr (sj::is_copyable_fixedsize<std::vector<double>>::value == true) { --rtn; }
    if constexpr (sj::is_copyable_fixedsize<std::vector<double>&>::value == true) { --rtn; }

    std::cout << "sj::vvec is fixed size? " << (sj::is_copyable_fixedsize<sj::vvec<unsigned char>>::value ? "true" : "false") << std::endl;
    if (sj::is_copyable_fixedsize<sj::vvec<unsigned char>>::value == true) { --rtn; }
    if (sj::is_copyable_fixedsize<sj::vvec<unsigned char>&>::value == true) { --rtn; }
    if constexpr (sj::is_copyable_fixedsize<sj::vvec<unsigned char>>::value == true) { --rtn; }
    if constexpr (sj::is_copyable_fixedsize<sj::vvec<unsigned char>&>::value == true) { --rtn; }

    std::cout << "list is fixed size? " << (sj::is_copyable_fixedsize<std::list<double>>::value ? "true" : "false") << std::endl;
    if (sj::is_copyable_fixedsize<std::list<double>>::value == true) { --rtn; }
    if constexpr (sj::is_copyable_fixedsize<std::list<double>>::value == true) { --rtn; }
    if constexpr (sj::is_copyable_fixedsize<std::list<double>&>::value == true) { --rtn; }

    std::cout << "deque is fixed size? " << (sj::is_copyable_fixedsize<std::deque<double>>::value ? "true" : "false") << std::endl;
    if (sj::is_copyable_fixedsize<std::deque<double>>::value == true) { --rtn; }
    if constexpr (sj::is_copyable_fixedsize<std::deque<double>>::value == true) { --rtn; }
    if constexpr (sj::is_copyable_fixedsize<std::deque<double>&>::value == true) { --rtn; }

    std::cout << "double is fixed size container with const size method? " << (sj::is_copyable_fixedsize<double>::value ? "true" : "false") << std::endl;
    if constexpr (sj::is_copyable_fixedsize<double>::value == true) { --rtn; }

    std::cout << "int is fixed size container with const size method? " << (sj::is_copyable_fixedsize<int>::value ? "true" : "false") << std::endl;
    if constexpr (sj::is_copyable_fixedsize<int>::value == true) { --rtn; }

    std::cout << "Test " << (rtn == 0 ? " PASSED" : " FAILED") << std::endl;
    return rtn;
}
