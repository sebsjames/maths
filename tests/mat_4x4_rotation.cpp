// More rotation tests (some are in mat_4x4_translate.cpp

#include <iostream>

import sm.vec;
import sm.quaternion;
import sm.mat;

typedef double F;

int main()
{
    int rtn = 0;

    using mc = sm::mathconst<F>;

    sm::mat<F, 4> rotate_from_axis_angle_method;
    rotate_from_axis_angle_method.rotate (sm::vec<F>::uz(), mc::pi_over_2);

    sm::quaternion<F> qz (sm::vec<F>::uz(), mc::pi_over_2);
    sm::mat<F, 4> rot_from_q_constructor (qz);


    std::cout << "rotate method:\n" << rotate_from_axis_angle_method << std::endl;
    std::cout << "quaternion constructor method:\n" << rot_from_q_constructor << std::endl;

    if (rotate_from_axis_angle_method != rot_from_q_constructor) { --rtn; }

    if (rtn == 0) {
        std::cout << "Rotation tests PASSED\n";
    } else {
        std::cout << "Rotation tests FAILED\n";
    }

    return rtn;
}
