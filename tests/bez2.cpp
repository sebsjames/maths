#include <memory>
#include <iostream>
#include <fstream>

import sm.bezcurvepath;
import sm.hexgrid;
import sm.vec;

int main()
{
    int rtn = -1;

    sm::vec<float, 2> v1 = {-0.28f, 0.0f};
    sm::vec<float, 2> v2 = {0.28f, 0.0f};
    sm::vec<float, 2> v3 = {0.28f, 0.45f};
    sm::vec<float, 2> v4 = {-0.28f, 0.45f};

    // hexgrid requires order 3 curves
    //                       (ip, fp, c1,               c2)
    sm::bezcurve<float, 3> c1(v1, v2, (v1 + v2) / 2.0f, (v1 + v2) / 2.0f);
    sm::bezcurve<float, 3> c2(v2, v3, (v2 + v3) / 2.0f, (v2 + v3) / 2.0f);
    sm::bezcurve<float, 3> c3(v3, v4, (v3 + v4) / 2.0f, (v3 + v4) / 2.0f);
    sm::bezcurve<float, 3> c4(v4, v1, (v4 + v1) / 2.0f, (v4 + v1) / 2.0f);
    std::cout << "instanciated curves" << std::endl;
    sm::bezcurvepath<float, 3> bound;
    std::cout << "instanciated curvepath" << std::endl;

    bound.add_curve(c1);
    bound.add_curve(c2);
    bound.add_curve(c3);
    bound.add_curve(c4);

    auto hgrid = std::make_unique<sm::hexgrid>(0.02f, 4.0f, 0.0f);
    std::cout << "setBoundary..." << std::endl;
    hgrid->setBoundary (bound);
    std::cout << "Number of hexes is: " << hgrid->num() << std::endl;

    if (hgrid->num() == 782) {
        // Success
        rtn = 0;
    }

    return rtn;
}
