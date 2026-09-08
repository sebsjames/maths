// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * Defines a class to manage a hexagon which lives in a grid of hexagons.
 *
 * \author: Seb James
 * \date: 2018/07
 */
module;

#include <cstdint>
#include <string>
#include <list>
#include <array>
#include <cmath>
#include <type_traits>

export module sm.hex;

export import sm.mathconst;
export import sm.bezcoord;
export import sm.vec;
import sm.mat;

export namespace sm
{
    //! A tesselation of hexagons may be arranged so that a vertices 'point up (and down)' or so
    //! that a 'flat is up' (and a vertex points to the right/left)
    enum class hexalign { point_up, flat_up };

    /*!
     * Set true when ne has been set. Use of iterators (hex::ne etc) rather than pointers for
     * neighbouring hexes means we can't do any kind of check to see if the iterator is valid, so we
     * have to keep separate boolean flags for whether or not each hex has a neighbour. Those flags
     * are kept in hex::flags.
     */
    constexpr std::uint32_t HEX_HAS_NE              = 0x1;
    //! True when this hex has a Neighbour to the North East
    constexpr std::uint32_t HEX_HAS_NNE             = 0x2;
    //! True when this hex has a Neighbour to the North
    constexpr std::uint32_t HEX_HAS_NN              = 0x4;
    //! True when this hex has a Neighbour to the North West
    constexpr std::uint32_t HEX_HAS_NNW             = 0x8;
    //! True when this hex has a Neighbour to the West
    constexpr std::uint32_t HEX_HAS_NW              = 0x10;
    //! True when this hex has a Neighbour to the South West
    constexpr std::uint32_t HEX_HAS_NSW             = 0x20;
    //! True when this hex has a Neighbour to the South
    constexpr std::uint32_t HEX_HAS_NS              = 0x40;
    //! True when this hex has a Neighbour to the South East
    constexpr std::uint32_t HEX_HAS_NSE             = 0x80;

    //! All hexes marked as boundary hexes, including some that are additional to requirements:
    constexpr std::uint32_t HEX_IS_BOUNDARY         = 0x100;
    //! All hexes inside boundary plus as much of the boundary as needed to make a contiguous boundary:
    constexpr std::uint32_t HEX_INSIDE_BOUNDARY     = 0x200;
    //! All hexes inside the domain of computation:
    constexpr std::uint32_t HEX_INSIDE_DOMAIN       = 0x400;
    //! hex is a 'region boundary hex'. Regions are intended to be temporary to aid client code.
    constexpr std::uint32_t HEX_IS_REGION_BOUNDARY  = 0x800;
    //! hex is inside the region
    constexpr std::uint32_t HEX_INSIDE_REGION       = 0x1000;

    //! Unused flags pad the 'hexgrid' flags and the user flags
    constexpr std::uint32_t HEX_UNUSED_FLAG_0 = 0x2000;
    constexpr std::uint32_t HEX_UNUSED_FLAG_1 = 0x4000;
    constexpr std::uint32_t HEX_UNUSED_FLAG_2 = 0x8000;

    //! Sixteen flags for client code to use for its own devices.
    constexpr std::uint32_t HEX_USER_FLAG_0   = 0x00010000;
    constexpr std::uint32_t HEX_USER_FLAG_1   = 0x00020000;
    constexpr std::uint32_t HEX_USER_FLAG_2   = 0x00040000;
    constexpr std::uint32_t HEX_USER_FLAG_3   = 0x00080000;
    constexpr std::uint32_t HEX_USER_FLAG_4   = 0x00100000;
    constexpr std::uint32_t HEX_USER_FLAG_5   = 0x00200000;
    constexpr std::uint32_t HEX_USER_FLAG_6   = 0x00400000;
    constexpr std::uint32_t HEX_USER_FLAG_7   = 0x00800000;
    constexpr std::uint32_t HEX_USER_FLAG_8   = 0x01000000;
    constexpr std::uint32_t HEX_USER_FLAG_9   = 0x02000000;
    constexpr std::uint32_t HEX_USER_FLAG_10  = 0x04000000;
    constexpr std::uint32_t HEX_USER_FLAG_11  = 0x08000000;
    constexpr std::uint32_t HEX_USER_FLAG_12  = 0x10000000;
    constexpr std::uint32_t HEX_USER_FLAG_13  = 0x20000000;
    constexpr std::uint32_t HEX_USER_FLAG_14  = 0x40000000;
    constexpr std::uint32_t HEX_USER_FLAG_15  = 0x80000000;
    //! Four bits high: all user flags set
    constexpr std::uint32_t HEX_ALL_USER      = 0xffff0000;
    //! Bitmask for all the flags that aren't the 16 user flags.
    constexpr std::uint32_t HEX_NON_USER      = 0x0000ffff;

