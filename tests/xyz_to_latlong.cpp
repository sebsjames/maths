#include <iostream>

#include <sm/vec>
#include <sm/geometry>

int main()
{
    int rtn = 0;

    constexpr float r = 1.0f;
    sm::vec<float, 3> xyz = { 0, 0, 1 };
    xyz.renormalize();
    xyz *= r;
    sm::vec<float, 2> ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz, r);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;

    xyz = { 0, 0, -1 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz, r);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;

    // x axis is at front of earth
    xyz = { 1, 0, 0 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz, r);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;
    if (ll[1] != 0) { std::cout << "Failed on +x\n"; --rtn; }

    xyz = { 0, 1, 0 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz, r);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;
    if (ll[1] != sm::mathconst<float>::pi_over_2) { std::cout << "Failed on +y axis\n"; --rtn; }

    xyz = { -1, 0, 0 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz, r);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;
    if (ll[1] != sm::mathconst<float>::pi) { std::cout << "Failed on -x axis\n"; --rtn; }

    xyz = { 0, -1, 0 };
    xyz.renormalize();
    xyz *= r;
    ll = sm::geometry::spherical_projection::xyz_to_latlong (xyz, r);
    std::cout << xyz << " has Latitude "<< ll[0] << " and longitude " << ll[1] << std::endl;
    if (ll[1] != -sm::mathconst<float>::pi_over_2) { std::cout << "Failed on -y axis\n"; --rtn; }

    return rtn;
}
