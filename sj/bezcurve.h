/*!
 * \file
 * \brief Bezier curve class
 * \author Seb James
 * \date 2019-2020
 */

#pragma once

#include <utility>
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <cstddef>
#include <concepts>

// This is left as a hint in case anyone tries to compile this with Intel's compiler:
#ifdef __ICC__
# define ARMA_ALLOW_FAKE_GCC 1
#endif
#include <armadillo>
#include <sj/mathconst.h>
#include <sj/optimization/nm_simplex.h>
#include <random>

#include <sj/bezcoord.h>
#include <sj/vvec.h>

namespace sj
{
    /*!
     * Store the first N=21 rows of Pascal's triangle in a linear array. To get the
     * values from row n, where n starts at 0 (and ends at N-1), you step along a number
     * given by the triangle sequence (n(n+1)/2) and then read n+1 values. The triangle
     * has a total number of elements given by N(N+1)/2, which for 21 rows is 21(22)/2 =
     * 210.
     */
    const std::array<unsigned int, 231> Pascal =
    {1,
     1,1,
     1,2,1,
     1,3,3,1,
     1,4,6,4,1,
     1,5,10,10,5,1,
     1,6,15,20,15,6,1,
     1,7,21,35,35,21,7,1,
     1,8,28,56,70,56,28,8,1,
     1,9,36,84,126,126,84,36,9,1,
     1,10,45,120,210,252,210,120,45,10,1,
     1,11,55,165,330,462,462,330,165,55,11,1,
     1,12,66,220,495,792,924,792,495,220,66,12,1,
     1,13,78,286,715,1287,1716,1716,1287,715,286,78,13,1,
     1,14,91,364,1001,2002,3003,3432,3003,2002,1001,364,91,14,1,
     1,15,105,455,1365,3003,5005,6435,6435,5005,3003,1365,455,105,15,1,
     1,16,120,560,1820,4368,8008,11440,12870,11440,8008,4368,1820,560,120,16,1,
     1,17,136,680,2380,6188,12376,19448,24310,24310,19448,12376,6188,2380,680,136,17,1,
     1,18,153,816,3060,8568,18564,31824,43758,48620,43758,31824,18564,8568,3060,816,153,18,1,
     1,19,171,969,3876,11628,27132,50388,75582,92378,92378,75582,50388,27132,11628,3876,969,171,19,1,
     1,20,190,1140,4845,15504,38760,77520,125970,167960,184756,167960,125970,77520,38760,15504,4845,1140,190,20,1};
    //! How many rows in the table above.
    const unsigned int PascalRows = 21;

    /*!
     * A Bezier curve class which allows the computation of Cartesian coordinates
     * (though with x right, y down, and hence a left-hand coordinate system) of points
     * on a Bezier curve which is specified using a parameter (often called t) which is
     * in the range [0, 1]
     */
    template <typename F> requires std::is_floating_point_v<F>
    struct bezcurve
    {
        static constexpr bool debug_bezcurve = false;

        //! Do-nothing constructor
        bezcurve() { this->order = 0; }

        /*!
         * Construct a Bezier curve of order cp.size()-1 with the initial and final
         * points making up part of cp.
         */
       bezcurve (sj::vvec<sj::vec<F, 2>> cp)
        {
            this->C.set_size (cp.size(), 2);
            int i = 0;
            for (auto c : cp) {
                this->C(i,0) = c[0];
                this->C(i,1) = c[1];
                ++i;
            }
            this->init();
        }

        //! Construct a Bezier curve using the control points provided in the matrix @cmat.
        bezcurve (const arma::Mat<F>& cmat)
        {
            this->C = cmat;
            this->init();
        }

        /*!
         * Construct a cubic Bezier curve with a specification of the curve as inital
         * and final position with two control points.
         */
        bezcurve (const sj::vec<F, 2>& ip,
                  const sj::vec<F, 2>& fp,
                  const sj::vec<F, 2>& c1,
                  const sj::vec<F, 2>& c2)
        {
            this->C.set_size (4,2);
            this->C(0,0) = ip[0];
            this->C(0,1) = ip[1];
            this->C(1,0) = c1[0];
            this->C(1,1) = c1[1];
            this->C(2,0) = c2[0];
            this->C(2,1) = c2[1];
            this->C(3,0) = fp[0];
            this->C(3,1) = fp[1];
            this->init();
        }

        /*!
         * Construct a quadratic Bezier curve with a specification of the curve as
         * inital and final position with a single control point
         */
        bezcurve (const sj::vec<F, 2>& ip, const sj::vec<F, 2>& fp, const sj::vec<F, 2>& c1)
        {
            this->C.set_size (3,2);
            this->C(0,0) = ip[0];
            this->C(0,1) = ip[1];
            this->C(1,0) = c1[0];
            this->C(1,1) = c1[1];
            this->C(2,0) = fp[0];
            this->C(2,1) = fp[1];
            this->init();
        }

        //! Construct a linear Bezier curve for production of straight lines.
        bezcurve (const sj::vec<F, 2>& ip, const sj::vec<F, 2>& fp)
        {
            this->C.set_size (2,2);
            this->C(0,0) = ip[0];
            this->C(0,1) = ip[1];
            this->C(1,0) = fp[0];
            this->C(1,1) = fp[1];
            this->init();
        }