    /*!
     *
     * IF \tparam A is hexalign::point_up:
     *
     * Describes a regular hexagon arranged with vertices pointing vertically and two flat sides
     * perpendicular to the horizontal axis:
     *\code{.unparsed}
     *            *
     *         *     *
     *         *     *
     *            *
     *\endcode
     * The centre of the hex in a Cartesian right hand coordinate system is represented with x, y
     * and z:
     *\code{.unparsed}
     *  y
     *  ^
     *  |
     *  |
     *  0-----> x     z out of screen/page
     *\endcode
     * Directions are "r" "g" and "b" and their negatives:
     *\code{.unparsed}
     *         b  * g
     * -r <--  *     * ---> r
     *         *     *
     *         -g * -b
     *\endcode
     *
     * I've defined numbering for the hex's vertices and for its edges.
     *
     * Vertices: NE: 0, N: 1, NW: 2, SW: 3, S: 4, SE: 5.
     *
     * Edges/Sides: East: 0, North-East: 1, North-West: 2 West: 3, South-West: 4, South-East: 5
     *
     * ELSE, we have 'flat_up', which (with the Cartesian right hand coordinate system unchanged) means:
     *
     *\code{.unparsed}
     *    *   *
     *   *     *
     *    *   *
     *\endcode
     *
     * Directions r, g and b are now 'North east', 'North' and 'North West'.
     *
     *\code{.unparsed}
     *      g
     *  b *   * r
     *   *     *
     * -r *   * -b
     *     -g
     *\endcode
     *
     * That is, r, g and 6 are rotated pi/6 wrt r, g and b in the point_up case.
     *
     * Vertex and edge/side numbers are different (these are mostly used internally):
     *
     * Vertices: NE: 0, NW: 1, W: 2, SW: 3, SE: 4, E: 5.
     *
     * Edges/Sides: North-East: 0, North: 1, North-West: 2 South-West: 3, South: 4, South-East: 5
     *
     */
    template<typename F, sm::hexalign A = sm::hexalign::point_up> requires std::is_floating_point_v<F>
    class hex
    {
    public:
        hex() {}
        /*!
         * Constructor taking index, dimension and integer position indices. Computes Cartesian
         * location from these.
         */
        hex (const std::uint32_t& idx, const F& d_, const std::int32_t& r_, const std::int32_t& g_)
        {
            this->vi = idx;
            this->d = d_;
            this->ri = r_;
            this->gi = g_;
            this->compute_location();
        }

        //! Comparison operation to enable use of set<hex>
        bool operator< (const hex<F, A>& rhs) const
        {
            // Compare position first.
            if (this->x < rhs.x) { return true; }
            if (this->x > rhs.x) { return false; }
            if (this->y < rhs.y) { return true; }
            if (this->y > rhs.y) { return false; }
            // If position can't differentiate, compare vector index
            if (this->vi < rhs.vi) { return true; }
            return false; // no need for vi > rhs.vi test
        }

