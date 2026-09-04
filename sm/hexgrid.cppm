// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Author: Seb James
 *
 * A hexagonal grid class. The grid can have a boundary of arbitrary shape.
 *
 * Date: 2018/07
 */
module;

#include <cstdint>
#include <set>
#include <list>
#include <string>
#include <array>
#include <stdexcept>
#include <deque>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <limits>

export module sm.hexgrid;

export import sm.mathconst;
export import sm.bezcurvepath;
export import sm.vec;
export import sm.hex;
import sm.vvec;
import sm.mat;

export namespace sm
{
    /*!
     * This class is used to build an hexagonal grid of hexagons. The member hexagons
     * are all arranged with a vertex pointing vertically - "point up". The extent of
     * the grid is determined by the x_span set during construction; the number of
     * hexes in the grid by d and x_span.
     *
     * Optionally, a boundary may be set by calling set_boundary (const
     * bezcurvepath&). If this is done, then the boundary is converted to a set of
     * hexes, then those hexes in the hexagonal grid lying outside the boundary are
     * removed.
     *
     * Another option for boundary setting is to pass in a list of hexes whose
     * positions will be used to mark out the boundary.
     *
     * This class manages the integer iterators stored in each hex (hex::vi), which
     * may be used to index into external data structures (arrays or vectors) which
     * contain information about the 2D surface represented by the hexgrid which is to
     * be computed.
     */
    template<typename F=float>
    class alignas(8) hexgrid
    {
    public:
        /*!
         * Domain attributes
         * -----------------
         *
         * Vectors containing the "domain" info extracted from the list of hexes. The
         * "domain" is the set of hexes left over after the boundary has been applied
         * and the original, outer hexes have been reduced down to those that will be
         * used in the computation.
         *
         * Each of these is prefixed d_ and is carefully aligned.
         *
         * The order in which these are populated is raster-style, from top left to
         * bottom right.
         */
        alignas(alignof(std::vector<F>)) std::vector<F> d_x;
        alignas(alignof(std::vector<F>)) std::vector<F> d_y;
        alignas(8) std::vector<std::int32_t> d_ri;
        alignas(8) std::vector<std::int32_t> d_gi;
        alignas(8) std::vector<std::int32_t> d_bi;

        /*
         * Neighbour iterators. For use when the stride to the neighbour ne or nw is
         * not constant. i.e. for use when the domain of computation is not a
         * parallelogram. Note that d_ne and d_nw ARE required, because even though
         * the neighbour east or west is always +/- 1 in memory address space in the
         * parallelogram and rectangular domain cases, if the domain is hexagonal or
         * arbitrary boundary, then even this is not true.
         */
        alignas(8) std::vector<std::int32_t> d_ne;
        alignas(8) std::vector<std::int32_t> d_nne;
        alignas(8) std::vector<std::int32_t> d_nnw;
        alignas(8) std::vector<std::int32_t> d_nw;
        alignas(8) std::vector<std::int32_t> d_nsw;
        alignas(8) std::vector<std::int32_t> d_nse;

        /*!
         * _flags, such as "on boundary", "inside boundary", "outside boundary", "has
         * neighbour east", etc.
         */
        alignas(8) std::vector<std::uint32_t> d_flags;

        /*!
         * Distance to boundary for any hex.
         */
        alignas(8) std::vector<F> d_dist_to_boundary;

        /*!
         * The length of a row in the domain. The first hex in the first row will
         * overhang to the left.
         */
        std::uint32_t d_rowlen = 0;

        /*!
         * The number of rows in the domain.
         */
        std::uint32_t d_numrows = 0;

        /*!
         * d_rowlen * d_numrows is the domain size in number of hexes. Client code
         * will create vectors of length d_size and hold the variables pertaining to
         * the hex domain therein.
         */
        std::uint32_t d_size = 0;

        /*!
         * How many additional hexes to grow out to the left and right; top and
         * bottom? Set this to a larger number if the boundary is expected to grow
         * during a simulation.
         */
        std::uint32_t d_growthbuffer_horz = 0;
        std::uint32_t d_growthbuffer_vert = 0;

        //! Add entries to all the d_ vectors for the hex pointed to by hi.
        void d_push_back (std::list<sm::hex<F>>::iterator hi)
        {
            d_x.push_back (hi->x);
            d_y.push_back (hi->y);
            d_ri.push_back (hi->ri);
            d_gi.push_back (hi->gi);
            d_bi.push_back (hi->bi);
            d_flags.push_back (hi->get_flags());
            d_dist_to_boundary.push_back (hi->dist_to_boundary);

            // record in the hex the iterator in the d_ vectors so that d_nne and friends can be set up later.
            hi->di = d_x.size()-1;
        }

        //! Once hex::di attributes have been set, populate d_nne and friends.
        void populate_d_neighbours()
        {
            // Resize d_nne and friends
            this->d_nne.resize (this->d_x.size(), 0);
            this->d_ne.resize (this->d_x.size(), 0);
            this->d_nnw.resize (this->d_x.size(), 0);
            this->d_nw.resize (this->d_x.size(), 0);
            this->d_nsw.resize (this->d_x.size(), 0);
            this->d_nse.resize (this->d_x.size(), 0);

            typename std::list<sm::hex<F>>::iterator hi = this->hexen.begin();
            while (hi != this->hexen.end()) {

                if (hi->has_ne() == true) {
                    this->d_ne[hi->di] = hi->ne->di;
                } else {
                    this->d_ne[hi->di] = -1;
                }

                if (hi->has_nne() == true) {
                    this->d_nne[hi->di] = hi->nne->di;
                } else {
                    this->d_nne[hi->di] = -1;
                }

                if (hi->has_nnw() == true) {
                    this->d_nnw[hi->di] = hi->nnw->di;
                } else {
                    this->d_nnw[hi->di] = -1;
                }

                if (hi->has_nw() == true) {
                    this->d_nw[hi->di] = hi->nw->di;
                } else {
                    this->d_nw[hi->di] = -1;
                }

                if (hi->has_nsw() == true) {
                    this->d_nsw[hi->di] = hi->nsw->di;
                } else {
                    this->d_nsw[hi->di] = -1;
                }

                if (hi->has_nse() == true) {
                    this->d_nse[hi->di] = hi->nse->di;
                } else {
                    this->d_nse[hi->di] = -1;
                }

                ++hi;
            }
        }

        //! Clear out all the d_ vectors
        void d_clear()
        {
            this->d_x.clear();
            this->d_y.clear();
            this->d_ri.clear();
            this->d_gi.clear();
            this->d_bi.clear();
            this->d_flags.clear();
        }

        /*
         * Convenience accessors for testing neighbours. The step along for neighbours on the
         * rows above/below is given by:
         *
         * Dest  | step
         * ----------------------
         * NNE   | +rowlen
         * NNW   | +rowlen - 1
         * NSW   | -rowlen
         * NSE   | -rowlen + 1
         */
        std::int32_t ne (const std::uint32_t hi) const { return this->d_ne[hi]; }
        std::int32_t has_ne (const std::uint32_t hi) const { return this->d_ne[hi] == -1 ? false : true; }

        std::int32_t nw (const std::uint32_t hi) const { return this->d_nw[hi]; }
        std::int32_t has_nw (const std::uint32_t hi) const { return this->d_nw[hi] == -1 ? false : true; }

        std::int32_t nne (const std::uint32_t hi) const { return this->d_nne[hi]; }
        std::int32_t has_nne (const std::uint32_t hi) const { return this->d_nne[hi] == -1 ? false : true; }

        std::int32_t nnw (const std::uint32_t hi) const { return this->d_nnw[hi]; }
        std::int32_t has_nnw (const std::uint32_t hi) const { return this->d_nnw[hi] == -1 ? false : true; }

        std::int32_t nse (const std::uint32_t hi) const { return this->d_nse[hi]; }
        std::int32_t has_nse (const std::uint32_t hi) const { return this->d_nse[hi] == -1 ? false : true; }

        std::int32_t nsw (const std::uint32_t hi) const { return this->d_nsw[hi]; }
        std::int32_t has_nsw (const std::uint32_t hi) const { return this->d_nsw[hi] == -1 ? false : true; }

        /*!
         * Default constructor
         */
        hexgrid(): d(1.0f), x_span(1.0f), z(0.0f) {}

        /*!
         * Construct the hexagonal hex grid with a hex to hex distance of @a d_
         * (centre to centre) and approximate diameter of @a x_span_. Set z to @a z_
         * which may be useful as an identifier if several hexgrids are being managed
         * by client code, but is not otherwise made use of.
         */
        hexgrid (F d_, F x_span_, F z_ = 0.0f) : d(d_), x_span(x_span_), z(z_)
        {
            this->v = this->d * sm::mathconst<F>::root_3_over_2;
            this->init();
        }

        /*!
         * Initialise with the passed-in parameters; a hex to hex distance of @a d_
         * (centre to centre) and approximate diameter of @a x_span_. Set z to @a z_
         * which may be useful as an identifier if several hexgrids are being managed
         * by client code, but it not otherwise made use of.
         */
        void init (F d_, F x_span_, F z_ = 0.0f)
        {
            this->d = d_;
            this->v = this->d * sm::mathconst<F>::root_3_over_2;
            this->x_span = x_span_;
            this->z = z_;
            this->init();
        }

        /*!
         * Compute the centroid of the passed in list of hexes.
         */
        sm::vec<F, 2> compute_centroid (const std::list<sm::hex<F>>& phexes)
        {
            sm::vec<F, 2> centroid = {0,0};
            for (auto h : phexes) {
                centroid[0] += h.x;
                centroid[1] += h.y;
            }
            centroid /= phexes.size();
            return centroid;
        }

        /*!
         * Find the hex in the hex grid which is closest to the x,y position given by
         * pos.
         */
        std::list<sm::hex<F>>::iterator find_hex_nearest (const sm::vec<F, 2>& pos)
        {
            typename std::list<sm::hex<F>>::iterator nearest = this->hexen.end();
            typename std::list<sm::hex<F>>::iterator hi = this->hexen.begin();
            F dist = std::numeric_limits<F>::max();
            while (hi != this->hexen.end()) {
                F dx = pos[0] - hi->x;
                F dy = pos[1] - hi->y;
                F dl = std::sqrt (dx*dx + dy*dy);
                if (dl < dist) {
                    dist = dl;
                    nearest = hi;
                }
                ++hi;
            }
            return nearest;
        }

        // If possible, get the hex at the given rgb position
        std::list<sm::hex<F>>::iterator find_hex_at (const sm::vec<std::int32_t, 3>& rgbpos)
        {
            typename std::list<sm::hex<F>>::iterator hi = this->hexen.begin(); // First hex in hexen is always 0,0,0

            // +ri is East
            std::int32_t inc = rgbpos[0] > 0 ? 1 : -1;
            for (std::int32_t ri = 0; ri != rgbpos[0] && hi != this->hexen.end(); ri+=inc) {
                if (inc > 0) {
                    hi = hi->has_ne() ? hi->ne : this->hexen.end();
                } else {
                    hi = hi->has_nw() ? hi->nw : this->hexen.end();
                }
            }

            // gi is north-east
            inc = rgbpos[1] > 0 ? 1 : -1;
            for (std::int32_t ri = 0; ri != rgbpos[1] && hi != this->hexen.end(); ri+=inc) {
                if (inc > 0) {
                    hi = hi->has_nne() ? hi->nne : this->hexen.end();
                } else {
                    hi = hi->has_nsw() ? hi->nsw : this->hexen.end();
                }
            }

            // bi is north-west
            inc = rgbpos[2] > 0 ? 1 : -1;
            for (std::int32_t ri = 0; ri != rgbpos[2] && hi != this->hexen.end(); ri+=inc) {
                if (inc > 0) {
                    hi = hi->has_nnw() ? hi->nnw : this->hexen.end();
                } else {
                    hi = hi->has_nse() ? hi->nse : this->hexen.end();
                }
            }

            return hi;
        }

        static constexpr bool debug_boundary = false;

        /*!
         * Sets boundary to match the list of hexes passed in as @a phexes. Note, that
         * unlike void set_boundary (const bezcurvepath& p), this method does not apply
         * any offset to the positions of the hexes in @a phexes.
         */
        void set_boundary (const std::list<sm::hex<F>>& phexes)
        {
            if constexpr (debug_boundary) { std::cout << __FUNCTION__ << "(const std::list<sm::hex<F>>& phexes) called\n"; }
            this->boundary_centroid = this->compute_centroid (phexes);

            typename std::list<sm::hex<F>>::iterator bpoint = this->hexen.begin();
            typename std::list<sm::hex<F>>::iterator bpi = this->hexen.begin();
            while (bpi != this->hexen.end()) {
                typename std::list<sm::hex<F>>::const_iterator ppi = phexes.begin();
                while (ppi != phexes.end()) {
                    // NB: The assumption right now is that the phexes are from the same dimension hex grid
                    // as this->hexen.
                    if (bpi->ri == ppi->ri && bpi->gi == ppi->gi) {
                        // Set h as boundary hex.
                        bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
                        bpoint = bpi;
                        break;
                    }
                    ++ppi;
                }
                ++bpi;
            }

            // Check that the boundary is contiguous.
            std::set<std::uint32_t> seen;
            typename std::list<sm::hex<F>>::iterator hi = bpoint;
            if (this->boundary_contiguous (bpoint, hi, seen) == false) {
                std::stringstream ee;
                ee << "The boundary is not a contiguous sequence of hexes.";
                throw std::runtime_error (ee.str());
            }

            this->discard_outside_boundary();
            this->populate_d_vectors();
        }

