// Unused but necessary-for-gcc15 #include
//
// Use of vvec requires one of:
// iostream, memory, cmath, string
// to be #included even though not used in this .cpp file.
//
// Don't work: cstdio, cstdint
//
// Think this is a gcc15 bug.
#include <memory>

import sm.vvec;
int main()
{
    int rtn = 0;
    sm::vvec<float> fvv1 = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    sm::vvec<float> fvv2 = { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
    if (fvv1.all_elements_equal (1.0f) == true) { --rtn; }
    if (fvv2.all_elements_equal (2.0f) == false) { --rtn; }
    return rtn;
}
