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

    spl.compute_coefficients();

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");

    sm::vvec<float> x;
    x.linspace (1, 8, 40);
    sm::vvec<float> y = spl.compute (x);

    std::cout << std::endl << "..." << std::endl;

    if (spl.compute (1) - 2.0f > 0.00001f) { --rtn; }
    if (spl.compute (3) - 3.0f > 0.00001f) { --rtn; }
    if (spl.compute (5) - 9.0f > 0.00001f) { --rtn; }
    if (spl.compute (8) - 10.0f > 0.00001f) { --rtn; }

    // Can plot x/y. Here's a script in octave format
    std::cout << "x = " << x.str_mat() << ";\n"
              << "y = " << y.str_mat() << ";\n"
              << "plot (x, y);\n";

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