        /*!
         * Sets boundary to \a p, then runs the code to discard hexes lying outside
         * this boundary. Finishes up by calling sm::hexgrid::discard_outside.
         * The bezcurvepath's centroid may not be 0,0. If loffset has its default value
         * of true, then this method offsets the boundary so that when it is applied to
         * the hexgrid, the centroid IS (0,0). If \a loffset is false, then \a p is not
         * translated in this way.
         */
        void set_boundary (const bezcurvepath<F, 3>& p, bool loffset = true)
        {
            if constexpr (debug_boundary) { std::cout << __FUNCTION__ << "(const bezcurvepath<F, 3>& p, bool loffset = true) called\n"; }

            this->boundary = p;
            if (!this->boundary.is_null()) {
                // Compute the points on the boundary using half of the hex to hex
                // spacing as the step size. The 'false' argument does not invert the y axis.
                this->boundary.compute_points (this->d/2.0f);
                std::vector<sm::bezcoord<F>> bpoints = this->boundary.get_points();
                this->set_boundary (bpoints, loffset);
            }
        }

        /*!
         * This sets a boundary, just as sm::hexgrid::set_boundary(const
         * sm::bezcurvepath<F> p, bool offset) does but WITHOUT discarding hexes
         * outside the boundary. Also, it first clears the previous boundary flags so
         * the new ones are the only ones marked on the boundary. It does this because
         * it does not discard hexes outside the boundary or repopulate the hexgrid but
         * it draws a new boundary that can be used by client code
         */
        void set_boundary_only (const bezcurvepath<F>& p, bool loffset = true)
        {
            this->boundary = p;
            if (!this->boundary.is_null()) {
                this->boundary.compute_points (this->d/2.0f);
                std::vector<sm::bezcoord<F>> bpoints = this->boundary.get_points();
                this->set_boundary_only (bpoints, loffset);
            }
        }

        /*!
         * Sets the boundary of the hexgrid to \a bpoints, then runs the code to discard
         * hexes lying outside this boundary. Finishes up by calling
         * hexgrid::discard_outside. By default, this method translates \a bpoints so
         * that when the boundary is applied to the hexgrid, its centroid is (0,0). If
         * the default value of \a loffset is changed to false, \a bpoints is NOT
         * translated.
         */
        void set_boundary (std::vector<bezcoord<F>>& bpoints, bool loffset = true)
        {
            if constexpr (debug_boundary) { std::cout << __FUNCTION__ << "(std::vector<bezcoord<F>>& bpoints, bool loffset = true) called\n"; }

            this->boundary_centroid = sm::bezcurvepath<F>::get_centroid (bpoints);

            auto bpi = bpoints.begin();
            // conditional executed if we reset the centre
            if (loffset) {
                while (bpi != bpoints.end()) {
                    bpi->subtract (this->boundary_centroid);
                    ++bpi;
                }
                // Copy the centroid
                this->original_boundary_centroid = this->boundary_centroid;
                // Zero out the centroid, as the boundary is now centred on 0,0
                this->boundary_centroid = {0.0f, 0.0f};
                bpi = bpoints.begin();
            }

            // now proceed with centroid changed or unchanged
            typename std::list<sm::hex<F>>::iterator nearby_boundary_point = this->hexen.begin(); // i.e the hex at 0,0
            bpi = bpoints.begin();
            while (bpi != bpoints.end()) {
                nearby_boundary_point = this->set_boundary (*bpi++, nearby_boundary_point);
            }

            // Check that the boundary is contiguous.
            {
                std::set<std::uint32_t> seen;
                typename std::list<sm::hex<F>>::iterator hi = nearby_boundary_point;
                if (this->boundary_contiguous (nearby_boundary_point, hi, seen) == false) {
                    std::stringstream ee;
                    ee << "The constructed boundary is not a contiguous sequence of hexes.";
                    throw std::runtime_error (ee.str());
                }
            }

            this->discard_outside_boundary();
            this->populate_d_vectors();
        }

        /*!
         * This sets a boundary, just as
         * sm::hexgrid::set_boundary(vector<sm::bezcoord<F>& bpoints, bool offset)
         * does but WITHOUT discarding hexes outside the boundary. Also, it first clears
         * the previous boundary flags so the new ones are the only ones marked on the
         * boundary. It does this because it does not discard hexes outside the boundary
         * or repopulate the hexgrid but it draws a new boundary that can be used by
         * client code
         */
        void set_boundary_only (std::vector<bezcoord<F>>& bpoints, bool loffset)
        {
            this->boundary_centroid = sm::bezcurvepath<F>::get_centroid (bpoints);

            auto bpi = bpoints.begin();
            // conditional executed if we reset the centre
            if (loffset) {
                while (bpi != bpoints.end()) {
                    bpi->subtract (this->boundary_centroid);
                    ++bpi;
                }
                // Copy the centroid
                this->original_boundary_centroid = this->boundary_centroid;
                // Zero out the centroid, as the boundary is now centred on 0,0
                this->boundary_centroid = {0.0f, 0.0f};
                bpi = bpoints.begin();
            }

            // now proceed with centroid changed or unchanged. First: clear all boundary flags
            for (auto h : this->hexen) { h.unset_user_flag (sm::HEX_IS_BOUNDARY); }

            typename std::list<sm::hex<F>>::iterator nearby_boundary_point = this->hexen.begin(); // i.e the hex at 0,0
            bpi = bpoints.begin();
            while (bpi != bpoints.end()) {
                nearby_boundary_point = this->set_boundary (*bpi++, nearby_boundary_point);
            }

            // Check that the boundary is contiguous.
            {
                std::set<std::uint32_t> seen;
                typename std::list<sm::hex<F>>::iterator hi = nearby_boundary_point;
                if (this->boundary_contiguous (nearby_boundary_point, hi, seen) == false) {
                    std::stringstream ee;
                    ee << "The constructed boundary is not a contiguous sequence of hexes.";
                    throw std::runtime_error (ee.str());
                }
            }
        }

        /*!
         * Set all the outer hexes as being "boundary" hexes. This makes it possible
         * to create the default hexagon of hexes, then mark the outer hexes as being
         * the boundary.
         *
         * Works only on the initial hexagonal layout of hexes.
         */
        void set_boundary_on_outer_edge()
        {
            // From centre head to boundary, then mark boundary and walk
            // around the edge.
            typename std::list<sm::hex<F>>::iterator bpi = this->hexen.begin();
            while (bpi->has_nne()) { bpi = bpi->nne; }
            bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            while (bpi->has_ne()) {
                bpi = bpi->ne;
                bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            }
            while (bpi->has_nse()) {
                bpi = bpi->nse;
                bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            }
            while (bpi->has_nsw()) {
                bpi = bpi->nsw;
                bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            }
            while (bpi->has_nw()) {
                bpi = bpi->nw;
                bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            }
            while (bpi->has_nnw()) {
                bpi = bpi->nnw;
                bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            }
            while (bpi->has_nne()) {
                bpi = bpi->nne;
                bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            }
            while (bpi->has_ne() && bpi->ne->test_flags(sm::HEX_IS_BOUNDARY) == false) {
                bpi = bpi->ne;
                bpi->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            }
            // Check that the boundary is contiguous.
            std::set<std::uint32_t> seen;
            typename std::list<sm::hex<F>>::iterator hi = bpi;
            if (this->boundary_contiguous (bpi, hi, seen) == false) {
                std::stringstream ee;
                ee << "The boundary is not a contiguous sequence of hexes.";
                throw std::runtime_error (ee.str());
            }

            // _boundary IS contiguous, discard hexes outside the boundary.
            this->discard_outside_boundary();
            this->populate_d_vectors();
        }

        /*!
         * Get all the boundary hexes in a list. This assumes that a boundary has
         * already been set with one of the set_boundary() methods and so there is
         * therefore a set of hexes which are already marked as being on the boundary
         * (with the attribute hex::boundaryhex == true) Do this by going around the
         * boundary neighbour to neighbour?
         *
         * Now a getter for this->bhexen.
         */
        std::list<sm::hex<F>> get_boundary() const
        {
            typename std::list<sm::hex<F>> bhexen_concrete;
            auto hh = this->bhexen.begin();
            while (hh != this->bhexen.end()) {
                bhexen_concrete.push_back (*(*hh));
                ++hh;
            }
            return bhexen_concrete;
        }

        /*!
         * Compute a set of coordinates arranged as a rectangle
         * \param x width
         * \param y height
         * \param c centre argument so that the rectangle centre is offset from the coordinate origin
         * \return A vector of the coordinates of points on the generated rectangle
         */
        std::vector<bezcoord<F>> rectangle_compute (const F x, const F y,
                                                    const sm::vec<F, 2> c = {0.0f, 0.0f})
        {
            std::vector<sm::bezcoord<F>> bpoints;

            // Go to bottom left first
            sm::vec<F, 2> bleft = {-0.5f * x, -0.5f * y};
            sm::vec<F, 2> tright = {0.5f * x, 0.5f * y};
            bleft += c;
            tright += c;

            // 'Draw' bottom, a distance x. Divide up into about this->d/2 steps
            F step = 0.5f * this->d;
            for (F x1 = bleft[0]; x1 < tright[0]; x1 += step) {
                sm::bezcoord<F> b(sm::vec<F, 2>{x1, bleft[1]});
                bpoints.push_back (b);
            }
            // Right
            for (F y1 = bleft[1]; y1 < tright[1]; y1 += step) {
                sm::bezcoord<F> b(sm::vec<F, 2>{tright[0], y1});
                bpoints.push_back (b);
            }
            // Top
            for (F x1 = tright[0]; x1 >= bleft[0]; x1 -= step) {
                sm::bezcoord<F> b(sm::vec<F, 2>{x1, tright[1]});
                bpoints.push_back (b);
            }
            // Left
            for (F y1 = tright[1]; y1 >= bleft[1]; y1 -= step) {
                sm::bezcoord<F> b(sm::vec<F, 2>{bleft[0], y1});
                bpoints.push_back (b);
            }

            return bpoints;
        }

        /*!
         * Compute a set of coordinates arranged as a parallelogram
         * \param re Number of hexes to the E
         * \param gne Number of hexes to the NE
         * \param rw Number of hexes to the W
         * \param gsw Number of hexes to the SW
         * \param c centre argument so that the parallelogram centre is offset from the coordinate origin
         * \return A vector of the coordinates of points on the generated pgram
         */
        std::vector<bezcoord<F>> parallelogram_compute (const std::int32_t re, const std::int32_t gne,
                                                        const std::int32_t rw, const std::int32_t gsw,
                                                        const sm::vec<F, 2> c = {0.0f, 0.0f})
        {
            std::vector<sm::bezcoord<F>> bpoints;
            // Go to bottom left first
            sm::vec<F, 2> xy = {-(rw * this->d + gsw * this->d / 2.0f), -gsw * this->v};
            xy += c;

            // 'Draw' bottom
            for (std::int32_t i = 0; i < 2 * (rw + re); ++i) {
                sm::bezcoord<F> b(xy);
                bpoints.push_back (b);
                xy[0] += this->d / 2.0f;
            }
            // Right
            for (std::int32_t i = 0; i < 2 * (gsw + gne); ++i) {
                sm::bezcoord<F> b(xy);
                bpoints.push_back (b);
                xy[0] += this->d / 4.0f;
                xy[1] += this->v / 2.0f;
            }
            // Top
            for (std::int32_t i = 0; i < 2 * (rw + re); ++i) {
                sm::bezcoord<F> b(xy);
                bpoints.push_back (b);
                xy[0] -= this->d / 2.0f;
            }
            // Left
            for (std::int32_t i = 0; i < 2 * (gsw + gne); ++i) {
                sm::bezcoord<F> b(xy);
                bpoints.push_back (b);
                xy[0] -= this->d / 4.0f;
                xy[1] -= this->v / 2.0f;
            }

            return bpoints;
        }

