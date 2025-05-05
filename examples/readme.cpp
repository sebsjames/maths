#include <sj/mathconst.h>
#include <sj/vec.h>
#include <sj/quaternion.h>

int main()
{
    using mc = sj::mathconst<float>;
    sj::vec<float, 3> v1 = { 1, 2, 3 };
    sj::quaternion<float> q1 (sj::vec<float, 3>{1, 0, 0}, mc::pi_over_2);
    // Rotate v1:

    auto v1_rotated = q1 * v1;
    std::cout << v1 << " rotated pi/2 about x-axis is " << v1_rotated << "\n";
}
