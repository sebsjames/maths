#include <iostream>
#include <sm/quaternion>
#include <sm/vec>

constexpr int test_quat1()
{
    int rtn = 0;

    sm::quaternion<float> q;
    q.renormalize();

    float angularSpeed = 0.2;
    sm::vec<float> rotationAxis = {1.0f, 0.0f, 0.0f};
    sm::quaternion<float> rotationquaternion (rotationAxis, angularSpeed);

    sm::quaternion<float> p = q;
    if (p == q) {  } else { rtn++; }
    if (p != q) { rtn++; }

    sm::quaternion<float> qq1 (1.0f, -2.0f, 3.0f, -4.0f);

    sm::quaternion<float> qq1i = qq1.inverse();

    sm::quaternion<float> qiqi = qq1i * qq1;
    sm::quaternion<float> qident;
    if (qident != qiqi) { --rtn; }

    using mc = sm::mathconst<float>;
    sm::quaternion<float> q1(sm::vec<float>({1,0,0}), mc::pi_over_3);
    sm::quaternion<float> q2(sm::vec<float>({0,1,0}), mc::pi_over_4);
    sm::quaternion<float> q3 = q1 * q2;
    if (q3.x == 0) { q3.x += 0.1f; }


    // (q2 q1)* = q1* q2*, not q2* q1*
    sm::quaternion<float> qcpc = q1.conjugate() * q2.conjugate();
    sm::quaternion<float> pqconj = (q2 * q1).conjugate();
    sm::quaternion<float> qpconj = (q1 * q2).conjugate();
    if (qcpc != pqconj) { --rtn; }
    if (qcpc == qpconj) { --rtn; }

    if (qident.magnitude() != 1.0f) { --rtn; }
    qident.reset();
    q1.postmultiply(q2);
    q2.premultiply(qident);

    q2.rotate (1.0f, 0.0f, 0.0f, mc::pi_over_2);
    q2.rotate (std::array<float, 3>{1.0f, 0.0f, 0.0f}, mc::pi_over_2);
    q2.rotate (sm::vec<float, 3>{1.0f, 0.0f, 0.0f}, mc::pi_over_2);

    std::array<float, 16> am = q1.rotationMatrix();
    if (am[0] != 1.0f) { am[0] += 1.0f; } // Just avoid unused variable for am

    std::array<float, 16> am2 = q2.unitRotationMatrix();
    if (am2[0] != 1.0f) { am2[0] += 1.0f; } // Just avoid unused variable for am

    sm::quaternion<float> qinvert = q1.invert();
    if (qinvert.x == 0) { qinvert.x += 1; }
    return rtn;
}

int main()
{
    constexpr int rtn = test_quat1();
    if constexpr (rtn == 0) { std::cout << "Success\n"; }
    return rtn;
}
