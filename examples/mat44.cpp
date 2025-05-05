/*
 * An example oof using the transformation matrix class mat44 to perform rotation,
 * translation and scaling.
 */
#include <iostream>
#include <sj/mathconst.h>
#include <sj/vec.h>
#include <sj/quaternion.h>
#include <sj/mat44.h>

int main()
{
    // A quaternion, used to specify a rotation (here, pi/4 radians about the y axis)
    constexpr sj::quaternion<float> q1 (sj::vec<float>{0,1,0}, sj::mathconst<float>::pi_over_4);

    // A transformation matrix, which is initialized as the identity matrix
    constexpr sj::mat44<float> t;

    //t.rotate (q1); // Rotate is not really constexpr capable // still nok

    std::cout << t << std::endl;
#if 0

    // Apply a rotation and a translation to t. It does not matter if the rotate() or
    // translate() method is called first, the result is the same.
    t.rotate (q1);
    t.translate (sj::vec<float, 3>{ 0, 0, 2 });

    // Apply a scaling to the transformation matrix t. The order of the call to scale()
    // with respect to any call to rotate() *does* matter.
    t.scale (sj::vec<float, 3>{ 0.5f, 2.0f, 0.25f });

    // An initial vector
    sj::vec<float, 3> v1 = { 1, 0, 0 };

    // Apply the transformation to v1; multiply v1 by the transformation matrix. There
    // are operator* overloads for mat44<T> * sj::vec<T, 3> and for mat44<T> *
    // sj::vec<T, 4>, but both return vec<T, 4>:
    //
    sj::vec<float, 4> v_4d = t * v1;
    // which you can then reduce with vec<>::less_one_dim():
    sj::vec<float, 3> v_3d = (t * v1).less_one_dim();

    std::cout << "Result of our rotation/translation then scaling of " << v1 << " is: " << v_4d
              << " or " << v_3d << " in three dimensions\n";
#endif
}
