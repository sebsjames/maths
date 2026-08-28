#include <cstdint>
#include <iostream>
#include <utility>
#include <list>

import sm.hex;

std::int32_t main()
{
    std::int32_t rtn = 0;

    std::int32_t r = 0;
    std::int32_t g = 0;
    const float d = 2.0f;
    std::uint32_t idx = 0;
    sm::hex<float> h0(idx++, d, r, g);
    sm::hex<float> h1(idx++, d, r + 1, g);

    h0.compute_location();
    h1.compute_location();

    std::cout << "Locations: " << h0.output_xy() << ", " << h1.output_xy() << std::endl;

    // Test iterators
    std::list<sm::hex<float>> hexlist;
    hexlist.push_back (h0);
    hexlist.push_back (h1);
    h1.nw = hexlist.begin();

    std::cout << "h1: " << h1.output() << std::endl;
    std::cout << "h1's west neighbour: " << h1.nw->output() << std::endl;

    if (h1.nw->x != 0 || h1.nw->y  != 0) {
        --rtn;
    }

    // Distance from
    std::cout << "h0 to h1 distance: " << h1.distance_from (h0) << std::endl;
    if (h1.distance_from (h0) != 2.0f) { --rtn; }
    if (h0.distance_from (h1) != 2.0f) { --rtn; }

    // Test comparison
    if (h1 < h0) {
        --rtn;
    } else {
        std::cout << "h0 < h1 as expected\n";
    }
    if (h0 < h1) {
        std::cout << "h0 < h1 as expected\n";
    } else {
        --rtn;
    }

    if (rtn != 0) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "SUCCESS" << std::endl;
    }
    return rtn;
}
