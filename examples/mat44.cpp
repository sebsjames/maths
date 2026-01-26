/*
 * Example: Using the transformation matrix class mat44 to perform rotation, translation
 * and scaling.
 */
#include <iostream>
#include <sm/mathconst>
#include <sm/vec>
#include <sm/quaternion>
#include <sm/mat>

int main()
{
    // A quaternion, used to specify a rotation (here, pi/4 radians about the y axis)
    sm::quaternion<float> q1 (sm::vec<float>{0,1,0}, sm::mathconst<float>::pi_over_4);

    // A 4x4 transformation matrix, which is initialized as the identity matrix
    sm::mat<float, 4> t; // sm::mat<float, 4, 4> is equivalent

    t.rotate (q1); // Rotate is not really constexpr capable // still nok

    std::cout << t << std::endl;

    // Apply a rotation and a translation to t. It does not matter if the rotate() or
    // translate() method is called first, the result is the same.
    t.rotate (q1);
    t.translate (sm::vec<float, 3>{ 0, 0, 2 });

    // Apply a scaling to the transformation matrix t. The order of the call to scale()
    // with respect to any call to rotate() *does* matter.
    t.scale (sm::vec<float, 3>{ 0.5f, 2.0f, 0.25f });

    // An initial vector
    sm::vec<float, 3> v1 = { 1, 0, 0 };

    // Apply the transformation to v1; multiply v1 by the transformation matrix. There
    // are operator* overloads for mat<T, 4> * sm::vec<T, 3> and for mat<T, 4> *
    // sm::vec<T, 4>, but both return vec<T, 4>:
    //
    sm::vec<float, 4> v_4d = t * v1;
    // which you can then reduce with vec<>::less_one_dim():
    sm::vec<float, 3> v_3d = (t * v1).less_one_dim();

    std::cout << "Result of our rotation/translation then scaling of " << v1 << " is: " << v_4d
              << " or " << v_3d << " in three dimensions\n";

    sm::mat<float, 4> mi = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
    std::cout << "mi =\n" << mi << std::endl;

    sm::mat<float, 4> mi0 = { 1,2,3,4 };
    std::cout << "mi0 =\n" << mi0 << std::endl;

    sm::mat<float, 4> mi2 = std::array<float, 16>{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
    std::cout << "mi2 =\n" << mi2 << std::endl;
    sm::mat<float, 4> mi3 = sm::vec<float, 16>{ 2,2,2,2 };
    std::cout << "mi3 =\n" << mi3 << std::endl;

    mi3 = { 4,3,2,1, 4,3,2,1, 4,3,2,1, 4,3,2,1 };
    std::cout << "mi3 reassigned =\n" << mi3 << std::endl;

    // Addition and multiplication of matrices
    sm::mat<double, 4> m1;
    sm::mat<double, 4> m2;
    sm::mat<double, 4> m3 = m1 + m2;
    sm::mat<double, 4> m4 = m1 - m2;
    sm::mat<double, 4> m5 = m1 * m2;
    sm::mat<double, 4> m6 = m1 + 4.0;
    sm::mat<double, 4> m7 = m1 - 4u;

    std::cout << m1 << "\n\n+\n" << m2 << "\n=\n" << m3 << std::endl;
    std::cout << m1 << "\n\n-\n" << m2 << "\n=\n" << m4 << std::endl;
    std::cout << m1 << "\n\n*\n" << m2 << "\n=\n" << m5 << std::endl;
    std::cout << m1 << "\n\n+\n" << 4.0 << "\n=\n" << m6 << std::endl;
    std::cout << m1 << "\n\n-\n" << 4u << "\n=\n" << m7 << std::endl;
    // += scalar
    m7 += 10.0f;
    std::cout << "\n\n+=10 gives\n" << m7 << std::endl;
    // -= matrix
    m7 -= m2;
    std::cout << "\n\n-=\n " << m2 << " gives\n" << m7 << std::endl;

    std::array<double, 16> arr = { 1, 2, 3, 4, 1, 2, 3, 4, 5, 6, 7, 8, 5, 6, 7, 8 };
    // mat44<T> + std::array<T, 16>std::array<T, 16>
    std::cout << "mat44 + arr: " << (m1 + arr) << std::endl;

    // mat<double, 4> + vec<double, N> should fail and does, because vec<> is not double
    // and operator+ is defined for mat44<T> and const T&, so the argument must be
    // castable to type T.
    //
    // sm::vec<double, 4> vam1 = {1,2,3,4};
    // std::cout << "mat44 + vec<T, 4>: " << (m1 + vam1) << std::endl;

    // perspective
    sm::mat<double, 4> p1 = sm::mat<double, 4>::perspective (30, 1.33, 0.1, 100);
    std::cout << "\np1\n" << p1 << std::endl;
    sm::mat<double, 4> p2;
    p2.perspective_inplace (30, 1.33, 0.1, 100);

    // orthographic
    sm::vec<float, 2> ov1 = {-1,-1};
    sm::vec<float, 2> ov2 = {2,2};
    sm::mat<float, 4> o1 = sm::mat<float, 4>::orthographic (ov1, ov2, 0.1, 100);
    std::cout << "\no1\n" << o1 << std::endl;
    sm::mat<float, 4> o2;
    o2.orthographic_inplace (ov1, ov2, 0.1, 100);
}
