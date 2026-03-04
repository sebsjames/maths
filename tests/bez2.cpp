#include <memory>
#include <iostream>
#include <fstream>

#include <sm/hexgrid>
#include <sm/bezcurve>
#include <sm/bezcurvepath>
#include <sm/vec>

int main()
{
    int rtn = -1;

    sm::vec<float, 2> v1 = {-0.28f, 0.0f};
    sm::vec<float, 2> v2 = {0.28f, 0.0f};
    sm::vec<float, 2> v3 = {0.28f, 0.45f};
    sm::vec<float, 2> v4 = {-0.28f, 0.45f};

    sm::bezcurve<float> c1(v1,v2);
    sm::bezcurve<float> c2(v2,v3);
    sm::bezcurve<float> c3(v3,v4);
    sm::bezcurve<float> c4(v4,v1);
    std::cout << "instanciated curves" << std::endl;
    sm::bezcurvepath<float> bound;
    std::cout << "instanciated curvepath" << std::endl;

    bound.addCurve(c1);
    bound.addCurve(c2);
    bound.addCurve(c3);
    bound.addCurve(c4);

    auto Hgrid = std::make_unique<sm::hexgrid>(0.02f, 4.0f, 0.0f);
    std::cout << "setBoundary..." << std::endl;
    Hgrid->setBoundary (bound);
    std::cout << "Number of hexes is: " << Hgrid->num() << std::endl;

    if (Hgrid->num() == 783) {
        // Success
        rtn = 0;
    }

    return rtn;
}