        //! Construct a Bezier curve of order cp.size()+1
        bezcurve (const sj::vec<F, 2>& ip, const sj::vec<F, 2>& fp,
                  const sj::vvec<sj::vec<F, 2>>& cp)
        {
            unsigned int n_ctrls = cp.size()+2;
            this->C.set_size (n_ctrls, 2);
            this->C(0,0) = ip[0];
            this->C(0,1) = ip[1];
            unsigned int i = 1;
            for (auto cpi : cp) {
                this->C(i,0) = cpi[0];
                this->C(i,1) = cpi[1];
                ++i;
            }
            this->C(n_ctrls-1,0) = fp[0];
            this->C(n_ctrls-1,1) = fp[1];
            this->init();
        }

        void updateControls (const sj::vvec<sj::vec<F, 2>>& cp)
        {
            this->C.set_size (cp.size(), 2);
            int i = 0;
            for (auto c : cp) {
                this->C(i,0) = c[0];
                this->C(i,1) = c[1];
                ++i;
            }
            this->init();
        }

        /*!
         * Fit a curve to @points, lining up with the curve @c. Assumes this curve
         * appends to the end of @c. *May also modify @c*. Set @optimize to true to try
         * out experimental fit improvements.
         */
        void fit (const sj::vvec<sj::vec<F, 2>>& points, bezcurve<F>& preceding, bool optimize=false)
        {
            // First, find the best fit for @points, without reference to the @preceding curve.
            this->fit (points);

            // preceding control points.
            sj::vvec<sj::vec<F, 2>> prec_ctrl = preceding.getControls();
            std::size_t len = prec_ctrl.size();
            if (len < 3) { return; }

            // va is vector from join to the previous ctrl
            sj::vec<F, 2> va = prec_ctrl[len-2] - prec_ctrl[len-1]; // "prev ctrl - join"
            // vb is vector from join to the next ctrl.
            sj::vec<F, 2> vb = {C(1,0) - C(0,0), C(1,1) - C(0,1)};
            // Use atan2 to get angles with direction here.
            F ang_a = std::atan2 (va[1], va[0]); // NB: args in order y, x!
            F ang_b = std::atan2 (vb[1], vb[0]);
            // theta is the angle between vector a and vector b
            F theta = ang_a - ang_b;
            if constexpr (debug_bezcurve == true) {
                std::cout << "ang_a = " << ang_a << " rads "
                          << (ang_a * 180 / sj::mathconst<F>::pi) << " deg" << std::endl;
                std::cout << "ang_b = " << ang_b << " rads "
                          << (ang_b * 180 / sj::mathconst<F>::pi) << " deg" << std::endl;
                std::cout << "theta = " << theta << " rads "
                          << (theta * 180 / sj::mathconst<F>::pi) << " deg" << std::endl;
            }
            // phi is the angle that conforms to: theta + 2 phi = pi radians
            // thus 2 phi = pi - theta
            // thus   phi = 1/2(pi - theta)
            F phi = F{0.5} * (sj::mathconst<F>::pi - std::abs(theta));
            if constexpr (debug_bezcurve == true) {
                std::cout << "phi = " << phi << " rads "
                          << (phi * 180 / sj::mathconst<F>::pi) << " deg" << std::endl;
            }

            // Construct rotn matrix (one for positive rotation, one for negative)
            arma::Mat<F> rotmat_pos (2,2);
            rotmat_pos(0,0) = std::cos (phi);
            rotmat_pos(0,1) = std::sin (phi);
            rotmat_pos(1,0) = -rotmat_pos(0,1);
            rotmat_pos(1,1) = rotmat_pos(0,0);

            arma::Mat<F> rotmat_neg (2,2);
            rotmat_neg(0,0) = std::cos (-phi);
            rotmat_neg(0,1) = std::sin (-phi);
            rotmat_neg(1,0) = -rotmat_neg(0,1);
            rotmat_neg(1,1) = rotmat_neg(0,0);

            // Now we rotate each point by +/-phi
            // p0 is the point which joins the two curves:
            arma::Mat<F> p0 = C.row(0);

            // Rotate the vector 'va' in the rotmat_neg direction
            arma::Mat<F> pm1 (1,2);
            pm1(0,0) = prec_ctrl[len-2][0];
            pm1(0,1) = prec_ctrl[len-2][1];
            // Offset so we rotate va about p0
            arma::Mat<F> pm1_r = pm1 - p0;

            // Rotate the vector vb in the opposing direction (rotmat_pos)
            arma::Mat<F> pm2 = C.row(1);
            arma::Mat<F> pm2_r = pm2 - p0;

            // Apply rotations depending on the quadrant in which ang_a and ang_b (and thus theta) lay in.
            arma::Mat<F> pm1_r_after;
            arma::Mat<F> pm2_r_after;
            if (ang_b < F{0}) {
                if (ang_a > F{0}) {
                    if constexpr (debug_bezcurve == true) {
                        std::cout << "bezcurve::fit(): Type I join" << std::endl;
                        std::cout << "                 Rotate va +phi, vb -phi." << std::endl;
                    }
                    pm1_r_after = pm1_r * rotmat_pos;
                    pm2_r_after = pm2_r * rotmat_neg;
                } else {
                    if constexpr (debug_bezcurve == true) {
                        std::cout << "bezcurve::fit(): Type II join" << std::endl;
                        std::cout << "                 Rotate va +phi, vb +phi." << std::endl;
                    }
                    pm1_r_after = pm1_r * rotmat_pos;
                    pm2_r_after = pm2_r * rotmat_pos;
                }
            } else {
                if (ang_a > F{0}) {
                    if constexpr (debug_bezcurve == true) {
                        std::cout << "bezcurve::fit(): Type III join" << std::endl;
                        std::cout << "                 Rotate va -phi, vb -phi." << std::endl;
                    }
                    pm1_r_after = pm1_r * rotmat_neg;
                    pm2_r_after = pm2_r * rotmat_neg;
                } else {
                    if constexpr (debug_bezcurve == true) {
                        std::cout << "bezcurve::fit(): Type IV join" << std::endl;
                        std::cout << "                 Rotate va -phi, vb +phi." << std::endl;
                    }
                    pm1_r_after = pm1_r * rotmat_neg;
                    pm2_r_after = pm2_r * rotmat_pos;
                }
            }

            // Translate the points back by p0 to place them in the correct final position
            arma::Mat<F> pm1_r_final = pm1_r_after + p0;
            arma::Mat<F> pm2_r_final = pm2_r_after + p0;

            C.row(1) = pm2_r_final;

            this->init();

            // Update the other curve's control points, also.
            prec_ctrl[len-2][0] = pm1_r_final(0,0);
            prec_ctrl[len-2][1] = pm1_r_final(0,1);
            preceding.updateControls (prec_ctrl);

            if constexpr (debug_bezcurve == true) {
                std::cout << "Preceding controls: " << preceding.outputControl();
                std::cout << "C (after line-up):\n" << this->C;
            }

            // First, need a cost function:
            // If client code requests NOT to optimize, then return
            if (!optimize) { return; }

            /*
             * Nelder-Mead gradient descent optimization of intermediate control points.
             */

            // Optimization stage. Move control points other than those we just fixed to
            // be in line with each other, to minimize the deviation of this curve and
            // @c from the user-points provided.
            if constexpr (debug_bezcurve == true) {
                std::cout << "Optimization..." << std::endl;
            }

            F startsos = this->computeObjective (points);
            if constexpr (debug_bezcurve == true) {
                std::cout << "Objective with no optimization: " << startsos << std::endl;
            }
            arma::Mat<F> Copy = this->C;

            // Convert the middle rows of C to vector<F> to be the first NM vertex
            sj::vvec<F> v0;
            int startrow = 2;
            int endrow = 2; // 2 means don't change the angle of the end of the curve
            for (int r = startrow; r < (int)C.n_rows-endrow; ++r) {
                v0.push_back (this->C(r,0));
                v0.push_back (this->C(r,1));
            }

            if (v0.empty()) {
                std::cout << "No further optimization possible" << std::endl;
                return;
            }

            // Make a set of random vertices to init the nm_simplex algo with.
            sj::vvec<sj::vvec<F>> nm_vertices;

            // First, push back the existing set of controls as the first NM vertex
            nm_vertices.push_back (v0);

            // Set up random
            std::random_device rd;  // Will be used to obtain a seed for the random number engine
            std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
            std::uniform_real_distribution<F> dis(F{0}, F{1});

            // Add some more vertices:
            F propchange = F{0.2};
            F propchangeov2 = propchange / F{2};
            for (std::size_t i = 0; i < v0.size(); ++i) {
                sj::vvec<F> v;
                for (std::size_t j = 0; j < v0.size(); ++j) {
                    // Perturbate v0[j] a bit and add to vector<F>
                    F v_j = v0[j];
                    F v_1 = (v0[j]*propchange);
                    F rn = dis(gen);
                    v_j += (v_1 * rn);
                    v_j -= (v0[j]*propchangeov2);
                    v.push_back (v_j);
                }
                if constexpr (debug_bezcurve == true) {
                    std::cout << "v has size " << v.size() << ", is ";
                    for (auto vv : v) { std::cout << vv << ","; }
                }
                nm_vertices.push_back (v);
                if constexpr (debug_bezcurve == true) {
                    std::cout << " nm_vertices has size " << nm_vertices.size() << std::endl;
                }
            }

            // Start out with a simplex with a vertex at the centroid of the domain vertices, and
            // then two other vertices at the first domain vertex (v) and its neighbour (vn).
            sj::optimization::nm_simplex<F> simp (nm_vertices);
            // Set a termination threshold for the SD of the vertices of the simplex
            simp.termination_threshold = F{0.00001};//2.0 * std::numeric_limits<F>::epsilon();
            // Set an operation limit, in case the above threshold can't be reached
            simp.too_many_operations = 1000;

            // Tweak the NM parameters to help it find solutions
            // The reflection coefficient
            simp.alpha = 0.1; // 1
            // The expansion coefficient
            simp.gamma = 0.2; // 2
            // The contraction coefficient
            simp.rho = 0.05; // .5
            // The shrink coefficient
            simp.sigma = 0.05; // .5

            while (simp.state != optimization::nm_simplex_state::ReadyToStop) {

                if (simp.state == optimization::nm_simplex_state::NeedToComputeThenOrder) {
                    // 1. apply objective to each vertex
                    for (unsigned int i = 0; i <= simp.n; ++i) {
                        this->setCFromV (simp.vertices[i], startrow);
                        this->init(); // Re-setup this bezcurve
                        simp.values[i] = this->computeObjective (points);
                    }
                    simp.order();

                } else if (simp.state == optimization::nm_simplex_state::NeedToOrder) {
                    simp.order();

                } else if (simp.state == optimization::nm_simplex_state::NeedToComputeReflection) {
                    this->setCFromV (simp.xr, startrow);
                    this->init(); // Re-setup this bezcurve
                    F val = this->computeObjective (points);
                    simp.apply_reflection (val);

                } else if (simp.state == optimization::nm_simplex_state::NeedToComputeExpansion) {
                    this->setCFromV (simp.xe, startrow);
                    this->init(); // Re-setup this bezcurve
                    F val = this->computeObjective (points);
                    simp.apply_expansion (val);

                } else if (simp.state == optimization::nm_simplex_state::NeedToComputeContraction) {
                    this->setCFromV (simp.xc, startrow);
                    this->init(); // Re-setup this bezcurve
                    F val = this->computeObjective (points);
                    simp.apply_contraction (val);
                }
            }
            std::cout << "NM finished in " << simp.operation_count << " simplex change operations)" << std::endl;
            sj::vvec<F> vP = simp.best_vertex();
            F min_sos = simp.best_value();
            std::cout << "Best value had objective = " << min_sos << std::endl;
            if (min_sos < startsos) {
                std::cout << "This was an improvement" << std::endl;
                this->setCFromV (simp.best_vertex(), startrow);
                this->init(); // Re-setup this bezcurve
                F bestval = this->computeObjective (points);
                std::cout << "FINISHED! Best approximation:\n" << this->C << "has value " << bestval << std::endl;
            } else {
                std::cout << "Optimization failed to improve. Back to C." << std::endl;
                this->C = Copy;
                this->init(); // Re-setup this bezcurve
            }
        }

