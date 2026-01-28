#include <sm/quaternion>
#include <sm/vec>
#include <sm/mathconst>
#include <sm/random>

int main()
{
    int rtn = 0;

    sm::quaternion<float> q;
    std::cout << q << std::endl;
    sm::quaternion<float> q_save = q;
    q.renormalize();
    std::cout << q << std::endl;

    if (q != q_save) { rtn++; }

    float angularSpeed = 0.2 * sm::mathconst<float>::deg2rad;
    sm::vec<float> rotationAxis = {1.0f, 0.0f, 0.0f};
    sm::quaternion<float> rotationquaternion(rotationAxis, angularSpeed);
    sm::quaternion<float> rq_expected (float{1}, angularSpeed/float{2}, float{0}, float{0});
    std::cout << "quaternion(" << rotationAxis<< ", " << angularSpeed<< ") constructor generates\n"
              << rotationquaternion
              << "\nvs expected:\n" << rq_expected
              << std::endl;
    sm::vec<float, 4> rqerr;
    rqerr[0] = rq_expected.x - rotationquaternion.x;
    rqerr[1] = rq_expected.y - rotationquaternion.y;
    rqerr[2] = rq_expected.z - rotationquaternion.z;
    rqerr[3] = rq_expected.w - rotationquaternion.w;
    if (rqerr.abs().max() > float{15} * std::numeric_limits<float>::epsilon()) {
        std::cout << "Failed on rotation. Errors: " << rqerr << " cf epsilon: "
                  << std::numeric_limits<float>::epsilon() << std::endl;
        rtn++;
    }


    sm::quaternion<float> p = q;
    if (p == q) {  } else { rtn++; }
    if (p != q) { rtn++; }

    sm::quaternion<float> qq1 (1.0f, -2.0f, 3.0f, -4.0f);
    std::cout << std::endl << qq1 << " conjugate (q*): " << qq1.conjugate() << std::endl << std::endl;
    sm::quaternion<float> qq1conj (1.0f, 2.0f, -3.0f, 4.0f);
    if (qq1.conjugate() != qq1conj) { ++rtn; }


    sm::quaternion<float> qq1i = qq1.inverse();
    std::cout << qq1 << " inverse (q^-1 or 1/q): " << qq1i << std::endl << std::endl;

    sm::quaternion<float> qq2 (1.0f, 0.0f, 0.0f, 0.0f);
    sm::quaternion<float> qq2i = qq2.inverse();
    std::cout << qq2 << " inverse (q^-1 or 1/q): " << qq2i << std::endl << std::endl;

    sm::quaternion<float> qiqi = qq1i * qq1;
    std::cout << "qq1i * qq1 = " << qiqi << std::endl << std::endl; // 1,0,0,0
    sm::quaternion<float> qident;
    if (qident == qiqi) {
        std::cout << "Good\n";
    } else {
        --rtn;
        std::cout << "Non good\n";
    }
    std::cout << "cf epsilon: " << std::numeric_limits<float>::epsilon() << std::endl;

    using mc = sm::mathconst<float>;
    sm::quaternion<float> q1(sm::vec<float>({1,0,0}), mc::pi_over_3);
    sm::quaternion<float> q2(sm::vec<float>({0,1,0}), mc::pi_over_4);
    sm::quaternion<float> q3 = q1 * q2;

    std::cout << q3 << " = " << q1 << " * " << q2 << std::endl;
    // q2 *= q1; // no good
    // std::cout << "q2 after *= q1: " << q2 << " cf " << q3 << std::endl;

    // (q2 q1)* = q1* q2*, not q2* q1*
    sm::quaternion<float> qcpc = q1.conjugate() * q2.conjugate();
    sm::quaternion<float> pqconj = (q2 * q1).conjugate();
    sm::quaternion<float> qpconj = (q1 * q2).conjugate();
    if (qcpc == pqconj) {
        std::cout << "Good\n";
    } else {
        --rtn;
    }
    if (qcpc != qpconj) {
        std::cout << "Good\n";
    } else {
        --rtn;
    }

    // Rotation matrices
    sm::quaternion<float> qfm (1, 2, -3, 4); // NOT unit.
    sm::vec<float, 16> vmatA = qfm.rotation_matrix();
    sm::vec<float, 16> vmatB = qfm.unit_rotation_matrix();

    std::cout << "Rotation matrices of non-unit qfm\n";
    std::cout << "rotation_matrix:     " << vmatA << std::endl;
    std::cout << "unit_rotation_matrix: " << vmatB << std::endl;


    std::cout << "Rotation matrices of unit qfm\n";
    qfm.renormalize();
    sm::vec<float, 16> vmatAA = qfm.rotation_matrix();
    sm::vec<float, 16> vmatBB = qfm.unit_rotation_matrix();

    std::cout << "rotation_matrix:     " << vmatAA << std::endl;
    std::cout << "unit_rotation_matrix: " << vmatBB << std::endl;


    sm::vec<float> myaxis = { 1, 2, 3 };
    float myangle = 0.12f;
    myaxis.renormalize();
    sm::quaternion<float> qaa (myaxis, myangle);
    sm::vec<float, 4> aa = qaa.axis_angle();

    std::cout << "\nquaternion " << qaa << "\n"
              << "was set from rotn " << myangle << " about axis " << myaxis << "\n"
              << "and its axis_angle method returns " << aa << "\n"
              << "which is a rotation of " << aa[3] << " rads about axis " << aa.less_one_dim() << std::endl << std::endl;

    float eps = 0.00001; // A fair amount of precision is lost extracting axis angle if type is float
    if (std::abs(aa[0] - myaxis[0]) > eps
        || std::abs(aa[1] - myaxis[1]) > eps
        || std::abs(aa[2] - myaxis[2]) > eps
        || std::abs(aa[3] - myangle) > eps) {
        --rtn;
    }

    sm::quaternion<float> q_unitf (1.0f, 2.0f, -3.0f, 4.0f); // NOT unit.
    q_unitf.renormalize();
    sm::rand_uniform<float> rngf;
    sm::vec<float> vecf;
    float amountf;
    sm::range<float> metric_rangef (0.0f, 0.0f);
    for (unsigned int i = 0; i < 1000000; ++i) {
        rngf.get(vecf);
        vecf.renormalize();
        amountf = rngf.get();
        q_unitf.rotate (vecf, amountf);
        float metricf = sm::cem::abs(float{1} - (q_unitf.w*q_unitf.w + q_unitf.x*q_unitf.x + q_unitf.y*q_unitf.y + q_unitf.z*q_unitf.z));
        // how big is metric? Should be about 0.
        metric_rangef.update (metricf);
    }
    std::cout << "metric_range (float): " << metric_rangef << std::endl;
    // metric_range should be smaller than unit_thresh
    if (metric_rangef.max > sm::quaternion<float>::unit_thresh()) {
        --rtn;
    }

    sm::quaternion<double> q_unitd (1.0, 2.0, -3.0, 4.0); // NOT unit.
    q_unitd.renormalize();
    sm::rand_uniform<double> rngd;
    sm::vec<double> vecd;
    double amountd;
    sm::range<double> metric_ranged (0.0f, 0.0f);
    for (unsigned int i = 0; i < 1000000; ++i) {
        rngd.get(vecd);
        vecd.renormalize();
        amountd = rngd.get();
        q_unitd.rotate (vecd, amountd);
        double metricd = sm::cem::abs(double{1} - (q_unitd.w*q_unitd.w + q_unitd.x*q_unitd.x + q_unitd.y*q_unitd.y + q_unitd.z*q_unitd.z));
        // how big is metric? Should be about 0.
        metric_ranged.update (metricd);
    }
    std::cout << "metric_range (double): " << metric_ranged << std::endl;
    // metric_range should be smaller than unit_thresh
    if (metric_ranged.max > sm::quaternion<double>::unit_thresh()) {
        --rtn;
    }

    if (rtn == 0) {
        std::cout << "quaternion tests PASSED\n";
    } else {
        std::cout << "quaternion tests FAILED\n";
    }

    // init
    sm::quaternion<float> cq = { 2.0f, 1.0f, 2.2f, 1.4f };
    std::cout << "cq = { 2.0f, 1.0f, 2.2f, 1.4f} gives " << cq << std::endl;
    if (cq.w != 2.0f || cq.x != 1.0f || cq.y != 2.2f || cq.z != 1.4f) {
        ++rtn;
    }
    cq.renormalize();
    std::cout << "cq after renormalize " << cq << std::endl;

    sm::quaternion<float> minus_cq = -cq;
    std::cout << "-cq " << minus_cq << std::endl;

    if (minus_cq.w != -cq.w || minus_cq.x != -cq.x || minus_cq.y != -cq.y || minus_cq.z != -cq.z) { --rtn; }

    return rtn;
}
