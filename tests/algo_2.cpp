#include <iostream>
#include <sm/algo>

int main()
{
    int rtn = 0;

    sm::range<sm::vec<float>> box = { {0,0,0}, {1,1,1} };

    sm::vec<float> p0 = {0.5, -2, 0.5};
    sm::vec<float> p1 = {0.5,  2, 0.5};

    if (sm::algo::aabb_line_intersect (box, p0, p1) == false) { --rtn; }

    p0 = {1.5, -2, 1.5};
    p1 = {1.5,  2, 1.5};
    if (sm::algo::aabb_line_intersect (box, p0, p1) == true) { --rtn; }

    std::cout << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
