/*
 * Testing vector plane projection
 */

#include <iostream>
#include <sm/geometry>

int main()
{
    int rtn = 0;


    sm::vec<float> n = {0, 0, 1};
    sm::vec<float> v = {0, 1, 1};

    sm::vec<float> p = sm::geometry::vector_plane_projection (n, v);
    std::cout << v << " projected onto the plane " << n << " is " << p << std::endl;

    if (p != sm::vec<float>::uy()) { --rtn; }

    v = { 0, 1, 20 };
    p = sm::geometry::vector_plane_projection (n, v);
    std::cout << v << " projected onto the plane " << n << " is " << p << std::endl;

    if (p != sm::vec<float>::uy()) { --rtn; }

    v = { 1, 0, 20 };
    p = sm::geometry::vector_plane_projection (n, v);
    std::cout << v << " projected onto the plane " << n << " is " << p << std::endl;

    if (p != sm::vec<float>::ux()) { --rtn; }

    n = {0.3, 0.2, -0.5};
    p = sm::geometry::vector_plane_projection (n, v);
    std::cout << v << " projected onto the plane " << n << " is " << p << std::endl;

    std::cout << "Test " << (rtn ? "FAIL" : "success") << std::endl;

    return rtn;
}
