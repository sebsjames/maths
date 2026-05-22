// More rotation tests (some are in mat_4x4_translate.cpp)

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

    // Had an issue with mat.rotation. These two matrices have a small difference in rotation but caused an error before finding the bug
    sm::mat<float, 4> mat0 = { -0.0849919, -0.996382, -9.22128e-05, 0, 0.0932567, -0.00804698, 0.995609, 0, -0.992008, 0.0846101, 0.0936033, 0, 0,0,0,1 };
    sm::mat<float, 4> mat1 = { -0.0868514, -0.996221, -9.22128e-05, 0, 0.0932415, -0.00822102, 0.995609, 0, -0.991848, 0.0864614, 0.0936033, 0, 0,0,0,1 };

    std::cout << "mat0.determinant(): " << mat0.determinant() << std::endl;
    std::cout << "mat1.determinant(): " << mat1.determinant() << std::endl;

    // They should generate similar rotation quaternions
    sm::quaternion<float> rq0 = mat0.rotation();
    sm::quaternion<float> rq1 = mat1.rotation();

    // These should be equivalent
    sm::quaternion<float> rql0 = mat0.linear().rotation();
    sm::quaternion<float> rql1 = mat1.linear().rotation();

    std::cout << "mat0 * ux = " << mat0 * sm::vec<>::ux() << std::endl;
    std::cout << "rq0 * ux = " << rq0 * sm::vec<>::ux() << std::endl;
    std::cout << "rql0 * ux = " << rql0 * sm::vec<>::ux() << std::endl;
    sm::vec<> diff0 = (mat0 * sm::vec<>::ux()).less_one_dim() - rq0 * sm::vec<>::ux();
    sm::vec<> diff0l = (mat0 * sm::vec<>::ux()).less_one_dim() - rql0 * sm::vec<>::ux();
    std::cout << "mat1 * ux = " << mat1 * sm::vec<>::ux() << std::endl;
    std::cout << "rq1 * ux = " << rq1 * sm::vec<>::ux() << std::endl;
    std::cout << "rql1 * ux = " << rql1 * sm::vec<>::ux() << std::endl;
    sm::vec<> diff1 = (mat1 * sm::vec<>::ux()).less_one_dim() - rq1 * sm::vec<>::ux();
    sm::vec<> diff1l = (mat1 * sm::vec<>::ux()).less_one_dim() - rql1 * sm::vec<>::ux();

    std::cout << "diff0.abs().sum() = " << diff0.abs().sum() << std::endl;
    std::cout << "diff1.abs().sum() = " << diff1.abs().sum() << std::endl;

    if (diff0.abs().sum() > 10 * std::numeric_limits<float>::epsilon()) { --rtn; }
    if (diff1.abs().sum() > 10 * std::numeric_limits<float>::epsilon()) { --rtn; }
    if (diff0l.abs().sum() > 10 * std::numeric_limits<float>::epsilon()) { --rtn; }
    if (diff1l.abs().sum() > 10 * std::numeric_limits<float>::epsilon()) { --rtn; }

    if (rtn == 0) {
        std::cout << "Rotation tests PASSED\n";
    } else {
        std::cout << "Rotation tests FAILED\n";
    }

    return rtn;
}
