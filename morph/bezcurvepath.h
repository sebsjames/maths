/*!
 * \file
 * \brief Bezier curve path class (path made of Bezier curves).
 * \author Seb James
 * \date 2019-2020
 */

#pragma once

#include <morph/bezcurve.h>

#include <limits>
#include <list>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

namespace morph
{
    /*!
     * A class defining a path made up of Bezier curves. This has an
     * initial position, and then a list of curves that make up the
     * path. I've kept this very simple with all public member
     * attributes.
     */
    template <typename Flt>
    class bezcurvepath
    {
    public:
        /*!
         * The name of this bezcurvepath. This is intended to be taken
         * from the layer name of the drawing from which the path was
         * read.
         */
        std::string name = "";

        //! The initial coordinate for the bezcurvepath.
        morph::vec<Flt, 2> initialCoordinate = { Flt{0}, Flt{0} };

        //! A list of the bezcurves that make up the full bezcurvepath.
        std::list<bezcurve<Flt>> curves;

        //! A scaling factor that's used to convert the path into mm.
        Flt scale = Flt{1};

        /*!
         * This can be filled with a set of points on the path made up by the Bezier
         * curves. Do so with computePoints.
         */
        std::vector<bezcoord<Flt>> points;

        //! The unit tangents to the curve at each point
        std::vector<bezcoord<Flt>> tangents;

        //! Unit normals
        std::vector<bezcoord<Flt>> normals;

        /*!
         * A null bezcurvepath is one which has no curves. If curves
         * is empty then the bezcurvepath is null.
         */
        bool isNull() const
        {
            return this->curves.empty();
        }

        //! Reset this bezcurvepath
        void reset()
        {
            this->curves.clear();
            this->initialCoordinate.zero();
            this->scale = Flt{1};
            this->name = "";
        }

        //! Set scaling on all member Bezier curves.
        void setScale (const Flt s)
        {
            this->scale = s;
            this->initialCoordinate *= this->scale;
            typename std::list<bezcurve<Flt>>::iterator i = this->curves.begin();
            while (i != this->curves.end()) {
                i->setScale (this->scale);
                ++i;
            }
        }

        //! Add a curve to this->curves.
        void addCurve (bezcurve<Flt>& c)
        {
            if (c.getOrder() == 0) {
                std::cout << "Not adding 0th order curve." << std::endl;
            } else {
                if (this->curves.empty()) {
                    this->initialCoordinate = c.getInitialPointScaled();
                }
                this->curves.push_back (c);
            }
        }

        //! Remove a curve from this->curves.
        void removeCurve()
        {
            if (!this->curves.empty()) { this->curves.pop_back(); }
        }

        //! Output for debugging.
        void output() const
        {
            std::cout << "------ bezcurvepath ------" << std::endl;
            std::cout << "Name: " << this->name << std::endl;
            std::cout << "Initial coord: (" << this->initialCoordinate.first
                      << "," << this->initialCoordinate.second << ")" << std::endl;
            std::cout << "Number of curves: " << this->curves.size() << std::endl;
            typename std::list<bezcurve<Flt>>::const_iterator i = this->curves.begin();
            while (i != this->curves.end()) {
                std::cout << i->output(static_cast<unsigned int>(20));
                ++i;
            }
            std::cout << "------ End bezcurvepath ------" << std::endl;
        }

