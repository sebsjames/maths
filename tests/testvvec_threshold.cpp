#include <iostream>
#include <sj/vvec>

int main()
{
    int rtn = 0;

    sj::vvec<float> a = { 0.0f, 4.0f, -3.0f, 8.8f, -7.001f, -0.0f };

    sj::vvec<float> b = a.threshold (-5.0f, 5.0f);

    sj::vvec<float> expct = { 0.0f, 4.0f, -3.0f, 5.0f, -5.0f, -0.0f };

    std::cout << a << " thresholded: " << b << std::endl;

    a.threshold_inplace (-5, 5);

    std::cout << "And thresholded in place: " << a << std::endl;

    if (a != expct || b != expct) { --rtn; }

    return rtn;
}
