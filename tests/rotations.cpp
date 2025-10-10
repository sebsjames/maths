#include <sm/quaternion>
#include <sm/vec>
#include <sm/mathconst>
#include <sm/random>
#include <sm/mat44>

int main()
{
    int rtn = 0;

    sm::vec<float> axis = {1, 0, 1}; // Non-length 1 vector
    auto theta = sm::mathconst<float>::pi_over_4;

    sm::mat44<float> rmat;
    rmat.rotate (axis, theta);

    auto rv = (rmat * sm::vec<>::ux()).less_one_dim();
    std::cout << "ux mat44 rotated pi_over_4 about un-normalized axis " << axis << " is " << rv << " length " << rv.length() << std::endl;

    // Internally, we convert to Quaternion, then rotate.
    sm::quaternion<float> q;
    q.rotate (axis, theta);
    std::cout << "q formed from unnormalized axis/theta is " << q << std::endl;
    q.renormalize();
    std::cout << "after renormalization of q, we get " << q << std::endl;

    rmat.setToIdentity();
    axis.renormalize();
    rmat.rotate (axis, theta);

    auto rv2 = (rmat * sm::vec<>::ux()).less_one_dim();
    std::cout << "ux mat44 rotated pi_over_4 about *normalized* axis " << axis << " is " << rv2 << " length " << rv2.length() << std::endl;

    sm::quaternion<float> q1;
    q1.rotate (axis, theta);
    std::cout << "q1 formed from *normalized* axis/theta is " << q1 << std::endl;

    auto rvdiff = rv - rv2;
    if (rvdiff.longest() > std::numeric_limits<float>::epsilon()) {
        std::cout << "unnormed axis used to rotate gives " << rv << " != " << rv2 << " formed from normalized axis\n";
        --rtn;
    }

    return rtn;
}
