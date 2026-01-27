#include <iostream>
#include <sm/quaternion>
#include <sm/mat>

int main()
{
    int rtn = 0;

    sm::mat<float, 3> m1 = { 0, 1, 0,  1, 0, 0,  0, 0, -1 };

    std::cout << "m1:\n" << m1 << std::endl;

    std::cout << "m1.rotation(): " << m1.rotation() << std::endl;

    std::cout << "m1.rotation quaternion magnitude: "
              << m1.rotation().magnitude() << std::endl;

    sm::quaternion<float> r = m1.rotation();
    if (r.w == 0.0f && r.x == sm::mathconst<float>::one_over_root_2
        && r.y == sm::mathconst<float>::one_over_root_2 && r.z == 0.0f) {
        rtn = 0;
    } else {
        rtn = 1;
    }

    return rtn;
}
