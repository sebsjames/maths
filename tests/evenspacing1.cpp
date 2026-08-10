#include <iostream>

import sm.mathconst;
import sm.evenspacing;

// Define some 1D function
float f_lin (const float x) { return 1.0f * x; }

int main()
{
    int rtn = 0;

    const float x0 = 0.0f;
    const float x1 = 10.0f;

    auto ln = sm::evenspacing::estimate_length<float> (x0, x1, 2, f_lin);
    if (ln == sm::mathconst<float>::root_2 * 10.0f) {} else { --rtn; }

    std::cout << "2 The function f(x) between " << x0 << " and " << x1
              << " has length approx: " << ln << '\n';

    sm::vvec<sm::vec<float, 2>> es1 = sm::evenspacing::find_coordinates<float> (x0, x1, 1, f_lin);
    std::cout << "1 point: find_coordinates<float> (x0, x1, 1, f_lin) evenly spaced:\n" << es1 << std::endl;
    if ((es1[0] - sm::vec<float, 2>{0.0f, 0.0f}).abs().sum() > 0.01) { --rtn; }

    sm::vvec<sm::vec<float, 2>> es2 = sm::evenspacing::find_coordinates<float> (x0, x1, 2, f_lin);
    std::cout << "2 points: find_coordinates<float> (x0, x1, 2, f_lin) evenly spaced:\n" << es2 << std::endl;
    if ((es2[1] - sm::vec<float, 2>{10.0f, 10.0f}).abs().sum() > 0.01) { --rtn; }

    sm::vvec<sm::vec<float, 2>> es3 = sm::evenspacing::find_coordinates<float> (x0, x1, 3, f_lin);
    std::cout << "3 points: find_coordinates<float> (x0, x1, 3, f_lin) evenly spaced:\n" << es3 << std::endl;
    if ((es3[1] - sm::vec<float, 2>{5.0f, 5.0f}).abs().sum() > 0.01) { --rtn; }


    sm::vvec<sm::vec<float, 2>> es5 = sm::evenspacing::find_coordinates<float> (x0, x1, 5, f_lin);
    if ((es5[2] - sm::vec<float, 2>{5.0f, 5.0f}).abs().sum() > 0.01) { --rtn; }
    std::cout << "5 points: find_coordinates<float> (x0, x1, 5, f_lin) evenly spaced:\n" << es5 << std::endl;

    std::cout << "Test " << (rtn ? "FAILED" : "PASSED") << std::endl;
    return rtn;
}
