#include <iostream>
#include <sm/vvec>
#include <sm/vec>

int main()
{
    int rtn = 0;
    sm::vvec<float> a = { 6,0,6 };
    sm::range<float> r = a.range();
    if (r.min != 0.0f || r.max != 6.0f) { --rtn; }

    sm::vvec<sm::vec<double, 3>> vd = {
        sm::vec<double, 3>{1,2,3},
        sm::vec<double, 3>{2,2,3},
        sm::vec<double, 3>{-3,2,1},
        sm::vec<double, 3>{1,1,1},
        sm::vec<double, 3>{0}
    };
    for (auto v : vd) {
        std::cout << v << " has length "<< v.length() << std::endl;
    }
    std::cout << "Calling vd.range()\n";
    sm::range<sm::vec<double, 3>> vr = vd.range();
    std::cout << "Range of " << vd << " is\n" << vr << std::endl;

    std::cout << "Calling vd.extent()\n";
    auto ve = vd.extent();
    std::cout << "Extent of " << vd << " is\n" << ve << std::endl;

    // Range and extent should be the same
    if (!(vr == ve)) { --rtn; }

    if (vr.min != sm::vec<double>{-3,0,0} || vr.max != sm::vec<double>{2,2,3}) { --rtn; }

    vd.push_back ({2,4,3});
    vr = vd.range();
    if (vr.min != sm::vec<double>{-3,0,0} || vr.max != sm::vec<double>{2,4,3}) { --rtn; }

    std::cout << "Tests " << (rtn ? "failed\n" : "passed\n");
    return rtn;
}
