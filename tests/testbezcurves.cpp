#include <utility>
#include <iostream>
#include <fstream>
#include <vector>

#include <sj/bezcurve>
#include <sj/bezcurvepath>
#include <sj/vec>
#include <sj/vvec>

int main()
{
    int rtn = -1;

    // Make some control points
    sj::vec<float, 2> i, f, c1, c2;
    i = {1,1};
    c1 = {5,5};
    c2 = {2,-4};
    f = {10,1};
    // Make a cubic curve
    sj::bezcurve<float> cc3(i, f, c1, c2);

    // Make a second quartic curve.
    sj::vvec<sj::vec<float, 2>> quart = {f, {10.0f,10.0f}, {10.0f,0.0f},  {12.0f,-5.0f},  {14.0f,0.0f}};
    sj::bezcurve<float> cc4(quart);

    // Put em in a bezcurvepath
    sj::bezcurvepath<float> bcp;
    bcp.name = "testbezcurves";
    bcp.addCurve (cc3);
    bcp.addCurve (cc4);

    unsigned int nPoints = 201;
    bcp.computePoints (nPoints);
    std::vector<sj::bezcoord<float>> points = bcp.getPoints();
    std::vector<sj::bezcoord<float>> tans = bcp.getTangents();

    for (auto p : points) {
        std::cout << p.x() << "," << p.y() << std::endl;
    }
    std::cout << "Tangents" << std::endl;
    for (auto ta : tans) {
        std::cout << ta.x() << "," << ta.y() << std::endl;
    }

    if (points.size() == nPoints) { rtn = 0; }

    return rtn;
}
