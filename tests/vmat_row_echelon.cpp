#include <iostream>

import sm.vmat;
import sm.mat;

int main()
{
    int rtn = 0;

    sm::vmat<float> vm1(4, 4);
    vm1.arr = { 0, 10, 0, 0,  4, 0, 2, 0,  0, 0, -8, 0,  0, 0, 0, 9 };
    std::cout << "vm1:\n" << vm1 << std::endl;
    std::cout << "vm1 row echelon:\n" << vm1.row_echelon_form() << std::endl;
    //std::cout << "vm1 reduced row echelon:\n" << vm1.reduced_row_echelon_form() << std::endl;

    sm::mat<float, 4, 4> m1;
    m1.arr = { 0, 10, 0, 0,  4, 0, 2, 0,  0, 0, -8, 0,  0, 0, 0, 9 };
    std::cout << "m1:\n" << m1 << std::endl;
    std::cout << "m1 row echelon:\n" << m1.row_echelon_form() << std::endl;
    //std::cout << "m1 reduced row echelon:\n" << m1.reduced_row_echelon_form() << std::endl;

    return rtn;
}