        static constexpr bool penalise_curve_length = false;
        F computeObjective (const sj::vvec<sj::vec<F, 2>>& points) const
        {
            // Compute relative positions of pairs in @points
            sj::vvec<F> sample_t;
            sample_t.push_back (F{0});
            F totaldist = F{0};
            for (std::size_t i = 1U; i < points.size(); ++i) {
                F lindist = (points[i-1]-points[i]).length();
                sample_t.push_back (lindist);
                totaldist += lindist;
            }
            sj::vvec<sj::vec<F, 2>> curvePoints;
            for (std::size_t i = 0U; i < sample_t.size(); ++i) {
                sample_t[i] /= totaldist;
                // Have the t parameter value to sample our Bezier curve at now...
                bezcoord<F> bc = this->computePoint (sample_t[i]);
                curvePoints.push_back (bc.coord);
            }
            // Can now compare points and curvePoints.
            if (curvePoints.size() != points.size()) {
                std::cout << "Can't optimize" << std::endl;
                return F{-1};
            }
            F sos = F{0};
            for (std::size_t i = 0U; i < points.size(); ++i) {
                sos += (points[i] - curvePoints[i]).length_sq();
            }

            if constexpr (penalise_curve_length == true) {
                // Add a penalty for the length of the curve, also, which should be as close
                // to the linear length from point to point.
                F clen = this->computeLength(50);
                F distpart = (clen-totaldist)*(clen-totaldist);
                std::cout << "sos part: " << sos << ", distance part: " << distpart << std::endl;
                return sos + distpart;
            } else {
                return sos;
            }
        }

