#include <sj/vvec.h>

int main()
{
    int rtn = 0;
    // Test convolve
    sj::vvec<float> a = {1, 2, 3};
    sj::vvec<float> b = {2, 3, 2};
    sj::vvec<float> r1expct = {7, 14, 13};
    sj::vvec<float> r1 = a.convolve (b);
    std::cout << a << " * " << b << " = " << r1  << " (no wrap, expect " << r1expct << ")" << std::endl;
    if (r1 != r1expct) { rtn -= 1; }
    sj::vvec<float> r2expct = {13, 14, 15};
    sj::vvec<float> r2 = a.convolve<sj::vvec<float>::wrapdata::wrap> (b);
    std::cout << a << " * " << b << " = " << r2  << " (wrap, expect " << r2expct << ")" << std::endl;
    if (r2 != r2expct) { rtn -= 1; }

    // Test convolve in place
    r1 = a;
    r2 = a;
    r1.convolve_inplace (b);
    r2.convolve_inplace<sj::vvec<float>::wrapdata::wrap> (b);
    if (r1 != r1expct) { rtn -= 1; }
    if (r2 != r2expct) { rtn -= 1; }

    std::cout << (rtn ? "FAIL\n" : "PASS\n");

    return rtn;
}
