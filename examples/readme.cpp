#include <morph/mathconst.h>
#include <morph/vec.h>
#include <morph/quaternion.h>

int main()
{
    using mc = morph::mathconst<float>;
    morph::vec<float, 3> v1 = { 1, 2, 3 };
    morph::quaternion<float> q1 (morph::vec<float, 3>{1, 0, 0}, mc::pi_over_2);
    // Rotate v1:

    auto v1_rotated = q1 * v1;
    std::cout << v1 << " rotated pi/2 about x-axis is " << v1_rotated << "\n";
}