        /*!
         * Using the given points, make this a best-fit Bezier curve with
         * points.size()-1 control points.
         */
        void fit (const sj::vvec<sj::vec<F, 2>>& points)
        {
            // Set the order for the curve
            int n = points.size();
            this->order = n - 1;

            // Empty C here, in advance of this->matrixSetup(), and make sure it has the
            // right size, though it can just be zeros.
            this->C = arma::Mat<F> (n, 2, arma::fill::zeros);

            // This call to matrixSetup will set up this->M (required for the fit)
            this->matrixSetup();

            int i = 0;

            // Note that you really need double precision in the matrices whilst
            // computing a Bezier best fit. If we use single precision, the fits are
            // only good up to Bezier order 4 or 5, rather than 8-10.
            arma::Mat<double> P (n, 2, arma::fill::zeros);
            for (auto p : points) {
                P(i,0) = p[0];
                P(i++,1) = p[1];
            }

            // Compute candidate t values for the points.
            i = 0;
            arma::Mat<double> D (n, 1, arma::fill::zeros);
            arma::Mat<double> S (n, 1, arma::fill::zeros);
            double total_len = F{0};
            for (i = 1; i < n; ++i) {
                double xdiff = P(i,0) - P(i-1,0);
                double ydiff = P(i,1) - P(i-1,1);
                double len = std::sqrt (xdiff*xdiff + ydiff*ydiff);
                total_len += len;
                D(i,0) = total_len;
            }
            for (i = 0; i < n; ++i) {
                S(i,0) = D(i,0) / total_len;
            }
            // S now contains the t values for the fitting.

            // Make TT matrix (T with double bar in
            // https://pomax.github.io/bezierinfo/#curvefitting) This takes each t and
            // makes one column containing all the powers of t relevant to the order
            // that we're looking for.
            arma::Mat<double> TT (n, n, arma::fill::ones);
            for (i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    double s = S(i,0);
                    TT(i, j) = std::pow (s, j);
                }
            }

            // Convert to double precision
            arma::Mat<double> Md = arma::conv_to<arma::Mat<double>>::from (this->M);

            // Magic matrix incantation to find the best set of coordinates:
            arma::Mat<double> Cd = Md.i() * (TT.t()*TT).i() * TT.t() * P;

            // Cast back to Fs
            this->C = arma::conv_to<arma::Mat<F>>::from (Cd);

            // Re-init
            this->init();
        }

        //! Obtain and return the derivative of this Bezier curve
        bezcurve<F> derivative() const
        {
            arma::Mat<F> deriv_cp(this->order, 2);
            for (unsigned int i = 0; i < this->order; ++i) {
                deriv_cp.row(i) = this->order * (this->C.row(i+1) - this->C.row(i));
            }
            return sj::bezcurve<F> (deriv_cp);
        }

