#include <iostream>
#include <iomanip>
#include <cmath>
#include <sm/quaternion>
#include <sm/mat33>

int main()
{
    int rtn = 0;

    sm::mat33<float> m1 = { 0, 1, 0,  1, 0, 0,  0, 0, -1 };

    std::cout << "m1:\n" << m1 << std::endl;

    std::cout << "m1.rotation(): " << m1.rotation() << std::endl;

    std::cout << "m1.rotation quaternion magnitude: "
              << m1.rotation().magnitude() << std::endl;

    sm::quaternion<float> r = m1.rotation();
    if (r.w == 0.0f && r.x == sm::mathconst<float>::one_over_root_2
        && r.y == sm::mathconst<float>::one_over_root_2 && r.z == 0.0f) {
        std::cout << "✓ Rotation quaternion test passed\n" << std::endl;
    } else {
        std::cout << "✗ Rotation quaternion test FAILED\n" << std::endl;
        --rtn;
    }

    std::cout << (rtn == 0 ? "\nAll tests passed\n" : "\nSome tests failed\n");
    return rtn;
}
