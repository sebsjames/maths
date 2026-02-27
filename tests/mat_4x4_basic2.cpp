#include <iostream>
#include <sm/quaternion>
#include <sm/mat>

int main()
{
    int rtn = 0;

    {
        sm::mat<float, 4> m1 = { 0, 1, 0, 0,  1, 0, 0, 0,  0, 0, -1, 0,  0, 0, 0, 1 };

        std::cout << "m1:\n" << m1 << std::endl;

        sm::mat<float, 4> m2;

        std::cout << "m2:\n" << m2 << std::endl;

        std::cout << "m1 + m2:\n" << (m1 + m2) << std::endl;

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

        sm::mat<float, 1, 3> onethree = { 1, 2, 3 };

        sm::mat<float, 3, 2> threetwo = { 1, 2, 3, 4, 5, 6 };

        std::cout << "mat mult: " << (onethree * threetwo) << std::endl;

        sm::mat<float, 3> m3 = {1, 2, 1,
            3, 4, 5,
            6, 7, 8};
        std::cout << "m3\n" << m3 << std::endl;
        m3.transpose_inplace();

        std::cout << "m3.T\n" << m3 << std::endl;

        // Multiply by scalar:
        std::cout << m3 << " * 2 = " << m3 * 2 << std::endl;
        sm::mat<float, 3> m3x2 = {
            2, 4, 2,
            6, 8, 10,
            12, 14, 16
        };
        m3x2.transpose_inplace();

        if (m3x2 != m3 * 2) { --rtn; }
    }

    return rtn;
}
