#include <iostream>
#include <vector>
#include <sm/winder>
#include <sm/vec>

/*
 * More tests for winder
 */
int main()
{
    int rtn = 0;

    // Make another boundary
    std::vector<sm::vec<float, 2>> bnd;
    bnd.push_back ({1.59597492,0.242018759});
    bnd.push_back ({1.3965255,0.986374199});
    bnd.push_back ({0.851619482,1.53128028});
    bnd.push_back ({0.10726402,1.73072958});
    bnd.push_back ({-0.637091398,1.53128028});
    bnd.push_back ({-1.18199754,0.986373961});
    bnd.push_back ({-1.38144684,0.242018625});
    bnd.push_back ({-1.18199718,-0.502336919});
    bnd.push_back ({-0.637091219,-1.04724264});
    bnd.push_back ({0.107264102,-1.24669218});
    bnd.push_back ({0.851620078,-1.0472424});
    bnd.push_back ({1.39652574,-0.502336323});

    sm::winder w (bnd);
    sm::vec<float, 2> px = { 11,0 }; // outside, but fails!
    std::cout << "pt = " << px.str_mat() << std::endl;
    int wn = w.wind (px);
    std::cout << "(outside) Winding number for " << px << " = " << wn << std::endl;
    if (wn != 0) { --rtn; }

    // Check many outside locations
    for (int i = 0; i < 360; ++i) {
        px = { 11.0f * std::cos (i * sm::mathconst<float>::deg2rad), 11.0f * std::sin (i * sm::mathconst<float>::deg2rad) };
        wn = w.wind (px);
        std::cout << "(outside) Winding number for " << px << " = " << wn << std::endl;
        if (wn != 0) { --rtn; }
    }

    // inside
    px = { 0, 0 };
    wn = w.wind (px);
    std::cout << "(inside) Winding number for " << px << " = " << wn << std::endl;
    if (wn != 1) { --rtn; }

    // Check many inside locations
    for (int i = 0; i < 360; ++i) {
        px = { 0.2f * std::cos (i * sm::mathconst<float>::deg2rad), 0.2f * std::sin (i * sm::mathconst<float>::deg2rad) };
        wn = w.wind (px);
        std::cout << "(inside) Winding number for " << px << " = " << wn << std::endl;
        if (wn != 1) { --rtn; }
    }

    return rtn;
}
