#include <iostream>
#include <complex>
#include <deque>
#include <sm/vec>
#include <sm/trait_tests>


// What does sm::number_type return for std::complex? This is a test just of sm::number_type
// from trait_tests.h (it used to be in its own header)
int main()
{
    int rtn = 0;

    // Scalars
    if constexpr (sm::number_type<float>::value == 1) {
        std::cout << "float is scalar\n";
    } else { --rtn; }
    if constexpr (sm::number_type<double>::value == 1) {
        std::cout << "double is scalar\n";
    } else { --rtn; }
    if constexpr (sm::number_type<int>::value == 1) {
        std::cout << "int is scalar\n";
    } else { --rtn; }
    if constexpr (sm::number_type<unsigned int>::value == 1) {
        std::cout << "unsigned int is scalar\n";
    } else { --rtn; }

    // Mathematical vectors
    if constexpr (sm::number_type<sm::vec<float, 3>>::value == 0) {
        std::cout << "vec<float> is a mathematical vector\n";
    } else { --rtn; }
    if constexpr (sm::number_type<std::vector<int>>::value == 0) {
        std::cout << "std::vector<int> is a mathematical vector\n";
    } else { --rtn; }

    if constexpr (sm::number_type<std::deque<double>>::value == 0) {
        std::cout << "deque<double> is a mathematical vector\n";
    } else { --rtn; }

    // Complex numbers
    if constexpr (sm::number_type<std::complex<float>>::value == 2) {
        std::cout << "std::complex<float> is a complex scalar\n";
    } else { --rtn; }
    if constexpr (sm::number_type<std::complex<double>>::value == 2) {
        std::cout << "std::complex<double> is a complex scalar\n";
    } else { --rtn; }
    if constexpr (sm::number_type<std::complex<int>>::value == 2) {
        std::cout << "std::complex<int> is a complex scalar\n";
    } else { --rtn; }
    // A complex number of vectors is NOT acceptable/valid
    if constexpr (sm::number_type<std::complex<std::array<float, 3>>>::value == 2) {
        std::cout << "std::complex<std::array<float, 3>> should NOT be a complex scalar\n";
        --rtn;
    } else {
        // Correctly get -1
        std::cout << "sm::number_type<std::complex<std::array<float, 3>>>::value = "
                  << sm::number_type<std::complex<std::array<float, 3>>>::value << std::endl;
        if constexpr (sm::number_type<std::complex<std::array<float, 3>>>::value != -1) {
            --rtn;
        }
    }

    // Vectors of complex numbers
    if constexpr (sm::number_type<sm::vec<std::complex<float>, 2>>::value == 3) {
        std::cout << "sm::vec<std::complex<float>> is a vector of complex numbers\n";
    } else { --rtn; }
    if constexpr (sm::number_type<sm::vec<std::complex<double>, 2>>::value == 3) {
        std::cout << "sm::vec<std::complex<double>> is a vector of complex numbers\n";
    } else { --rtn; }
    if constexpr (sm::number_type<sm::vec<std::complex<int>, 2>>::value == 3) {
        std::cout << "sm::vec<std::complex<int>> is a vector of complex numbers\n";
    } else { --rtn; }

    // An std::pair is not something we handle as a number type (even though it could be a 2D vector
    // or be used to store a complex number).
    if constexpr (sm::number_type<std::pair<float, float>>::value == -1) {
        std::cout << "We ignore std::pair as a number type\n";
    } else { --rtn; }


    std::cout << "\nsm::number_type test " << (rtn == 0 ? "passed" : "failed") << std::endl;
    return rtn;
}
