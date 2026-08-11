#include <iostream>
import sm.vec;
import sm.interval;

int main()
{
    int rtn = 0;

    sm::interval<float> a = { 0, 1 };
    sm::interval<float> b = { 0.5, 1.5 };
    sm::interval<float> c = { 1.5, 1.6 };
    sm::interval<float> d = { -1.5, 1.6 };
    sm::interval<float> e = { 1, 2 };

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

    // Adjacemt intervals where one is closed the other open should not intersect
    sm::interval<float, sm::interval_endpoint::open, sm::interval_endpoint::closed> eo = { 1, 2 };
    std::cout << a << " n " << eo << ": " << (a.intersects(eo) ? "true" : "false") << std::endl;
    if (a.intersects(eo) == true) { --rtn; }
    if (eo.intersects(a) == true) { --rtn; }

    sm::interval<float, sm::interval_endpoint::closed, sm::interval_endpoint::open> ao = { 0, 1 };
    std::cout << ao << " n " << eo << ": " << (ao.intersects(eo) ? "true" : "false") << std::endl;
    if (ao.intersects(eo) == true) { --rtn; }
    if (eo.intersects(ao) == true) { --rtn; }

    std::cout << ao << " n " << e << ": " << (ao.intersects(e) ? "true" : "false") << std::endl;
    if (ao.intersects(e) == true) { --rtn; }
    if (e.intersects(ao) == true) { --rtn; }

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
