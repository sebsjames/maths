#include <limits>

import sm.vec;
import sm.vvec;

int main()
{
    int rtn = 0;

    sm::vec<int, 5> iv1 = { 1, 2, 3, 4, 5 };
    sm::vec<int, 5> iv2 = { 2, 2, 2, 2, 2 };
    if (iv1.all_elements_equal (1) == true) { --rtn; }
    if (iv2.all_elements_equal (2) == false) { --rtn; }

    sm::vec<float, 5> fv1 = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    sm::vec<float, 5> fv2 = { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
    if (fv1.all_elements_equal (1.0f) == true) { --rtn; }
    if (fv2.all_elements_equal (2.0f) == false) { --rtn; }

    // If we add 2 epsilons to one of the elements of fv2, we should cause all_elements_equal() to return false:
    fv2[4] += 2 * std::numeric_limits<float>::epsilon();
    if (fv2.all_elements_equal (2.0f) == true) { --rtn; }

    // Quick test of the vvec one here, too:
    sm::vvec<float> fvv1 = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    sm::vvec<float> fvv2 = { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };

    if (fvv1.all_elements_equal (1.0f) == true) { --rtn; }
    if (fvv2.all_elements_equal (2.0f) == false) { --rtn; }

    return rtn;
}
