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

    n = {0, -0.707, 0.707};
    v = {0, 1, 1};
    p = sm::geometry::vector_plane_projection (n, v);
    std::cout << v << " projected onto the plane " << n << " is " << p << std::endl;
    if (p != v) { --rtn; }

    n = {0, 0.707, -0.707};
    v = {0, 1, 1};
    p = sm::geometry::vector_plane_projection (n, v);
    std::cout << v << " projected onto the plane " << n << " is " << p << std::endl;
    if (p != v) { --rtn; }

    n = {0, 0.807, 0.707};
    v = {0, 1, 1};
    p = sm::geometry::vector_plane_projection (n, v);
    std::cout << v << " projected onto the plane " << n << " is " << p << std::endl;

    n = {0, 0.707, 0.707};
    v = {0, 1, 1};
    p = sm::geometry::vector_plane_projection (n, v);
    std::cout << v << " projected onto the plane " << n << " is " << p << std::endl;
    if (p != sm::vec<float>{}) { --rtn; }

    std::cout << "Test " << (rtn ? "FAIL" : "success") << std::endl;

    return rtn;
}
