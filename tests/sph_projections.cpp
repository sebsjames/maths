// Test spherical projections and their inverses.
//
// This just tests a few cases of latlong -> xy and back.
//
// A good unit test would test that the forward transform generated expected numbers, too.

#include <iostream>
#include <sm/vec>
#include <sm/geometry>

int main()
{
    int rtn = 0;

    typedef float F;

    F r = 1.0f;
    F l0 = 0.0f;

    std::cout << "epsilon (F) = " << std::numeric_limits<F>::epsilon() << std::endl;

    F thr = std::numeric_limits<F>::epsilon();

    sm::vec<F, 2> latlong = {};
    sm::vec<F, 2> mxy = sm::geometry::spherical_projection::mercator (latlong, r, l0);
    sm::vec<F, 2> mll_back = sm::geometry::spherical_projection::inverse_mercator (mxy, r, l0);
    std::cout << "with lambda0 = " << l0 << ", latlong " << latlong << " mercators to " << mxy
              << " which inverses back to " << mll_back << " diffs: " << (latlong - mll_back).abs()  << std::endl;
    if ((latlong - mll_back).abs() > thr) { --rtn; }

    latlong = {0, sm::mathconst<F>::pi_over_2};
    mxy = sm::geometry::spherical_projection::mercator (latlong, r, l0);
    mll_back = sm::geometry::spherical_projection::inverse_mercator (mxy, r, l0);
    std::cout << "with lambda0 = " << l0 << ", latlong " << latlong << " mercators to " << mxy
              << " which inverses back to " << mll_back  << " diffs: " << (latlong - mll_back).abs() << std::endl;
    if ((latlong - mll_back).abs() > thr) { --rtn; }


    l0 = sm::mathconst<F>::pi_over_4;
    mxy = sm::geometry::spherical_projection::mercator (latlong, r, l0);
    mll_back = sm::geometry::spherical_projection::inverse_mercator (mxy, r, l0);
    std::cout << "with lambda0 = " << l0 << ", latlong " << latlong << " mercators to " << mxy
              << " which inverses back to " << mll_back << " diffs: " << (latlong - mll_back).abs() << std::endl;
    if ((latlong - mll_back).abs() > thr) { --rtn; }


    l0 = sm::mathconst<F>::pi_over_4;
    mxy = sm::geometry::spherical_projection::cassini (latlong, r, l0);
    mll_back = sm::geometry::spherical_projection::inverse_cassini (mxy, r, l0);
    std::cout << "with lambda0 = " << l0 << ", latlong " << latlong << " cassinis to " << mxy
              << " which inverses back to " << mll_back << " diffs: " << (latlong - mll_back).abs() << std::endl;
    if ((latlong - mll_back).abs() > thr) { --rtn; }

    l0 = sm::mathconst<F>::pi_over_4;
    mxy = sm::geometry::spherical_projection::equirectangular (latlong, r, l0);
    mll_back = sm::geometry::spherical_projection::inverse_equirectangular (mxy, r, l0);
    std::cout << "with lambda0 = " << l0 << ", latlong " << latlong << " equirectangulars to " << mxy
              << " which inverses back to " << mll_back << " diffs: " << (latlong - mll_back).abs() << std::endl;
    if ((latlong - mll_back).abs() > thr) { --rtn; }

    return rtn;
}