        /*!
         * Compute a set of coordinates arranged on an ellipse
         * \param a first elliptical radius
         * \param b second elliptical radius
         * \param c centre argument so that the ellipse centre is offset from the coordinate origin
         * \return A vector of the coordinates of points on the generated ellipse
         */
        std::vector<bezcoord<F>> ellipse_compute (const F a, const F b,
                                                  const sm::vec<F, 2> c = {0.0f, 0.0f})
        {
            // Compute the points on the boundary using the parametric elliptical formula and
            // half of the hex to hex spacing as the angular step size. Return as bpoints.
            std::vector<sm::bezcoord<F>> bpoints;

            // Estimate a good delta_phi based on the larger of a and b. Compute the delta_phi
            // required to travel a fraction of one hex-to-hex distance.
            double delta_phi = 0.0;
            double dfraction = this->d / 2.0;
            if (a > b) {
                delta_phi = std::atan2 (dfraction, a);
            } else {
                delta_phi = std::atan2 (dfraction, b);
            }

            // Loop around phi, computing x and y of the elliptical boundary and filling up bpoints
            for (double phi = 0.0; phi < sm::mathconst<double>::two_pi; phi += delta_phi) {
                sm::vec<F, 2> xy_pt = {
                    static_cast<F>(a * std::cos (phi)),
                    static_cast<F>(b * std::sin (phi))
                };
                xy_pt += c;
                sm::bezcoord<F> b(xy_pt);
                bpoints.push_back (b);
            }

            return bpoints;
        }

        /*!
         * calculate perimeter of ellipse with radii \a a and \a b
         */
        F ellipse_perimeter (const F a, const F b)
        {
            double apb = static_cast<double>(a + b);
            double amb = static_cast<double>(a - b);
            double h = amb * amb / (apb * apb);
            // Compute approximation to the ellipses perimeter (7 terms)
            double sum = 1.0
            + (0.25)              * h
            + (1.0 / 64.0)        * h * h
            + (1.0 / 256.0)       * h * h * h
            + (25.0 / 16384.0)    * h * h * h * h
            + (49.0 / 65536.0)    * h * h * h * h * h
            + (441.0 / 1048576.0) * h * h * h * h * h * h;
            double p = sm::mathconst<double>::pi * apb * sum;

            return static_cast<F>(p);
        }

        /*!
         * Set the boundary to be an ellipse with the given radii parameters a and b.
         * \param a first elliptical radius
         * \param b second elliptical radius
         * \param c allows the centre of the ellipse to be offset from the coordinate origin
         * \param offset determines if boundary is recentred or remains in place
         */
        void set_elliptical_boundary (const F a, const F b,
                                      const sm::vec<F, 2> c = {0.0f, 0.0f}, bool offset=true)
        {
            std::vector<sm::bezcoord<F>> bpoints = ellipse_compute (a, b, c);
            this->set_boundary (bpoints, offset);
        }

        /*!
         * Set the boundary to be a circle with the given radius a.
         * \param a The radius of the circle
         * \param c allows the centre of the circle to be offset from the coordinate origin
         * \param offset determines if boundary is recentred or remains in place
         */
        void set_circular_boundary (const F a,
                                    const sm::vec<F, 2> c = {0.0f, 0.0f}, bool offset=true)
        {
            std::vector<sm::bezcoord<F>> bpoints = ellipse_compute (a, a, c);
            this->set_boundary (bpoints, offset);
        }

        /*!
         * Set up a rectangular boundary of width x and height y.
         */
        void set_rectangular_boundary (const F x, const F y,
                                       const sm::vec<F, 2> c = {0.0f, 0.0f}, bool offset=true)
        {
            std::vector<sm::bezcoord<F>> bpoints = rectangle_compute (x, y, c);
            this->set_boundary (bpoints, offset);
        }

        /*!
         * Set up a parallelogram boundary extending r hexes to the E and g hexes to the NE
         */
        void set_parallelogram_boundary (const std::int32_t r, const std::int32_t g,
                                         const sm::vec<F, 2> c = {0.0f, 0.0f}, bool offset=true)
        {
            std::vector<sm::bezcoord<F>> bpoints = parallelogram_compute (r, g, r, g, c);
            this->set_boundary (bpoints, offset);
        }

        /*!
         * \brief Accessor for the size of hexen.
         *
         * return The number of hexes in the grid.
         */
        std::uint32_t num() const { return this->hexen.size(); }

        /*!
         * \brief Obtain the vector index of the last hex in hexen.
         *
         * return hex::vi from the last hex in the grid.
         */
        std::uint32_t last_vector_index() const { return this->hexen.rbegin()->vi; }

        /*!
         * Output some text information about the hexgrid.
         */
        std::string output() const
        {
            std::stringstream ss;
            ss << "hex grid with " << this->hexen.size() << " hexes.\n";
            auto i = this->hexen.begin();
            F lasty = this->hexen.front().y;
            std::uint32_t rownum = 0;
            ss << "\nRow/Ring " << rownum++ << ":\n";
            while (i != this->hexen.end()) {
                if (i->y > lasty) {
                    ss << "\nRow/Ring " << rownum++ << ":\n";
                    lasty = i->y;
                }
                ss << i->output() << std::endl;
                ++i;
            }
            return ss.str();
        }

        /*!
         * Show the coordinates of the vertices of the overall hex grid generated.
         */
        std::string extent() const
        {
            std::stringstream ss;
            if (grid_reduced == false) {
                ss << "Grid vertices: \n"
                   << "           NW: (" << this->vertex_nw->x << "," << this->vertex_nw->y << ") "
                   << "      NE: (" << this->vertex_ne->x << "," << this->vertex_ne->y << ")\n"
                   << "     W: (" << this->vertex_w->x << "," << this->vertex_w->y << ") "
                   << "                              E: (" << this->vertex_e->x << "," << this->vertex_e->y << ")\n"
                   << "           SW: (" << this->vertex_sw->x << "," << this->vertex_sw->y << ") "
                   << "      SE: (" << this->vertex_se->x << "," << this->vertex_se->y << ")";
            } else {
                ss << "Initial grid vertices are no longer valid.";
            }
            return ss.str();
        }

        /*!
         * Returns the width of the hexgrid (from -x to +x)
         */
        F width() const
        {
            // {xmin, xmax, ymin, ymax, gi at xmin, gi at xmax}
            std::array<std::int32_t, 6> extents = this->find_boundary_extents();
            F xmin = this->d * F(extents[0]);
            F xmax = this->d * F(extents[1]);
            return (xmax - xmin);
        }

        /*!
         * Returns the 'depth' of the hexgrid (from -y to +y)
         */
        F depth() const
        {
            std::array<std::int32_t, 6> extents = this->find_boundary_extents();
            F ymin = this->v * F(extents[2]);
            F ymax = this->v * F(extents[3]);
            return (ymax - ymin);
        }

        /*!
         * Getter for d.
         */
        F get_d() const { return this->d; }

        /*!
         * Getter for v - vertical hex spacing.
         */
        F get_v() const { return this->v; }

        /*!
         * Get the shortest distance from the centre to the perimeter. This is the
         * "short radius".
         */
        F get_sr() const { return this->d / 2; }

        /*!
         * The distance from the centre of the hex to any of the vertices. This is the
         * "long radius".
         */
        F get_lr() const { return (this->d / sm::mathconst<F>::root_3); }

        /*!
         * The vertical distance from the centre of the hex to the "north east" vertex
         * of the hex.
         */
        F get_v_to_ne() const { return (this->d / (2.0f * sm::mathconst<F>::root_3)); }

        /*!
         * Compute and return the area of one hex in the grid. The area is that of 6
         * triangles: (1/2 LR * d / 2) * 6 // or (d*d*3)/(2 * sqrt(3)) = d * d * sqrt(3)/2
         */
        F get_hex_area() const { return (this->d * this->d * sm::mathconst<F>::root_3_over_2); }

        /*!
         * Find the minimum value of x' on the hexgrid, where x' is the x axis rotated
         * by phi degrees.
         */
        F get_x_min (F phi = 0.0f) const
        {
            F xmin = 0.0f;
            F x_ = 0.0f;
            bool first = true;
            for (auto h : this->hexen) {
                x_ = h.x * std::cos (phi) + h.y * std::sin (phi);
                if (first) {
                    xmin = x_;
                    first = false;
                }
                if (x_ < xmin) {
                    xmin = x_;
                }
            }
            return xmin;
        }

        /*!
         * Find the maximum value of x' on the hexgrid, where x' is the x axis rotated
         * by phi degrees.
         */
        F get_x_max (F phi = 0.0f) const
        {
            F xmax = 0.0f;
            F x_ = 0.0f;
            bool first = true;
            for (auto h : this->hexen) {
                x_ = h.x * std::cos (phi) + h.y * std::sin (phi);
                if (first) {
                    xmax = x_;
                    first = false;
                }
                if (x_ > xmax) {
                    xmax = x_;
                }
            }
            return xmax;
        }

        // If hexes have been transformed, then we have to store the transform matrix so that it can
        // be used by client code (such as mathplot's HexGridVisual)
        sm::mat<F, 4> tfm = sm::mat<F, 4>::identity();

        // Transform the positions of the hexes. After transforming, the domain vectors may have to be recomputed
        void transform (const sm::mat<F, 4>& tf)
        {
            this->tfm = tf;
            typename std::list<sm::hex<F>>::iterator h = this->hexen.begin();
            while (h != this->hexen.end()) {
                h->transform (this->tfm);
                ++h;
            }

            if (this->d_x.empty() == false) { this->populate_d_vectors(); }
        }

        /*!
         * Run through all the hexes and compute the distance to the nearest boundary
         * hex.
         */
        void compute_distance_to_boundary()
        {
            typename std::list<sm::hex<F>>::iterator h = this->hexen.begin();
            while (h != this->hexen.end()) {
                if (h->test_flags(sm::HEX_IS_BOUNDARY) == true) {
                    h->dist_to_boundary = 0.0f;
                } else {
                    if (h->test_flags(sm::HEX_INSIDE_BOUNDARY) == false) {
                        // Set to a dummy, negative value
                        h->dist_to_boundary = -100.0;
                    } else {
                        // Not a boundary hex, but inside boundary
                        typename std::list<sm::hex<F>>::iterator bh = this->hexen.begin();
                        while (bh != this->hexen.end()) {
                            if (bh->test_flags(sm::HEX_IS_BOUNDARY) == true) {
                                F delta = h->distance_from (*bh);
                                if (delta < h->dist_to_boundary || h->dist_to_boundary < 0.0f) {
                                    h->dist_to_boundary = delta;
                                }
                            }
                            ++bh;
                        }
                    }
                }
                ++h;
            }
        }

        /*!
         * Populate the d_* vectors
         */
        void populate_d_vectors()
        {
            // The starting hex is always the centre one.
            typename std::list<sm::hex<F>>::iterator hi = this->hexen.begin();
            // Clear the d_ vectors.
            this->d_clear();
            // Now raster through the hexes, building the d_ vectors.
            while (hi != this->hexen.end()) {
                this->d_push_back (hi);
                hi++;
            }
            // Set up the neighbour relations
            this->populate_d_neighbours();
        }

        /*!
         * Get a vector of hex pointers for all hexes that are inside/on the path
         * defined by the bezcurvepath \a p, thus this gets a 'region of hexes'. The hex
         * flags "region" and "region_boundary" are used, temporarily to mark out the
         * region. The idea is that client code will then use the vector of sm::hex<F>* to work
         * with the region however it needs to.
         *
         * The centroid of the region is placed in \a region_centroid (i.e. \a
         * region_centroid is a return argument)
         *
         * It's assumed that the bezcurvepath defines a closed region.
         *
         * If \a apply_original_boundary_centroid is true, then the region is translated by
         * the same amount that the overall boundary was translated to ensure that the
         * boundary's centroid is at 0,0.
         *
         * \return a vector of iterators to the hexes that make up the region.
         */
        std::vector<typename std::list<sm::hex<F>>::iterator> get_region (bezcurvepath<F>& p, sm::vec<F, 2>& region_centroid,
                                                                          bool apply_original_boundary_centroid = true)
        {
            p.compute_points (this->d / 2.0f);
            std::vector<sm::bezcoord<F>> bpoints = p.get_points();
            return this->get_region (bpoints, region_centroid, apply_original_boundary_centroid);
        }

