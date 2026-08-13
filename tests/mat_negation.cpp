/*
 * Test the unary negate operator
 */

#include <iostream>
import sm.mat;

int main()
{
    int rtn = 0;

    sm::mat<float, 2, 4> twobyfour;
    twobyfour = { 1, -2, 3, -4, 5, -6, 7, -8 };

    std::cout << "A matrix:\n" << twobyfour << std::endl;
    std::cout << "Its negative:\n" << -twobyfour << std::endl;

    sm::mat<float, 4, 6> fourbysix;
    fourbysix = { -1, 2, -3, 4, -5, 6, -7, 8, -9, 10, -11, 12, -13, 14, -15, 16, -17, 18, -19, 20, -21, 22, -23, 24 };

    std::cout << "Another matrix:\n" << fourbysix << std::endl;
    std::cout << "Its negative:\n" << -fourbysix << std::endl;

    sm::mat<float, 3> threesq = { -1, 2, 3, -4, 5, 6, -7, 8, 9 };

    std::cout << "Another matrix:\n" << threesq << std::endl;
    std::cout << "Its negative:\n" << -threesq << std::endl;

    if (-twobyfour  != twobyfour * -1.0f) { --rtn; }
    if (-fourbysix  != fourbysix * -1.0f) { --rtn; }
    if (-threesq  != threesq * -1.0f) { --rtn; }

    std::cout << (rtn == 0 ? "SUCCESS\n" : "FAILURE\n");
    return rtn;
}
