#include <iostream>
#include <sm/quaternion>
#include <sm/mat44>

#include <Eigen/Dense>
#include <Eigen/Geometry>

bool check_equal (const sm::mat44<float>& m44,
                  const Eigen::Transform<float, 3, Eigen::TransformTraits::Affine>& em)
{
    bool rtn = true;
    auto emat = em.matrix();

    for (int i = 0; i < 16; ++i) {
        //std::cout << "mat44 " << m44[i] << " cf " << emat(i) << std::endl;
        float fd = std::abs (m44[i] - emat(i));
        if (fd > std::numeric_limits<float>::epsilon()) { rtn = false; }
    }

    return rtn;
}

int main()
{
    int rtn = 0;
    // Check that .translate is same as Eigen
    sm::mat44<float> m1;
    auto ra = sm::vec<>{1,1,0};
    ra.renormalize();
    m1.rotate (ra, sm::mathconst<float>::pi_over_4);

    Eigen::Transform<float, 3, Eigen::TransformTraits::Affine> em1;
    em1.setIdentity();
    Eigen::Vector3f era (ra[0], ra[1], ra[2]);
    em1.rotate (Eigen::AngleAxisf(sm::mathconst<float>::pi_over_4, era));

    std::cout << "m1 with rotate:\n" << m1 << std::endl;
    std::cout << "em1 with rotate:\n" << em1.matrix() << std::endl;

    auto tv = sm::vec<>{1,2,3};
    Eigen::Vector3f etv(tv[0], tv[1], tv[2]);
    sm::mat44<float> m1pt = m1;
    m1pt.pretranslate (tv);
    sm::mat44<float> m1t = m1;
    m1t.translate (tv);
    Eigen::Transform<float, 3, Eigen::TransformTraits::Affine> em1pt = em1;
    em1pt.pretranslate(etv);
    Eigen::Transform<float, 3, Eigen::TransformTraits::Affine> em1t = em1;
    em1t.translate(etv);

    std::cout << "m1 with translate:\n" << m1t << std::endl;
    std::cout << "m1 with pretranslate:\n" << m1pt << std::endl;
    std::cout << "em1 with translate:\n" << em1t.matrix() << std::endl;
    std::cout << "em1 with pretranslate:\n" << em1pt.matrix() << std::endl;

    // Test that m1pt == em1pt
    if (check_equal (m1t, em1t) == false) {
        std::cout << "Fail, m1t != em1t\n";
        --rtn;
    }
    if (check_equal (m1pt, em1pt) == false) {
        std::cout << "Fail, m1pt != em1pt\n";
        --rtn;
    }
    if (check_equal (m1t, em1pt) == true) {
        std::cout << "Fail, m1t == em1pt\n";
        --rtn;
    }
    if (check_equal (m1pt, em1t) == true) {
        std::cout << "Fail, m1pt == em1t\n";
        --rtn;
    }

    std::cout << (rtn == 0 ? "Test passed\n" : "Test failed\n");
    return rtn;
}