        /*!
         * The overload of get_region that does all the work on a vector of coordinates
         */
        std::vector<typename std::list<sm::hex<F>>::iterator> get_region (std::vector<bezcoord<F>>& bpoints, sm::vec<F, 2>& region_centroid,
                                                                          bool apply_original_boundary_centroid = true)
        {
            // First clear all region boundary flags, as we'll be defining a new region boundary
            this->clear_region_boundary_flags();

            // Compute region centroid from bpoints
            region_centroid = sm::bezcurvepath<F>::get_centroid (bpoints);

            // A return object
            std::vector<typename std::list<sm::hex<F>>::iterator> the_region;

            if (apply_original_boundary_centroid) {
                auto bpi = bpoints.begin();
                while (bpi != bpoints.end()) {
                    bpi->subtract (this->original_boundary_centroid);
                    ++bpi;
                }

                // Subtract original_boundary_centroid from region centroid so that region centroid is translated
                region_centroid -= this->original_boundary_centroid;
            }

            // Now find the hexes on the boundary of the region
            typename std::list<sm::hex<F>>::iterator nearby_region_boundary_point = this->hexen.begin(); // i.e the hex at 0,0
            typename std::vector<sm::bezcoord<F>>::iterator bpi = bpoints.begin();
            while (bpi != bpoints.end()) {
                nearby_region_boundary_point = this->set_region_boundary (*bpi++, nearby_region_boundary_point);
            }

            // Check that the region boundary is contiguous.
            {
                std::set<std::uint32_t> seen;
                typename std::list<sm::hex<F>>::iterator hi = nearby_region_boundary_point;
                if (this->region_boundary_contiguous (nearby_region_boundary_point, hi, seen) == false) {
                    std::stringstream ee;
                    ee << "The constructed region boundary is not a contiguous sequence of hexes.";
                    return the_region;
                }
            }

            // Mark hexes inside region. Use centroid of the region.
            typename std::list<sm::hex<F>>::iterator inside_regionhex = this->find_hex_nearest (region_centroid);
            this->mark_hexes_inside (inside_regionhex, sm::HEX_IS_REGION_BOUNDARY, sm::HEX_INSIDE_REGION);

            // Populate the_region, then return it
            typename std::list<sm::hex<F>>::iterator hi = this->hexen.begin();
            while (hi != this->hexen.end()) {
                if (hi->test_flags (sm::HEX_INSIDE_REGION) == true) {
                    the_region.push_back (hi);
                }
                ++hi;
            }

            return the_region;
        }

        //! Obtain a hexagonal region of hexes around a given central hex, marked by its
        //! d_ index. This is easier than getting a properly circular region of hexes.
        std::vector<typename std::list<sm::hex<F>>::iterator> get_hexagonal_region (std::uint32_t centreindex, F radius)
        {
            std::vector<typename std::list<sm::hex<F>>::iterator> the_region;

            // Find the hex with index centreindex
            typename std::list<sm::hex<F>>::iterator sh = this->hexen.begin(); // start hex
            while (sh != this->hexen.end()) {
                if (sh->vi == centreindex) { break; }
                sh++;
            }

            // Return if we didn't find the start hex
            if (sh == this->hexen.end()) { return the_region; }

            the_region.push_back (sh);
            // For each of 6 directions, step out to collect up the hexes on the disc
            // ring by ring. For rings 2 and above, also need to fill in hexes
            // (otherwise you end up with a snowflake shaped disc)
            typename std::list<sm::hex<F>>::iterator h;
            typename std::list<sm::hex<F>>::iterator h2; // for the tangent direction
            for (std::uint16_t i = 0; i < 6; ++i) {
                h = sh;
                if (h->has_neighbour(i)) {
                    h = h->get_neighbour(i);
                    the_region.push_back (h);
                    std::int32_t j = 1;
                    std::uint16_t tangentdir = (i+4)%6;
                    while (this->d*j < radius) {
                        if (h->has_neighbour(i)) {
                            h = h->get_neighbour(i);
                            the_region.push_back (h);
                            h2 = h;
                            for (std::int32_t k = 0; k<=(j-1); ++k) {
                                // Go in tangentdir
                                if (h2->has_neighbour (tangentdir)) {
                                    h2 = h2->get_neighbour (tangentdir);
                                    the_region.push_back (h2);
                                }
                            }
                        } else {
                            break;
                        }
                        ++j;
                    }
                }
            }
            return the_region;
        }

        /*!
         * For every hex in hexen, unset the flags sm::HEX_IS_REGION_BOUNDARY and
         * sm::HEX_INSIDE_REGION
         */
        void clear_region_boundary_flags()
        {
            for (auto& hh : this->hexen) {
                hh.unset_flag (sm::HEX_IS_REGION_BOUNDARY | sm::HEX_INSIDE_REGION);
            }
        }

        /*!
         * Using this hexgrid as the domain, convolve the domain data \a data with the
         * kernel data \a kerneldata, which exists on another hexgrid, \a
         * kernelgrid. Return the result in \a result.
         */
        template<typename T>
        void convolve (const hexgrid& kernelgrid, const std::vector<T>& kerneldata, const std::vector<T>& data, std::vector<T>& result)
        {
            if (result.size() != this->hexen.size()) {
                throw std::runtime_error ("The result vector is not the same size as the hexgrid.");
            }
            if (result.size() != data.size()) {
                throw std::runtime_error ("The data vector is not the same size as the hexgrid.");
            }
            if (kernelgrid.get_d() != this->d) {
                throw std::runtime_error ("The kernel hexgrid must have same d as this hexgrid to carry out convolution.");
            }
            if (&data == &result) {
                throw std::runtime_error ("Pass in separate memory for the result.");
            }

            // For each hex in this hexgrid, compute the convolution kernel
            typename std::list<sm::hex<F>>::iterator hi = this->hexen.begin();
            for (; hi != this->hexen.end(); ++hi) {
                T sum = T{0};
                // For each kernel hex, sum up.
                for (auto kh : kernelgrid.hexen) {
                    typename std::list<sm::hex<F>>::iterator dhi = hi;
                    // Kernel hex coords r,g are: kh.ri, kh.gi, which may be (are EXPECTED to be) +ve or -ve
                    //
                    // Origin hex coords are h.ri, h.gi
                    //
                    // To get the hex whose data we want to multiply with kh's value,
                    // can go via neighbour relations, but must be prepared to take a
                    // variable path because going directly in r direction then directly
                    // in g direction could take us temporarily outside the boundary of
                    // the hexgrid.
                    std::int32_t rr = kh.ri;
                    std::int32_t gg = kh.gi;
                    bool failed = false;
                    bool finished = false;
                    //while (gg != 0 && rr != 0) {
                    while (!finished) {
                        bool moved = false;
                        // Try to move in r direction
                        if (rr > 0) {
                            if (dhi->has_ne()) {
                                dhi = dhi->ne;
                                --rr;
                                moved = true;
                            } // Didn't move in +r direction
                        } else if (rr < 0) {
                            if (dhi->has_nw()) {
                                dhi = dhi->nw;
                                ++rr;
                                moved = true;
                            } // Didn't move in -r direction
                        }
                        // Try to move in g direction
                        if (gg > 0) {
                            if (dhi->has_nne()) {
                                dhi = dhi->nne;
                                --gg;
                                moved = true;
                            } // Didn't move in +g direction
                        } else if (gg < 0) {
                            if (dhi->has_nsw()) {
                                dhi = dhi->nsw;
                                ++gg;
                                moved = true;
                            } // Didn't move in -g direction
                        }

                        if (rr == 0 && gg == 0) {
                            finished = true;
                            break;
                        }

                        if (!moved) {
                            // We're stuck; Can't move in r or g direction, so can't add a contribution
                            failed = true;
                            break;
                        }
                    }

                    if (!failed) {
                        // Can do the sum
                        sum +=  data[dhi->vi] * kerneldata[kh.vi];
                    }
                }

                result[hi->vi] = sum;
            }
        }

        /*!
         * Resampling function (monochrome) for _data with regularly spaced coordinates. g_sigma
         * should be the characteristic distance between elements in _coords.
         *
         * Here, we assume that coords are regularly spaced. If the coords are not regularly spaced,
         * then the result of this algorithm will be incorrect (with slight distortions).
         *
         * We also assume the _coords are centered wrt the hexgrid.
         */
        sm::vvec<F> resample_regular_data (const sm::vvec<F>& _data,
                                           const sm::vvec<sm::vec<F, 2>>& _coords,
                                           const F g_sigma)
        {
            std::uint32_t csz = _data.size();

            // Return data object for the resampled result
            sm::vvec<F> expr_resampled(this->num(), 0.0f);

            // Before resampling, check if all the values in _data are identical. In this case,
            // we can short-cut the resampling process.
            F i0 = _data[0];
            bool all_same = true;
            for (auto id : _data) {
                if (id != i0) {
                    all_same = false;
                    break;
                }
            }
            if (all_same) {
                // Short-cut - just set all values in the resampled data to the same as in the input data
                expr_resampled.set_from (i0);
                return expr_resampled;
            }

            sm::interval<F> drange = _data.range();

            // Copy the data and normalize
            sm::vvec<F> data = _data;
            data -= drange.min; // Shift min to zero

            auto shiftedrange = data.range();
            data /= shiftedrange.max;

            // Pass in a Gaussian for the sigma
            sm::vec<F, 2> dist_per_pix = {g_sigma, g_sigma};
            // Parameters for the Gaussian computation
            sm::vec<F, 2> params = 1.0f / (2.0f * dist_per_pix * dist_per_pix);
            sm::vec<F, 2> threesig = 3.0f * dist_per_pix;

#pragma omp parallel for // parallel on this outer loop gives best result (5.8 s vs 7 s)
            for (typename std::vector<F>::size_type xi = 0u; xi < this->d_x.size(); ++xi) {
                F expr = 0.0f;
                for (std::uint32_t i = 0; i < csz; ++i) {
                    // Get x/y pixel coords:
                    // sm::vec<std::uint32_t, 2> idx = {(i % image_pixelsz[0]), (i / image_pixelsz[0])};
                    // Get the coordinates of the pixel at index i (in hexgrid units):
                    // Distance from input pixel to output hex:
                    const F _d_x = this->d_x[xi] - _coords[i][0];
                    const F _d_y = this->d_y[xi] - _coords[i][1];
                    // Compute contributions to each hex pixel, using 2D (elliptical) Gaussian
                    if (_d_x < threesig[0] && _d_y < threesig[1]) { // Testing for distance gives slight speedup
                        expr += std::exp ( - ( (params[0] * _d_x * _d_x) + (params[1] * _d_y * _d_y) ) ) * data[i];
                    }
                }
                expr_resampled[xi] = expr;
            }

            // renormalise result
            auto errange = expr_resampled.range();
            expr_resampled -= errange.min;
            expr_resampled /= errange.span();

            // Shift back to match the range of input data
            expr_resampled *= shiftedrange.max;
            expr_resampled += drange.min;

            return expr_resampled;
        }

        /*!
         * Resampling function (monochrome).
         *
         * \param image_data (input) The monochrome image as a vvec of Fs.  The
         * image is interpreted as running from bottom left to top right. Thus, the very
         * first F in the vvec is at x=0, y=0.
         *
         * \param image_pixelwidth (input) The number of pixels that the image is wide
         * \param image_scale (input) The size that the image should be resampled to (same units as hexgrid)
         * \param image_offset (input) An offset in hexgrid units to shift the image wrt to the hexgrid's origin
         *
         * \return A new data vvec containing the resampled (and renormalised) hex pixel values
         */
        sm::vvec<F> resample_image (const sm::vvec<F>& image_data,
                                    const std::uint32_t image_pixelwidth,
                                    const sm::vec<F, 2>& image_scale,
                                    const sm::vec<F, 2>& image_offset)
        {
            std::uint32_t csz = image_data.size();
            sm::vec<std::uint32_t, 2> image_pixelsz = {image_pixelwidth, csz / image_pixelwidth};

            // Return data object for the resampled result
            sm::vvec<F> expr_resampled(this->num(), 0.0f);

            // Before resampling, check if all the values in image_data are identical. In this case,
            // we can short-cut the resampling process.
            F i0 = image_data[0];
            bool all_same = true;
            for (auto id : image_data) {
                if (id != i0) {
                    all_same = false;
                    break;
                }
            }
            if (all_same) {
                // Short-cut - just set all values in the resampled data to the same as in the input data
                expr_resampled.set_from (i0);
                return expr_resampled;
            }

            // Distance per pixel in the image. This defines the Gaussian width (sigma) for the
            // resample. Assume that the unscaled image pixels are square. Use the image width to
            // set the distance per pixel (hence divide by image_scale by image_pixelsz[*0*]).
            sm::vec<F, 2> dist_per_pix = image_scale / (image_pixelsz[0] - 1u);
            // This is an offset to centre the image wrt to the hexgrid
            sm::vec<F, 2> input_centering_offset = dist_per_pix * image_pixelsz * 0.5f;
            // Parameters for the Gaussian computation
            sm::vec<F, 2> params = 1.0f / (2.0f * dist_per_pix * dist_per_pix);
            sm::vec<F, 2> threesig = 3.0f * dist_per_pix;

#pragma omp parallel for // parallel on this outer loop gives best result (5.8 s vs 7 s)
            for (typename std::vector<F>::size_type xi = 0u; xi < this->d_x.size(); ++xi) {
                F expr = 0.0f;
                for (std::uint32_t i = 0; i < csz; ++i) {
                    // Get x/y pixel coords:
                    sm::vec<std::uint32_t, 2> idx = {(i % image_pixelsz[0]), (i / image_pixelsz[0])};
                    // Get the coordinates of the pixel at index i (in hexgrid units):
                    sm::vec<F, 2> posn = (dist_per_pix * idx) - input_centering_offset + image_offset;
                    // Distance from input pixel to output hex:
                    F _d_x = this->d_x[xi] - posn[0];
                    F _d_y = this->d_y[xi] - posn[1];
                    // Compute contributions to each hex pixel, using 2D (elliptical) Gaussian
                    if (_d_x < threesig[0] && _d_y < threesig[1]) { // Testing for distance gives slight speedup
                        expr += std::exp ( - ( (params[0] * _d_x * _d_x) + (params[1] * _d_y * _d_y) ) ) * image_data[i];
                    }
                }
                expr_resampled[xi] = expr;
            }

            expr_resampled /= expr_resampled.max(); // renormalise result
            return expr_resampled;
        }

