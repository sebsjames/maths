#include <utility>
#include <iostream>
#include <fstream>
#include <vector>

import sm.bezcurve;
import sm.vec;
import sm.vvec;

int main()
{
    int rtn = -1;

    // Make some control points
    sm::vec<float, 2> i, f, c1, c2;
    i = {1,1};
    c1 = {5,5};
    c2 = {2,-4};
    f = {10,1};
    // Make a curve
    sm::bezcurve<float> cc3(i, f, c1, c2);

    // Make a second quartic curve.
    sm::vvec<sm::vec<float, 2>> quart = {f, {10.0f,10.0f}, {10.0f,0.0f},  {12.0f,-5.0f},  {14.0f,0.0f}};
    sm::bezcurve<float> cc4(quart);

    return rtn;
}
