#include <iostream>

#include <sm/vec>
#include <sm/vvec>
#include <sm/geometry>

int main()
{
    int rtn = 0;

    // Test 1
    sm::vvec<sm::vec<float, 2>> points = {
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.2f, 0.3f },
        { 0.1f, 0.9f }
    };

    sm::vvec<sm::vec<float, 2>> expected = {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f }
    };

    sm::vvec<sm::vec<float, 2>> bnd = sm::geometry::graham_scan (points);

    if (bnd != expected) {
        std::cout << "Bad boundary? " << bnd << std::endl;
        --rtn;
    }

    // Test 2
    points[4] = { 0.5f, 0.5f };
    points[5] = { 0.75f, 0.75f };

    bnd = sm::geometry::graham_scan (points);

    if (bnd != expected) {
        std::cout << "Bad boundary? " << bnd << std::endl;
        --rtn;
    }

    // Test 3
    std::cout << "Test 3\n";
    points = {
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.5f, 0.5f },
        { 0.75f, 0.75f }
    };
    expected = {
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f }
    };
    bnd = sm::geometry::graham_scan (points);

    if (bnd != expected) {
        std::cout << "Bad boundary? " << bnd << std::endl;
        --rtn;
    }

    std::cout << "Test " << (rtn ? "failed\n" : "passed\n");
    return rtn;
}