        // Set up wrapping. This works only on parallelogram shaped domains.
        void set_parallelogram_wrap (bool on_r, bool on_g)
        {
            if (!(on_r && on_g)) {
                throw std::runtime_error ("Test single axis wrapping then remove this exception.");
            }

            // Find furthest SW hex
            bool first = true;
            std::array<F, 4> limits = {{0,0,0,0}};
            auto h = this->hexen.begin();
            typename std::list<sm::hex<F>>::iterator bl_hex = this->hexen.begin();
            while (h != this->hexen.end()) {
                if (h->test_flags(sm::HEX_IS_BOUNDARY) == true) {
                    if (first) {
                        limits = {{h->x, h->x, h->y, h->y}};
                        first = false;
                    }
                    if (h->x < limits[0] && h->y <= limits[2]) {
                        limits[0] = h->x; limits[2] = h->y;
                        bl_hex = h;
                    }
                }
                ++h;
            }
            // Find hex nearest limits. Really?
            //std::cout << "Bottom left hex is " << bl_hex->output_cart() << std::endl;

            std::int32_t count = 0;
            typename std::list<sm::hex<F>>::iterator row_start = bl_hex;
            if (on_r) {
                // go to end of each row and wrap back to the start. This may only work
                // for parallelograms, at least in an initial implementation.
                // First row
                typename std::list<sm::hex<F>>::iterator cur_hex = row_start;
                while (cur_hex->has_ne()) { cur_hex = cur_hex->ne; }
                cur_hex->set_ne(bl_hex);
                bl_hex->set_nw(cur_hex);
                //std::cout << "set E hex of " << cur_hex->output_cart() << " to " << bl_hex->output_cart() << std::endl;
                // Rest of the rows
                while (row_start->has_nne()) {
                    row_start = row_start->nne;
                    cur_hex = row_start;
                    count = 0;
                    while (cur_hex->has_ne()) {
                        cur_hex = cur_hex->ne;
                        ++count;
                    }
                    //std::cout << "set E hex of " << cur_hex->output_cart() << " to " << row_start->output_cart() << std::endl;

                    cur_hex->set_ne (row_start);
                    row_start->set_nw (cur_hex);
                }
            }

            typename std::list<sm::hex<F>>::iterator col_start = bl_hex;
            std::int32_t vcount = 0;
            if (on_g) { // scan up columns in the 'G' direction
                // First col
                typename std::list<sm::hex<F>>::iterator cur_hex = col_start;
                while (cur_hex->has_nne()) { cur_hex = cur_hex->nne; ++vcount; }
                cur_hex->set_nne (bl_hex);
                bl_hex->set_nsw (cur_hex);
                //std::cout << "Firstcol. set NE hex of " << cur_hex->output_rg() << " to " << bl_hex->output_rg() << std::endl;
                //std::cout << "Firstcol. set SW hex of " << bl_hex->output_rg() << " to " << cur_hex->output_rg() << std::endl;

                cur_hex->set_nnw(bl_hex->nw);
                bl_hex->nw->set_nse(cur_hex->ne);
                //std::cout << "Firstcol. set NW hex of " << cur_hex->output_rg() << " to " << bl_hex->nw->output_rg() << std::endl;
                //std::cout << "Firstcol. set SE hex of " << bl_hex->nw->output_rg() << " to " << cur_hex->ne->output_rg() << std::endl;

                // Rest of the rows
                for (std::int32_t i = 0; i < count; ++i) { // NB: Assumes every row the same length
                    col_start = col_start->ne;
                    cur_hex = col_start;
                    while (cur_hex->has_nne()) { cur_hex = cur_hex->nne; }

                    cur_hex->set_nne(col_start);
                    col_start->set_nsw(cur_hex);
                    //std::cout << "set NE hex of " << cur_hex->output_rg() << " to " << col_start->output_rg() << std::endl;
                    //std::cout << "set SW hex of " << col_start->output_rg() << " to " << cur_hex->output_rg() << std::endl;

                    // Also set the nnw of the current hex to be the nse of the start of the prev col
                    cur_hex->set_nnw(col_start->nw);
                    col_start->nw->set_nse(cur_hex);
                    //std::cout << "set NW hex of " << cur_hex->output_rg() << " to " << col_start->nw->output_rg() << std::endl;
                    //std::cout << "set SE hex of " << col_start->nw->output_rg() << " to " << cur_hex->output_rg() << std::endl;
                }
            }

            // Final scan across to set se neighbours of end rows and nw neighbours of start rows.
            row_start = bl_hex;
            if (on_r && on_g) {
                typename std::list<sm::hex<F>>::iterator cur_hex = row_start;
                // First row
                for (std::int32_t i = 0; i < count; ++i) { cur_hex = cur_hex->ne; }
                row_start->set_nnw(cur_hex->nne);
                cur_hex->set_nse(row_start->nsw);
                // Rest of the rows
                for (std::int32_t j = 0; j < vcount; ++j) {
                    row_start = row_start->nne;
                    cur_hex = row_start;
                    for (std::int32_t i = 0; i < count; ++i) { cur_hex = cur_hex->ne; }
                    row_start->set_nnw(cur_hex->nne);
                    cur_hex->set_nse(row_start->nsw);
                }
            }
        }

        /*!
         * The list of hexes that make up this hexgrid.
         */
        std::list<sm::hex<F>> hexen;

        /*!
         * Once boundary secured, fill this vector. Experimental - can I do parallel
         * loops with vectors of hexes? Ans: Not very well.
         */
        std::vector<sm::hex<F>*> vhexen;

        /*!
         * While determining if boundary is continuous, fill this maps container of
         * hexes.
         */
        std::list<const sm::hex<F>*> bhexen; // Not better as a separate list<sm::hex<F>>?

        /*!
         * Store the centroid of the boundary path. The centroid of a read-in
         * bezcurvepath [see void set_boundary (const bezcurvepath& p)] is subtracted
         * from each generated point on the boundary path so that the boundary once it
         * is expressed in the hexgrid will have a (2D) centroid of roughly
         * (0,0). Hence, this is usually roughly (0,0).
         */
        sm::vec<F, 2> boundary_centroid = {0.0f, 0.0f};

        /*!
         * Holds the centroid of the boundary before all points on the boundary were
         * translated so that the centroid of the boundary would be 0,0
         */
        sm::vec<F, 2> original_boundary_centroid = {0.0f, 0.0f};

