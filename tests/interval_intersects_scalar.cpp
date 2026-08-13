#include <iostream>
import sm.vec;
import sm.interval;

template<typename F> requires std::is_floating_point_v<F>
int test()
{
    int rtn = 0;

    sm::interval<F> a = { 0, 1 };
    sm::interval<F> b = { 0.5, 1.5 };
    sm::interval<F> c = { 1.5, 1.6 };
    sm::interval<F> d = { -1.5, 1.6 };
    sm::interval<F> e = { 1, 2 };

    // Clearly overlapping closed intervals
    std::cout << a << " n " << b << ": " << (a.intersects(b) ? "true" : "false") << std::endl;
    if (a.intersects(b) == false) { --rtn; }
    if (b.intersects(a) == false) { --rtn; }

    // Clearly NON overlapping closed intervals
    std::cout << a << " n " << c << ": " << (a.intersects(c) ? "true" : "false") << std::endl;
    if (a.intersects(c) == true) { --rtn; }
    if (c.intersects(a) == true) { --rtn; }

    // Overlapping as one is completely enclosed
    std::cout << a << " n " << d << ": " << (a.intersects(d) ? "true" : "false") << std::endl;
    if (a.intersects(d) == false) { --rtn; }
    if (d.intersects(a) == false) { --rtn; }

    // Adjacent closed intervals should intersect
    std::cout << a << " n " << e << ": " << (a.intersects(e) ? "true" : "false") << std::endl;
    if (a.intersects(e) == false) { --rtn; }
    if (e.intersects(a) == false) { --rtn; }

    // Adjacent intervals where one is closed the other open should not intersect
    sm::interval<F, sm::interval_endpoint::open, sm::interval_endpoint::closed> eo = { 1, 2 };
    std::cout << a << " n " << eo << ": " << (a.intersects(eo) ? "true" : "false") << std::endl;
    if (a.intersects(eo) == true) { --rtn; }
    if (eo.intersects(a) == true) { --rtn; }

    sm::interval<F, sm::interval_endpoint::closed, sm::interval_endpoint::open> ao = { 0, 1 };
    std::cout << ao << " n " << eo << ": " << (ao.intersects(eo) ? "true" : "false") << std::endl;
    if (ao.intersects(eo) == true) { --rtn; }
    if (eo.intersects(ao) == true) { --rtn; }

    std::cout << ao << " n " << e << ": " << (ao.intersects(e) ? "true" : "false") << std::endl;
    if (ao.intersects(e) == true) { --rtn; }
    if (e.intersects(ao) == true) { --rtn; }

    // Invalid intervals should not intersect
    sm::interval<F> einv = { 2, 1 }; // einv is invalid
    if (einv.valid() == true) { --rtn; }
    if (b.valid() == false) { --rtn; } // b is valid
    if (b.intersects(einv) == true) { --rtn; } // can't (or at least, we don't) intersect valid and invalid intervals

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}

int main()
{
    int rtn = 0;
    std::cout << "Test with float...\n";
    rtn += test<float>();
    std::cout << "Test with double...\n";
    rtn += test<double>();
    return rtn;
}
