#include <memory>
#include <iostream>
#include <fstream>

#include <morph/hexgrid.h>
#include <morph/bezcurve.h>
#include <morph/bezcurvepath.h>
#include <morph/vec.h>

int main()
{
    int rtn = -1;

    morph::vec<float, 2> v1 = {-0.28f, 0.0f};
    morph::vec<float, 2> v2 = {0.28f, 0.0f};
    morph::vec<float, 2> v3 = {0.28f, 0.45f};
    morph::vec<float, 2> v4 = {-0.28f, 0.45f};

    morph::bezcurve<float> c1(v1,v2);
    morph::bezcurve<float> c2(v2,v3);
    morph::bezcurve<float> c3(v3,v4);
    morph::bezcurve<float> c4(v4,v1);
    std::cout << "instanciated curves" << std::endl;
    morph::bezcurvepath<float> bound;
    std::cout << "instanciated curvepath" << std::endl;

    bound.addCurve(c1);
    bound.addCurve(c2);
    bound.addCurve(c3);
    bound.addCurve(c4);

    auto Hgrid = std::make_unique<morph::hexgrid>(0.02f, 4.0f, 0.0f);
    std::cout << "setBoundary..." << std::endl;
    Hgrid->setBoundary (bound);
    std::cout << "Number of hexes is: " << Hgrid->num() << std::endl;

    if (Hgrid->num() == 783) {
        // Success
        rtn = 0;
    }

    return rtn;
}