        /*!
         * Return (control points for) two Bezier curves that split up this one.
         *
         * Using the matrix representation find, from this->C, a C1 and C2 that trace
         * the same trajectory.
         */
        std::pair<arma::Mat<F>, arma::Mat<F>> split (F z) const
        {
            int n = this->order + 1;
            // 'z prime':
            F zp = z-F{1};
            arma::Mat<F> C1 (n, 2, arma::fill::zeros);
            arma::Mat<F> C2 (n, 2, arma::fill::zeros);
            F sign0 = F{1};
            F sign = sign0;
            arma::Mat<F> Q (n, n, arma::fill::zeros);
            for (int i = 0; i < n; ++i) {
                sign = sign0;
                for (int j = 0; j <= i; ++j) {
                    F binom = static_cast<F>(bezcurve::binomial_lookup(i, j));
                    Q(i,j) = sign * binom * std::pow(z, j) * std::pow (zp, i-j);
                    sign = sign > F{0} ? F{-1} : F{1};
                }
                sign0 = sign0 > F{0} ? F{-1} : F{1};
            }
            C1 = Q * this->C;
            // Shift rows then flip
            for (int i = 0; i < n; ++i) {
                Q.row(i) = arma::shift (Q.row(i), (n-i-1));
            }
            C2 = arma::flipud (Q) * this->C;

            return std::make_pair(C1, C2);
        }

        /*!
         * Compute n points on the curve whose parameters, t, are equally spaced in
         * parameter space. The first point will be the start of the curve (t==0) and
         * the last point will be at the end of the curve (t==1).
         */
        std::vector<bezcoord<F>> computePoints (unsigned int n) const
        {
            std::vector<bezcoord<F>> rtn;
            for (unsigned int i = 0; i < n; ++i) {
                F t = i/static_cast<F>(n);
                rtn.push_back (this->computePoint (t));
            }
            return rtn;
        }

        /*!
         * Compute points on the curve which are distance l from each other in Cartesian
         * space. This will return 1 or more points in the vector. The last point in the
         * vector will be a nullCoordinate bezcoord which will contain the Euclidean
         * distance to the end of the curve.
         *
         * If firstl is set and non-zero, then the first point will be a Cartesian
         * distance firstl from the initial point of the curve, rather than being a
         * distance l from the initial point.
         */
        std::vector<bezcoord<F>> computePoints (F l, F firstl = F{0}) const
        {
            std::vector<bezcoord<F>> rtn;
            F t = F{0};
            bool lastnull = false;

            if (firstl > F{0}) {
                // firstl is the desired distance to the first point and, if non-zero,
                // overrides l for the first point.
                bezcoord<F> b = this->computePoint (t, firstl);
                rtn.push_back (b);
                t = b.t();
                lastnull = b.getNullCoordinate();
            }

            // This searches forward to try to find a point which is 'l' further on. If
            // at any point t exceeds 1.0, we have to break out.
            while (t < F{1} && lastnull == false) {
                bezcoord<F> b = this->computePoint (t, l);
                rtn.push_back (b);
                t = rtn.back().t();
                lastnull = b.getNullCoordinate();
            }
            return rtn;
        }

        //! Get a vector of points on the curve with horizontal spacing x.
        std::vector<bezcoord<F>> computePointsHorz (F x) const
        {
            std::vector<bezcoord<F>> rtn;
            F t = F{0};
            bool lastnull = false;
            while (t != F{1} && lastnull == false) {
                bezcoord<F> b = this->computePointBySearchHorz (t, x);
                rtn.push_back (b);
                t = rtn.back().t();
                lastnull = b.getNullCoordinate();
            }
            return rtn;
        }

        /*!
         * Compute one point on the curve, distance t along the curve from the starting
         * position with t in range [0,1]. This chooses either optimzed quartic/cubic
         * functions, or defaults to the matrix computation method.
         */
        bezcoord<F> computePoint (F t) const
        {
            switch (this->order) {
            case 1:
                return this->computePointLinear (t);
            case 2:
                return this->computePointQuadratic (t);
            case 3:
                return this->computePointCubic (t);
            default:
                // Default to matrix, as this is faster than computePointGeneral
                return this->computePointMatrix (t);
            }
        }

        //! Compute a Bezier curve of general order using the matrix method.
        bezcoord<F> computePointMatrix (F t) const
        {
            this->checkt(t);
            int mp = this->order+1;
            arma::Mat<F> T(1, mp, arma::fill::ones);// First element is one anyway
            for (int i = 1; i < mp; ++i) {
                T(i) = std::pow (t, static_cast<double>(i));
            }
            arma::Mat<F> bp = T * this->MC;
            sj::vec<F, 2> _bp = {static_cast<F>(bp(0)), static_cast<F>(bp(1))};
            return bezcoord<F> (t, _bp);
        }

        //! Compute a Bezier curve of general order using the conventional method.
        bezcoord<F> computePointGeneral (F t) const
        {
            this->checkt (t);
            F t_ = 1-t;
            sj::vec<F, 2> b;
            // x
            b[0] = std::pow(t_, this->order) * this->C(0,0);
            for(unsigned int k=1; k<this->order; k++) {
                b[0] += static_cast<F> (bezcurve::binomial_lookup(this->order, k))
                    * std::pow (t_, this->order-k) * std::pow (t, k) * this->C(k,0);
            }
            b[0] += std::pow (t, this->order) * this->C(this->order,0);

            // y
            b[1] = std::pow(t_, this->order) * this->C(0,1);
            for (unsigned int k=1; k<this->order; k++) {
                b[1] += static_cast<F> (bezcurve::binomial_lookup(this->order, k))
                    * std::pow (t_, this->order-k) * std::pow (t, k) * this->C(k,1);
            }
            b[1] += std::pow(t, this->order) * this->C(this->order,1);

            b *= this->scale;

            return bezcoord<F>(t, b);
        }

