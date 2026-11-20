#include <iostream>

import sm.mat;
import sm.vmat;

int main()
{
    int rtn = 0;

    sm::vmat<float> vm1(4, 4);
    vm1.arr = { 0, 1, 0, 0,  1, 0, 0, 0,  0, 0, -1, 0,  0, 0, 0, 1 };

    std::cout << "vm1:\n" << vm1 << std::endl;

    std::cout << "vm1 * vm1 = \n" << vm1 * vm1 << std::endl;

    sm::mat<float, 4> m1 = { 0, 1, 0, 0,  1, 0, 0, 0,  0, 0, -1, 0,  0, 0, 0, 1 };

    std::cout << "m1:\n" << m1 << std::endl;

    std::cout << "m1 * m1 = \n" << m1 * m1 << std::endl;
    
    return rtn;
}
