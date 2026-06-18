#include <iostream>

import sm.interval;

constexpr sm::interval<float> test_update()
{
    sm::interval<float> r{2.0f, 4.0f};
    r.update (1.0f);
    r.update (5.0f);
    return r;
}

constexpr sm::interval<float> test_update_and_search_init()
{
    sm::interval<float> r;
    r.search_init();
    r.update (1.0f);
    r.update (5.0f);
    return r;
}

constexpr bool test_update_and_contains()
{
    sm::interval<float> r(2.0f, 4.0f); // also test 2 arg constructor
    r.update (1.0f);
    r.update (5.0f);
    int rtn = 0;
    if (r.contains (3.0f) == false) { --rtn; }
    if (r.contains (0.5f) == true) { --rtn; }

    return (rtn == 0 ? true : false);
}

constexpr float test_span()
{
    sm::interval<float> r{2.0f, 4.0f};
    return r.span();
}

constexpr float test_set()
{
    sm::interval<float> r;
    r.set (56.0f, 59.0f);
    return r.span();
}

int main()
{
    int rtn = 0;

    constexpr sm::interval<float> r1 = test_update();
    if (r1.min == 1.0f && r1.max == 5.0f) {
        // good
    } else {
        --rtn;
    }
    constexpr sm::interval<float> r2 = test_update_and_search_init();
    if (r2.min == 1.0f && r2.max == 5.0f) {
        // good
    } else {
        --rtn;
    }
    constexpr bool tres = test_update_and_contains();
    if (tres == false) { --rtn; }

    constexpr float spn = test_span();
    if (spn != 2.0f) { --rtn; }

    constexpr float spn2 = test_set();
    if (spn2 != 3.0f) { --rtn; }

    std::cout << "Test " << (rtn == 0 ? "Passed" : "Failed") << std::endl;
    return rtn;
}
