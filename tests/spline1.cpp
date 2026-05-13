#include <iostream>

import sm.spline;

int main()
{
    int rtn = 0;

    sm::spline<float, 4> spl;

    spl.p[0] = {1, 2};
    spl.p[1] = {3, 3};
    spl.p[2] = {5, 9};
    spl.p[3] = {8, 10};

    spl.compute();

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
