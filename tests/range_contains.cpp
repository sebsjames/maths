#include <iostream>
#include <sm/vec>
#include <sm/range>

int main()
{
    int rtn = 0;

    sm::range<float, sm::range_endpoint::open, sm::range_endpoint::closed> os (2, 3);
    std::cout << "Open semi range " << os << std::endl;
    sm::range<float> cc (2, 3);
    std::cout << "Closed range " << cc << std::endl;

    if (os.contains (cc)) {
        std::cout << os << " contains " << cc << std::endl;
        --rtn;
    }
    if (cc.contains (os)) {
        std::cout << cc << " contains " << os << std::endl;
    }

    sm::range<float> cc2 (2.1f, 3.0f);
    if (cc2.contains (os)) {
        std::cout << cc2 << " contains " << os << std::endl;
        --rtn;
    }

    sm::range<float, sm::range_endpoint::open, sm::range_endpoint::open> oo (2, 3);
    sm::range<float> cc3 (2.1f, 2.9f);
    if (cc3.contains (oo)) {
        std::cout << cc3 << " contains " << oo << std::endl;
        --rtn;
    }
    if (oo.contains (cc3)) {
        std::cout << oo << " contains " << cc3 << std::endl;
    }

    sm::range<float> cc4 (2.5, 3.5);
    if (cc.contains (cc4) || cc4.contains (cc)) { --rtn; }

    std::cout << std::endl << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
    return rtn;
}