    private:
        /*!
         * Initialise a grid of hexes in a hex spiral, setting neighbours as the grid
         * spirals out. This method populates hexen based on the grid parameters set
         * in d and x_span.
         */
        void init()
        {
            // Use span_x to determine how many rings out to traverse.
            F half_x = this->x_span / 2.0f;
            std::uint32_t max_ring = std::abs (std::ceil (half_x / this->d));

            // "Creating hexagonal hex grid with max_ring: " << max_ring

            // The "vector iterator" - this is an identity iterator that is added to each hex in the grid.
            std::uint32_t vi = 0;

            // Vectors of list-iterators to hexes in this->hexen. Used to keep a track of nearest
            // neighbours. I'm using vector, rather than a list as this allows fast random access of
            // elements and I'll not be inserting or erasing in the middle of the arrays.
            std::vector<typename std::list<sm::hex<F>>::iterator> prev_ring_even;
            std::vector<typename std::list<sm::hex<F>>::iterator> prev_ring_odd;

            // Swap pointers between rings.
            std::vector<typename std::list<sm::hex<F>>::iterator>* prev_ring = &prev_ring_even;
            std::vector<typename std::list<sm::hex<F>>::iterator>* next_prev_ring = &prev_ring_odd;

            // Direction iterators used in the loop for creating hexes
            std::int32_t ri = 0;
            std::int32_t gi = 0;

            // Create central "ring" first (the single hex)
            this->hexen.emplace_back (vi++, this->d, ri, gi);

            // Put central ring in the prev_ring vector:
            {
                typename std::list<sm::hex<F>>::iterator h = this->hexen.end(); --h;
                prev_ring->push_back (h);
            }

            // Now build up the rings around it, setting neighbours as we go. Each ring has 6 more hexes
            // than the previous one (except for ring 1, which has 6 instead of 1 in the centre).
            std::uint32_t num_in_ring = 6;

            // How many hops in the same direction before turning a corner?  Increases for each
            // ring. Increases by 1 in each ring.
            std::uint32_t ring_side_len = 1;

            // These are used to iterate along the six sides of the hexagonal ring that's inside, but
            // adjacent to the hexagonal ring that's under construction.
            std::int32_t walkstart = 0;
            std::int32_t walkinc = 0;
            std::int32_t walkmin = walkstart - 1;
            std::int32_t walkmax = 1;

            for (std::uint32_t ring = 1; ring <= max_ring; ++ring) {

                // Set start ri, gi. This moves up a hex and left a hex onto the start hex of the new ring.
                --ri; ++gi;

                next_prev_ring->clear();

                // Now walk around the ring, in 6 walks, that will bring us round to just before we
                // started. walkstart has the starting iterator number for the vertices of the hexagon.

                // Walk in the r direction first:
                for (std::uint32_t i = 0; i < ring_side_len; ++i) {

                    this->hexen.emplace_back (vi++, this->d, ri++, gi);
                    auto hi = this->hexen.end(); hi--;
                    auto lasthi = hi;
                    --lasthi;

                    // Set vertex
                    if (i == 0) { vertex_nw = hi; }

                    // 1. Set my W neighbour to be the previous hex in THIS ring, if possible
                    if (i > 0) {
                        hi->set_nw (lasthi);
                        // Set me (hi) as E neighbour to previous hex in the ring (lasthi):
                        lasthi->set_ne (hi);
                    }
                    // else i must be 0 in this case, we would set the SW neighbour now,
                    // but as this won't have been added to the ring, we have to leave it

                    // 2. SW neighbour
                    std::int32_t j = walkstart + static_cast<std::int32_t>(i) - 1;
                    if (j > walkmin && j < walkmax) {
                        // Set my SW neighbour:
                        hi->set_nsw ((*prev_ring)[j]);
                        // Set me as NE neighbour to those in prev_ring:
                        (*prev_ring)[j]->set_nne (hi);
                    }
                    ++j;

                    // 3. Set my SE neighbour:
                    if (j <= walkmax) {
                        hi->set_nse ((*prev_ring)[j]);
                        // Set me as NW neighbour:
                        (*prev_ring)[j]->set_nnw (hi);
                    }

                    // Put in me next_prev_ring:
                    next_prev_ring->push_back (hi);
                }
                walkstart += walkinc;
                walkmin   += walkinc;
                walkmax   += walkinc;

                // Walk in -b direction
                for (std::uint32_t i = 0; i < ring_side_len; ++i) {
                    this->hexen.emplace_back (vi++, this->d, ri++, gi--);
                    auto hi = this->hexen.end(); hi--;
                    auto lasthi = hi;
                    --lasthi;

                    // Set vertex
                    if (i == 0) { vertex_ne = hi; }

                    // 1. Set my NW neighbour to be the previous hex in THIS ring, if possible
                    if (i > 0) {
                        hi->set_nnw (lasthi);
                        // Set me as SE neighbour to previous hex in the ring:
                        lasthi->set_nse (hi);
                    } else {
                        // Set my W neighbour for the first hex in the row.
                        hi->set_nw (lasthi);
                        // Set me as E neighbour to previous hex in the ring:
                        lasthi->set_ne (hi);
                    }

                    // 2. W neighbour
                    std::int32_t j = walkstart + static_cast<std::int32_t>(i) - 1;
                    if (j > walkmin && j < walkmax) {
                        // Set my W neighbour:
                        hi->set_nw ((*prev_ring)[j]);
                        // Set me as E neighbour to those in prev_ring:
                        (*prev_ring)[j]->set_ne (hi);
                    }
                    ++j;

                    // 3. Set my SW neighbour:
                    if (j <= walkmax) {
                        hi->set_nsw ((*prev_ring)[j]);
                        // Set me as NE neighbour:
                        (*prev_ring)[j]->set_nne (hi);
                    }

                    next_prev_ring->push_back (hi);
                }
                walkstart += walkinc;
                walkmin += walkinc;
                walkmax += walkinc;

                // Walk in -g direction
                for (std::uint32_t i = 0; i < ring_side_len; ++i) {

                    this->hexen.emplace_back (vi++, this->d, ri, gi--);
                    auto hi = this->hexen.end(); hi--;
                    auto lasthi = hi;
                    --lasthi;

                    // Set vertex
                    if (i == 0) { vertex_e = hi; }

                    // 1. Set my NE neighbour to be the previous hex in THIS ring, if possible
                    if (i > 0) {
                        hi->set_nne (lasthi);
                        // Set me as SW neighbour to previous hex in the ring:
                        lasthi->set_nsw (hi);
                    } else {
                        // Set my NW neighbour for the first hex in the row.
                        hi->set_nnw (lasthi);
                        // Set me as SE neighbour to previous hex in the ring:
                        lasthi->set_nse (hi);
                    }

                    // 2. NW neighbour
                    std::int32_t j = walkstart + static_cast<std::int32_t>(i) - 1;
                    if (j > walkmin && j < walkmax) {
                        // Set my NW neighbour:
                        hi->set_nnw ((*prev_ring)[j]);
                        // Set me as SE neighbour to those in prev_ring:
                        (*prev_ring)[j]->set_nse (hi);
                    }
                    ++j;

                    // 3. Set my W neighbour:
                    if (j <= walkmax) {
                        hi->set_nw ((*prev_ring)[j]);
                        // Set me as E neighbour:
                        (*prev_ring)[j]->set_ne (hi);
                    }

                    // Put in me next_prev_ring:
                    next_prev_ring->push_back (hi);
                }
                walkstart += walkinc;
                walkmin += walkinc;
                walkmax += walkinc;

                // Walk in -r direction
                for (std::uint32_t i = 0; i < ring_side_len; ++i) {

                    this->hexen.emplace_back (vi++, this->d, ri--, gi);
                    auto hi = this->hexen.end(); hi--;
                    auto lasthi = hi;
                    --lasthi;

                    // Set vertex
                    if (i == 0) { vertex_se = hi; }

                    // 1. Set my E neighbour to be the previous hex in THIS ring, if possible
                    if (i > 0) {
                        hi->set_ne (lasthi);
                        // Set me as W neighbour to previous hex in the ring:
                        lasthi->set_nw (hi);
                    } else {
                        // Set my NE neighbour for the first hex in the row.
                        hi->set_nne (lasthi);
                        // Set me as SW neighbour to previous hex in the ring:
                        lasthi->set_nsw (hi);
                    }

                    // 2. NE neighbour:
                    std::int32_t j = walkstart + static_cast<std::int32_t>(i) - 1;
                    if (j > walkmin && j < walkmax) {
                        // Set my NE neighbour:
                        hi->set_nne ((*prev_ring)[j]);
                        // Set me as SW neighbour to those in prev_ring:
                        (*prev_ring)[j]->set_nsw (hi);
                    }
                    ++j;

                    // 3. Set my NW neighbour:
                    if (j <= walkmax) {
                        hi->set_nnw ((*prev_ring)[j]);
                        // Set me as SE neighbour:
                        (*prev_ring)[j]->set_nse (hi);
                    }

                    next_prev_ring->push_back (hi);
                }
                walkstart += walkinc;
                walkmin += walkinc;
                walkmax += walkinc;

                // Walk in b direction
                for (std::uint32_t i = 0; i < ring_side_len; ++i) {
                    this->hexen.emplace_back (vi++, this->d, ri--, gi++);
                    auto hi = this->hexen.end(); hi--;
                    auto lasthi = hi;
                    --lasthi;

                    // Set vertex
                    if (i == 0) { vertex_sw = hi; }

                    // 1. Set my SE neighbour to be the previous hex in THIS ring, if possible
                    if (i > 0) {
                        hi->set_nse (lasthi);
                        // Set me as NW neighbour to previous hex in the ring:
                        lasthi->set_nnw (hi);
                    } else { // i == 0
                        // Set my E neighbour for the first hex in the row.
                        hi->set_ne (lasthi);
                        // Set me as W neighbour to previous hex in the ring:
                        lasthi->set_nw (hi);
                    }

                    // 2. E neighbour:
                    std::int32_t j = walkstart + static_cast<std::int32_t>(i) - 1;
                    if (j > walkmin && j < walkmax) {
                        // Set my E neighbour:
                        hi->set_ne ((*prev_ring)[j]);
                        // Set me as W neighbour to those in prev_ring:
                        (*prev_ring)[j]->set_nw (hi);
                    }
                    ++j;

                    // 3. Set my NE neighbour:
                    if (j <= walkmax) {
                        hi->set_nne ((*prev_ring)[j]);
                        // Set me as SW neighbour:
                        (*prev_ring)[j]->set_nsw (hi);
                    }

                    next_prev_ring->push_back (hi);
                }
                walkstart += walkinc;
                walkmin += walkinc;
                walkmax += walkinc;

                // Walk in g direction up to almost the last hex
                for (std::uint32_t i = 0; i < ring_side_len; ++i) {

                    this->hexen.emplace_back (vi++, this->d, ri, gi++);
                    auto hi = this->hexen.end(); hi--;
                    auto lasthi = hi;
                    --lasthi;

                    // Set vertex
                    if (i == 0) { vertex_w = hi; }

                    // 1. Set my SW neighbour to be the previous hex in THIS ring, if possible
                    if (i == (ring_side_len - 1)) {
                        // Special case at end; on last g walk hex, set the NE neighbour Set my NE neighbour
                        // for the first hex in the row.
                        hi->set_nne ((*next_prev_ring)[0]); // (*next_prev_ring)[0] is an iterator to the first hex
                        // Set me as NW neighbour to previous hex in the ring:
                        (*next_prev_ring)[0]->set_nsw (hi);
                    }
                    if (i > 0) {
                        hi->set_nsw (lasthi);
                        // Set me as NE neighbour to previous hex in the ring:
                        lasthi->set_nne (hi);
                    } else {
                        // Set my SE neighbour for the first hex in the row.
                        hi->set_nse (lasthi);
                        // Set me as NW neighbour to previous hex in the ring:
                        lasthi->set_nnw (hi);
                    }

                    // 2. E neighbour:
                    std::int32_t j = walkstart + static_cast<std::int32_t>(i) - 1;
                    if (j > walkmin && j < walkmax) {
                        // Set my SE neighbour:
                        hi->set_nse ((*prev_ring)[j]);
                        // Set me as NW neighbour to those in prev_ring:
                        (*prev_ring)[j]->set_nnw (hi);
                    }
                    ++j;

                    // 3. Set my E neighbour:
                    if (j == walkmax) { // We're on the last square and need to set the East neighbour of the
                        // first hex in the last ring.
                        hi->set_ne ((*prev_ring)[0]);
                        // Set me as W neighbour:
                        (*prev_ring)[0]->set_nw (hi);

                    } else if (j < walkmax) {
                        hi->set_ne ((*prev_ring)[j]);
                        // Set me as W neighbour:
                        (*prev_ring)[j]->set_nw (hi);
                    }

                    // Put in me next_prev_ring:
                    next_prev_ring->push_back (hi);
                }
                // Should now be on the last hex.

                // Update the walking increments for finding the vertices of the hexagonal ring. These are
                // for walking around the ring *inside* the ring of hexes being created and hence note that
                // I set walkinc to num_in_ring/6 BEFORE incrementing num_in_ring by 6, below.
                walkstart = 0;
                walkinc = num_in_ring / 6;
                walkmin = walkstart - 1;
                walkmax = walkmin + 1 + walkinc;

                // Always 6 additional hexes in the next ring out
                num_in_ring += 6;

                // And ring side length goes up by 1
                ring_side_len++;

                // Swap prev_ring and next_prev_ring.
                std::vector<typename std::list<sm::hex<F>>::iterator>* tmp = prev_ring;
                prev_ring = next_prev_ring;
                next_prev_ring = tmp;
            }
            // "Finished creating " << this->hexen.size() << " hexes in " << max_ring << " rings."
        }

        /*!
         * Starting from \a start_from, and following nearest-neighbour relations, find
         * the closest hex in hexen to the coordinate point \a point, and set its
         * hex::on_boundary attribute to true.
         *
         * \return An iterator into hexgrid::hexen which refers to the closest hex to \a point.
         */
        std::list<sm::hex<F>>::iterator set_boundary (const sm::bezcoord<F>& point,
                                                      std::list<sm::hex<F>>::iterator start_from)
        {
            typename std::list<sm::hex<F>>::iterator h = this->find_hex_near_point (point, start_from);
            h->set_flag (sm::HEX_IS_BOUNDARY | sm::HEX_INSIDE_BOUNDARY);
            return h;
        }

        /*!
         * Determine whether the boundary is contiguous. Whilst doing so, populate a
         * list<sm::hex<F>> containing just the boundary hexes.
         */
        bool boundary_contiguous()
        {
            this->bhexen.clear();
            typename std::list<sm::hex<F>>::const_iterator bhi = this->hexen.begin();
            if (this->find_boundaryhex (bhi) == false) {
                // Found no boundary hex
                return false;
            }
            std::set<std::uint32_t> seen;
            typename std::list<sm::hex<F>>::const_iterator hi = bhi;
            return this->boundary_contiguous (bhi, hi, seen);
        }

