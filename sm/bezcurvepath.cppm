// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 * \brief Bezier curve path class (path made of Bezier curves).
 * \author Seb James
 * \date 2019-2020
 */
module;

#include <cstdint>
#include <limits>
#include <list>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <concepts>

export module sm.bezcurvepath;

export import sm.bezcurve;
export import sm.bezcoord;
import sm.vec;

export namespace sm
{
    /*!
     * A class defining a path made up of Bezier curves. This has an initial position,
     * and then a list of curves that make up the path.
     */
    template <typename F, std::uint32_t order = 3> requires std::is_floating_point_v<F>
    struct bezcurvepath
    {
        /*!
         * The name of this bezcurvepath. This is intended to be taken
         * from the layer name of the drawing from which the path was
         * read.
         */
        std::string name = "";

        //! The initial coordinate for the bezcurvepath.
        sm::vec<F, 2> initial_coordinate = { F{0}, F{0} };

        //! A list of the bezcurves that make up the full bezcurvepath.
        std::list<bezcurve<F, order>> curves;

        //! A scaling factor that's used to convert the path into mm.
        F scale = F{1};

        /*!
         * This can be filled with a set of points on the path made up by the Bezier
         * curves. Do so with compute_points.
         */
        std::vector<bezcoord<F>> points;

        //! The unit tangents to the curve at each point
        std::vector<bezcoord<F>> tangents;

        //! Unit normals
        std::vector<bezcoord<F>> normals;

        /*!
         * A null bezcurvepath is one which has no curves. If curves
         * is empty then the bezcurvepath is null.
         */
        bool is_null() const
        {
            return this->curves.empty();
        }

        //! Reset this bezcurvepath
        void reset()
        {
            this->curves.clear();
            this->initial_coordinate.zero();
            this->scale = F{1};
            this->name = "";
        }

        //! Set scaling on all member Bezier curves.
        void set_scale (const F s)
        {
            this->scale = s;
            this->initial_coordinate *= this->scale;
            typename std::list<bezcurve<F, order>>::iterator i = this->curves.begin();
            while (i != this->curves.end()) {
                i->set_scale (this->scale);
                ++i;
            }
        }

        //! Add a curve to this->curves.
        void add_curve (bezcurve<F, order>& c)
        {
            if (this->curves.empty()) {
                this->initial_coordinate = c.get_initial_point_scaled();
            }
            this->curves.push_back (c);
        }

        //! Remove a curve from this->curves.
        void remove_curve()
        {
            if (!this->curves.empty()) { this->curves.pop_back(); }
        }

        //! Output for debugging.
        void output() const
        {
            std::cout << "------ bezcurvepath ------" << std::endl;
            std::cout << "Name: " << this->name << std::endl;
            std::cout << "Initial coord: (" << this->initial_coordinate.first
                      << "," << this->initial_coordinate.second << ")" << std::endl;
            std::cout << "Number of curves: " << this->curves.size() << std::endl;
            typename std::list<bezcurve<F, order>>::const_iterator i = this->curves.begin();
            while (i != this->curves.end()) {
                std::cout << i->output(static_cast<std::uint32_t>(20));
                ++i;
            }
            std::cout << "------ End bezcurvepath ------" << std::endl;
        }

        /*!
         * Save to a file for debugging. Using distance step which is
         * assumed to have been pre-scaled - step is in mm, not in SVG
         * drawing units.
         */
        void save (F step) const
        {
            std::ofstream f;
            std::string fname = this->name + ".csv";
            f.open (fname.c_str(), std::ios::out|std::ios::trunc);
            if (f.is_open()) {
                typename std::list<bezcurve<F, order>>::const_iterator i = this->curves.begin();
                // Don't forget to set the scaling factor in each
                // bezcurve before generating points:
                while (i != this->curves.end()) {
                    f << i->output (step);
                    ++i;
                }
                f.close();
            }
        }

        /*!
         * Compute the as-the-crow-flies distance from the initial coordinate of this
         * bezcurvepath to the final coordinate. Uses the scale factor.
         */
        F get_end_to_end() const
        {
            // Distance from this->initial_coordinate to final point:
            if (this->curves.empty()) { return F{0}; }
            sm::vec<F, 2> cend = this->curves.back().get_final_point_scaled();
            return (cend - initial_coordinate).length();
        }

        //! Compute & return the centroid of the passed in set of positions.
        static sm::vec<F, 2> get_centroid (const std::vector<bezcoord<F>>& points)
        {
            sm::vec<F, 2> c = {F{0}, F{0}};
            for (const bezcoord<F>& i : points) {
                c += i.coord;
            }
            c /= points.size();
            return c;
        }