        /*!
         * Compute one point on the curve, starting at the curve point which is found
         * for parameter value t and extending a (Euclidean) distance l along the curve
         * from the starting position.
         *
         * If it is not possible, without exceeding t, to advance a distance l, then set
         * a null bezcoord and return that.
         */
        bezcoord<F> computePoint (F t, F l) const
        {
            switch (this->order) {
            case 1:
                return this->computePointLinear (t, l);
            default: // (including order 2 or 3)
                return this->computePointBySearch (t, l);
            }
        }

        //! Compute the unit tangent and unit normal at t.
        std::pair<bezcoord<F>, bezcoord<F>> computeTangentNormal (const F t) const
        {
            bezcoord<F> tang;
            if (this->C.n_rows == 2) {
                // Can't compute tangent using the derivative as derivative would be a
                // curve with a single control point. The tangent to a line is
                // simply the line:
                tang = this->computePoint (t);
            } else {
                bezcurve<F> deriv = this->derivative();
                tang = deriv.computePoint (t);
            }
            tang.normalize();
            bezcoord<F> norm = tang; // copies the parameter
            // rotate norm:
            norm.coord = {-tang.y(), tang.x()};
            return std::make_pair (tang, norm);
        }

        /*!
         * For debugging - output, as a string, the bezcoords of this curve, choosing
         * numPoints points evenly spaced in the parameter space t=[0,1].
         */
        std::string output (unsigned int numPoints) const
        {
            std::stringstream ss;
            std::vector<bezcoord<F>> points = this->computePoints (numPoints);
            typename std::vector<bezcoord<F>>::const_iterator i = points.begin();
            while (i != points.end()) {
                if (!i->isNull()) {
                    ss << i->x() << "," << i->y() << std::endl;
                }
                ++i;
            }
            return ss.str();
        }

        /*!
         * For debugging/file use. Output, as a string, the bezcoords of this curve with
         * the step size step in Cartesian space.
         */
        std::string output (F step) const
        {
            std::stringstream ss;
            std::vector<bezcoord<F>> points = this->computePoints (step);
            typename std::vector<bezcoord<F>>::const_iterator i = points.begin();
            while (i != points.end()) {
                if (!i->isNull()) {
                    ss << i->x() << "," << i->y() << std::endl;
                }
                ++i;
            }
            return ss.str();
        }

        //! Output the control points.
        std::string outputControl() const
        {
            std::stringstream ss;
            ss << this->C;
            return ss.str();
        }

        //! A setter for the scaling factor.
        void setScale (const F s)
        {
            this->scale = s;
            this->linlengthscaled = this->scale * this->linlength;
        }

        //! A setter for the length threshold.
        void setLthresh (const F l) { this->lthresh = l; }

        //! Gets the initial control point, unscaled
        sj::vec<F, 2> getInitialPointUnscaled() const
        {
            sj::vec<F, 2> ip_unscaled;
            ip_unscaled[0] = this->C(0,0);
            ip_unscaled[1] = this->C(0,1);
            return ip_unscaled;
        }

        //! Gets the final control point, unscaled
        sj::vec<F, 2> getFinalPointUnscaled() const
        {
            sj::vec<F, 2> fp_unscaled;
            fp_unscaled[0] = this->C(this->order,0);
            fp_unscaled[1] = this->C(this->order,1);
            return fp_unscaled;
        }

        //! Gets the initial control point, scaled by the factor bezcurve::scale
        sj::vec<F, 2> getInitialPointScaled() const
        {
            sj::vec<F, 2> ip_scaled;
            ip_scaled[0] = this->scale * this->C(0,0);
            ip_scaled[1] = this->scale * this->C(0,1);
            return ip_scaled;
        }

        //! Gets the final control point, scaled by the factor bezcurve::scale
        sj::vec<F, 2> getFinalPointScaled() const
        {
            sj::vec<F, 2> fp_scaled;
            fp_scaled[0] = this->scale * this->C(this->order,0);
            fp_scaled[1] = this->scale * this->C(this->order,1);
            return fp_scaled;
        }

        //! Getter for the control points in vector pair format
        sj::vvec<sj::vec<F, 2>> getControls() const {
            sj::vvec<sj::vec<F, 2>> rtn;
            for (unsigned int r = 0; r<this->C.n_rows; ++r) {
                rtn.push_back (sj::vec<F, 2>({this->C(r,0), this->C(r,1)}));
            }
            return rtn;
        }

        //! Get the order of the curve
        unsigned int getOrder() const { return this->order; }

    private:

        //! Perform common initialization tasks.
        void init()
        {
            this->order = this->C.n_rows-1;
            this->linlength = std::sqrt ((C(order,0)-C(0,0)) * (C(order,0) - C(0,0))
                                         + (C(order,1)-C(0,1)) * (C(order,1) - C(0,1)));
            this->linlengthscaled = this->scale * this->linlength;
            this->matrixSetup();
        }

        /*!
         * Set C from the vector for floats vf, which ONLY changes the rows of C from
         * startrow and on.
         */
        void setCFromV (const std::vector<F>& vf, int r)
        {
            for (std::size_t i = 0U; i < vf.size(); i+=2) {
                this->C(r,0) = vf[i];
                this->C(r,1) = vf[i+1];
                ++r;
            }
        }

