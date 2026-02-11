/*
 * Testing triangle area
 */

#include <iostream>
#include <sm/geometry>
#include <sm/mat>

int main()
{
    int rtn = 0;

    // Projections of 3D vectors onto 2D planes
    sm::vec<float> t0 = {0, 0, 1};
    sm::vec<float> t1 = {0, 1, 1};
    sm::vec<float> t2 = {0, 1, 1};

    float area = sm::geometry::tri_area (t0, t1, t2);
    std::cout << "Area (should be 0): " << area << std::endl;
    if (area != 0.0f) { --rtn; }

    t0.zero();
    t1 = {};
    t2.zero();

    area = sm::geometry::tri_area (t0, t1, t2);
    std::cout << "Area (should be 0): " << area << std::endl;
    if (area != 0.0f) { --rtn; }

    t0 = {0, 0, 0};
    t1 = {1, 0, 0};
    t2 = {1, 1, 0};
    area = sm::geometry::tri_area (t0, t1, t2);
    std::cout << "Area: (should be 0.5) " << area << std::endl;
    if (area != 0.5f) { --rtn; }

    sm::mat<float, 4> tf;
    tf.translate (sm::vec<>{0.2f, 0.4f, 0.6f});
    tf.rotate (sm::vec<>::uy(), 0.23f);
    t0 = (tf * t0).less_one_dim();
    t1 = (tf * t1).less_one_dim();
    t2 = (tf * t2).less_one_dim();
    std::cout << "Transformed t0,1,2 " << t0 << ", " << t1 << ", " << t2 << std::endl;
    area = sm::geometry::tri_area (t0, t1, t2);
    std::cout << "Area: (should be 0.5) " << area << std::endl;

    // Allow precision error here:
    if (std::abs(area - 0.5f) > 1e-7) { --rtn; }

    if (rtn == 0) {
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED\n";
    }

    return rtn;
}
