#include <utility>
#include <iostream>
#include <fstream>
#include <vector>

import sm.bezcurvepath;
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
    // Make a cubic curve
    sm::bezcurve<float, 3> cc3(i, f, c1, c2);

    // Make a second quartic curve.
    sm::vvec<sm::vec<float, 2>> quart = {f, {10.0f,10.0f}, {10.0f,0.0f},  {12.0f,-5.0f},  {14.0f,0.0f}};
    sm::bezcurve<float, 4> cc4(quart);

    // Put em in a bezcurvepath
    sm::bezcurvepath<float> bcp;
    bcp.name = "testbezcurves";
    bcp.add_curve (cc3);
    //bcp.add_curve (cc4); // Can no longer mix as we're using fixed sized sm::mat

    unsigned int n_points = 201;
    bcp.compute_points (n_points);
    std::vector<sm::bezcoord<float>> points = bcp.get_points();
    std::vector<sm::bezcoord<float>> tans = bcp.get_tangents();

    for (auto p : points) {
        std::cout << p.x() << "," << p.y() << std::endl;
    }
    std::cout << "Tangents" << std::endl;
    for (auto ta : tans) {
        std::cout << ta.x() << "," << ta.y() << std::endl;
    }

    if (points.size() == n_points) { rtn = 0; }

    return rtn;
}