        /*!
         * Compute an approximation to the distance along the curve, by computing
         * npoints and summing their linear separations.
         */
        F computeLength (unsigned int npoints) const
        {
            std::vector<bezcoord<F>> pts = this->computePoints (npoints);
            F dist = F{0};
            for (std::size_t i = 1U; i < pts.size(); ++i) {
                dist += (pts[i-1].coord - pts[i].coord).length();
            }
            return dist;
        }

        /*!
         * Compute one point on the linear curve, distance t along the curve from the
         * starting position.
         */
        bezcoord<F> computePointLinear (F t) const
        {
            this->checkt(t);
            sj::vec<F, 2> b;
            b[0] =  ((1-t) * this->C(0,0) + t * this->C(1,0)) * this->scale;
            b[1] = ((1-t) * this->C(0,1) + t * this->C(1,1)) * this->scale;
            return bezcoord<F>(t, b);
        }

        /*!
         * Compute one point on the linear curve, starting at the curve point which is
         * found for parameter value t and extending a distance l along the curve from
         * the starting position.
         *
         * The key to this is to compute a change in t from the l that you want to move
         * along the line. It's not hard to do the maths for the linear case; see
         * LinearBez1.jpg and LinearBez2.jpg for the sums.
         */
        bezcoord<F> computePointLinear (F t, F l) const
        {
            bezcoord<F> b1 = this->computePoint (t);
            bezcoord<F> e1 = this->computePoint (F{1});
            F toEnd = b1.distanceTo (e1);
            if (toEnd < l) {
                // Return null coordinate as the result and set remaining to toEnd and
                // the last param to t.
                bezcoord<F> rtn (true);
                rtn.setRemaining (toEnd);
                rtn.param = t;
                return rtn;
            }
            // Compute new t from l.
            F dt = l/this->linlengthscaled;
            t = t+dt;
            return this->computePointLinear (t);
        }

        /*!
         * Compute one point on the quadratic curve, distance t along the curve from the
         * starting position.
         */
        bezcoord<F> computePointQuadratic (F t) const
        {
            this->checkt (t);
            sj::vec<F, 2> b;
            F t_ = 1-t;
            b[0] = (t_ * t_ * this->C(0,0)
                    + 2 * t_ * t * this->C(1,0)
                    + t * t * this->C(2,0)) * this->scale;
            b[1] = (t_ * t_ * this->C(0,1)
                    + 2 * t_ * t * this->C(1,1)
                    + t * t * this->C(2,1)) * this->scale;
            return bezcoord<F>(t, b);
        }

        /*!
         * Compute one point on the cubic curve, distance t along the curve from the
         * starting position.
         */
        bezcoord<F> computePointCubic (F t) const
        {
            this->checkt (t);
            sj::vec<F, 2> b;
            F t_ = 1-t;
            b[0] = (t_ * t_ * t_ * this->C(0,0)
                    + 3 * t_ * t_ * t * this->C(1,0)
                    + 3 * t_ * t * t * this->C(2,0)
                    + t * t * t * this->C(3,0)) * this->scale;
            b[1] = (t_ * t_ * t_ * this->C(0,1)
                    + 3 * t_ * t_ * t * this->C(1,1)
                    + 3 * t_ * t * t * this->C(2,1)
                    + t * t * t * this->C(3,1)) * this->scale;
            return bezcoord<F>(t, b);
        }

        /*!
         * Look up the binomial coefficient (n,k) from sj::Pascal.
         *
         * To get the values from row n, where n starts at 0 (and ends at N-1), you step
         * along a number given by the triangle sequence (n(n+1)/2) and then read n+1
         * values. OR to get n,k, step along a number given by the triangle sequence
         * (n(n+1)/2) and then step another k space to the result.
         */
        static unsigned int binomial_lookup (unsigned int n, unsigned int k)
        {
            unsigned int idx = (n * (n+1) / 2) + k;
            return sj::Pascal[idx];
        }

        /*!
         * A computePoint starting from the point for parameter value t and going to a
         * point which is Euclidean distance l from the starting point.
         *
         * This one uses a binary search to find the next point, and works for quadratic
         * and cubic Bezier curves for which it is difficult to compute the t that would
         * give a Euclidean extension l (it would work for linear curves too).
         */
        bezcoord<F> computePointBySearch (F t, F l) const
        {
            // Min and max of possible range for dt to make a step of length l in posn space
            F dtmin = F{0};
            F dtmax = F{1} - t;

            // First guess for dt. Arb. units in parameter space.
            F dt = dtmin + (dtmax-dtmin)/F{2};

            bezcoord<F> b1 = this->computePoint (t);

            // Find distance from the initial position to the end of the
            // curve. If this is a shorter distance than l, then return.
            bezcoord<F> e1 = this->computePoint (F{1});
            F toEnd = b1.distanceTo (e1);
            if (toEnd < l) {
                // Return null coordinate as the result and set remaining to
                // toEnd and the last param to t.
                bezcoord<F> rtn (true);
                rtn.setRemaining (toEnd);
                rtn.param = t;
                return rtn;
            }

            // On every call, compute a threshold. lthresh is a percentage, so compute
            // the absolute threshold, lt as a percentage of l.
            F lt = this->lthresh * F{0.01} * l;

            // Do a binary search to find the value of dt which gives a b2 that is l
            // further on
            bezcoord<F> b2 (true);
            bool finished = false;
            while (!finished && ((t+dt) <= F{1})) {

                // Compute position of candidate point dt beyond t in param space
                b2 = this->computePoint (t+dt);
                F dl = b1.distanceTo (b2);
                if (std::abs(l-dl) < lt) {
                    // Stop here.
                    finished = true;
                } else {
                    if (dl > l) {
                        dtmax = dt;
                    } else { // dl < l
                        dtmin = dt;
                    }
                    dt = dtmin + (dtmax-dtmin)/F{2};
                }
            }

            if (!finished) {
                // Return a null coordinate
                bezcoord<F> rtn (true);
                return rtn;
            }

            return b2;
        }

