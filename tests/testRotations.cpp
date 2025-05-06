// Testing rotations of unit vectors about unit axes with mat44 multiplication
// and quaternion multiplication

#include <sj/quaternion>
#include <sj/mat44>
#include <sj/vec>
#include <sj/mathconst>

#ifndef FLT
typedef float F;
#else
typedef FLT   F;
#endif

int main()
{
    int rtn = 0;

    constexpr sj::vec<F> ux = { 1, 0, 0 };
    constexpr sj::vec<F> uy = { 0, 1, 0 };
    constexpr sj::vec<F> uz = { 0, 0, 1 };
    constexpr sj::vec<F> minus_ux = { -1, 0, 0 };
    constexpr sj::vec<F> minus_uy = { 0, -1, 0 };
    constexpr sj::vec<F> minus_uz = { 0, 0, -1 };

    // Expected rotations
    constexpr sj::vec<F> ux_about_x_truth = ux;
    constexpr sj::vec<F> uy_about_x_truth = uz;
    constexpr sj::vec<F> uz_about_x_truth = minus_uy;

    constexpr sj::vec<F> ux_about_y_truth = minus_uz;
    constexpr sj::vec<F> uy_about_y_truth = uy;
    constexpr sj::vec<F> uz_about_y_truth = ux;

    constexpr sj::vec<F> ux_about_z_truth = uy;
    constexpr sj::vec<F> uy_about_z_truth = minus_ux;
    constexpr sj::vec<F> uz_about_z_truth = uz;

    using mc = sj::mathconst<F>;

    sj::quaternion<F> qx (ux, mc::pi_over_2);
    sj::vec<F> ux_about_x = qx * ux;
    sj::vec<F> uy_about_x = qx * uy;
    sj::vec<F> uz_about_x = qx * uz;
    std::cout << "ux: " << ux << " rotated about the x axis is " << ux_about_x << std::endl;
    std::cout << "uy: " << uy << " rotated about the x axis is " << uy_about_x << std::endl;
    std::cout << "uz: " << uz << " rotated about the x axis is " << uz_about_x << std::endl;

    std::cout << "For this floating point type, epsilon = " << std::numeric_limits<F>::epsilon() << std::endl;
    std::cout << "ux about x max error: " << (ux_about_x - ux_about_x_truth).abs().max() << std::endl;
    std::cout << "uy about x max error: " << (uy_about_x - uy_about_x_truth).abs().max() << std::endl;
    std::cout << "uz about x max error: " << (uz_about_x - uz_about_x_truth).abs().max() << std::endl;

    if ((ux_about_x - ux_about_x_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uy_about_x - uy_about_x_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uz_about_x - uz_about_x_truth).abs().max() > std::numeric_limits<F>::epsilon()) { --rtn; }

    sj::quaternion<F> qy (uy, mc::pi_over_2);
    sj::vec<F> ux_about_y = qy * ux;
    sj::vec<F> uy_about_y = qy * uy;
    sj::vec<F> uz_about_y = qy * uz;
    std::cout << std::endl
              << "ux: " << ux << " rotated about the y axis is " << ux_about_y << std::endl;
    std::cout << "uy: " << uy << " rotated about the y axis is " << uy_about_y << std::endl;
    std::cout << "uz: " << uz << " rotated about the y axis is " << uz_about_y << std::endl;

    std::cout << "ux about y max error: " << (ux_about_y - ux_about_y_truth).abs().max() << std::endl;
    std::cout << "uy about y max error: " << (uy_about_y - uy_about_y_truth).abs().max() << std::endl;
    std::cout << "uz about y max error: " << (uz_about_y - uz_about_y_truth).abs().max() << std::endl;

    if ((ux_about_y - ux_about_y_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uy_about_y - uy_about_y_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uz_about_y - uz_about_y_truth).abs().max() > std::numeric_limits<F>::epsilon()) { --rtn; }

    sj::quaternion<F> qz (uz, mc::pi_over_2);
    sj::vec<F> ux_about_z = qz * ux;
    sj::vec<F> uy_about_z = qz * uy;
    sj::vec<F> uz_about_z = qz * uz;
    std::cout << std::endl
              << "ux: " << ux << " rotated about the z axis is " << ux_about_z << std::endl;
    std::cout << "uy: " << uy << " rotated about the z axis is " << uy_about_z << std::endl;
    std::cout << "uz: " << uz << " rotated about the z axis is " << uz_about_z << std::endl;

    std::cout << "ux about z max error: " << (ux_about_z - ux_about_z_truth).abs().max() << std::endl;
    std::cout << "uy about z max error: " << (uy_about_z - uy_about_z_truth).abs().max() << std::endl;
    std::cout << "uz about z max error: " << (uz_about_z - uz_about_z_truth).abs().max() << std::endl;

    if ((ux_about_z - ux_about_z_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uy_about_z - uy_about_z_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uz_about_z - uz_about_z_truth).abs().max() > std::numeric_limits<F>::epsilon()) { --rtn; }

    std::cout << "\n\n";

    sj::mat44<F> tmx;
    tmx.rotate (qx);
    sj::vec<F, 4> ux_about_tmx = tmx * ux;
    sj::vec<F, 4> uy_about_tmx = tmx * uy;
    sj::vec<F, 4> uz_about_tmx = tmx * uz;
    std::cout << "ux: " << ux << " rotated about the x axis by TM is " << ux_about_tmx << std::endl;
    std::cout << "uy: " << uy << " rotated about the x axis by TM is " << uy_about_tmx << std::endl;
    std::cout << "uz: " << uz << " rotated about the x axis by TM is " << uz_about_tmx << std::endl;

    if ((ux_about_tmx.less_one_dim() - ux_about_x_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uy_about_tmx.less_one_dim() - uy_about_x_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uz_about_tmx.less_one_dim() - uz_about_x_truth).abs().max() > std::numeric_limits<F>::epsilon()) { --rtn; }

    sj::mat44<F> tmy;
    tmy.rotate (qy);
    sj::vec<F, 4> ux_about_tmy = tmy * ux;
    sj::vec<F, 4> uy_about_tmy = tmy * uy;
    sj::vec<F, 4> uz_about_tmy = tmy * uz;
    std::cout << std::endl
              << "ux: " << ux << " rotated about the y axis by TM is " << ux_about_tmy << std::endl;
    std::cout << "uy: " << uy << " rotated about the y axis by TM is " << uy_about_tmy << std::endl;
    std::cout << "uz: " << uz << " rotated about the y axis by TM is " << uz_about_tmy << std::endl;

    if ((ux_about_tmy.less_one_dim() - ux_about_y_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uy_about_tmy.less_one_dim() - uy_about_y_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uz_about_tmy.less_one_dim() - uz_about_y_truth).abs().max() > std::numeric_limits<F>::epsilon()) { --rtn; }

    sj::mat44<F> tmz;
    tmz.rotate (qz);
    sj::vec<F, 4> ux_about_tmz = tmz * ux;
    sj::vec<F, 4> uy_about_tmz = tmz * uy;
    sj::vec<F, 4> uz_about_tmz = tmz * uz;
    std::cout << std::endl
              << "ux: " << ux << " rotated about the z axis by TM is " << ux_about_tmz << std::endl;
    std::cout << "uy: " << uy << " rotated about the z axis by TM is " << uy_about_tmz << std::endl;
    std::cout << "uz: " << uz << " rotated about the z axis by TM is " << uz_about_tmz << std::endl;

    if ((ux_about_tmz.less_one_dim() - ux_about_z_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uy_about_tmz.less_one_dim() - uy_about_z_truth).abs().max() > std::numeric_limits<F>::epsilon()
        || (uz_about_tmz.less_one_dim() - uz_about_z_truth).abs().max() > std::numeric_limits<F>::epsilon()) { --rtn; }

    if (rtn == 0) {
        std::cout << "Rotations tests PASSED\n";
    } else {
        std::cout << "Rotations tests FAILED\n";
    }

    return rtn;
}
