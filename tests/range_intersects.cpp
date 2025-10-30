#include <iostream>
#include <sm/vec>
#include <sm/range>

int main()
{
    int rtn = 0;

    sm::range<sm::vec<float>> a = { {0,   0,   0  }, {1,   1,   1  } };

    sm::range<sm::vec<float>> b = { {0.5, 0.5, 0.5}, {1.5, 1.5, 1.5} };
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {1, 1, 1}, {2, 2, 2} };
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {-1, -1, -1}, {0, 0, 0} };
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {-2, -2, -2}, {-1, -1, -1} };
    if (a.intersects(b) == true) { --rtn; }
    if (b.intersects(a) == true) { --rtn; }

    b = { {0, 0, -2000}, {1, 1, 1000} };
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {2, 2, -2000}, {3, 3, 1000} };
    if (a.intersects(b) == true) { --rtn; }
    if (b.intersects(a) == true) { --rtn; }

    b = { {.5, .5, -1000}, {.5, .5, 1000} };
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {1.5, 1.5, -1000}, {1.5, 1.5, 1000} };
    if (a.intersects(b) == true) { --rtn; }
    if (b.intersects(a) == true) { --rtn; }

    // Scalar intersects
    sm::range<float> sa = { 4, 5 };
    sm::range<float, sm::range_endpoint::open, sm::range_endpoint::open> sb = { 5, 6 };
    std::cout << sa << ".intersects(" << sb << ") ? " << sa.intersects (sb) << std::endl;
    if (sa.intersects (sb)) { --rtn; }
    std::cout << sb << ".intersects(" << sa << ") ? " << sb.intersects (sa) << std::endl;
    if (sb.intersects (sa)) { --rtn; }

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
