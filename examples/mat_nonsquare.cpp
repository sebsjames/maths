/*
 * Some examples of sm::mat used for non-square matrix arithmetic
 */
#include <iostream>
#include <sm/mat>

int main()
{
    sm::mat<float, 2, 4> twobyfour;
    std::cout << "twobyfour initialized:\n" << twobyfour << std::endl;

    twobyfour = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::cout << "twobyfour:\n" << twobyfour << std::endl;

    sm::mat<float, 4, 6> fourbysix;
    std::cout << "fourbysix initialized:\n" << fourbysix << std::endl;
    fourbysix = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };

    std::cout << "twobyfour * fourbysix =\n" << twobyfour * fourbysix << std::endl;

    // Won't compile:
    //std::cout << "twobyfour + fourbysix =\n" << twobyfour + fourbysix << std::endl;
    //std::cout << "fourbysix * twobyfour =\n" << fourbysix * twobyfour << std::endl;
}
