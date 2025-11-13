/*
 * Testing ray intersection functions
 */

#include <iostream>
#include <sm/geometry>

int main()
{
    int rtn = 0;

    sm::vec<float, 3> p0 = {1, 0, 0};
    sm::vec<float, 3> n = {1, 0, 0};
    sm::vec<float, 3> l0 = {0, 1, 0};
    sm::vec<float, 3> l = {1, 1, 0};
    l.renormalize();

    float t = sm::geometry::ray_plane_intersection<float> (p0, n, l0, l);

    std::cout << "Distance to ray intersection with plane = " << t << std::endl;
    std::cout << "Plane intersection coordinate: " << (l0 + l * t) << std::endl;

    sm::vec<float, 3> t0 = {1, 0, 0};
    sm::vec<float, 3> t1 = {1, 1, 0};
    sm::vec<float, 3> t2 = {1, 0, 1};

    //std::tuple<bool, sm::vec<float>> rti;
    auto [ inside, p ] = sm::geometry::ray_tri_intersection<float> (t0, t1, t2, l0, l);
    std::cout << "Intersection of ray with triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t2, t1, l0, l);
    std::cout << "Intersection of ray with rev triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;

    l0 = {0, 0.5, 0.1};
    l = {1, 0, 0};
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t1, t2, l0, l);
    std::cout << "Intersection of ray with triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t2, t1, l0, l);
    std::cout << "Intersection of ray with rev triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;

    l0 = {2, 0.5, 0.1};
    l = {1, 0, 0};
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t1, t2, l0, l);
    std::cout << "Intersection of ray with triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t2, t1, l0, l);
    std::cout << "Intersection of ray with rev triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;

    l0 = {0, -0.7, 0.1};
    l = {1, 1, 0};
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t1, t2, l0, l);
    std::cout << "Intersection of ray with triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;

    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t2, t1, l0, l);
    std::cout << "Intersection of ray with rev triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;

    // Test intersection of point on edge with l0 down
    l0 = { 1.0f, 0.5f, 0.5f };
    l = { -1.0f, 0.0f, 0.0f };

    t = sm::geometry::ray_plane_intersection<float> (p0, n, l0, l);

    std::cout << "Distance to ray intersection with plane = " << t << std::endl;
    std::cout << "Plane intersection coordinate: " << (l0 + l * t) << std::endl;

    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t1, t2, l0, l);
    std::cout << "Intersection of EDGE ray with rev triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;

    l0 = t0;
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t1, t2, l0, l);
    std::cout << "Intersection of VERTEX ray with rev triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;
    l0 = t1;
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t1, t2, l0, l);
    std::cout << "Intersection of VERTEX ray with rev triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;
    l0 = t2;
    std::tie (inside, p) = sm::geometry::ray_tri_intersection<float> (t0, t1, t2, l0, l);
    std::cout << "Intersection of VERTEX ray with rev triangle: " << (inside ? "IN" : "OUT") << " is at " << p << std::endl;

    return rtn;
}
