//
// Solve equation systems using the matrix method: If AX = B then X = A^-1B
//

#include <iostream>
#include <cmath>

import sm.mat;

int main()
{
    int rtn = 0;

    {
        // Check https://www.mathsisfun.com/algebra/systems-linear-equations-matrices.html example:
        sm::mat<float, 3> A = { 1, 0, 2,  1, 2, 5,  1, 5, -1 };
        sm::vec<float, 3> B = { 6, -4, 27 };
        sm::vec<float, 3> X = A.inverse() * B;
        std::cout << "mathisfun? " << X << std::endl; // 5, 3, -2

        sm::vec<float, 3> X_expected = { 5.0f, 3.0f, -2.0f };
        sm::vec<float, 3> diffs = X - X_expected;
        if (diffs.abs().sum() > 10.0f * std::numeric_limits<float>::epsilon()) {
            std::cout << "diffs.abs() are " << diffs.abs() << " and abs().sum(): " << diffs.abs().sum() << std::endl;
            --rtn;
        }
    }

    {
        // Compute parameters a3, a4 and a5 for the min-jerk trajectory to travel a distance xf
        // (starting at x=0) in time tf:
        //
        // x(t) = a3 t^3 + a4 t^4 + a5 t^5
        const float tf = 0.5f;
        const float xf = 10.0f;
        sm::mat<float, 3> A = {
            std::pow (tf, 3.0f), 3.0f  * std::pow (tf, 2.0f), 6.0f * tf,
            std::pow (tf, 4.0f), 4.0f  * std::pow (tf, 3.0f), 12.0f * std::pow (tf, 2.0f),
            std::pow (tf, 5.0f), 5.0f * std::pow (tf, 4.0f), 20.0f * std::pow (tf, 3.0f)
        };
        sm::vec<float, 3> B = { xf, 0.0f, 0.0f };
        sm::vec<float, 3> X = A.inverse() * B;
        std::cout << "Min jerk values " << X << std::endl;

        sm::vec<float, 3> X_expected = { 800.0f, -2400.0f, 1920.0f };
        sm::vec<float, 3> diffs = X - X_expected;
        if (diffs.abs().sum() > 10.0f * std::numeric_limits<float>::epsilon()) { --rtn; }
    }

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");

    return rtn;
}
