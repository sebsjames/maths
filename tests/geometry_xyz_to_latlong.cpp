#include <iostream>

import sm.vec;
import sm.geometry;

bool not_about_equal (const float a, const float b, const float eps)
{
    if (std::abs (a - b) > eps) { return true; }
    return false;
}

int main()
{
    constexpr float eps = std::numeric_limits<float>::epsilon();
    int rtn = 0;

    constexpr float r = 1.0f;
    sm::vec<float, 3> xyz = { 0, 0, 1 };
    xyz.renormalize();
    xyz *= r;
    sm::vec<float, 2> ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;

    xyz = { 0, 0, -1 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;

    // x axis is at front of earth
    xyz = { 1, 0, 0 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;
    if (ll[1] != 0) { std::cout << "Failed on +x\n"; --rtn; }

    xyz = { 0, 1, 0 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;
    if (ll[1] != sm::mathconst<float>::pi_over_2) { std::cout << "Failed on +y axis\n"; --rtn; }

    xyz = { -1, 0, 0 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;
    if (not_about_equal (ll[1], sm::mathconst<float>::pi, eps) == true) { std::cout << "Failed on -x axis\n"; --rtn; }

    xyz = { 0, -1, 0 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;
    if (not_about_equal (ll[1], -sm::mathconst<float>::pi_over_2, eps) == true) { std::cout << "Failed on -y axis\n"; --rtn; }

    return rtn;
}
