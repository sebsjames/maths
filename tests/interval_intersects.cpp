#include <iostream>
import sm.vec;
import sm.interval;

int main()
{
    int rtn = 0;

    sm::interval<sm::vec<float>> a = { {0,   0,   0  }, {1,   1,   1  } };

    sm::interval<sm::vec<float>> b = { {0.5, 0.5, 0.5}, {1.5, 1.5, 1.5} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {1, 1, 1}, {2, 2, 2} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {1.1, 1, 1}, {2, 2, 2} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == true) { --rtn; }
    if (b.intersects(a) == true) { --rtn; }

    b = { {-1, -1, -1}, {0, 0, 0} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {-2, -2, -2}, {-1, -1, -1} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == true) { --rtn; }
    if (b.intersects(a) == true) { --rtn; }

    b = { {0, 0, -2000}, {1, 1, 1000} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {2, 2, -2000}, {3, 3, 1000} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == true) { --rtn; }
    if (b.intersects(a) == true) { --rtn; }

    b = { {.5, .5, -1000}, {.5, .5, 1000} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    b = { {1.5, 1.5, -1000}, {1.5, 1.5, 1000} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == true) { --rtn; }
    if (b.intersects(a) == true) { --rtn; }

    b = { {-1, -1, -1}, {1.1, 1.1, 1.1} };
    std::cout << a << " n " << b << " ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    // Invalid AABB intervals
    a = { { 10.2, 4.6, 0.8 }, { 12.8, 4.3, -1.3 } };
    b = { { 10.5, 4.2, 0.5 }, { 10.8, 4.5, -0.03 } };
    std::cout << "Is " << a << " a valid interval (AABB)?: " << (a.valid() ? "Yes" : "No") << std::endl;
    std::cout << "Is " << b << " a valid interval (AABB)?: " << (b.valid() ? "Yes" : "No") << std::endl;
    if (a.valid() == true || b.valid() == true) { --rtn; }

    // Expect these NOT to intersect because they are not valid. (a.min[i] !< a.max[i] Vi)
    std::cout << a << " n\n" << b << " \n ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    std::cout << b << " n\n" << a << " \n ? " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == true) { --rtn; }
    if (b.intersects(a) == true) { --rtn; }

    // Simpler, more readable invalid interval
    a = { { -1, -1, -1 }, { 1, 1, 1 } }; // valid
    b = { { 1, -1, -1 }, { -1, 1, 1 } }; // invalid, diagonal would make same sized cube that would intersect though.
    std::cout << "Is " << a << " a valid interval (AABB)?: " << (a.valid() ? "Yes" : "No") << std::endl;
    std::cout << "Is " << b << " a valid interval (AABB)?: " << (b.valid() ? "Yes" : "No") << std::endl;
    if (a.valid() == false || b.valid() == true) { --rtn; }
    if (a.intersects(b) == true) { --rtn; }

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