        /*!
         * Crunch the numbers to generate the coordinates for the path, doing the right
         * thing between curves (skipping remaining, then advancing step-remaining into
         * the next curve and so on).
         *
         * If invert_y is true, then multiply all the y values in the coordinates by
         * -1. SVG is encoded in a left hand coordinate system, so if you're going to
         * plot the bezcoord points in a right hand system, set invert_y to true.
         */
        void compute_points (F step, bool invert_y = false)
        {
            this->points.clear();
            this->tangents.clear();
            this->normals.clear();

            // First the very start point:
            bezcoord<F> start_pt = this->curves.front().compute_point (F{0});
            if (invert_y) {
                start_pt.invert_y();
            }
            this->points.push_back (start_pt);

            // Make cp a complete set of points for the current curve *including
            // the point in the curve for t=0*
            std::pair<bezcoord<F>, bezcoord<F>> tn0 = this->curves.front().compute_tangent_normal(F{0});
            this->tangents.push_back (tn0.first);
            this->normals.push_back (tn0.second);

            typename std::list<bezcurve<F, order>>::const_iterator i = this->curves.begin();
            // Don't forget to set the scaling factor in each
            // bezcurve before generating points:
            F firstl = F{0};
            while (i != this->curves.end()) {
                std::vector<bezcoord<F>> cp = i->compute_points (step, firstl);
                if (cp.back().is_null()) {
                    firstl = step - cp.back().get_remaining();
                    cp.pop_back();
                }
                if (invert_y) {
                    typename std::vector<bezcoord<F>>::iterator bci = cp.begin();
                    while (bci != cp.end()) {
                        bci->invert_y();
                        ++bci;
                    }
                }
                this->points.insert (this->points.end(), cp.begin(), cp.end());

                // Now compute tangents and normals
                for (bezcoord<F> bp : cp) {
                    std::pair<bezcoord<F>, bezcoord<F>> tn = i->compute_tangent_normal(bp.t());
                    this->tangents.push_back (tn.first);
                    this->normals.push_back (tn.second);
                }
                ++i;
            }
        }

        // Getters
        std::vector<bezcoord<F>> get_points() const { return this->points; }
        std::vector<bezcoord<F>> get_tangents() const { return this->tangents; }
        std::vector<bezcoord<F>> get_normals() const { return this->normals; }

        /*!
         * Similar to the above, but ensure that there are @n_points evenly spaced
         * points along the curve. @invert_y has the same meaning as in the other
         * overload of this function.
         */
        void compute_points (std::uint32_t n_points, bool invert_y = false)
        {
            // Get end-to-end distance and compute a candidate step, then call other
            // overload.
            if (n_points == 0) {
                std::cout << "n_points should be >0, returning" << std::endl;
                return;
            }
            if (this->curves.empty()) {
                std::cout << "Curve is empty, returning" << std::endl;
                return;
            }

            this->points.clear();

            F etoe = this->get_end_to_end();
            F step = etoe/(n_points-1);
            std::uint32_t actual_points = 0;

            while (actual_points != n_points) {
                this->points.clear();
                // std::cout << "Getting points with step size " << step << std::endl;
                this->compute_points (step, invert_y);
                actual_points = this->points.size();
                if (actual_points != n_points) {

                    // Modify step
                    F steptrial = F{0};
                    if (actual_points > n_points) {
                        // Increase step size, starting with a doubling, then a half
                        // extra, then a quarter extra, etc
                        actual_points = 0;
                        F stepinc = step;
                        while (actual_points < n_points) {
                            steptrial = step + stepinc;
                            this->points.clear();
                            this->compute_points (steptrial, invert_y);
                            actual_points = this->points.size();
                            stepinc /= 2.0f;
                        }

                        if (std::abs(step - steptrial) < std::numeric_limits<F>::epsilon()) {
                            std::cout << "Numeric limit reached; can't change step a small "
                                      << "enough amount to change the number of points" << std::endl;
                            return;
                        }
                        step = steptrial;

                    } else { // actual_points < n_points
                        // Decrease step size, starting with a halving, then a
                        // quartering until we exceed n_points
                        actual_points = 0;
                        F stepinc = step/2.0f;
                        while (actual_points < n_points) {
                            steptrial = step - stepinc;
                            this->points.clear();
                            this->compute_points (steptrial, invert_y);
                            actual_points = this->points.size();
                            stepinc /= 2.0f;
                        }
                        if (std::abs(step - steptrial) < std::numeric_limits<F>::epsilon()) {
                            std::cout << "Numeric limit reached; can't change step a small "
                                      << "enough amount to change the number of points" << std::endl;
                            return;
                        }
                        step = steptrial;
                    }
                }
            }
        }
    };

} // namespace sm