        /*!
         * Determine whether the boundary is contiguous, starting from the boundary
         * hex iterator \a bhi.
         *
         * The overload with bhexes takes a list of hex pointers and populates it with
         * pointers to the hexes on the boundary.
         */
        bool boundary_contiguous (std::list<sm::hex<F>>::const_iterator bhi,
                                  std::list<sm::hex<F>>::const_iterator hi, std::set<std::uint32_t>& seen)
        {
            bool rtn = false;
            typename std::list<sm::hex<F>>::const_iterator hi_next;
            seen.insert (hi->vi);
            // Insert into the std::list of hex pointers, too
            this->bhexen.push_back (&(*hi));

            if (rtn == false && hi->has_ne() && hi->ne->test_flags(sm::HEX_IS_BOUNDARY) == true && seen.find(hi->ne->vi) == seen.end()) {
                hi_next = hi->ne;
                rtn = (this->boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nne() && hi->nne->test_flags(sm::HEX_IS_BOUNDARY) == true && seen.find(hi->nne->vi) == seen.end()) {
                hi_next = hi->nne;
                rtn = (this->boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nnw() && hi->nnw->test_flags(sm::HEX_IS_BOUNDARY) == true && seen.find(hi->nnw->vi) == seen.end()) {
                hi_next = hi->nnw;
                rtn =  (this->boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nw() && hi->nw->test_flags(sm::HEX_IS_BOUNDARY) == true && seen.find(hi->nw->vi) == seen.end()) {
                hi_next = hi->nw;
                rtn =  (this->boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nsw() && hi->nsw->test_flags(sm::HEX_IS_BOUNDARY) == true && seen.find(hi->nsw->vi) == seen.end()) {
                hi_next = hi->nsw;
                rtn =  (this->boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nse() && hi->nse->test_flags(sm::HEX_IS_BOUNDARY) == true && seen.find(hi->nse->vi) == seen.end()) {
                hi_next = hi->nse;
                rtn =  (this->boundary_contiguous (bhi, hi_next, seen));
            }

            if (rtn == false) {
                // Checked all neighbours
                if (hi == bhi) {
                    // Back at start, nowhere left to go! return true.
                    rtn = true;
                }
            }

            return rtn;
        }

        /*!
         * Set the hex closest to point as being on the region boundary. Region
         * boundaries are supposed to be temporary, so that client code can find a
         * region, extract the pointers to all the hexes in that region and store that
         * information for later use.
         */
        std::list<sm::hex<F>>::iterator set_region_boundary (const bezcoord<F>& point, std::list<sm::hex<F>>::iterator start_from)
        {
            typename std::list<sm::hex<F>>::iterator h = this->find_hex_near_point (point, start_from);
            h->set_flag (sm::HEX_IS_REGION_BOUNDARY | sm::HEX_INSIDE_REGION);
            return h;
        }

        /*!
         * Determine whether the region boundary is contiguous, starting from the
         * boundary hex iterator #bhi.
         */
        bool region_boundary_contiguous (std::list<sm::hex<F>>::const_iterator bhi,
                                         std::list<sm::hex<F>>::const_iterator hi, std::set<std::uint32_t>& seen)
        {
            bool rtn = false;
            typename std::list<sm::hex<F>>::const_iterator hi_next;
            seen.insert (hi->vi);
            // Insert into the list of hex pointers, too
            this->bhexen.push_back (&(*hi));

            if (rtn == false && hi->has_ne() && hi->ne->test_flags(sm::HEX_IS_REGION_BOUNDARY) == true && seen.find(hi->ne->vi) == seen.end()) {
                hi_next = hi->ne;
                rtn = (this->region_boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nne() && hi->nne->test_flags(sm::HEX_IS_REGION_BOUNDARY) == true && seen.find(hi->nne->vi) == seen.end()) {
                hi_next = hi->nne;
                rtn = this->region_boundary_contiguous (bhi, hi_next, seen);
            }
            if (rtn == false && hi->has_nnw() && hi->nnw->test_flags(sm::HEX_IS_REGION_BOUNDARY) == true && seen.find(hi->nnw->vi) == seen.end()) {
                hi_next = hi->nnw;
                rtn =  (this->region_boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nw() && hi->nw->test_flags(sm::HEX_IS_REGION_BOUNDARY) == true && seen.find(hi->nw->vi) == seen.end()) {
                hi_next = hi->nw;
                rtn =  (this->region_boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nsw() && hi->nsw->test_flags(sm::HEX_IS_REGION_BOUNDARY) == true && seen.find(hi->nsw->vi) == seen.end()) {
                hi_next = hi->nsw;
                rtn =  (this->region_boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nse() && hi->nse->test_flags(sm::HEX_IS_REGION_BOUNDARY) == true && seen.find(hi->nse->vi) == seen.end()) {
                hi_next = hi->nse;
                rtn =  (this->region_boundary_contiguous (bhi, hi_next, seen));
            }

            if (rtn == false) {
                // Checked all neighbours
                if (hi == bhi) { rtn = true; }
            }

            return rtn;
        }

        /*!
         * Find a hex, any hex, that's on the boundary specified by #boundary. This
         * assumes that set_boundary (const bezcurvepath&) has been called to mark the
         * hexes that lie on the boundary.
         */
        bool find_boundaryhex (std::list<sm::hex<F>>::const_iterator& hi) const
        {
            if (hi->test_flags(sm::HEX_IS_BOUNDARY) == true) {
                // No need to change the hex iterator
                return true;
            }

            if (hi->has_ne()) {
                typename std::list<sm::hex<F>>::const_iterator ci(hi->ne);
                if (this->find_boundaryhex (ci) == true) {
                    hi = ci;
                    return true;
                }
            }
            if (hi->has_nne()) {
                typename std::list<sm::hex<F>>::const_iterator ci(hi->nne);
                if (this->find_boundaryhex (ci) == true) {
                    hi = ci;
                    return true;
                }
            }
            if (hi->has_nnw()) {
                typename std::list<sm::hex<F>>::const_iterator ci(hi->nnw);
                if (this->find_boundaryhex (ci) == true) {
                    hi = ci;
                    return true;
                }
            }
            if (hi->has_nw()) {
                typename std::list<sm::hex<F>>::const_iterator ci(hi->nw);
                if (this->find_boundaryhex (ci) == true) {
                    hi = ci;
                    return true;
                }
            }
            if (hi->has_nsw()) {
                typename std::list<sm::hex<F>>::const_iterator ci(hi->nsw);
                if (this->find_boundaryhex (ci) == true) {
                    hi = ci;
                    return true;
                }
            }
            if (hi->has_nse()) {
                typename std::list<sm::hex<F>>::const_iterator ci(hi->nse);
                if (this->find_boundaryhex (ci) == true) {
                    hi = ci;
                    return true;
                }
            }

            return false;
        }

        /*!
         * Find the hex near @point, starting from start_from, which should be as close
         * as possible to point in order to reduce computation time.
         */
        std::list<sm::hex<F>>::iterator find_hex_near_point (const bezcoord<F>& point, std::list<sm::hex<F>>::iterator start_from)
        {
            bool neighbour_nearer = true;

            typename std::list<sm::hex<F>>::iterator h = start_from;
            F dmin = h->distance_from (point);
            F dcur = 0.0f;

            while (neighbour_nearer == true) {

                neighbour_nearer = false;
                if (h->has_ne() && (dcur = h->ne->distance_from (point)) < dmin) {
                    dmin = dcur;
                    h = h->ne;
                    neighbour_nearer = true;

                } else if (h->has_nne() && (dcur = h->nne->distance_from (point)) < dmin) {
                    dmin = dcur;
                    h = h->nne;
                    neighbour_nearer = true;

                } else if (h->has_nnw() && (dcur = h->nnw->distance_from (point)) < dmin) {
                    dmin = dcur;
                    h = h->nnw;
                    neighbour_nearer = true;

                } else if (h->has_nw() && (dcur = h->nw->distance_from (point)) < dmin) {
                    dmin = dcur;
                    h = h->nw;
                    neighbour_nearer = true;

                } else if (h->has_nsw() && (dcur = h->nsw->distance_from (point)) < dmin) {
                    dmin = dcur;
                    h = h->nsw;
                    neighbour_nearer = true;

                } else if (h->has_nse() && (dcur = h->nse->distance_from (point)) < dmin) {
                    dmin = dcur;
                    h = h->nse;
                    neighbour_nearer = true;
                }
            }

            return h;
        }

        /*!
         * Mark hexes as being inside the boundary given that \a hi refers to a boundary
         * hex and at least one adjacent hex to \a hi has already been marked as inside
         * the boundary (thus allowing the algorithm to know which side of the boundary
         * hex is the inside)
         *
         * \param hi list iterator to starting hex.
         *
         * By changing \a bdry_flag and \a inside_flag, it's possible to use this method
         * with region boundaries.
         */
        void mark_from_boundary (std::list<sm::hex<F>>::iterator hi,
                                 std::uint32_t bdry_flag = sm::HEX_IS_BOUNDARY,
                                 std::uint32_t inside_flag = sm::HEX_INSIDE_BOUNDARY)
        {
            this->mark_from_boundary (&(*hi), bdry_flag, inside_flag);
        }

        /*!
         * Mark hexes as being inside the boundary given that \a hi refers to a boundary
         * hex and at least one adjacent hex to \a hi has already been marked as inside
         * the boundary (thus allowing the algorithm to know which side of the boundary
         * hex is the inside)
         *
         * \param hi list iterator to a pointer to the starting hex.
         *
         * By changing \a bdry_flag and \a inside_flag, it's possible to use this method
         * with region boundaries.
         */
        void mark_from_boundary (std::list<sm::hex<F>*>::iterator hi,
                                 std::uint32_t bdry_flag = sm::HEX_IS_BOUNDARY,
                                 std::uint32_t inside_flag = sm::HEX_INSIDE_BOUNDARY)
        {
            this->mark_from_boundary ((*hi), bdry_flag, inside_flag);
        }

        /*!
         * Mark hexes as being inside the boundary given that \a hi refers to a boundary
         * hex and at least one adjacent hex to \a hi has already been marked as inside
         * the boundary (thus allowing the algorithm to know which side of the boundary
         * hex is the inside)
         *
         * \param hi pointer to the starting hex.
         *
         * By changing \a bdry_flag and \a inside_flag, it's possible to use this method
         * with region boundaries.
         */
        void mark_from_boundary (sm::hex<F>* hi,
                                 std::uint32_t bdry_flag = sm::HEX_IS_BOUNDARY,
                                 std::uint32_t inside_flag = sm::HEX_INSIDE_BOUNDARY)
        {
            // Find a marked-inside hex next to this boundary hex. This will be the first direction to mark
            // a line of inside hexes in.
            typename std::list<sm::hex<F>>::iterator first_inside = this->hexen.begin();
            std::uint16_t firsti = 0;
            for (std::uint16_t i = 0; i < 6; ++i) {
                if (hi->has_neighbour(i)
                    && hi->get_neighbour(i)->test_flags(inside_flag) == true
                    && hi->get_neighbour(i)->test_flags(bdry_flag) == false
                    ) {
                    first_inside = hi->get_neighbour(i);
                    firsti = i;
                    break;
                }
            }

            // Mark a line in the first direction
            this->mark_from_boundary_common (first_inside, firsti, bdry_flag, inside_flag);

            // For each other direction also mark lines. Count direction upwards until we hit a boundary hex:
            short diri = (firsti + 1) % 6;
            // Can debug first *count up* direction with sm::hex::neighbour_pos(diri)
            while (hi->has_neighbour(diri) && hi->get_neighbour(diri)->test_flags(bdry_flag)==false && diri != firsti) {
                first_inside = hi->get_neighbour(diri);
                this->mark_from_boundary_common (first_inside, diri, bdry_flag, inside_flag);
                diri = (diri + 1) % 6;
            }

            // Then count downwards until we hit the other boundary hex
            diri = (firsti - 1);
            if (diri < 0) { diri = 5; }
            while (hi->has_neighbour(diri) && hi->get_neighbour(diri)->test_flags(bdry_flag)==false && diri != firsti) {
                first_inside = hi->get_neighbour(diri);
                this->mark_from_boundary_common (first_inside, diri, bdry_flag, inside_flag);
                diri = (diri - 1);
                if (diri < 0) { diri = 5; }
            }
        }

        /*!
         * Common code used by mark_from_boundary()
         */
        void mark_from_boundary_common (std::list<sm::hex<F>>::iterator first_inside, std::uint16_t firsti,
                                        std::uint32_t bdry_flag = sm::HEX_IS_BOUNDARY,
                                        std::uint32_t inside_flag = sm::HEX_INSIDE_BOUNDARY)
        {
            // From the "first inside the boundary hex" head in the direction specified by firsti until a
            // boundary hex is reached.
            typename std::list<sm::hex<F>>::iterator straight = first_inside;

            while (straight->test_flags(bdry_flag) == false) {
                // Set inside_boundary true
                straight->set_flag (inside_flag);
                if (straight->has_neighbour(firsti)) {
                    straight = straight->get_neighbour (firsti);
                } else {
                    // no further neighbour in this direction
                    if (straight->test_flags(bdry_flag) == false) { break; }
                }
            }
        }

        /*!
         * Given the current boundary hex iterator, bhi and the n_recents last boundary
         * hexes in recently_seen, and assuming that bhi has had all its adjacent inside
         * hexes marked as inside_boundary, find the next boundary hex.
         *
         * \param bhi The boundary hex iterator. From this hex, find the next boundary
         * hex.
         *
         * \param recently_seen a deque containing the recently processed boundary
         * hexes. for a boundary which is always exactly one hex thick, you only need a
         * memory of the last boundary hex to keep you going in the right direction
         * around the boundary BUT if your boundary has some "double thickness"
         * sections, then you need to know a few more recent hexes to avoid looping
         * around and returning to the start!
         *
         * \param n_recents The number of hexes to record in \a recently_seen. The
         * actual number you will need depends on the "thickness" of your boundary -
         * does it have sections that are two hexes thick, or sections that are six
         * hexes thick? It also depends on the length along which the boundary may be
         * two hexes thick. In theory, if you have a boundary section two hexes thick
         * for 5 pairs, then you need to store 10 previous hexes. However, due to the
         * way that this algorithm tests hexes (always testing direction '0' which is
         * East first, then going anti-clockwise to the next direction; North-East and
         * so on), n_recents=2 appears to be sufficient for a thickness 2 boundary,
         * which is what can occur when setting a boundary using the method
         * hexgrid::set_elliptical_boundary. Boundaries that are more than thickness 2
         * shouldn't really occur, whereas a boundary with a short section of thickness
         * 2 can quite easily occur, as in set_elliptical_boundary, where insisting that
         * the boundary was strictly always only 1 hex thick would make that algorithm
         * more complex.
         *
         * \param bdry_flag The flag used to recognise a boundary hex.
         *
         * \param inside_flag The flag used to recognise a hex that is inside the
         * boundary.
         *
         * \return true if a next boundary neighbour was found, false otherwise.
         */
        bool find_next_boundary_neighbour (std::list<sm::hex<F>>::iterator& bhi,
                                           std::deque<typename std::list<sm::hex<F>>::iterator>& recently_seen,
                                           std::uint32_t n_recents = 2U,
                                           std::uint32_t bdry_flag = sm::HEX_IS_BOUNDARY,
                                           std::uint32_t inside_flag = sm::HEX_INSIDE_BOUNDARY) const
        {
            bool gotnextneighbour = false;

            // From each boundary hex, loop round all 6 neighbours until we get to a new neighbour
            for (std::uint16_t i = 0; i < 6 && gotnextneighbour == false; ++i) {

                // This is "if it's a neighbour and the neighbour is a boundary hex"
                if (bhi->has_neighbour(i) && bhi->get_neighbour(i)->test_flags(bdry_flag)) {

                    // cbhi is "candidate boundary hex iterator", now guaranteed to be a boundary hex
                    typename std::list<sm::hex<F>>::iterator cbhi = bhi->get_neighbour(i);

                    // Test if the candidate boundary hex is in the 'recently seen' deque
                    bool hex_already_seen = false;
                    for (auto rs : recently_seen) {
                        if (rs == cbhi) {
                            // This candidate hex has been recently seen. continue to next i
                            hex_already_seen = true;
                        }
                    }
                    if (hex_already_seen) { continue; }

                    std::uint16_t i_opp = ((i+3)%6);

                    // Go round each of the candidate boundary hex's neighbours (but j!=i)
                    for (std::uint16_t j = 0; j < 6; ++j) {

                        // Ignore the candidate boundary hex itself. if j==i_opp, then
                        // i's neighbour in dirn sm::hex<F>::neighbour_pos(j) is the
                        // candidate iself, continue to next i
                        if (j==i_opp) { continue; }

                        // What is this logic. If the candidate boundary hex (which is already
                        // known to be on the boundary) has a neighbour which is inside the
                        // boundary and not itself a boundary hex, then cbhi IS the next
                        // boundary hex.
                        if (cbhi->has_neighbour(j)
                            && cbhi->get_neighbour(j)->test_flags(inside_flag)==true
                            && cbhi->get_neighbour(j)->test_flags(bdry_flag)==false) {
                            recently_seen.push_back (bhi);
                            if (recently_seen.size() > n_recents) { recently_seen.pop_front(); }
                            bhi = cbhi;
                            gotnextneighbour = true;
                            break;
                        }
                    }
                }
            }

            return (gotnextneighbour);
        }

        /*!
         * Mark hexes as inside_boundary if they are inside the boundary. Starts from
         * \a hi which is assumed to already be known to refer to a hex lying inside the
         * boundary.
         */
        void mark_hexes_inside (std::list<sm::hex<F>>::iterator hi,
                                std::uint32_t bdry_flag = sm::HEX_IS_BOUNDARY,
                                std::uint32_t inside_flag = sm::HEX_INSIDE_BOUNDARY)
        {
            // Run to boundary, marking as we go
            typename std::list<sm::hex<F>>::iterator bhi(hi);
            while (bhi->test_flags (bdry_flag) == false && bhi->has_nne()) {
                bhi->set_flag (inside_flag);
                bhi = bhi->nne;
            }
            typename std::list<sm::hex<F>>::iterator bhi_start = bhi;

            // Mark from first boundary hex and across the region
            this->mark_from_boundary (bhi, bdry_flag, inside_flag);

            // a deque to hold the 'n_recents' most recently seen boundary hexes.
            std::deque<typename std::list<sm::hex<F>>::iterator> recently_seen;
            std::uint32_t n_recents = 16U; // 2 should be sufficient for boundaries with double thickness
            // sections. If problems occur, trying increasing this.
            bool gotnext = this->find_next_boundary_neighbour (bhi, recently_seen, n_recents, bdry_flag, inside_flag);
            // Loop around boundary, marking inwards in all possible directions from each boundary hex
            while (gotnext && bhi != bhi_start) {
                this->mark_from_boundary (bhi, bdry_flag, inside_flag);
                gotnext = this->find_next_boundary_neighbour (bhi, recently_seen, n_recents, bdry_flag, inside_flag);
            }
        }

        /*!
         * Recursively mark hexes to be kept if they are inside the rectangular hex
         * domain.
         */
        void mark_hexes_inside_rectangular_domain (const std::array<std::int32_t, 6>& extnts)
        {
            // Check ri,gi,bi and reduce to equivalent ri,gi,bi=0.  Use gi to determine whether outside
            // top/bottom region Add gi contribution to ri to determine whether outside left/right region

            // Is the bottom row's gi even or odd?  extnts[2] is gi for the bottom row. If it's even, then
            // we add 0.5 to all rows with even gi. If it's odd then we add 0.5 to all rows with ODD gi.
            F even_addn = 0.5f;
            F odd_addn = 0.0f;
            F addleft = 0;
            if (extnts[2]%2 == 0) {
                // bottom row has EVEN gi (extnts[2])
                even_addn = 0.0f;
                odd_addn = 0.5f;
            } else {
                // bottom row has odd gi (extnts[2])
                addleft += 0.5f;
            }

            if (std::abs(extnts[2]%2) == std::abs(extnts[4]%2)) {
                // Left most hex is on a parity-matching line to bottom line, no need to add left.
            } else {
                // Need to add left.
                if (extnts[2]%2 == 0) {
                    addleft += 1.0f;
                    // For some reason, only in this case do we addleft (and not in the case where BR is
                    // ODD and Left most hex NOT matching, which makes addleft = 0.5 + 0.5). I can't work
                    // it out.
                    this->d_rowlen += addleft;
                    this->d_size = this->d_rowlen * this->d_numrows;
                } else {
                    addleft += 0.5f;
                }
            }

            auto hi = this->hexen.begin();
            while (hi != this->hexen.end()) {

                // Here, hz is "horizontal index", made up of the ri index, half the gi index.
                //
                // plus a row-varying addition of a half (the row of hexes above is shifted right by 0.5 a
                // hex width).
                F hz = hi->ri + 0.5*(hi->gi); /*+ (hi->gi%2 ? odd_addn : even_addn)*/;
                F parityhalf = (hi->gi%2 ? odd_addn : even_addn);

                if (hz < (extnts[0] - addleft + parityhalf)) {
                    // outside
                } else if (hz > (extnts[1] + parityhalf)) {
                    // outside
                } else if (hi->gi < extnts[2]) {
                    // outside
                } else if (hi->gi > extnts[3]) {
                    // outside
                } else {
                    // inside
                    hi->set_inside_domain();
                }
                ++hi;
            }
        }

        /*!
         * Mark hexes to be kept if they are in a parallelogram domain.
         */
        void mark_hexes_inside_parallelogram_domain (const std::array<std::int32_t, 6>& extnts)
        {
            // Check ri,gi,bi and reduce to equivalent ri,gi,bi=0.  Use gi to determine whether outside
            // top/bottom region Add gi contribution to ri to determine whether outside left/right region
            auto hi = this->hexen.begin();
            while (hi != this->hexen.end()) {
                if (hi->ri < extnts[0]
                    || hi->ri > extnts[1]
                    || hi->gi < extnts[2]
                    || hi->gi > extnts[3]) {
                    // outside
                } else {
                    // inside
                    hi->set_inside_domain();
                }
                ++hi;
            }
        }

        /*!
         * Mark ALL hexes as inside the domain
         */
        void mark_all_hexes_inside_domain()
        {
            typename std::list<sm::hex<F>>::iterator hi = this->hexen.begin();
            while (hi != this->hexen.end()) {
                hi->set_inside_domain();
                hi++;
            }
        }

        /*!
         * Discard hexes in this->hexen that are outside the boundary #boundary.
         */
        void discard_outside_boundary()
        {
            // Mark those hexes inside the boundary
            typename std::list<sm::hex<F>>::iterator centroidhex = this->find_hex_nearest (this->boundary_centroid);
            this->mark_hexes_inside (centroidhex);
            // Run through and discard those hexes outside the boundary:
            auto hi = this->hexen.begin();
            while (hi != this->hexen.end()) {
                if (hi->test_flags(sm::HEX_INSIDE_BOUNDARY) == false) {
                    // When erasing a hex, I need to update the neighbours of its neighbours.
                    hi->disconnect_neighbours();
                    // Having disconnected the neighbours, erase the hex.
                    hi = this->hexen.erase (hi);
                } else {
                    ++hi;
                }
            }
            // The hex::vi indices need to be re-numbered.
            this->renumber_vector_indices();
            // Finally, do something about the hexagonal grid vertices; set this to true to mark that the
            // iterators to the outermost vertices are no longer valid and shouldn't be used.
            this->grid_reduced = true;
        }

        /*!
         * Discard hexes in this->hexen that are outside the rectangular hex domain.
         */
        void discard_outside_domain()
        {
            // Similar to discard_outside_boundary:
            auto hi = this->hexen.begin();
            while (hi != this->hexen.end()) {
                if (hi->inside_domain() == false) {
                    hi->disconnect_neighbours();
                    hi = this->hexen.erase (hi);
                } else {
                    ++hi;
                }
            }
            this->renumber_vector_indices();
            this->grid_reduced = true;
        }

        /*!
         * Find the extents of the boundary hexes. Find the ri for the left-most hex and
         * the ri for the right-most hex (elements 0 and 1 of the return array). Find
         * the gi for the top most hex and the gi for the bottom most hex. Assumes bi is
         * 0.
         *
         * Return object contains: {ri-left, ri-right, gi-bottom, gi-top, gi at ri-left,
         * gi at ri-right}
         *
         * gi at ri-left, gi at ri-right are returned so that the bottom left hex can be
         * set correctly and the entire boundary is enclosed - it's important to know if
         * the bottom line is parity-matched with the line on which the left and right
         * most boundary hexes are found.
         */
        std::array<std::int32_t, 6> find_boundary_extents() const
        {
            // Return object contains {ri-left, ri-right, gi-bottom, gi-top, gi at ri-left, gi at ri-right}
            // i.e. {xmin, xmax, ymin, ymax, gi at xmin, gi at xmax}
            std::array<std::int32_t, 6> rtn = {{0,0,0,0,0,0}};

            // Check to see if there are any boundary hexes at all.
            std::uint32_t bhcount = 0;
            for (auto h : this->hexen) { bhcount += h.test_flags(sm::HEX_IS_BOUNDARY) == true ? 1 : 0; }
            if (bhcount == 0) { return rtn; }

            // Find the furthest left and right hexes and the further up and down hexes.
            std::array<F, 4> limits = {{0,0,0,0}};
            bool first = true;
            for (auto h : this->hexen) {
                if (h.test_flags(sm::HEX_IS_BOUNDARY) == true) {
                    if (first) {
                        limits = {{h.x, h.x, h.y, h.y}};
                        first = false;
                    }
                    if (h.x < limits[0]) {
                        limits[0] = h.x;
                        rtn[4] = h.gi;
                    }
                    if (h.x > limits[1]) {
                        limits[1] = h.x;
                        rtn[5] = h.gi;
                    }
                    if (h.y < limits[2]) {
                        limits[2] = h.y;
                    }
                    if (h.y > limits[3]) {
                        limits[3] = h.y;
                    }
                }
            }

            // Now compute the ri and gi values that these xmax/xmin/ymax/ymin correspond to. THIS, if
            // nothing else, should auto-vectorise!  d_ri is the distance moved in ri direction per x, d_gi
            // is distance
            F d_ri = this->hexen.front().get_d();
            F d_gi = this->hexen.front().get_v();
            rtn[0] = static_cast<std::int32_t>(limits[0] / d_ri);
            rtn[1] = static_cast<std::int32_t>(limits[1] / d_ri);
            rtn[2] = static_cast<std::int32_t>(limits[2] / d_gi);
            rtn[3] = static_cast<std::int32_t>(limits[3] / d_gi);
            return rtn;
        }

        /*!
         * Does what it says on the tin. Re-number the hex::vi vector index in each
         * hex in the hexgrid, from the start of the list<sm::hex<F>> hexen until the end.
         */
        void renumber_vector_indices()
        {
            std::uint32_t vi = 0;
            this->vhexen.clear();
            auto hi = this->hexen.begin();
            while (hi != this->hexen.end()) {
                hi->vi = vi++;
                this->vhexen.push_back (&(*hi));
                ++hi;
            }
        }

    public:
        /*!
         * The centre to centre hex distance between adjacent members of the hex grid.
         */
        F d = 1.0f;

        /*!
         * The centre to centre hex distance between hexes on adjacent rows - the
         * 'vertical' distance.
         */
        F v = 1.0f * sm::mathconst<F>::root_3_over_2;

        /*!
         * Give the hexagonal hex grid a diameter of approximately x_span in the
         * horizontal direction, which is perpendicular to one of the edges of the
         * member hexagons.
         */
        F x_span = 10.0f;

        /*!
         * The z coordinate of this hex grid layer
         */
        F z;

        /*!
         * A boundary to apply to the initial, rectangular grid.
         */
        bezcurvepath<F> boundary;

        /*
         * hex references to the hexes on the vertices of the hexagonal
         * grid. Configured during init(). These will become invalid when a new
         * boundary is applied to the original hexagonal grid. When this occurs,
         * grid_reduced should be set false.
         */
        std::list<sm::hex<F>>::iterator vertex_e;
        std::list<sm::hex<F>>::iterator vertex_ne;
        std::list<sm::hex<F>>::iterator vertex_nw;
        std::list<sm::hex<F>>::iterator vertex_w;
        std::list<sm::hex<F>>::iterator vertex_sw;
        std::list<sm::hex<F>>::iterator vertex_se;

        /*!
         * Set true when a new boundary has been applied. This means that
         * the #vertex_e, #vertex_w, and similar iterators are no longer valid.
         */
        bool grid_reduced = false;

    };

} // namespace sm
