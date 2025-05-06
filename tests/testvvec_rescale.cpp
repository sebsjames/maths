#include <iostream>
#include <sj/vvec>

int main()
{
    int rtn = 0;

    sj::vvec<float> a = { 6,0,6 };

    sj::vvec<float> b = a;
    b.rescale();
    std::cout << a << " rescaled: " << b << std::endl;

    sj::vvec<float> c = a;
    c.rescale_sym();
    std::cout << a << " symetrically rescaled: " << c << std::endl;

    sj::vvec<float> g = a;
    g.rescale_neg();
    std::cout << a << " neg rescaled: " << g << std::endl;

    if (b != sj::vvec<float>({1, 0, 1})) { --rtn; }
    if (c != sj::vvec<float>({1,-1, 1})) { --rtn; }
    if (g != sj::vvec<float>({0,-1, 0})) { --rtn; }

    sj::vvec<float> e = { -8,-7,-4,-2 };

    sj::vvec<float> d = e;
    d.rescale();
    std::cout << e << " rescaled: " << d << std::endl;

    sj::vvec<float> f = e;
    f.rescale_sym();
    std::cout << e << " sym rescaled: " << f << std::endl;

    sj::vvec<float> h = e;
    h.rescale_neg();
    std::cout << e << " neg rescaled: " << h << std::endl;

    return rtn;
}
