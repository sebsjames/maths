// Testing rotations of unit vectors about unit axes with mat44 multiplication
// and quaternion multiplication

#include <sm/mathconst>
#include <sm/vec>
#include <sm/quaternion>
#include <sm/mat>

#ifndef FLT
typedef float F;
#else
typedef FLT   F;
#endif

int main()
{
    int rtn = 0;

    constexpr sm::vec<F> ux = sm::vec<F>::ux();
    constexpr sm::vec<F> uy = sm::vec<F>::uy();
    constexpr sm::vec<F> uz = sm::vec<F>::uz();

    using mc = sm::mathconst<F>;

    // Translation of [1,0,0], then the rotation 90 deg around z axis

    sm::mat<F, 4> truth_mat_tr;
    truth_mat_tr.translate (ux);
    sm::mat<F, 4> truth_mat_rot;
    truth_mat_rot.rotate (uz, sm::mathconst<F>::pi_over_2);
    sm::mat<F, 4> truth_mat = truth_mat_tr * truth_mat_rot;

    sm::vec<F> ux_about_z_truth_pretrans = (truth_mat * ux).less_one_dim(); //{ 0.0, 2.0, 0.0 };
    sm::vec<F> uy_about_z_truth_pretrans = (truth_mat * uy).less_one_dim(); //{-1.0, 1.0, 0.0 };
    sm::vec<F> uz_about_z_truth_pretrans = (truth_mat * uz).less_one_dim(); //{ 0.0, 1.0, 1.0 };

    sm::quaternion<F> qz (uz, mc::pi_over_2);

    sm::mat<F, 4> tmz_pt;
    tmz_pt.rotate (qz);       // I * R
    tmz_pt.pretranslate (ux); // T * (I * R) == T * R

    std::cout << "Linear part returned : " << tmz_pt.linear() << std::endl << std::endl;
    std::cout << "Translation part returned : " << tmz_pt.translation() << std::endl << std::endl;

    sm::vec<F, 4> ux_about_tmz_pt = tmz_pt * ux;
    sm::vec<F, 4> uy_about_tmz_pt = tmz_pt * uy;
    sm::vec<F, 4> uz_about_tmz_pt = tmz_pt * uz;

    std::cout << std::endl
              << "ux: " << ux << " rotated about the z axis and pre-translated by ux using TM is "
              << ux_about_tmz_pt << "\nTRUTH : " << ux_about_z_truth_pretrans << std::endl << std::endl;
    std::cout << "uy: " << uy << " rotated about the z axis and pre-translated by ux using TM is "
              << uy_about_tmz_pt << "\nTRUTH : " << uy_about_z_truth_pretrans << std::endl << std::endl;
    std::cout << "uz: " << uz << " rotated about the z axis and pre-translated by ux using TM is "
              << uz_about_tmz_pt << "\nTRUTH : " << uz_about_z_truth_pretrans << std::endl << std::endl;

    // Had to scale the epsilon here because the pretranslate pushes the values further from 1.
    if ((ux_about_tmz_pt.less_one_dim() - ux_about_z_truth_pretrans).abs().max() > 2.0 * std::numeric_limits<F>::epsilon()
        || (uy_about_tmz_pt.less_one_dim() - uy_about_z_truth_pretrans).abs().max() > 2.0 * std::numeric_limits<F>::epsilon()
        || (uz_about_tmz_pt.less_one_dim() - uz_about_z_truth_pretrans).abs().max() > 2.0 * std::numeric_limits<F>::epsilon()) {
        std::cout << "1 failed\n";
        --rtn;
    }

    // Alternative ordering. pretranslate first, then rotate should give the same result
    sm::mat<F, 4> tmz_pt2;
    tmz_pt2.pretranslate (ux); // T * I
    tmz_pt2.rotate (qz);       // (T * I) * R == T * R

    // Translate first then rotate should also give the same result
    sm::mat<F, 4> tmz_pt3;
    tmz_pt3.translate (ux);    // I * T
    tmz_pt3.rotate (qz);       // (I * T) * R == T * R

    sm::vec<F, 4> ux_about_tmz_pt2 = tmz_pt2 * ux;
    std::cout << "tmz_pt2 * ux = " << ux_about_tmz_pt2<< " cf. tmz_pt * ux = " << ux_about_tmz_pt << std::endl;
    std::cout << "tmz_pt3 * ux = " << (tmz_pt3 * ux) << " cf. tmz_pt * ux = " << ux_about_tmz_pt << std::endl;

    if ((tmz_pt2 * ux) != (tmz_pt * ux)) {
        std::cout << "2 failed\n";
        --rtn;
    }
    if ((tmz_pt3 * ux) != (tmz_pt * ux)) {
        std::cout << "3 failed\n";
        --rtn;
    }

    // Translation of [0,1,0], then the rotation -90 deg around y axis

    sm::vec<F> ux_about_y_truth_pretrans = { 0.0, 1.0, -1.0 };
    sm::vec<F> uy_about_y_truth_pretrans = { 0.0, 2.0, 0.0 };
    sm::vec<F> uz_about_y_truth_pretrans = { 1.0, 1.0, 0.0 };

    sm::quaternion<F> qy (uy, mc::pi_over_2);

    sm::mat<F, 4> tmy_pt;
    tmy_pt.rotate (qy);        // I * R
    tmy_pt.pretranslate (uy);  // T * (I * R) == T * R

    sm::vec<F, 4> ux_about_tmy_pt = tmy_pt * ux;
    sm::vec<F, 4> uy_about_tmy_pt = tmy_pt * uy;
    sm::vec<F, 4> uz_about_tmy_pt = tmy_pt * uz;

    std::cout << std::endl
              << "ux: " << ux << " rotated about the y axis and pre-translated by uy using TM is "
              << ux_about_tmy_pt << "\nTRUTH : " << ux_about_y_truth_pretrans << std::endl << std::endl;
    std::cout << "uy: " << uy << " rotated about the y axis and pre-translated by uy using TM is "
              << uy_about_tmy_pt << "\nTRUTH : " << uy_about_y_truth_pretrans << std::endl << std::endl;
    std::cout << "uz: " << uz << " rotated about the y axis and pre-translated by uy using TM is "
              << uz_about_tmy_pt << "\nTRUTH : " << uz_about_y_truth_pretrans << std::endl << std::endl;

    // Had to scale the epsilon here because the pretranslate pushes the values further from 1.
    if ((ux_about_tmy_pt.less_one_dim() - ux_about_y_truth_pretrans).abs().max() > 2.0 * std::numeric_limits<F>::epsilon()
        || (uy_about_tmy_pt.less_one_dim() - uy_about_y_truth_pretrans).abs().max() > 2.0 * std::numeric_limits<F>::epsilon()
        || (uz_about_tmy_pt.less_one_dim() - uz_about_y_truth_pretrans).abs().max() > 2.0 * std::numeric_limits<F>::epsilon()) { --rtn; }


    if (rtn == 0) {
        std::cout << "Pretranslation tests PASSED\n";
    } else {
        std::cout << "Pretranslation tests FAILED\n";
    }

    return rtn;
}
