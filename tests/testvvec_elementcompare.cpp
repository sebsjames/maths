#include <iostream>
#include <sm/vvec>

int main()
{
    int rtn = 0;

    sm::vvec<float> a = { 0.0f, 4.0f, -3.0f, 8.8f, -7.001f, -0.0f };

    sm::vvec<float> b = a.element_compare_gt (5.0f);

    sm::vvec<float> expct = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

    std::cout << a << " element-wise > 5.0f: " << b << std::endl;

    if (b != expct) { std::cerr << "Test fail!\n"; --rtn; }

    return rtn;
}