        /*!
         * Produce a string containing information about this hex, showing grid location in
         * dimensionless r,g (but not b) units. Also show nearest neighbours.
         */
        std::string output() const
        {
            std::string s("hex ");
            s += std::to_string(this->vi) + " (";
            s += std::to_string(this->ri).substr(0,4) + ",";
            s += std::to_string(this->gi).substr(0,4) + "). ";

            if constexpr (A == hexalign::point_up) {
                if (this->has_ne()) {
                    s += "E: (" + std::to_string(this->n0->ri).substr(0,4) + "," + std::to_string(this->n0->gi).substr(0,4) + ") " + (this->n0->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nne()) {
                    s += "NE: (" + std::to_string(this->n1->ri).substr(0,4) + "," + std::to_string(this->n1->gi).substr(0,4) + ") " + (this->n1->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nnw()) {
                    s += "NW: (" + std::to_string(this->n2->ri).substr(0,4) + "," + std::to_string(this->n2->gi).substr(0,4) + ") " + (this->n2->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nw()) {
                    s += "W: (" + std::to_string(this->n3->ri).substr(0,4) + "," + std::to_string(this->n3->gi).substr(0,4) + ") " + (this->n3->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nsw()) {
                    s += "SW: (" + std::to_string(this->n4->ri).substr(0,4) + "," + std::to_string(this->n4->gi).substr(0,4) + ") " + (this->n4->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nse()) {
                    s += "SE: (" + std::to_string(this->n5->ri).substr(0,4) + "," + std::to_string(this->n5->gi).substr(0,4) + ") " + (this->n5->boundary_hex() == true ? "OB":"") + " ";
                }
            } else {
                if (this->has_nne()) {
                    s += "NE: (" + std::to_string(this->n0->ri).substr(0,4) + "," + std::to_string(this->n0->gi).substr(0,4) + ") " + (this->n0->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nn()) {
                    s += "N: (" + std::to_string(this->n1->ri).substr(0,4) + "," + std::to_string(this->n1->gi).substr(0,4) + ") " + (this->n1->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nnw()) {
                    s += "NW: (" + std::to_string(this->n2->ri).substr(0,4) + "," + std::to_string(this->n2->gi).substr(0,4) + ") " + (this->n2->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nsw()) {
                    s += "SW: (" + std::to_string(this->n3->ri).substr(0,4) + "," + std::to_string(this->n3->gi).substr(0,4) + ") " + (this->n3->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_ns()) {
                    s += "S: (" + std::to_string(this->n4->ri).substr(0,4) + "," + std::to_string(this->n5->gi).substr(0,4) + ") " + (this->n4->boundary_hex() == true ? "OB":"") + " ";
                }
                if (this->has_nse()) {
                    s += "SE: (" + std::to_string(this->n5->ri).substr(0,4) + "," + std::to_string(this->n5->gi).substr(0,4) + ") " + (this->n5->boundary_hex() == true ? "OB":"") + " ";
                }
            }
            if (this->boundary_hex()) {
                s += "(ON boundary)";
            } else  {
                s += "(not boundary)";
            }
            return s;
        }

        /*!
         * Produce a string containing information about this hex, focussing on Cartesian position
         * information.
         */
        std::string output_cart() const
        {
            std::string s("hex ");
            s += std::to_string(this->vi).substr(0,2) + " (";
            s += std::to_string(this->ri).substr(0,4) + ",";
            s += std::to_string(this->gi).substr(0,4) + ") is at (x,y) = ("
            + std::to_string(this->x).substr(0,4) + "," + std::to_string(this->y).substr(0,4) + ")";
            return s;
        }

        //! Output "(x,y)" coordinate string
        std::string output_xy() const
        {
            std::string s("(");
            s += std::to_string(this->x).substr(0,4) + "," + std::to_string(this->y).substr(0,4) + ")";
            return s;
        }

        //! Output a string containing just "RG(ri, gi)"
        std::string output_rg() const
        {
            std::string s("RG(");
            s += std::to_string(this->ri).substr(0,4) + ",";
            s += std::to_string(this->gi).substr(0,4) + ")";
            return s;
        }

        // The index (from 0 to 5) for neighbour positions depends on hexalign A.
        static constexpr std::uint32_t neighbour_idx_e()
        {
            if constexpr (A == hexalign::point_up) { return 0u; } else { return std::numeric_limits<std::uint32_t>::max(); }
        }
        static constexpr std::uint32_t neighbour_idx_ne()
        {
            if constexpr (A == hexalign::point_up) { return 1u; } else { return 0u; }
        }
        static constexpr std::uint32_t neighbour_idx_n()
        {
            if constexpr (A == hexalign::flat_up) { return 1u; } else { return std::numeric_limits<std::uint32_t>::max(); }
        }
        static constexpr std::uint32_t neighbour_idx_nw() { return 2u; }
        static constexpr std::uint32_t neighbour_idx_w()
        {
            if constexpr (A == hexalign::point_up) { return 3u; } else { return std::numeric_limits<std::uint32_t>::max(); }
        }
        static constexpr std::uint32_t neighbour_idx_sw()
        {
            if constexpr (A == hexalign::point_up) { return 4u; } else { return 3u; }
        }
        static constexpr std::uint32_t neighbour_idx_s()
        {
            if constexpr (A == hexalign::flat_up) { return 4u; } else { return std::numeric_limits<std::uint32_t>::max(); }
        }
        static constexpr std::uint32_t neighbour_idx_se() { return 5u; }

        // Vertex position indices also depend on hexalign A
        static constexpr std::uint32_t vertex_idx_e()
        {
            if constexpr (A == hexalign::flat_up) { return 5u; } else { return std::numeric_limits<std::uint32_t>::max(); }
        }
        static constexpr std::uint32_t vertex_idx_ne() { return 0u; }
        static constexpr std::uint32_t vertex_idx_n()
        {
            if constexpr (A == hexalign::point_up) { return 1u; } else { return std::numeric_limits<std::uint32_t>::max(); }
        }
        static constexpr std::uint32_t vertex_idx_nw() {
            if constexpr (A == hexalign::point_up) { return 2u; } else { return 1u; }
        }
        static constexpr std::uint32_t vertex_idx_w()
        {
            if constexpr (A == hexalign::flat_up) { return 2u; } else { return std::numeric_limits<std::uint32_t>::max(); }
        }
        static constexpr std::uint32_t vertex_idx_sw() { return 3u; }
        static constexpr std::uint32_t vertex_idx_s()
        {
            if constexpr (A == hexalign::point_up) { return 4u; } else { return std::numeric_limits<std::uint32_t>::max(); }
        }
        static constexpr std::uint32_t vertex_idx_se() {
            if constexpr (A == hexalign::point_up) { return 5u; } else { return 4u; }
        }

        /*!
         * Convert the neighbour position number into a short string representing the
         * direction/position of the neighbour.
         */
        static std::string neighbour_pos (std::uint32_t dir)
        {
            std::string s("");
            switch (dir) {
            case 0u:
            {
                if constexpr (A == hexalign::point_up) { s = "E"; } else { s = "NE"; }
                break;
            }
            case 1u:
            {
                if constexpr (A == hexalign::point_up) { s = "NE"; } else { s = "N"; }
                break;
            }
            case 2u:
            {
                s = "NW";
                break;
            }
            case 3u:
            {
                if constexpr (A == hexalign::point_up) { s = "W"; } else { s = "SW"; }
                break;
            }
            case 4u:
            {
                if constexpr (A == hexalign::point_up) { s = "SW"; } else { s = "S"; }
                break;
            }
            case 5u:
            {
                s = "SE";
                break;
            }
            };
            return s;
        }

        /*!
         * Convert ri, gi and bi indices into x and y coordinates and also r and phi coordinates,
         * based on the hex-to-hex distance d.
         */
        void compute_location()
        {
            const F v = this->get_v();
            if constexpr (A == hexalign::point_up) {
                // Compute Cartesian location
                this->x = this->d * (this->ri + F{0.5} * (this->gi - this->bi));
                this->y = v * (this->gi + this->bi);
            } else {
                this->x = v * (this->ri - this->bi);
                this->y = this->d * (this->gi + F{0.5} * (this->ri + this->bi));
            }
            // And location in the Polar coordinate system
            this->r = std::sqrt (x * x + y * y);
            this->phi = std::atan2 (y, x);
        }

        /*!
         * Compute the distance from the point given (in two-dimensions only; x and y) by @a
         * cartesian_point to the centre of this hex.
         */
        F distance_from (const sm::vec<F, 2> cartesian_point) const
        {
            F dx = cartesian_point[0] - x;
            F dy = cartesian_point[1] - y;
            return std::sqrt (dx * dx + dy * dy);
        }

        /*!
         * Compute the distance from the point given (in two-dimensions only; x and y) by the
         * bezcoord @a cartesian_point to the centre of this hex.
         */
        F distance_from (const sm::bezcoord<F>& cartesian_point) const
        {
            F dx = cartesian_point.x() - x;
            F dy = cartesian_point.y() - y;
            return std::sqrt (dx * dx + dy * dy);
        }

        //! Compute the distance from another hex to this one.
        F distance_from (const hex<F, A>& otherhex) const
        {
            F dx = otherhex.x - x;
            F dy = otherhex.y - y;
            return std::sqrt (dx * dx + dy * dy);
        }

        void transform (const sm::mat<F, 4>& tf)
        {
            sm::vec<F, 3> posn = { this->x, this->y };
            posn = (tf * posn).less_one_dim();
            this->x = posn[0];
            this->y = posn[1];
        }

        /*!
         * Vector index. This is the index into those data vectors which hold the relevant data
         * pertaining to this hex. This is a scheme which allows me to keep the data in separate
         * vectors and all the hex position information in this class.  What happens when I delete
         * some hex elements?  Simple - I can re-set the vi indices after creating a grid of hex
         * elements and then pruning down.
         */
        std::uint32_t vi = 0;

        /*!
         * This is the index into the d_ vectors in hexgrid which can be used to find
         * the variables recorded for this hex. It's used in sm::hexgrid to populate
         * hexgrid::d_nne, hexgrid::d_nnw, hexgrid::d_nsw and hexgrid::d_nse, etc.
         *
         * This indexes into the d_ vectors in the hexgrid object to which this hex belongs. The d_
         * vectors are ordered differently from the list<hex<F, A>> object in hexgrid::hexen and hence we
         * have this attribute di in addition to the vector index vi, which provides an index into
         * list<hex<F, A>> or vector<hex<F, A>> objects which either are, or are arranged like, hexgrid::hexen
         */
        std::uint32_t di = 0;

        //! Cartesian coordinate 'x' of the centre of the hex. Public, for direct access by client code.
        F x = F{0};
        //! Cartesian 'y' coordinate of the centre of the hex.
        F y = F{0};
        // Getter for (x,y) as a sm::vec
        sm::vec<F, 2> x_y() { return sm::vec<F, 2>({this->x, this->y}); }

        //! Polar coordinates of the centre of the hex. Public, for direct access by client code.
        F r = F{0};
        //! Polar coordinate angle
        F phi = F{0};

        //! Position z of the hex is common to both Cartesian and Polar coordinate systems.
        F z = F{0};

        //! Get the Cartesian position of this hex as a fixed size array.
        std::array<F, 3> position() const
        {
            std::array<F,3> rtn = { { this->x, this->y, this->z } };
            return rtn;
        }

        //! The centre-to-centre distance from one hex to an immediately adjacent hex.
        F d = F{1};

        //! A getter for d, for completeness. d is the centre-to-centre distance between adjacent hexes.
        F get_d() const { return this->d; }

        //! Get the shortest distance from the centre to the perimeter. This is the "short radius".
        F get_sr() const { return this->d / F{2}; }

        //! The distance from the centre of the hex to any of the vertices. This is the "long radius".
        //! Also the side-length of an edge of the hex.
        F get_lr() const
        {
            F lr = this->d * sm::mathconst<F>::one_over_root_3;
            return lr;
        }

        //! Compute and return the area of the hex
        F get_area() const { return (this->d * this->d * sm::mathconst<F>::root_3_over_2); }

        //! The vertical distance between hex centres on adjacent rows.
        F get_v() const
        {
            F v = this->d * sm::mathconst<F>::root_3_over_2;
            return v;
        }

        //! hexalign::point_up: The vertical distance from the centre of the hex to the "north east" vertex of the hex.
        //! hexalign::flat_up: The *horizontal* distance from the centre of the hex to the "north east" vertex of the hex.
        F get_d_to_ne() const
        {
            F v = this->d * sm::mathconst<F>::one_over_2_root_3;
            return v;
        }

        //! Return twice the vertical distance between hex centres on adjacent rows.
        F get_two_v() const
        {
            F tv = this->d * sm::mathconst<F>::root_3;
            return tv;
        }

        /*
         * Indices in hex directions. These lie in the x-y plane. They index in positive and
         * negative directions, starting from the hex at (0,0,z)
         */

        /*!
         * Index in r direction - positive "East", that is in the +x direction.
         */
        std::int32_t ri = 0;
        /*!
         * Index in g direction - positive "North-East". In a direction 30 degrees East of North or
         * 60 degrees North of East.
         */
        std::int32_t gi = 0;
        /*!
         * Index in b direction - positive "North-West". In a direction 30 degrees West of North
         */
        std::int32_t bi = 0;

        //! Getter for this->flags
        std::uint32_t get_flags() const { return this->flags; }

        //! Set one or more flags, defined by flg, true
        void set_flag (std::uint32_t flg) { this->flags |= flg; }
        //! Alias for hex::set_flag
        void set_flags (std::uint32_t flgs) { this->flags |= flgs; }

        //! Unset one or more flags, defined by flg, i.e. set false
        void unset_flag (std::uint32_t flg) { this->flags &= ~(flg); }
        //! Alias for hex::unset_flag
        void unset_flags (std::uint32_t flgs) { this->flags &= ~(flgs); }

        //! If flags match flg, then return true
        bool test_flag (std::uint32_t flg) const { return (this->flags & flg) == flg ? true : false; }
        //! Alias for hex::test_flag
        bool test_flags (std::uint32_t flgs) const { return (this->flags & flgs) == flgs ? true : false; }

        /*!
         * Set to true if this hex has been marked as being on a boundary. It is expected that
         * client code will then re-set the neighbour relations so that on_boundary() would return
         * true.
         */
        bool boundary_hex() const { return this->flags & HEX_IS_BOUNDARY ? true : false; }
        /*!
         * Mark the hex as a boundary hex. Boundary hexes are also, by definition, inside the
         * boundary.
         */
        void set_boundary_hex() { this->flags |= (HEX_IS_BOUNDARY | HEX_INSIDE_BOUNDARY); }
        void unset_boundary_hex() { this->flags &= ~(HEX_IS_BOUNDARY | HEX_INSIDE_BOUNDARY); }

        //! Returns true if this hex is known to be inside the boundary.
        bool inside_boundary() const { return this->flags & HEX_INSIDE_BOUNDARY ? true : false; }
        //! Set the flag that says this hex is known to be inside the boundary.
        void set_inside_boundary() { this->flags |= HEX_INSIDE_BOUNDARY; }
        //! Unset the flag that says this hex is inside the boundary.
        void unset_inside_boundary() { this->flags &= ~HEX_INSIDE_BOUNDARY; }

        //! Returns true if this hex is known to be inside a rectangular, parallelogram or hexagonal 'domain'.
        bool inside_domain() const { return this->flags & HEX_INSIDE_DOMAIN ? true : false; }
        //! Set flag that says this hex is known to be inside a rectangular, parallelogram or hexagonal 'domain'.
        void set_inside_domain() { this->flags |= HEX_INSIDE_DOMAIN; }
        //! Unset flag that says this hex is known to be inside domain.
        void unset_inside_domain() { this->flags &= ~HEX_INSIDE_DOMAIN; }

        /*!
         * Set the HEX_USER_FLAG_0/1/2/3 from the passed in std::uint32_t.
         *
         * E.g. hex->set_user_flags (HEX_USER_FLAG_0 | HEX_USER_FLAG_1);
         *
         * This will set HEX_USER_FLAG_0 and HEX_USER_FLAG_1 AND UNSET HEX_USER_FLAG_2 &
         * HEX_USER_FLAG_3.
         */
        void set_user_flags (std::uint32_t uflgs) { this->flags |= (uflgs & HEX_ALL_USER); }

        //! Set the single user flag 0, 1 2 or 3 as given by the passed-in std::uint32_t uflg_num.
        void set_user_flag (std::uint32_t uflg_num)
        {
            std::uint32_t flg = HEX_USER_FLAG_0 << uflg_num;
            this->flags |= flg;
        }

        //! Un-setter corresponding to set_user_flag(std::uint32_t)
        void unset_user_flag (std::uint32_t uflg_num)
        {
            std::uint32_t flg = HEX_USER_FLAG_0 << uflg_num;
            this->flags &= ~flg;
        }

        //! Set all user flags to the unset state
        void reset_user_flags() { this->flags &= HEX_NON_USER; }

        //! Getter for each user flag
        bool get_user_flag (std::uint32_t uflg_num) const
        {
            std::uint32_t flg = HEX_USER_FLAG_0 << uflg_num;
            return ((this->flags & flg) == flg);
        }

        /*!
         * This can be populated with the distance to the nearest boundary hex, so that an algorithm
         * can set values in a hex based this metric.
         */
        F dist_to_boundary = F{-1};

        //! A short cut for testing all the neighbour flags at once
        static constexpr std::uint32_t hex_has_neighb_all()
        {
            std::uint32_t HEX_HAS_NEIGHB_ALL = 0x0;
            if constexpr (A == hexalign::point_up) {
                HEX_HAS_NEIGHB_ALL = HEX_HAS_NE | HEX_HAS_NNE | HEX_HAS_NNW | HEX_HAS_NW | HEX_HAS_NSW | HEX_HAS_NSE;
            } else {
                HEX_HAS_NEIGHB_ALL = HEX_HAS_NNE | HEX_HAS_NN | HEX_HAS_NNW | HEX_HAS_NSW | HEX_HAS_NS | HEX_HAS_NSE;
            }
            return HEX_HAS_NEIGHB_ALL;
        }

        /*!
         * Return true if this is a boundary hex - one on the outside edge of a hex grid. The result
         * is based on testing neihgbour relations, rather than examining the value of the
         * HEX_IS_BOUNDARY flag.
         */
        bool on_boundary() const
        {
            constexpr std::uint32_t HEX_HAS_NEIGHB_ALL = hex_has_neighb_all();
            return ((this->flags & HEX_HAS_NEIGHB_ALL) == HEX_HAS_NEIGHB_ALL) ? false : true;
        }

        void set_n0 (std::list<hex<F, A>>::iterator it)
        {
            this->n0 = it;
            if constexpr (A == hexalign::point_up) {
                this->flags |= HEX_HAS_NE;
            } else {
                this->flags |= HEX_HAS_NNE;
            }
        }
        void set_n1 (std::list<hex<F, A>>::iterator it)
        {
            this->n1 = it;
            if constexpr (A == hexalign::point_up) {
                this->flags |= HEX_HAS_NNE;
            } else {
                this->flags |= HEX_HAS_NN;
            }
        }
        void set_n2 (std::list<hex<F, A>>::iterator it)
        {
            this->n2 = it;
            this->flags |= HEX_HAS_NNW;
        }
        void set_n3 (std::list<hex<F, A>>::iterator it)
        {
            this->n3 = it;
            if constexpr (A == hexalign::point_up) {
                this->flags |= HEX_HAS_NW;
            } else {
                this->flags |= HEX_HAS_NSW;
            }
        }
        void set_n4 (std::list<hex<F, A>>::iterator it)
        {
            this->n4 = it;
            if constexpr (A == hexalign::point_up) {
                this->flags |= HEX_HAS_NSW;
            } else {
                this->flags |= HEX_HAS_NS;
            }
        }
        void set_n5 (std::list<hex<F, A>>::iterator it)
        {
            this->n5 = it;
            this->flags |= HEX_HAS_NSE;
        }

        //! Set that \a it is the Neighbour to the East
        void set_ne (std::list<hex<F, A>>::iterator it)
        {
            if constexpr (A == hexalign::point_up) {
                this->n0 = it;
                this->flags |= HEX_HAS_NE;
            } // else no-op for hexalign::flat_up
        }
        //! Set that \a it is the Neighbour to the North East
        void set_nne (std::list<hex<F, A>>::iterator it)
        {
            if constexpr (A == hexalign::point_up) {
                this->n1 = it;
            } else {
                this->n0 = it;
            }
            this->flags |= HEX_HAS_NNE;
        }
        //! Set that \a it is the Neighbour to the North
        void set_nn (std::list<hex<F, A>>::iterator it)
        {
            if constexpr (A == hexalign::flat_up) {
                this->n1 = it;
                this->flags |= HEX_HAS_NN;
            } // else no-op
        }
        //! Set that \a it is the Neighbour to the North West
        void set_nnw (std::list<hex<F, A>>::iterator it)
        {
            this->n2 = it;
            this->flags |= HEX_HAS_NNW;
        }
        //! Set that \a it is the Neighbour to the West
        void set_nw (std::list<hex<F, A>>::iterator it)
        {
            if constexpr (A == hexalign::point_up) {
                this->n3 = it;
                this->flags |= HEX_HAS_NW;
            } // else no-op
        }
        //! Set that \a it is the Neighbour to the South West
        void set_nsw (std::list<hex<F, A>>::iterator it)
        {
            if constexpr (A == hexalign::point_up) {
                this->n4 = it;
            } else {
                this->n3 = it;
            }
            this->flags |= HEX_HAS_NSW;
        }
        //! Set that \a it is the Neighbour to the North
        void set_ns (std::list<hex<F, A>>::iterator it)
        {
            if constexpr (A == hexalign::flat_up) {
                this->n4 = it;
                this->flags |= HEX_HAS_NS;
            } // else no-op
        }
        //! Set that \a it is the Neighbour to the South East
        void set_nse (std::list<hex<F, A>>::iterator it)
        {
            this->n5 = it;
            this->flags |= HEX_HAS_NSE;
        }

        //! Return true if this hex has a Neighbour to the East
        bool has_ne() const
        {
            if constexpr (A == hexalign::point_up) {
                return ((this->flags & HEX_HAS_NE) == HEX_HAS_NE);
            } else {
                return false;
            }
        }
        //! Return true if this hex has a Neighbour to the North East
        bool has_nne() const { return ((this->flags & HEX_HAS_NNE) == HEX_HAS_NNE); }
        //! Return true if this hex has a Neighbour to the North East
        bool has_nn() const
        {
            if constexpr (A == hexalign::flat_up) {
                return ((this->flags & HEX_HAS_NN) == HEX_HAS_NN);
            } else {
                return false;
            }
        }
        //! Return true if this hex has a Neighbour to the North West
        bool has_nnw() const { return ((this->flags & HEX_HAS_NNW) == HEX_HAS_NNW); }
        //! Return true if this hex has a Neighbour to the West
        bool has_nw() const
        {
            if constexpr (A == hexalign::point_up) {
                return ((this->flags & HEX_HAS_NW) == HEX_HAS_NW);
            } else {
                return false;
            }
        }
        //! Return true if this hex has a Neighbour to the South West
        bool has_nsw() const { return ((this->flags & HEX_HAS_NSW) == HEX_HAS_NSW); }
        //! Return true if this hex has a Neighbour to the North East
        bool has_ns() const
        {
            if constexpr (A == hexalign::flat_up) {
                return ((this->flags & HEX_HAS_NS) == HEX_HAS_NS);
            } else {
                return false;
            }
        }

        //! Return true if this hex has a Neighbour to the South East
        bool has_nse() const { return ((this->flags & HEX_HAS_NSE) == HEX_HAS_NSE); }

        //! Set flags to say that this hex has NO neighbour to East
        void unset_ne() { this->flags ^= HEX_HAS_NE; }
        //! Set flags to say that this hex has NO neighbour to North East
        void unset_nne() { this->flags ^= HEX_HAS_NNE; }
        //! Set flags to say that this hex has NO neighbour to North
        void unset_nn() { this->flags ^= HEX_HAS_NN; }
        //! Set flags to say that this hex has NO neighbour to North West
        void unset_nnw() { this->flags ^= HEX_HAS_NNW; }
        //! Set flags to say that this hex has NO neighbour to West
        void unset_nw() { this->flags ^= HEX_HAS_NW; }
        //! Set flags to say that this hex has NO neighbour to South West
        void unset_nsw() { this->flags ^= HEX_HAS_NSW; }
        //! Set flags to say that this hex has NO neighbour to South
        void unset_ns() { this->flags ^= HEX_HAS_NS; }
        //! Set flags to say that this hex has NO neighbour to South East
        void unset_nse() { this->flags ^= HEX_HAS_NSE; }

        void unset_neighbour (const std::uint32_t ni)
        {
            if constexpr (A == hexalign::point_up) {
                if (ni == 0u) { this->flags ^= HEX_HAS_NE; }
                else if (ni == 1u) { this->flags ^= HEX_HAS_NNE; }
                else if (ni == 2u) { this->flags ^= HEX_HAS_NNW; }
                else if (ni == 3u) { this->flags ^= HEX_HAS_NW; }
                else if (ni == 4u) { this->flags ^= HEX_HAS_NSW; }
                else if (ni == 5u) { this->flags ^= HEX_HAS_NSE; }
            } else {
                if (ni == 0u) { this->flags ^= HEX_HAS_NNE; }
                else if (ni == 1u) { this->flags ^= HEX_HAS_NN; }
                else if (ni == 2u) { this->flags ^= HEX_HAS_NNW; }
                else if (ni == 3u) { this->flags ^= HEX_HAS_NSW; }
                else if (ni == 4u) { this->flags ^= HEX_HAS_NS; }
                else if (ni == 5u) { this->flags ^= HEX_HAS_NSE; }
            }
        }

        /*!
         * Test if have neighbour at position \a ni.
         * East: 0, North-East: 1, North-West: 2, West: 3, South-West: 4, South-East: 5
         */
        bool has_neighbour (const std::uint32_t ni) const
        {
            if constexpr (A == hexalign::point_up) {
                switch (ni) {
                case 0u:
                {
                    return (this->flags & HEX_HAS_NE) ? true : false;
                    break;
                }
                case 1u:
                {
                    return (this->flags & HEX_HAS_NNE) ? true : false;
                    break;
                }
                case 2u:
                {
                    return (this->flags & HEX_HAS_NNW) ? true : false;
                    break;
                }
                case 3u:
                {
                    return (this->flags & HEX_HAS_NW) ? true : false;
                    break;
                }
                case 4u:
                {
                    return (this->flags & HEX_HAS_NSW) ? true : false;
                    break;
                }
                case 5u:
                {
                    return (this->flags & HEX_HAS_NSE) ? true : false;
                    break;
                }
                default:
                {
                    break;
                }
                }
            } else { // hexalign flat_up
                switch (ni) {
                case 0u:
                {
                    return (this->flags & HEX_HAS_NNE) ? true : false;
                    break;
                }
                case 1u:
                {
                    return (this->flags & HEX_HAS_NN) ? true : false;
                    break;
                }
                case 2u:
                {
                    return (this->flags & HEX_HAS_NNW) ? true : false;
                    break;
                }
                case 3u:
                {
                    return (this->flags & HEX_HAS_NSW) ? true : false;
                    break;
                }
                case 4u:
                {
                    return (this->flags & HEX_HAS_NS) ? true : false;
                    break;
                }
                case 5u:
                {
                    return (this->flags & HEX_HAS_NSE) ? true : false;
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            return false;
        }

        /*!
         * Get a list<hex<F, A>>::iterator to the neighbour at position \a ni.
         * East: 0, North-East: 1, North-West: 2, West: 3, South-West: 4, South-East: 5
         */
        std::list<hex<F, A>>::iterator get_neighbour (const std::uint32_t ni) const
        {
            if (ni == 0u) {
                return this->n0;
            } else if (ni == 1u) {
                return this->n1;
            } else if (ni == 2u) {
                return this->n2;
            } else if (ni == 3u) {
                return this->n3;
            } else if (ni == 4u) {
                return this->n4;
            } else if (ni == 5u) {
                return this->n5;
            }
            typename std::list<sm::hex<F, A>>::iterator hi;
            return hi;
        }

        //! Turn the vertex index \a ni into a string name and return it.
        static std::string vertex_name (std::uint32_t ni)
        {
            std::string s("");
            switch (ni) {
            case 0:
            {
                if constexpr (A == hexalign::point_up) { s = "NE"; } else {  s = "NE"; }
                break;
            }
            case 1:
            {
                if constexpr (A == hexalign::point_up) { s = "N"; } else {  s = "NW"; }
                break;
            }
            case 2:
            {
                if constexpr (A == hexalign::point_up) { s = "NW"; } else {  s = "W"; }
                break;
            }
            case 3:
            {
                if constexpr (A == hexalign::point_up) { s = "SW"; } else {  s = "SW"; }
                break;
            }
            case 4:
            {
                if constexpr (A == hexalign::point_up) { s = "S"; } else {  s = "SE"; }
                break;
            }
            case 5:
            {
                if constexpr (A == hexalign::point_up) { s = "SE"; } else {  s = "E"; }
                break;
            }
            default:
            {
                break;
            }
            }
            return s;
        }

        /*!
         * Get the Cartesian coordinates of the given vertex of the hex. The hex has a north vertex,
         * a north east vertex and vertices for SE, S, SW and NW. The single argument @ni specifies
         * which vertex to return the coordinate for. Use the constexpr fn definitions vertex_idx_n(), etc to
         * pass in a human-readable label for the vertex.
         */
        sm::vec<F, 2> get_vertex_coord (std::uint32_t ni) const
        {
            sm::vec<F, 2> rtn = { F{0}, F{0} };
            if constexpr (A == hexalign::point_up) {
                switch (ni) {
                case 0u:
                {
                    rtn[0] = this->x + this->get_sr();
                    rtn[1] = this->y + this->get_d_to_ne();
                    break;
                }
                case 1u:
                {
                    rtn[0] = this->x;
                    rtn[1] = this->y + this->get_lr();
                    break;
                }
                case 2u:
                {
                    rtn[0] = this->x - this->get_sr();
                    rtn[1] = this->y + this->get_d_to_ne();
                    break;
                }
                case 3u:
                {
                    rtn[0] = this->x - this->get_sr();
                    rtn[1] = this->y - this->get_d_to_ne();
                    break;
                }
                case 4u:
                {
                    rtn[0] = this->x;
                    rtn[1] = this->y - this->get_lr();
                    break;
                }
                case 5u:
                {
                    rtn[0] = this->x + this->get_sr();
                    rtn[1] = this->y - this->get_d_to_ne();
                    break;
                }
                default:
                {
                    rtn[0] = F{-1};
                    rtn[1] = F{-1};
                    break;
                }
                }
            } else { // hexalign::flat_up
                switch (ni) {
                case 0u:
                {
                    rtn[0] = this->x + this->get_d_to_ne();
                    rtn[1] = this->y + this->get_sr();
                    break;
                }
                case 1u:
                {
                    rtn[0] = this->x - this->get_d_to_ne();
                    rtn[1] = this->y + this->get_sr();
                    break;
                }
                case 2u:
                {
                    rtn[0] = this->x - this->get_lr();
                    rtn[1] = this->y;
                    break;
                }
                case 3u:
                {
                    rtn[0] = this->x - this->get_d_to_ne();
                    rtn[1] = this->y - this->get_sr();
                    break;
                }
                case 4u:
                {
                    rtn[0] = this->x + this->get_d_to_ne();
                    rtn[1] = this->y - this->get_sr();
                    break;
                }
                case 5u:
                {
                    rtn[0] = this->x + this->get_lr();
                    rtn[1] = this->y;
                    break;
                }
                default:
                {
                    rtn[0] = F{-1};
                    rtn[1] = F{-1};
                    break;
                }
                }
            }
            return rtn;
        }

        /*!
         * Get the Cartesian coordinates of the given vertex of the hex. This sub-calls the overload
         * of get_vertex_coord which accepts a single, unsigned argument.
         */
        sm::vec<F, 2> get_vertex_coord (std::int32_t ni) const
        {
            sm::vec<F, 2> rtn = { F{-3}, F{-3} };
            if (ni > 5) {
                rtn[0] = F{-4};
                return rtn;
            }
            if (ni < 0) {
                rtn[1] = F{-4};
                return rtn;
            }
            rtn = this->get_vertex_coord (ni);
            return rtn;
        }

        /*!
         * Return true if coord is reasonably close to being in the same location as the vertex at
         * vertex \a ni with the distance threshold being set from the hex to hex spacing. This is for
         * distinguishing between vertices and hex centres on a hexgrid.
         */
        bool compare_vertex_coord (std::int32_t ni, sm::vec<F, 2>& coord) const
        {
            sm::vec<F, 2> vc = this->get_vertex_coord (ni);
            if (std::abs(vc[0] - coord[0]) < this->d / F{100}
                && std::abs(vc[1] - coord[1]) < this->d / F{100}) {
                return true;
            }
            return false;
        }

        //! Return true if the hex contains the vertex at \a coord
        bool contains_vertex (sm::vec<F, 2>& coord) const
        {
            // check each of my vertices, if any match coord, then return true.
            bool rtn = false;
            for (std::uint32_t ni = 0; ni < 6; ++ni) {
                if (this->compare_vertex_coord (ni, coord) == true) {
                    rtn = true;
                    break;
                }
            }
            return rtn;
        }

        /*!
         * Return true if coord is reasonably close to being in the same location as the centre of
         * the hex, with the distance threshold being set from the hex to hex spacing. This is for
         * distinguishing between vertices and hex centres on a hexgrid.
         */
        bool compare_coord (sm::vec<F, 2>& coord) const
        {
            if (std::abs (this->x - coord[0]) < this->d / F{100}
                && std::abs (this->y - coord[1]) < this->d / F{100}) {
                return true;
            }
            return false;
        }

        //! Un-set the pointers on all my neighbours so that THEY no longer point to ME.
        void disconnect_neighbours()
        {
            for (std::uint32_t i = 0; i < 6; ++i) {
                if (this->has_neighbour(i)) {
                    typename std::list<hex<F, A>>::iterator nbr = this->get_neighbour(i);
                    const std::uint32_t i_opp = (i + 3u) % 6u;
                    if (nbr->has_neighbour (i_opp)) { nbr->unset_neighbour (i_opp); }
                }
            }
        }

        /*
         * Nearest neighbours
         */

        //! Nearest neighbour to the East; in the plus r direction.
        std::list<hex<F, A>>::iterator n0;
        //! Nearest neighbour to the North-East; in the plus g direction.
        std::list<hex<F, A>>::iterator n1;
        //! Nearest neighbour to the North-West; in the plus b direction.
        std::list<hex<F, A>>::iterator n2;
        //! Nearest neighbour to the West; in the minus r direction.
        std::list<hex<F, A>>::iterator n3;
        //! Nearest neighbour to the South-West; in the minus g direction.
        std::list<hex<F, A>>::iterator n4;
        //! Nearest neighbour to the South-East; in the minus b direction.
        std::list<hex<F, A>>::iterator n5;

        //! The flags for this hex.
        std::uint32_t flags = 0u;
    };

} // namespace sm
