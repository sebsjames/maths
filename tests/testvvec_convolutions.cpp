#include <sm/vvec>

int main()
{
    int rtn = 0;
    // Test convolve
    sm::vvec<float> a = {1, 2, 3};
    sm::vvec<float> b = {2, 3, 2};
    sm::vvec<float> r1expct = {7, 14, 13};
    sm::vvec<float> r1 = a.convolve (b);
    std::cout << a << " * " << b << " = " << r1  << " (no wrap, expect " << r1expct << ")" << std::endl;
    if (r1 != r1expct) { rtn -= 1; }
    sm::vvec<float> r2expct = {13, 14, 15};
    sm::vvec<float> r2 = a.convolve<sm::vvec<float>::wrapdata::wrap> (b);
    std::cout << a << " * " << b << " = " << r2  << " (wrap, expect " << r2expct << ")" << std::endl;
    if (r2 != r2expct) { rtn -= 1; }

    // Test convolve in place
    r1 = a;
    r2 = a;
    r1.convolve_inplace (b);
    r2.convolve_inplace<sm::vvec<float>::wrapdata::wrap> (b);
    if (r1 != r1expct) { rtn -= 1; }
    if (r2 != r2expct) { rtn -= 1; }

    std::cout << (rtn ? "FAIL\n" : "PASS\n");

    return rtn;
}
