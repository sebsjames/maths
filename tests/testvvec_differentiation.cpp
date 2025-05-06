#include <sj/vvec>

int main()
{
    int rtn = 0;
    // Test differentiate.
    sj::vvec<float> a =     {1, 2, 3, 2, 1, 2, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 9, 7, 5, 1};
    sj::vvec<float> b = a;

    sj::vvec<float> diffexpct_wrap = {0.5,1,0,-1,0,1,0,-1,0,1,1,1,1,1,1,1,1,1,-0.5,-2,-2,-3,-2};
    sj::vvec<float> diffexpct_nowrap = {1,1,0,-1,0,1,0,-1,0,1,1,1,1,1,1,1,1,1,-0.5,-2,-2,-3,-4};

    b.diff_inplace(sj::vvec<float>::wrapdata::wrap);
    std::cout << "d/dx " << a << " = " << b << " (inplace, wrap)"<< std::endl;
    if (b != diffexpct_wrap) { rtn--; }
    sj::vvec<float> c = a.diff (sj::vvec<float>::wrapdata::wrap);
    std::cout << "d/dx " << a << " = " << c << " (rtnversion, wrap)"<< std::endl;
    if (c != diffexpct_wrap) { rtn--; }

    b = a;
    b.diff_inplace(sj::vvec<float>::wrapdata::none);
    std::cout << "d/dx " << a << " = " << b << " (inplace, NO wrap)"<< std::endl;
    if (b != diffexpct_nowrap) { rtn--; }
    c = a.diff (sj::vvec<float>::wrapdata::none);
    std::cout << "d/dx " << a << " = " << c << " (rtnversion, NO wrap)"<< std::endl;
    if (c != diffexpct_nowrap) { rtn--; }

    return rtn;
}