        /*!
         * Like computePointsBySearch, but instead of using the Euclidean distance,
         * space points with x between them in the first coordinate - the horizonal
         * coordinate.
         */
        bezcoord<F> computePointBySearchHorz (F t, F x) const
        {
            // Min and max of possible range for dt to make a step of length l in posn space
            F dtmin = F{0};
            F dtmax = F{1} - t;

            // First guess for dt. Arb. units in parameter space.
            F dt = dtmin + (dtmax-dtmin)/F{2};

            bezcoord<F> b1 = this->computePoint (t);

            // Find distance from the initial position to the end of the curve. If this
            // is a shorter distance than l, then return.
            bezcoord<F> e1 = this->computePoint (F{1});
            F toEnd = b1.horzDistanceTo (e1);
            if (toEnd < x) {
                // Return null coordinate as the result and set remaining to toEnd and
                // the last param to t.
                bezcoord<F> rtn (true);
                rtn.setRemaining (toEnd);
                rtn.param = t;
                return rtn;
            }

            // How close we need to be to the target x for a given choice of dt.
            F lt = this->lthresh * F{0.01} * x;

            // Do a binary search to find the value of dt
            bezcoord<F> b2 (true);
            bool finished = false;
            F lastdt = F{0};
            while (!finished && ((t+dt) <= F{1}) && lastdt != dt) {

                // Compute position of candidate point dt beyound t in param space
                b2 = this->computePoint (t+dt);
                F dx = b1.horzDistanceTo (b2);

                if (std::abs(x-dx) < lt) {
                    // Stop here.
                    finished = true;
                } else {
                    if (dx > x) {
                        dtmax = dt;
                    } else { // dl < l
                        dtmin = dt;
                    }
                    lastdt = dt;
                    dt = dtmin + (dtmax-dtmin)/F{2};
                }
            }

            if (!finished) {
                // Return a null coordinate
                bezcoord<F> rtn (true);
                return rtn;
            }

            return b2;
        }

        //! Test that t is in range [0,1]. Throw exception otherwise.
        void checkt (F t) const
        {
            if (t < F{0} || t > F{1}) {
                throw std::runtime_error ("t out of range [0,1]");
            }
        }

    private:

        /*!
         * A scaling factor to convert from the SVG drawing units into mm (or
         * whatever). This is used when computing the bezcoords to output.
         */
        F scale = F{1};

        /*!
         * How close we need to be to the target l for a given choice of dt. arb. units
         * in position space (not parameter space).  This is used in computeBySearch and
         * computeBySearchHorz.
         *
         * Should be set as an acceptable percentage error in the target l. So, 1.0
         * would mean that the threshold for finding a suitable dt to advance a distance
         * l along the curve would be l/100 * 1.0.
         */
        F lthresh = F{1};

        /*!
         * The as-the-crow-flies distance from p0 to p1. Use for for BEZLINEAR to avoid
         * repeat computations. See, especially, computePointLinear (F t, F l) const
         */
        F linlength = F{0};

        /*!
         * Scaled version of linlength
         */
        F linlengthscaled = F{0};

        /*!
         * The order of the Bezier curve. The value of the highest power of t. Thus 3 is
         * a cubic Bezier, 2 is a quadratic Bezier, etc. Note that 0th order Bezier
         * curve does not exist; so the constructor must update this number.
         */
        unsigned int order = 0;

        /*
         * Matrix representation
         */

        /*!
         * Set up M and MC. Called from constructors.  A description of how to write
         * out the matrix comes from Cohen & Riesenfeld (1982) General Matrix
         * Representations.
         */
        void matrixSetup()
        {
            // Check order here
            if (this->order >= PascalRows) {
                std::stringstream ee;
                ee << "This code is limited to Bezier Curves of order " << (PascalRows-1)
                   << " by the current size of the sj::Pascal lookup table";
                throw std::runtime_error (ee.str());
            }
            if (this->order == 0) {
                throw std::runtime_error ("No curves if order=0");
            }

            // Set up M.
            int m = (int)this->order;
            int mp = m+1; // order+1
            int r = 0;
            this->M.set_size (mp, mp);
            this->M.zeros();
            for (int i = 0; i < mp; ++i) { // i is column
                for (r = 0; r < mp-i; ++r) { // r is row
                    F element = static_cast<F>(bezcurve::binomial_lookup (m, i))
                        * static_cast<F>(bezcurve::binomial_lookup (m-i, m-i-r))
                        * std::pow (F{-1}, static_cast<F>(m-i-r));
                    // Ensure the matrix is inverted 'm-i', not just 'i'
                    this->M(m-i, r) = element;
                }
            }

            // Compute M * C
            this->MC = this->M * this->C;
        }

        //! The coefficients.
        arma::Mat<F> M;

        //! The control points.
        arma::Mat<F> C;

        //! M*C
        arma::Mat<F> MC;
    };

} // namespace sj
