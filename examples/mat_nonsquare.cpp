/*
 * Some examples of sm::mat used for non-square matrix arithmetic
 */
#include <iostream>

import sm.mat;

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

    sm::mat<float, 4, 2> fourbytwo;
    fourbytwo = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::cout << "fourbytwo:\n" << fourbytwo << std::endl;

    std::cout << "fourbytwo element at row 0, col 1 is fourbytwo(0,1) = " << fourbytwo(0,1) << std::endl;
    std::cout << "fourbytwo element at row 3, col 0 is fourbytwo(3,0) = " << fourbytwo(3,0) << std::endl;

    fourbytwo(3,0) = 30.0f;
    std::cout << "fourbytwo element at row 3, col 0 is now = " << fourbytwo(3,0) << std::endl;

    std::cout << "fourbytwo row 0: " << fourbytwo.row(0) << std::endl;
    std::cout << "fourbytwo row 1: " << fourbytwo.row(1) << std::endl;
    std::cout << "fourbytwo row 2: " << fourbytwo.row(2) << std::endl;
    std::cout << "fourbytwo row 3: " << fourbytwo.row(3) << std::endl;

    std::cout << "fourbytwo col 0: " << fourbytwo.col(0) << std::endl;
    std::cout << "fourbytwo col 1: " << fourbytwo.col(1) << std::endl;

    fourbytwo.set_col(0, sm::vec<float, 4>{7,8,9,10});
    std::cout << "after, fourbytwo col 0: " << fourbytwo.col(0) << std::endl;


    sm::mat<float, 4, 4> fourbyfour;
    fourbyfour = { 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8  };
    std::cout << "\nfourbyfour:\n" << fourbyfour << std::endl;

    auto m2 = (fourbyfour * fourbytwo);
    std::cout << "fourbyfour * fourbytwo:\n" << m2 << std::endl;

    std::cout << "flipud: " << m2.flipud() << std::endl;
    std::cout << "fliplr: " << m2.fliplr() << std::endl;

    // Won't compile:
    //std::cout << "twobyfour + fourbysix =\n" << twobyfour + fourbysix << std::endl;
    //std::cout << "fourbysix * twobyfour =\n" << fourbysix * twobyfour << std::endl;
}