        /*!
         * Save to a file for debugging. Using distance step which is
         * assumed to have been pre-scaled - step is in mm, not in SVG
         * drawing units.
         */
        void save (Flt step) const
        {
            std::ofstream f;
            std::string fname = this->name + ".csv";
            f.open (fname.c_str(), std::ios::out|std::ios::trunc);
            if (f.is_open()) {
                typename std::list<bezcurve<Flt>>::const_iterator i = this->curves.begin();
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
        Flt getEndToEnd() const
        {
            // Distance from this->initialCoordinate to final point:
            if (this->curves.empty()) { return Flt{0}; }
            morph::vec<Flt, 2> cend = this->curves.back().getFinalPointScaled();
            return (cend - initialCoordinate).length();
        }

        //! Compute & return the centroid of the passed in set of positions.
        static morph::vec<Flt, 2> getCentroid (const std::vector<bezcoord<Flt>>& points)
        {
            morph::vec<Flt, 2> c = {Flt{0}, Flt{0}};
            for (const bezcoord<Flt>& i : points) {
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
         * If invertY is true, then multiply all the y values in the coordinates by
         * -1. SVG is encoded in a left hand coordinate system, so if you're going to
         * plot the bezcoord points in a right hand system, set invertY to true.
         */
        void computePoints (Flt step, bool invertY = false)
        {
            this->points.clear();
            this->tangents.clear();
            this->normals.clear();

            // First the very start point:
            bezcoord<Flt> startPt = this->curves.front().computePoint (Flt{0});
            if (invertY) {
                startPt.invertY();
            }
            this->points.push_back (startPt);

            // Make cp a complete set of points for the current curve *including
            // the point in the curve for t=0*
            std::pair<bezcoord<Flt>, bezcoord<Flt>> tn0 = this->curves.front().computeTangentNormal(Flt{0});
            this->tangents.push_back (tn0.first);
            this->normals.push_back (tn0.second);

            typename std::list<bezcurve<Flt>>::const_iterator i = this->curves.begin();
            // Don't forget to set the scaling factor in each
            // bezcurve before generating points:
            Flt firstl = Flt{0};
            while (i != this->curves.end()) {
                std::vector<bezcoord<Flt>> cp = i->computePoints (step, firstl);
                if (cp.back().isNull()) {
                    firstl = step - cp.back().getRemaining();
                    cp.pop_back();
                }
                if (invertY) {
                    typename std::vector<bezcoord<Flt>>::iterator bci = cp.begin();
                    while (bci != cp.end()) {
                        bci->invertY();
                        ++bci;
                    }
                }
                this->points.insert (this->points.end(), cp.begin(), cp.end());

                // Now compute tangents and normals
                for (bezcoord<Flt> bp : cp) {
                    std::pair<bezcoord<Flt>, bezcoord<Flt>> tn = i->computeTangentNormal(bp.t());
                    this->tangents.push_back (tn.first);
                    this->normals.push_back (tn.second);
                }
                ++i;
            }
        }

        // Getters
        std::vector<bezcoord<Flt>> getPoints() const { return this->points; }
        std::vector<bezcoord<Flt>> getTangents() const { return this->tangents; }
        std::vector<bezcoord<Flt>> getNormals() const { return this->normals; }

        /*!
         * Similar to the above, but ensure that there are @nPoints evenly spaced
         * points along the curve. @invertY has the same meaning as in the other
         * overload of this function.
         */
        void computePoints (unsigned int nPoints, bool invertY = false)
        {
            // Get end-to-end distance and compute a candidate step, then call other
            // overload.
            if (nPoints == 0) {
                std::cout << "nPoints should be >0, returning" << std::endl;
                return;
            }
            if (this->curves.empty()) {
                std::cout << "Curve is empty, returning" << std::endl;
                return;
            }

            this->points.clear();

            Flt etoe = this->getEndToEnd();
            Flt step = etoe/(nPoints-1);
            unsigned int actualPoints = 0;

            while (actualPoints != nPoints) {
                this->points.clear();
                // std::cout << "Getting points with step size " << step << std::endl;
                this->computePoints (step, invertY);
                actualPoints = this->points.size();
                if (actualPoints != nPoints) {

                    // Modify step
                    Flt steptrial = Flt{0};
                    if (actualPoints > nPoints) {
                        // Increase step size, starting with a doubling, then a half
                        // extra, then a quarter extra, etc
                        actualPoints = 0;
                        Flt stepinc = step;
                        while (actualPoints < nPoints) {
                            steptrial = step + stepinc;
                            this->points.clear();
                            this->computePoints (steptrial, invertY);
                            actualPoints = this->points.size();
                            stepinc /= 2.0f;
                        }

                        if (std::abs(step-steptrial) < std::numeric_limits<Flt>::epsilon()) {
                            std::cout << "Numeric limit reached; can't change step a small "
                                      << "enough amount to change the number of points" << std::endl;
                            return;
                        }
                        step = steptrial;

                    } else { // actualPoints < nPoints
                        // Decrease step size, starting with a halving, then a
                        // quartering until we exceed nPoints
                        actualPoints = 0;
                        Flt stepinc = step/2.0f;
                        while (actualPoints < nPoints) {
                            steptrial = step - stepinc;
                            this->points.clear();
                            this->computePoints (steptrial, invertY);
                            actualPoints = this->points.size();
                            stepinc /= 2.0f;
                        }
                        if (std::abs(step-steptrial) < std::numeric_limits<Flt>::epsilon()) {
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

} // namespace morph
