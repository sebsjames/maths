// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Author: Seb James
 *
 * Date: 2021/02
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

export module sm.cartgrid;

export import sm.rect;

// cartgrid contains carried over code (from hexgrid) which allows for the imposition of
// arbitrary boundaries, specified as Bezier curves. To use a cartgrid with an arbitrary boundary, define
// CARTGRID_COMPILE_WITH_BEZCURVES
export import sm.bezcurvepath;
export import sm.bezcoord;

import sm.mathconst;
export import sm.grid; // for gridfeatures
import sm.vec;
import sm.vvec;
import sm.scale;
import sm.range;
import sm.boxfilter;

// If the cartgrid::save and cartgrid::load methods are required, define
// CARTGRID_COMPILE_LOAD_AND_SAVE. A link to libhdf5 will be required in your program.
//#ifdef CARTGRID_COMPILE_LOAD_AND_SAVE
//import sm.hdfdata; // see hexgrid_hdf for what to do
//#endif

export namespace sm
{
    /*!
     * This class is used to build a Cartesian grid of rectangular elements.
     *
     * It has been developed from hexgrid. It looks byzantine in complexity, given
     * than it's 'only' supposed to provide a way to track a rectangular grid. This is
     * because, as with hexgrid, the initial grid is intended to provide a region from
     * which an arbitrary boundary region can be 'cut out' AND it maintains all the
     * neighbour relationships correctly.
     *
     * Optionally, a boundary may be set by calling set_boundary (const
     * bezcurvepath&). If this is done, then the boundary is converted to a set of
     * elements, then those elements in the grid lying outside the boundary are removed.
     */
    class alignas(8) cartgrid
    {
    public:
        static constexpr bool debug_cartgrid = false;
        /*
         * Domain attributes
         * -----------------
         *
         * Vectors containing the "domain" info extracted from the list of elements. The
         * "domain" is the set of elements left over after the boundary has been applied
         *
         * Each of these is prefixed d_ and is carefully aligned.
         *
         * The order in which these are populated is raster-style, from top left to
         * bottom right.
         */
        alignas(alignof(std::vector<float>)) std::vector<float> d_x;
        alignas(alignof(std::vector<float>)) std::vector<float> d_y;

        /*!
         * For a given coordinate pair (x, y), the function returns the 1D index of the nearest cartgrid vertex
         * This is a simplified version of "find_rect_nearest".  find_rect_nearest can be used for non-recatngular grids.
         * This function is for rectangular grids only.
         */
        std::int32_t index_from_coord (const sm::vec<float, 2>& coord) const
        {
            std::int32_t x_ind = std::round ((coord.at(0) - x_minmax.min) / this->d);   // Index of nearest column
            std::int32_t y_ind = std::round ((coord.at(1) - y_minmax.min) / this->v);   // Index of nearest row
            std::int32_t nc = static_cast<std::int32_t>(((x_minmax.max - x_minmax.min) / this->d) + 1.0f);   // Number of columns in rectangular cartgrid
            return (nc * y_ind) + x_ind;
        }

        /*!
         * Shift the supplied coordinates cds by the metric amounts x_shift and y_shift (to the
         * nearest existing coordinate in the cartgrid) and return a vector of new coordinates. Only
         * for rectangular cartgrids.
         *
         * If any of the original coordinates get shifted off the edge of the cartgrid, then they
         * are simply omitted from the return vvec.
         */
        sm::vvec<sm::vec<float, 2>> shift_coords (const sm::vvec<sm::vec<float, 2>>& cds,
                                                  const float x_shift, const float y_shift) const
        {
            float x_step = this->d * std::round (x_shift / this->d); // find nearest x value in this->coords to x_shift
            float y_step = this->v * std::round (y_shift / this->v); // find nearest y value in this->coords to y_shift

            sm::vvec<sm::vec<float, 2>> new_coords;
            sm::vec<float, 2> ctmp;
            for (auto c : cds) {
                ctmp[0] = c[0] + x_step;
                if (this->x_minmax.contains (ctmp[0]) == false) { continue; }
                ctmp[1] = c[1] + y_step;
                if (this->y_minmax.contains (ctmp[1]) == false) { continue; }
                new_coords.push_back (ctmp);
            }
            return new_coords;
        }

        /*!
         * Shift the supplied indices inds by the metric (float) shifts x_shift and y_shift and
         * return a vector of new indices. The shifts are rounded to the nearest number of integer
         * pixel(rect) shifts. Only for rectangular cartgrids.
         *
         * If any of the original indices get shifted off the edge of the cartgrid, then they
         * are simply omitted from the return vvec.
         */
        sm::vvec<std::int32_t> shift_indices_by_metric (const sm::vvec<std::int32_t>& inds,
                                                        const float x_shift, const float y_shift) const
        {
            static constexpr bool debug_shift_indices = false;
            std::int32_t w = 1 + this->x_span/this->d;

            std::int32_t x_step = static_cast<std::int32_t>(std::round (x_shift / this->d));    // Find the number of rects in x_shift

            if constexpr (debug_shift_indices) {
                std::cout << "delta x  : " << this->d << std::endl;
                std::cout << "x shift input : " << x_shift << std::endl;
                std::cout << "x step output: " << x_step << std::endl;
            }

            std::int32_t y_step = static_cast<std::int32_t>(std::round (y_shift / this->v));    // Find the number of rects in y_shift

            if constexpr (debug_shift_indices) {
                std::cout << "delta y  : " << this->v << std::endl;
                std::cout << "y shift input : " << y_shift << std::endl;
                std::cout << "y step output: " << y_step << std::endl;
            }

            sm::vvec<std::int32_t> new_indices;

            for (std::uint32_t i = 0; i < inds.size(); i++) {

                std::int32_t orig_row = d_yi[inds[i]];
                std::int32_t orig_col = d_xi[inds[i]];

                std::int32_t x_moved = orig_col + x_step;
                if (this->xi_minmax.contains (x_moved) == false) { continue; }
                std::int32_t y_moved = (orig_row + y_step);
                if (this->yi_minmax.contains (y_moved) == false) { continue; }
                new_indices.push_back ((x_moved - this->xi_minmax.min) + w * (y_moved - yi_minmax.min));

                if constexpr (debug_shift_indices) {
                    std::cout << "inds[i] : " << inds[i] << std::endl;
                    std::cout << "orig_row: " << orig_row << std::endl;
                    std::cout << "orig_col: " << orig_col << std::endl;
                    std::cout << "x moved : " << x_moved << std::endl;
                    std::cout << "y moved : " << y_moved << std::endl;
                }
            }
            return new_indices;
        }

        //! Get all the (x,y,z) coordinates from the grid and return as vector of Vectors
        std::vector<sm::vec<float, 3>> get_coordinates3() const
        {
            std::vector<sm::vec<float, 3>> coords (this->num());
            for (std::uint32_t i = 0; i < this->num(); ++i) {
                coords[i] = { this->d_x[i], this->d_y[i], this->z };
            }
            return coords;
        }

        //! Get all the (x,y) coordinates from the grid and return as vector of vecs
        std::vector<sm::vec<float, 2>> get_coordinates2() const
        {
            std::vector<sm::vec<float, 2>> coords (this->num());
            for (std::uint32_t i = 0; i < this->num(); ++i) {
                coords[i] = { this->d_x[i], this->d_y[i] };
            }
            return coords;
        }

        // A get-the-coordinates function that returns a vvec of vec<float, 3>s
        sm::vvec<sm::vec<float, 3>> get_coords() const
        {
            sm::vvec<sm::vec<float, 3>> rtn (d_x.size(), {0,0,0});
            for (std::uint32_t i = 0; i < d_x.size(); ++i) {
                rtn[i][0] = d_x[i];
                rtn[i][1] = d_y[i];
            }
            return rtn;
        }

        // A get-the-coordinates function that returns a vvec of vec<float, 2>s
        sm::vvec<sm::vec<float, 2>> get_coords_2d() const
        {
            sm::vvec<sm::vec<float, 2>> rtn (d_x.size(), {0,0});
            for (std::uint32_t i = 0; i < d_x.size(); ++i) {
                rtn[i][0] = d_x[i];
                rtn[i][1] = d_y[i];
            }
            return rtn;
        }

        // Width and height of a cartgrid that happens to be of type griddomainshape::rectangle.
        std::int32_t w_px = -1;
        std::int32_t h_px = -1;

        /*
         * Neighbour iterators. For use when the stride to the neighbour ne or nw is not
         * constant. On a Cartesian grid, these are necessary if an arbitrary boundary
         * has been applied.
         */
        alignas(8) std::vector<std::int32_t> d_ne;
        alignas(8) std::vector<std::int32_t> d_nne;
        alignas(8) std::vector<std::int32_t> d_nn;
        alignas(8) std::vector<std::int32_t> d_nnw;
        alignas(8) std::vector<std::int32_t> d_nw;
        alignas(8) std::vector<std::int32_t> d_nsw;
        alignas(8) std::vector<std::int32_t> d_ns;
        alignas(8) std::vector<std::int32_t> d_nse;

        alignas(8) std::vector<std::int32_t> d_xi;
        alignas(8) std::vector<std::int32_t> d_yi;

        sm::range<std::int32_t> xi_minmax;
        sm::range<std::int32_t> yi_minmax;

        /*!
         * Flags, such as "on boundary", "inside boundary", "outside boundary", "has
         * neighbour east", etc.
         */
        alignas(8) std::vector<std::uint32_t> d_flags;

        //! Distance to boundary for any element.
        alignas(8) std::vector<float> d_dist_to_boundary;

        /*!
         * How many additional rects to grow out to the left and right; top and
         * bottom? Set this to a larger number if the boundary is expected to grow
         * during a simulation.
         */
        std::uint32_t d_growthbuffer_horz = 0;
        std::uint32_t d_growthbuffer_vert = 0;

        //! Add entries to all the d_ vectors for the rect pointed to by ri.
        void d_push_back (std::list<rect>::iterator ri)
        {
            d_x.push_back (ri->x);
            d_y.push_back (ri->y);
            d_xi.push_back (ri->xi);
            d_yi.push_back (ri->yi);
            d_flags.push_back (ri->get_flags());
            d_dist_to_boundary.push_back (ri->dist_to_boundary);

            // record in the rect the iterator in the d_ vectors so that d_nne and friends can be set up later.
            ri->di = d_x.size()-1;
        }

        //! Once rect::di attributes have been set, populate d_nne and friends.
        void populate_d_neighbours()
        {
            // Resize d_nne and friends
            this->d_nne.resize (this->d_x.size(), 0);
            this->d_nn.resize (this->d_x.size(), 0);
            this->d_ne.resize (this->d_x.size(), 0);
            this->d_nnw.resize (this->d_x.size(), 0);
            this->d_nw.resize (this->d_x.size(), 0);
            this->d_nsw.resize (this->d_x.size(), 0);
            this->d_ns.resize (this->d_x.size(), 0);
            this->d_nse.resize (this->d_x.size(), 0);

            std::list<sm::rect>::iterator ri = this->rects.begin();
            while (ri != this->rects.end()) {

                if (ri->has_ne() == true) {
                    this->d_ne[ri->di] = ri->ne->di;
                } else {
                    this->d_ne[ri->di] = -1;
                }

                if (ri->has_nne() == true) {
                    this->d_nne[ri->di] = ri->nne->di;
                } else {
                    this->d_nne[ri->di] = -1;
                }

                if (ri->has_nn() == true) {
                    this->d_nn[ri->di] = ri->nn->di;
                } else {
                    this->d_nn[ri->di] = -1;
                }

                if (ri->has_nnw() == true) {
                    this->d_nnw[ri->di] = ri->nnw->di;
                } else {
                    this->d_nnw[ri->di] = -1;
                }

                if (ri->has_nw() == true) {
                    this->d_nw[ri->di] = ri->nw->di;
                } else {
                    this->d_nw[ri->di] = -1;
                }

                if (ri->has_nsw() == true) {
                    this->d_nsw[ri->di] = ri->nsw->di;
                } else {
                    this->d_nsw[ri->di] = -1;
                }

                if (ri->has_ns() == true) {
                    this->d_ns[ri->di] = ri->ns->di;
                } else {
                    this->d_ns[ri->di] = -1;
                }

                if (ri->has_nse() == true) {
                    this->d_nse[ri->di] = ri->nse->di;
                } else {
                    this->d_nse[ri->di] = -1;
                }

                ++ri;
            }
        }

        //! Clear out all the d_ vectors
        void d_clear()
        {
            this->d_x.clear();
            this->d_y.clear();
            this->d_xi.clear();
            this->d_yi.clear();
            this->d_flags.clear();
        }

#ifdef CARTGRID_COMPILE_LOAD_AND_SAVE
        /*!
         * Save this cartgrid (and all the rects in it) into the HDF5 file at the
         * location \a path.
         */
        void save (const std::string& path)
        {
            sm::hdfdata cgdata (path, std::ios::out | std::ios::trunc);
            cgdata.add_val ("/d", d);
            cgdata.add_val ("/v", v);
            cgdata.add_val ("/x_span", x_span);
            cgdata.add_val ("/y_span", y_span);
            cgdata.add_val ("/z", z);
            cgdata.add_val ("/d_growthbuffer_horz", d_growthbuffer_horz);
            cgdata.add_val ("/d_growthbuffer_vert", d_growthbuffer_vert);

            // sm::vec<float, 2>
            cgdata.add_contained_vals ("/boundary_entroid", boundary_centroid);

            // Don't save bezcurvepath boundary - limit this to the ability to
            // save which elements are boundary elements and which aren't

            // vector<float>
            cgdata.add_contained_vals ("/d_x", d_x);
            cgdata.add_contained_vals ("/d_y", d_y);
            cgdata.add_contained_vals ("/d_dist_to_boundary", d_dist_to_boundary);
            // vector<std::int32_t>
            cgdata.add_contained_vals ("/d_xi", d_xi);
            cgdata.add_contained_vals ("/d_yi", d_yi);

            cgdata.add_contained_vals ("/d_ne", d_ne);
            cgdata.add_contained_vals ("/d_nne", d_nne);
            cgdata.add_contained_vals ("/d_nn", d_nn);
            cgdata.add_contained_vals ("/d_nnw", d_nnw);
            cgdata.add_contained_vals ("/d_nw", d_nw);
            cgdata.add_contained_vals ("/d_nsw", d_nsw);
            cgdata.add_contained_vals ("/d_ns", d_ns);
            cgdata.add_contained_vals ("/d_nse", d_nse);

            // vector<std::uint32_t>
            cgdata.add_contained_vals ("/d_flags", d_flags);

            // list<rect> rects
            // for i in list, save rect
            std::list<sm::rect>::const_iterator r = this->rects.begin();
            std::uint32_t rcount = 0;
            while (r != this->rects.end()) {
                // Make up a path
                std::string h5path = "/rects/" + std::to_string(rcount);
                r->save (cgdata, h5path);
                ++r;
                ++rcount;
            }
            cgdata.add_val ("/rcount", rcount);

            // What about vrects? Probably don't save and re-call method to populate.
            this->renumber_vector_indices();

            // What about brects? Probably re-run/test this->boundary_contiguous() on load.
            this->boundary_contiguous();
        }

        /*!
         * Populate this cartgrid from the HDF5 file at the location \a path.
         */
        void load (const std::string& path)
        {
            sm::hdfdata cgdata (path, std::ios::in);
            cgdata.read_val ("/d", this->d);
            cgdata.read_val ("/v", this->v);
            cgdata.read_val ("/x_span", this->x_span);
            cgdata.read_val ("/y_span", this->y_span);
            cgdata.read_val ("/z", this->z);
            cgdata.read_val ("/d_growthbuffer_horz", this->d_growthbuffer_horz);
            cgdata.read_val ("/d_growthbuffer_vert", this->d_growthbuffer_vert);

            cgdata.read_contained_vals ("/boundary_centroid", this->boundary_centroid);
            cgdata.read_contained_vals ("/d_x", this->d_x);
            cgdata.read_contained_vals ("/d_y", this->d_y);
            cgdata.read_contained_vals ("/d_dist_to_boundary", this->d_dist_to_boundary);
            cgdata.read_contained_vals ("/d_xi", this->d_xi);
            cgdata.read_contained_vals ("/d_yi", this->d_yi);
            cgdata.read_contained_vals ("/d_ne", this->d_ne);
            cgdata.read_contained_vals ("/d_nne", this->d_nne);
            cgdata.read_contained_vals ("/d_nnw", this->d_nnw);
            cgdata.read_contained_vals ("/d_nw", this->d_nw);
            cgdata.read_contained_vals ("/d_nsw", this->d_nsw);
            cgdata.read_contained_vals ("/d_nse", this->d_nse);
            cgdata.read_contained_vals ("/d_flags", this->d_flags);

            // Assume a boundary has been applied so set this true.
            this->grid_reduced = true;

            std::uint32_t rcount = 0;
            cgdata.read_val ("/rcount", rcount);
            for (std::uint32_t i = 0; i < rcount; ++i) {
                std::string h5path = "/rects/" + std::to_string(i);
                sm::rect r(cgdata, h5path);
                this->rects.push_back (r);
            }

            // After creating rects list, need to set neighbour relations in each rect, as loaded in
            // d_ne, etc.
            for (sm::rect& _r : this->rects) {
                // For each rect, six loops through rects:
                if (_r.has_ne() == true) {
                    bool matched = false;
                    std::uint32_t neighb_it = (std::uint32_t) this->d_ne[_r.vi];
                    std::list<sm::rect>::iterator ri = this->rects.begin();
                    while (ri != this->rects.end()) {
                        if (ri->vi == neighb_it) {
                            matched = true;
                            _r.ne = ri;
                            break;
                        }
                        ++ri;
                    }
                    if (!matched) {
                        throw std::runtime_error ("Failed to match rects neighbour E relation...");
                    }
                }

                if (_r.has_nne() == true) {
                    bool matched = false;
                    std::uint32_t neighb_it = (std::uint32_t) this->d_nne[_r.vi];
                    std::list<sm::rect>::iterator ri = this->rects.begin();
                    while (ri != this->rects.end()) {
                        if (ri->vi == neighb_it) {
                            matched = true;
                            _r.nne = ri;
                            break;
                        }
                        ++ri;
                    }
                    if (!matched) {
                        throw std::runtime_error ("Failed to match rects neighbour NE relation...");
                    }
                }

                if (_r.has_nn() == true) {
                    bool matched = false;
                    std::uint32_t neighb_it = (std::uint32_t) this->d_nn[_r.vi];
                    std::list<sm::rect>::iterator ri = this->rects.begin();
                    while (ri != this->rects.end()) {
                        if (ri->vi == neighb_it) {
                            matched = true;
                            _r.nn = ri;
                            break;
                        }
                        ++ri;
                    }
                    if (!matched) {
                        throw std::runtime_error ("Failed to match rects neighbour N relation...");
                    }
                }

                if (_r.has_nnw() == true) {
                    bool matched = false;
                    std::uint32_t neighb_it = (std::uint32_t) this->d_nnw[_r.vi];
                    std::list<sm::rect>::iterator ri = this->rects.begin();
                    while (ri != this->rects.end()) {
                        if (ri->vi == neighb_it) {
                            matched = true;
                            _r.nnw = ri;
                            break;
                        }
                        ++ri;
                    }
                    if (!matched) {
                        throw std::runtime_error ("Failed to match rects neighbour NW relation...");
                    }
                }

                if (_r.has_nw() == true) {
                    bool matched = false;
                    std::uint32_t neighb_it = (std::uint32_t) this->d_nw[_r.vi];
                    std::list<sm::rect>::iterator ri = this->rects.begin();
                    while (ri != this->rects.end()) {
                        if (ri->vi == neighb_it) {
                            matched = true;
                            _r.nw = ri;
                            break;
                        }
                        ++ri;
                    }
                    if (!matched) {
                        throw std::runtime_error ("Failed to match rects neighbour W relation...");
                    }
                }

                if (_r.has_nsw() == true) {
                    bool matched = false;
                    std::uint32_t neighb_it = (std::uint32_t) this->d_nsw[_r.vi];
                    std::list<sm::rect>::iterator ri = this->rects.begin();
                    while (ri != this->rects.end()) {
                        if (ri->vi == neighb_it) {
                            matched = true;
                            _r.nsw = ri;
                            break;
                        }
                        ++ri;
                    }
                    if (!matched) {
                        throw std::runtime_error ("Failed to match rects neighbour SW relation...");
                    }
                }

                if (_r.has_ns() == true) {
                    bool matched = false;
                    std::uint32_t neighb_it = (std::uint32_t) this->d_ns[_r.vi];
                    std::list<sm::rect>::iterator ri = this->rects.begin();
                    while (ri != this->rects.end()) {
                        if (ri->vi == neighb_it) {
                            matched = true;
                            _r.ns = ri;
                            break;
                        }
                        ++ri;
                    }
                    if (!matched) {
                        throw std::runtime_error ("Failed to match rects neighbour S relation...");
                    }
                }

                if (_r.has_nse() == true) {
                    bool matched = false;
                    std::uint32_t neighb_it = (std::uint32_t) this->d_nse[_r.vi];
                    std::list<sm::rect>::iterator ri = this->rects.begin();
                    while (ri != this->rects.end()) {
                        if (ri->vi == neighb_it) {
                            matched = true;
                            _r.nse = ri;
                            break;
                        }
                        ++ri;
                    }
                    if (!matched) {
                        throw std::runtime_error ("Failed to match rects neighbour SE relation...");
                    }
                }
            }
        }
#endif // CARTGRID_COMPILE_LOAD_AND_SAVE

        //! Default constructor creates symmetric grid centered about 0,0.
        cartgrid(): d(1.0f), v(1.0f), x_span(1.0f), y_span(1.0f), z(0.0f) {}

#ifdef CARTGRID_COMPILE_LOAD_AND_SAVE
        //! Construct then load from file.
        cartgrid (const std::string& path) : d(1.0f), v(1.0f), x_span(1.0f), z(0.0f) { this->load (path); }
#endif

        //! Construct the a symmetric, centered grid with a square element distance of \a d_ and
        //! square size length x_span. The number of elements will be computed. If x_ and x_span_ do
        //! not permit a symmetric, zero-centred grid to be created, an error will be thrown.
        cartgrid (float d_, float x_span_, float z_ = 0.0f, griddomainshape shape = griddomainshape::rectangle)
            : cartgrid (d_, d_, x_span_, x_span_, z_, shape) {}

        //! Construct a grid with rectangular element width d_, height v_ but still symmetric and
        //! centred. x_span_ is the distance from the centre of the left-most element to the centre
        //! of the right-most element. Similar for y_span_. The number of elements will be
        //! calculated based on d_, v_, x_span_ and y_span_. If the passed values do not permit a
        //! symmetric, zero-centred grid to be created an error will be thrown.
        cartgrid (float d_, float v_, float x_span_, float y_span_, float z_ = 0.0f,
                  griddomainshape shape = griddomainshape::rectangle)
        {
            this->d = d_;
            this->v = v_;
            this->x_span = x_span_;
            this->y_span = y_span_;
            this->z = z_;
            this->domain_shape = shape;

            // Test we can make a symmetric grid, if not throw an error
            float half_x = this->x_span / 2.0f;
            std::int32_t half_cols = std::abs (std::ceil (half_x / this->d));
            if (static_cast<float>(half_cols) * this->d != half_x) {
                std::stringstream ee;
                ee << "cartgrid: Cannot make a symmetric, zero-centred cartgrid with choices for d_ ("
                   << d_ << ") and x_span_ (" << x_span_ << ") because half_cols=" << half_cols << " and half_x=" << half_x;
                throw std::runtime_error (ee.str());
            }
            float half_y = this->y_span / 2.0f;
            std::int32_t half_rows = std::abs (std::ceil (half_y / this->v));
            if (static_cast<float>(half_rows) * this->v != half_y) {
                std::stringstream ee;
                ee << "cartgrid: Cannot make a symmetric, zero-centred cartgrid with choices for v_ ("
                   << v_ << ") and y_span_ (" << y_span_ << ") because half_rows=" << half_rows << " and half_y=" << half_y;
                throw std::runtime_error (ee.str());
            }

            this->init();
        }

        //! Construct with rectangular element width d_, height v_ starting at location x1,y1 and
        //! creating to x2,y2. The number of elements that need to be created will be determined
        //! from these. This is a non-symmetric constructor.
        cartgrid (float d_, float v_, float x1, float y1, float x2, float y2, float z_ = 0.0f,
                  griddomainshape shape = griddomainshape::rectangle,
                  griddomainwrap wrap = griddomainwrap::none)
        {
            if constexpr (debug_cartgrid) {
                std::cout << "cartgrid constructor (x1,y1 to x2,y2 version) called. 0x" << (std::uint64_t)this << "\n";
            }
            this->d = d_;
            this->v = v_;
            this->x_span = x2 - x1;
            this->y_span = y2 - y1;
            this->z = z_;
            this->domain_shape = shape;
            this->domain_wrap = wrap;

            // init2 is the non-symmetic initialisation for making arbitrary rectangular grids.
            this->init2 (x1, y1, x2, y2);
        }

        //! Initialisation common code
        void init (float d_, float v_, float x_span_, float y_span_, float z_ = 0.0f)
        {
            this->d = d_;
            this->v = v_;
            this->x_span = x_span_;
            this->y_span = y_span_;
            this->z = z_;
            this->init();
        }
        void init (float d_, float x_span_, float z_ = 0.0f)
        {
            this->init (d_, d_, x_span_, x_span_, z_);
        }

        //! Compute centre of mass of the passed in variable, defined across this cartgrid
        template<typename F = float>
        sm::vec<float, 2> centre_of_mass (sm::vvec<F>& data)
        {
            sm::vec<float, 2> com = {0,0};
            F datasum = F{0};
            for (std::uint32_t i = 0; i < this->num(); ++i) {
                com[0] += d_x[i] * data[i];
                com[1] += d_y[i] * data[i];
                datasum += data[i];
            }
            com /= datasum;
            return com;
        }

        //! Compute the centroid of the passed in list of rects.
        sm::vec<float, 2> compute_centroid (const std::list<rect>& p_rects)
        {
            sm::vec<float, 2> centroid = { 0.0f, 0.0f };
            for (auto r : p_rects) {
                centroid[0] += r.x;
                centroid[1] += r.y;
            }
            centroid /= p_rects.size();
            return centroid;
        }

        /*!
         * Sets boundary to match the list of rects passed in as \a p_rects. Note, that
         * unlike void set_boundary (const bezcurvepath& p), this method does not apply
         * any offset to the positions of the rects in \a p_rects.
         */
        void set_boundary (const std::list<rect>& p_rects)
        {
            this->boundary_centroid = this->compute_centroid (p_rects);

            std::list<sm::rect>::iterator bpoint = this->rects.begin();
            std::list<sm::rect>::iterator bpi = this->rects.begin();
            while (bpi != this->rects.end()) {
                std::list<sm::rect>::const_iterator ppi = p_rects.begin();
                while (ppi != p_rects.end()) {
                    // NB: The assumption right now is that the p_rects are from the same dimension grid
                    // as this->rects.
                    if (bpi->xi == ppi->xi && bpi->yi == ppi->yi) {
                        // Set h as boundary rect.
                        bpi->set_flag (RECT_IS_BOUNDARY | RECT_INSIDE_BOUNDARY);
                        bpoint = bpi;
                        break;
                    }
                    ++ppi;
                }
                ++bpi;
            }

            // Check that the boundary is contiguous.
            std::set<std::uint32_t> seen;
            std::list<sm::rect>::iterator ri = bpoint;
            if (this->boundary_contiguous (bpoint, ri, seen, RECT_NEIGHBOUR_POS_E) == false) {
                std::cout << "Uh oh\n";
                throw std::runtime_error ("The boundary is not a contiguous sequence of rects.");
            }

            if (this->domain_shape == sm::griddomainshape::boundary) {
                // Boundary IS contiguous, discard rects outside the boundary.
                this->discard_outside_boundary();
            } else {
                throw std::runtime_error ("For now, set_boundary (const list<rect>& p_rects) doesn't know what to "
                                          "do if domain shape is not griddomainshape::boundary.");
            }

            this->populate_d_vectors();
        }

        /*!
         * Sets boundary to \a p, then runs the code to discard rects lying outside
         * this boundary. Finishes up by calling sm::cartgrid::discard_outside.
         * The bezcurvepath's centroid may not be 0,0. If loffset has its default value
         * of true, then this method offsets the boundary so that when it is applied to
         * the cartgrid, the centroid IS (0,0). If \a loffset is false, then \a p is not
         * translated in this way.
         */
        void set_boundary (const sm::bezcurvepath<float>& p, bool loffset = true)
        {
            this->boundary = p;
            if (!this->boundary.is_null()) {
                // Compute the points on the boundary using half of the rect to rect
                // spacing as the step size. The 'true' argument inverts the y axis.
                this->boundary.compute_points (this->d/2.0f, true);
                std::vector<sm::bezcoord<float>> bpoints = this->boundary.get_points();
                this->set_boundary (bpoints, loffset);
            }
        }

        /*!
         * This sets a boundary, just as sm::cartgrid::set_boundary(const
         * sm::bezcurvepath<float> p, bool offset) does but WITHOUT discarding rects
         * outside the boundary. Also, it first clears the previous boundary flags so
         * the new ones are the only ones marked on the boundary. It does this because
         * it does not discard rects outside the boundary or repopulate the cartgrid but
         * it draws a new boundary that can be used by client code
         */
        void set_boundary_only (const sm::bezcurvepath<float>& p, bool loffset = true)
        {
            this->boundary = p;
            if (!this->boundary.is_null()) {
                this->boundary.compute_points (this->d/2.0f, true); // FIXME PROBABLY NEEDS TO BE DIFFERENT
                std::vector<sm::bezcoord<float>> bpoints = this->boundary.get_points();
                this->set_boundary_only (bpoints, loffset);
            }
        }

        /*!
         * Sets the boundary of the hexgrid to \a bpoints, then runs the code to discard
         * rects lying outside this boundary. Finishes up by calling
         * cartgrid::discard_outside. By default, this method translates \a bpoints so
         * that when the boundary is applied to the cartgrid, its centroid is (0,0). If
         * the default value of \a loffset is changed to false, \a bpoints is NOT
         * translated.
         */
        void set_boundary (std::vector<sm::bezcoord<float>>& bpoints, bool loffset = true)
        {
            this->boundary_centroid = sm::bezcurvepath<float>::get_centroid (bpoints);

            auto bpi = bpoints.begin();
            // conditionally executed if we reset the centre
            if (loffset) {
                while (bpi != bpoints.end()) {
                    bpi->subtract (this->boundary_centroid);
                    ++bpi;
                }
                // Copy the centroid
                this->original_boundary_centroid = this->boundary_centroid;
                // Zero out the centroid, as the boundary is now centred on 0,0
                this->boundary_centroid = { 0.0f, 0.0f };
                bpi = bpoints.begin();
            }

            // now proceed with centroid changed or unchanged
            std::list<sm::rect>::iterator nearby_boundary_point = this->rects.begin(); // i.e the rect at 0,0
            bpi = bpoints.begin();
            while (bpi != bpoints.end()) {
                nearby_boundary_point = this->set_boundary (*bpi++, nearby_boundary_point);
            }

            // Check that the boundary is contiguous.
            {
                std::set<std::uint32_t> seen;
                std::list<sm::rect>::iterator hi = nearby_boundary_point;
                if (this->boundary_contiguous (nearby_boundary_point, hi, seen, RECT_NEIGHBOUR_POS_E) == false) {
                    throw std::runtime_error ("The constructed boundary is not a contiguous sequence of rectangular elements.");
                }
            }

            if (this->domain_shape == sm::griddomainshape::boundary) {
                this->discard_outside_boundary();
                this->populate_d_vectors();
            } else {
                throw std::runtime_error ("Use griddomainshape::boundary when setting a boundary");
            }
        }

        // find the cartgrid position which corresponds to the max value in image_data.
        sm::vec<float, 2> findmax (const sm::vvec<float>& image_data)
        {
            std::uint32_t idx = image_data.argmax();
            return sm::vec<float, 2>{ this->d_x[idx], this->d_y[idx] };
        }

        // Create a radial representation of the image_data associated with this
        // cartgrid, which for this function is assumed to be rectangular. The
        // representation is taken from the location at view_pos, with an angular offset
        // of view_angle.
        void resample_to_polar (const sm::vvec<float>& image_data,
                                sm::cartgrid& cg_polar, sm::vvec<float>& polar_data,
                                sm::vec<float, 2> view_pos, float view_angle, sm::scaling_function radscale = sm::scaling_function::linear)
        {
            polar_data.zero();

            // distance per pixel in the image. This defines the Gaussian width (sigma) for the resample:
            sm::vec<float, 2> dist_per_pix = { this->d, this->v };
            sm::vec<float, 2> params = 1.0f / (2.0f * dist_per_pix * dist_per_pix);
            float assumecirc = params.mean();
            sm::vec<float, 2> polar_span = cg_polar.get_span();

            sm::vec<std::uint32_t, 2> polar_span_pix = cg_polar.get_span_pix();
            if (polar_span_pix[0]%2 == 0) {
                throw std::runtime_error ("Fix cg_polar to have an odd width (so that it runs from -x:0:+x)");
            }

            // Now now that polar_span in x is symmetric
            float rad_per_dist = sm::mathconst<float>::two_pi/(polar_span[0] + cg_polar.get_d());

            std::list<sm::rect>::iterator lastrect = this->rects.begin();
#pragma omp parallel for
            for (std::uint32_t xi = 0; xi < cg_polar.num(); ++xi) { // for each output pixel which is an r/phi pair

                float r = cg_polar.d_y[xi]; // Linear
                if (radscale == sm::scaling_function::logarithmic) {
                    r = std::log (this->v+cg_polar.d_y[xi]) - std::log(this->v);
                    r *= 0.4f; // You can play with this factor
                    //std::cout << "For r linear = " << cg_polar.d_y[xi] << ", log transform is " << r << std::endl;
                }
                //float phi = cg_polar.d_x[xi] * rad_per_dist;

                // r and phi in the image frame:
                float phi_imframe = (cg_polar.d_x[xi] * rad_per_dist) + view_angle;
                if (phi_imframe > sm::mathconst<float>::pi) { phi_imframe -= sm::mathconst<float>::two_pi; }
                // x,y in the image frame associated with r,phi in the polar rep:
                sm::vec<float, 2> abs_xy_imframe = {r * std::cos (phi_imframe), r * std::sin (phi_imframe)};
                abs_xy_imframe += view_pos;

                // If abs_xy_imframe is outside the bounds of the image region, then leave value 0 and move on.
                if (this->is_inside_rectangular_boundary (abs_xy_imframe) == false) { continue; }

                // Find pixel nearest abs_xy_imframe
                //std::list<sm::rect>::iterator nearest = this->find_rect_nearest (abs_xy_imframe);
                // or (if it's faster?):
                std::list<sm::rect>::iterator nearest = this->find_rect_near_point (abs_xy_imframe, lastrect);
                lastrect = nearest;

                // Now sum up contribution from nearest and its neighbours to polar_data[xi].

                // Closest pix
                std::list<sm::rect>::iterator curr = nearest;
                float dd = (abs_xy_imframe - sm::vec<float, 2>{ curr->x, curr->y }).length();
                float expr = std::exp ( -(assumecirc * dd * dd) ) * image_data[curr->vi];

                float contributors = 1.0f;
                // 8 Neighbours
                for (std::uint16_t nn = 0; nn < 8; ++nn) {
                    if (nearest->has_neighbour(nn)) {
                        curr = nearest->get_neighbour(nn);
                        float dd = (abs_xy_imframe - sm::vec<float, 2>{ curr->x, curr->y }).length();
                        // sum according to 2D Gaussian:
                        expr += std::exp ( -(assumecirc * dd * dd) ) * image_data[curr->vi];
                        contributors += 1.0f;
                    }
                }

                polar_data[xi] = expr / contributors;
            }

            //polar_data /= polar_data.max(); // renormalise?
        }

        /*!
         * This sets a boundary, just as
         * sm::cartgrid::set_boundary(vector<sm::bezcoord<float>& bpoints, bool offset)
         * does but WITHOUT discarding rects outside the boundary. Also, it first clears
         * the previous boundary flags so the new ones are the only ones marked on the
         * boundary. It does this because it does not discard rects outside the boundary
         * or repopulate the cartgrid but it draws a new boundary that can be used by
         * client code
         */
        void set_boundary_only (std::vector<sm::bezcoord<float>>& bpoints, bool loffset)
        {
            this->boundary_centroid = sm::bezcurvepath<float>::get_centroid (bpoints);

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
                this->boundary_centroid = { 0.0f, 0.0f };
                bpi = bpoints.begin();
            }

            // now proceed with centroid changed or unchanged. First: clear all boundary flags
            for (auto r : this->rects) { r.unset_user_flag (RECT_IS_BOUNDARY); }

            std::list<sm::rect>::iterator nearby_boundary_point = this->rects.begin(); // i.e the rect at 0,0
            bpi = bpoints.begin();
            while (bpi != bpoints.end()) {
                nearby_boundary_point = this->set_boundary (*bpi++, nearby_boundary_point);
            }

            // Check that the boundary is contiguous.
            {
                std::set<std::uint32_t> seen;
                std::list<sm::rect>::iterator ri = nearby_boundary_point;
                if (this->boundary_contiguous (nearby_boundary_point, ri, seen, RECT_NEIGHBOUR_POS_E) == false) {
                    throw std::runtime_error ("The constructed boundary is not a contiguous sequence of rects.");
                }
            }
        }

        /*!
         * Set all the outer rects as being "boundary" rects. This makes it possible to
         * create the default/original rectangle of rects, then mark the outer rects as
         * being the boundary.
         *
         * Works only on the initial layout of rects.
         */
        static constexpr bool debug_set_boundary = false;
        void set_boundary_on_outer_edge()
        {
            // From centre head to boundary, then mark boundary and walk
            // around the edge.
            std::list<sm::rect>::iterator bpi = this->rects.begin();
            // Head to the south west corner
            while (bpi->has_nw() && !bpi->wraps_w()) { bpi = bpi->nw; }
            while (bpi->has_ns() && !bpi->wraps_s()) { bpi = bpi->ns; }
            bpi->set_flag (RECT_IS_BOUNDARY | RECT_INSIDE_BOUNDARY);
            if constexpr (debug_set_boundary) {
                std::cout << "set flag at start on rect " << bpi->output_cart() << std::endl;
            }
            while (bpi->has_ne() && !bpi->wraps_e()) {
                bpi = bpi->ne;
                bpi->set_flag (RECT_IS_BOUNDARY | RECT_INSIDE_BOUNDARY);
                if constexpr (debug_set_boundary) {
                    std::cout << "set flag going E on rect " << bpi->output_cart() << std::endl;
                }
            }
            while (bpi->has_nn() && !bpi->wraps_n()) {
                bpi = bpi->nn;
                bpi->set_flag (RECT_IS_BOUNDARY | RECT_INSIDE_BOUNDARY);
                if constexpr (debug_set_boundary) {
                    std::cout << "set flag going N on rect " << bpi->output_cart() << std::endl;
                }
            }
            while (bpi->has_nw() && !bpi->wraps_w()) {
                bpi = bpi->nw;
                bpi->set_flag (RECT_IS_BOUNDARY | RECT_INSIDE_BOUNDARY);
                if constexpr (debug_set_boundary) {
                    std::cout << "set flag going W on rect " << bpi->output_cart() << std::endl;
                }
            }
            while (bpi->has_ns() && !bpi->wraps_s() && bpi->ns->test_flags(RECT_IS_BOUNDARY) == false) {
                bpi = bpi->ns;
                bpi->set_flag (RECT_IS_BOUNDARY | RECT_INSIDE_BOUNDARY);
                if constexpr (debug_set_boundary) {
                    std::cout << "set flag going S on rect " << bpi->output_cart() << std::endl;
                }
            }

            // Check that the boundary is contiguous, starting from SW corner and
            // heading E (to go anticlockwise)
            std::set<std::uint32_t> seen;
            std::list<sm::rect>::iterator ri = bpi;
            if (this->boundary_contiguous (bpi, ri, seen, RECT_NEIGHBOUR_POS_E) == false) {
                throw std::runtime_error ("The boundary is not a contiguous sequence of rects.");
            }

            if (this->domain_shape == sm::griddomainshape::boundary) {
                // Boundary IS contiguous, discard rects outside the boundary.
                this->discard_outside_boundary();
            }

            this->populate_d_vectors();
        }

        /*!
         * Get all the boundary rects in a list. This assumes that a boundary has already been set
         * with one of the set_boundary() methods and so there is therefore a set of rects which are
         * already marked as being on the boundary (with the attribute fn rect::boundary_rect()
         * returning true) Do this by going around the boundary neighbour to neighbour?
         *
         * Now a getter for this->brects.
         */
        std::list<rect> get_boundary() const
        {
            std::list<sm::rect> brects_concrete;
            auto rr = this->brects.begin();
            while (rr != this->brects.end()) {
                brects_concrete.push_back (*(*rr));
                ++rr;
            }
            return brects_concrete;
        }

        /*!
         * Compute a set of coordinates arranged on an ellipse
         * \param a first elliptical radius
         * \param b second elliptical radius
         * \param c centre argument so that the ellipse centre is offset from the coordinate origin
         * \return A vector of the coordinates of points on the generated ellipse
         */
        std::vector<sm::bezcoord<float>> ellipse_compute (const float a, const float b,
                                                          const sm::vec<float, 2> c = {0.0f, 0.0f}) const
        {
            // Compute the points on the boundary using the parametric elliptical formula and
            // half of the rect to rect spacing as the angular step size. Return as bpoints.
            std::vector<sm::bezcoord<float>> bpoints;

            // Estimate a good delta_phi based on the larger of a and b. Compute the delta_phi
            // required to travel a fraction of one rect-to-rect distance.
            double delta_phi = 0.0;
            double dfraction = this->d / 2.0;
            if (a > b) {
                delta_phi = std::atan2 (dfraction, a);
            } else {
                delta_phi = std::atan2 (dfraction, b);
            }

            // Loop around phi, computing x and y of the elliptical boundary and filling up bpoints
            for (double phi = 0.0; phi < sm::mathconst<double>::two_pi; phi+=delta_phi) {
                sm::vec<float, 2> xy_pt = { static_cast<float>(a * std::cos (phi) + c[0]), static_cast<float>(b * std::sin (phi) + c[1]) };
                sm::bezcoord<float> b(xy_pt);
                bpoints.push_back (b);
            }

            return bpoints;
        }

        //! calculate perimeter of ellipse with radii \a a and \a b
        float ellipse_perimeter (const float a, const float b) const
        {
            double apb = (double)a+b;
            double amb = (double)a-b;
            double h = amb * amb / (apb * apb);
            // Compute approximation to the ellipses perimeter (7 terms)
            double sum = 1.0
            + (0.25)      * h
            + (1.0/64.0)  * h * h
            + (1.0/256.0) * h * h * h
            + (25.0/16384.0) * h * h * h * h
            + (49.0/65536.0) * h * h * h * h * h
            + (441.0/1048576.0) * h * h * h * h * h * h;
            double p = sm::mathconst<double>::pi * apb * sum;

            return (float)p;
        }

        /*!
         * Set the boundary to be an ellipse with the given radii parameters a and b.
         * \param a first elliptical radius
         * \param b second elliptical radius
         * \param c allows the centre of the ellipse to be offset from the coordinate origin
         * \param offset determines if boundary is recentred or remains in place
         */
        void set_elliptical_boundary (const float a, const float b,
                                      const sm::vec<float, 2> c = {0.0f, 0.0f}, const bool offset=true)
        {
            std::vector<sm::bezcoord<float>> bpoints = ellipse_compute (a, b, c);
            this->set_boundary (bpoints, offset);
        }

        /*!
         * Set the boundary to be a circle with the given radius a.
         * \param a The radius of the circle
         * \param c allows the centre of the circle to be offset from the coordinate origin
         * \param offset determines if boundary is recentred or remains in place
         */
        void set_circular_boundary (const float a,
                                    const sm::vec<float, 2> c = {0.0f, 0.0f}, const bool offset=true)
        {
            std::vector<sm::bezcoord<float>> bpoints = ellipse_compute (a, a, c);
            this->set_boundary (bpoints, offset);
        }

        /*!
         * \brief Accessor for the size of rects.
         *
         * return The number of rects in the grid.
         */
        std::uint32_t num() const { return this->rects.size(); }

        /*!
         * \brief Obtain the vector index of the last rect in rects.
         *
         * return rect::vi from the last rect in the grid.
         */
        std::uint32_t last_vector_index() const { return this->rects.rbegin()->vi; }

        /*!
         * Output some text information about the hexgrid.
         */
        std::string output() const
        {
            std::stringstream ss;
            ss << "rect grid with " << this->rects.size() << " rects:\n";
            auto i = this->rects.begin();
            while (i != this->rects.end()) {
                ss << i->output() << std::endl;
                ++i;
            }
            return ss.str();
        }

        /*!
         * Returns the width of the cartgrid (from -x to +x)
         */
        float width() const
        {
            // {ximin, ximax, yimin, yimax}
            if constexpr (debug_cartgrid) {
                std::cout << "cartgrid::width(): calling find_boundary_extents()\n";
            }
            std::array<std::int32_t, 4> extents = this->find_boundary_extents();
            float xmin = this->d * float(extents[0]);
            float xmax = this->d * float(extents[1]);
            return (xmax - xmin);
        }

        /*!
         * Returns the visible width of the cartgrid if it were visualized as pixels (assuming
         * rectangular)
         */
        float width_of_pixels() const { return this->x_span + this->d; }

        sm::vec<float, 4> get_extents() const
        {
            sm::vec<std::int32_t, 4> extents;
            if constexpr (debug_cartgrid) {
                std::cout << "cartgrid::get_extents(): calling find_boundary_extents()\n";
            }
            extents.set_from (this->find_boundary_extents());
            sm::vec<float, 4> extents_mult = { this->d, this->d, this->v, this->v };
            return (extents.as_float() * extents_mult);
        }

        //! Return the number of elements that the cartgrid is wide
        std::int32_t widthnum() const
        {
            // {ximin, ximax, yimin, yimax}
            if constexpr (debug_cartgrid) {
                std::cout << "cartgrid::widthnum(): calling find_boundary_extents()\n";
            }
            std::array<std::int32_t, 4> extents = this->find_boundary_extents();
            std::int32_t wn = std::abs(extents[0]) + std::abs(extents[1]) + 1;
            return wn;
        }

        //! A faster widthnum function which only works if your cartgrid is rectangular
        std::int32_t widthnum_rectangular() const
        {
            return 1 + static_cast<std::int32_t>(std::round(this->x_span / this->d));
        }

        /*!
         * Returns the 'depth' of the cartgrid (from -y to +y)
         */
        float depth() const
        {
            if constexpr (debug_cartgrid) {
                std::cout << "cartgrid::depth(): calling find_boundary_extents()\n";
            }
            std::array<std::int32_t, 4> extents = this->find_boundary_extents();
            float ymin = this->v * float(extents[2]);
            float ymax = this->v * float(extents[3]);
            return (ymax - ymin);
        }

        //! Return the number of elements that the cartgrid is deep (or high) - y
        std::int32_t depthnum() const
        {
            // {ximin, ximax, yimin, yimax}
            if constexpr (debug_cartgrid) {
                std::cout << "cartgrid::depthnum(): calling find_boundary_extents()\n";
            }
            std::array<std::int32_t, 4> extents = this->find_boundary_extents();
            std::int32_t dn = std::abs(extents[2]) + std::abs(extents[3]) + 1;
            return dn;
        }

        //! Faster depthnum function which only works if cartgrid is rectangular
        std::int32_t depthnum_rectangular() const
        {
            return 1 + static_cast<std::int32_t>(std::round(this->y_span / this->v));
        }

        /*!
         * Getter for d.
         */
        float get_d() const { return this->d; }

        /*!
         * Getter for v - vertical rect spacing.
         */
        float get_v() const { return this->v; }

        //! Get the x_span/y_span
        sm::vec<float, 2> get_span() const { return sm::vec<float, 2>{this->x_span, this->y_span}; }

        //! Get the x/y span in elements/pixels
        sm::vec<std::uint32_t, 2> get_span_pix() const
        {
            std::uint32_t _x_pixdist = static_cast<std::uint32_t>(std::round(this->x_span/this->d));
            std::uint32_t _y_pixdist = static_cast<std::uint32_t>(std::round(this->y_span/this->v));
            return sm::vec<std::uint32_t, 2>{ 1 + _x_pixdist, 1 + _y_pixdist };
        }

        /*!
         * Get the shortest distance from the centre to the perimeter. This is the
         * "short radius".
         */
        float get_sr() const { return 0.5f * this->d; }

        /*!
         * The distance from the centre of the rect to any of the vertices. This is the
         * "long radius".
         */
        float get_lr() const { return 0.5f * std::sqrt (this->d * this->d + this->v * this->v); }

        /*!
         * The vertical distance from the centre of the rect to the "north east" vertex
         * of the rect.
         */
        float get_v_to_ne() const { return 0.5f * this->v; }

        /*!
         * Compute and return the area of one rect in the grid.
         */
        float get_rect_area() const { return this->d * this->v; }

        /*!
         * Run through all the rects and compute the distance to the nearest boundary
         * rect.
         */
        void compute_distance_to_boundary()
        {
            std::list<sm::rect>::iterator r = this->rects.begin();
            while (r != this->rects.end()) {
                if (r->test_flags(RECT_IS_BOUNDARY) == true) {
                    r->dist_to_boundary = 0.0f;
                } else {
                    if (r->test_flags(RECT_INSIDE_BOUNDARY) == false) {
                        // Set to a dummy, negative value
                        r->dist_to_boundary = -100.0;
                    } else {
                        // Not a boundary rect, but inside boundary
                        std::list<sm::rect>::iterator br = this->rects.begin();
                        while (br != this->rects.end()) {
                            if (br->test_flags(RECT_IS_BOUNDARY) == true) {
                                float delta = r->distance_from (*br);
                                if (delta < r->dist_to_boundary || r->dist_to_boundary < 0.0f) {
                                    r->dist_to_boundary = delta;
                                }
                            }
                            ++br;
                        }
                    }
                }
                ++r;
            }
        }

        /*!
         * Populate d_ vectors. simple version. (Finds extents, then calls
         * populate_d_vectors(const array<std::int32_t, 4>&)
         */
        void populate_d_vectors()
        {
            if constexpr (debug_cartgrid) {
                std::cout << "cartgrid::populate_d_vectors(): calling find_boundary_extents()\n";
            }
            std::array<std::int32_t, 4> extnts = this->find_boundary_extents();
            this->populate_d_vectors (extnts);
        }

        /*!
         * Populate d_ vectors, paying attention to domain_shape.
         */
        void populate_d_vectors (const std::array<std::int32_t, 4>& extnts)
        {
            // A rectangle iterator
            std::list<sm::rect>::iterator ri = this->rects.begin();
            // Bottom left rectangle
            std::list<sm::rect>::iterator blr = this->rects.end();

            if (this->domain_shape == sm::griddomainshape::rectangle) {

                // Use neighbour relations to go from bottom left to top right.  Find rect on bottom row.
                while (ri != this->rects.end()) {
                    if (ri->yi == extnts[2]) {
                        // std::cout << "We're on the bottom row\n";
                        break;
                    }
                    ++ri;
                }
                // ri now on bottom row; so travel west
                while (ri->has_nw() == true && !ri->wraps_w()) { ri = ri->nw; }
                // ri should now be the bottom left rect.
                blr = ri;

            } // else Hexagon or Boundary starts from 0, hi already set to rects.begin();

            // Clear the d_ vectors.
            this->d_clear();

            // Now raster through the rects, building the d_ vectors.
            if (this->domain_shape == sm::griddomainshape::rectangle) {

                this->d_push_back (ri);

                do {
                    if (ri->has_ne() == false || (ri->has_ne() && ri->wraps_e())) {
                        if (ri->yi == extnts[3]) {
                            // last (i.e. top) row and no neighbour east, so finished.
                            break;
                        } else {
                            // Carriage return
                            ri = blr;
                            // Line feed (Move to the next row up)
                            blr = ri->nn;
                            ri = blr;
                            this->d_push_back (ri);
                        }
                    } else {
                        ri = ri->ne;
                        this->d_push_back (ri);
                    }

                } while ((ri->has_ne() == true && !ri->wraps_e())
                         || (ri->has_nn() == true && !ri->wraps_n()));

            } else { // Boundary

                while (ri != this->rects.end()) {
                    this->d_push_back (ri);
                    ri++;
                }
            }

            // Create vectors containing the min and max x and y indices. Bottom left element is
            // d_xi[0] and top right is represented by d_yi[last]
            this->xi_minmax = sm::range<std::int32_t>(d_xi[0], d_xi[d_xi.size()-1]);
            this->yi_minmax = sm::range<std::int32_t>(d_yi[0], d_yi[d_yi.size()-1]);

            this->populate_d_neighbours();
        }

        /*!
         * Get a vector of rect pointers for all rects that are inside/on the path
         * defined by the bezcurvepath \a p, thus this gets a 'region of rects'. The rect
         * flags "region" and "region_boundary" are used, temporarily to mark out the
         * region. The idea is that client code will then use the vector of sm::rect* to work
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
         * \return a vector of iterators to the rects that make up the region.
         */
        std::vector<std::list<rect>::iterator> get_region (sm::bezcurvepath<float>& p,
                                                           sm::vec<float, 2>& region_centroid,
                                                           bool apply_original_boundary_centroid = true)
        {
            p.compute_points (this->d/2.0f, true);
            std::vector<sm::bezcoord<float>> bpoints = p.get_points();
            return this->get_region (bpoints, region_centroid, apply_original_boundary_centroid);
        }

        /*!
         * The overload of get_region that does all the work on a vector of coordinates
         */
        std::vector<std::list<rect>::iterator> get_region (std::vector<sm::bezcoord<float>>& bpoints,
                                                           sm::vec<float, 2>& region_centroid,
                                                           bool apply_original_boundary_centroid = true)
        {
            // First clear all region boundary flags, as we'll be defining a new region boundary
            this->clear_region_boundary_flags();

            // Compute region centroid from bpoints
            region_centroid = sm::bezcurvepath<float>::get_centroid (bpoints);

            // A return object
            std::vector<std::list<sm::rect>::iterator> the_region;

            if (apply_original_boundary_centroid) {
                auto bpi = bpoints.begin();
                while (bpi != bpoints.end()) {
                    bpi->subtract (this->original_boundary_centroid);
                    ++bpi;
                }

                // Subtract original_boundary_centroid from region centroid so that region centroid is translated
                region_centroid -= this->original_boundary_centroid;
            }

            // Now find the rects on the boundary of the region
            std::list<sm::rect>::iterator nearby_region_boundary_point = this->rects.begin(); // i.e the rect at 0,0
            typename std::vector<sm::bezcoord<float>>::iterator bpi = bpoints.begin();
            while (bpi != bpoints.end()) {
                nearby_region_boundary_point = this->set_region_boundary (*bpi++, nearby_region_boundary_point);
            }

            // Check that the region boundary is contiguous.
            {
                std::set<std::uint32_t> seen;
                std::list<sm::rect>::iterator hi = nearby_region_boundary_point;
                if (this->region_boundary_contiguous (nearby_region_boundary_point, hi, seen) == false) {
                    std::stringstream ee;
                    ee << "The constructed region boundary is not a contiguous sequence of rects.";
                    return the_region;
                }
            }

            // Mark rects inside region. Use centroid of the region.
            std::list<sm::rect>::iterator inside_region_rect = this->find_rect_nearest (region_centroid);
            this->mark_rects_inside (inside_region_rect, RECT_IS_REGION_BOUNDARY, RECT_INSIDE_REGION);

            // Populate the_region, then return it
            std::list<sm::rect>::iterator hi = this->rects.begin();
            while (hi != this->rects.end()) {
                if (hi->test_flags (RECT_INSIDE_REGION) == true) {
                    the_region.push_back (hi);
                }
                ++hi;
            }

            return the_region;
        }

        /*!
         * For every rect in rects, unset the flags RECT_IS_REGION_BOUNDARY and
         * RECT_INSIDE_REGION
         */
        void clear_region_boundary_flags()
        {
            for (auto& rr : this->rects) {
                rr.unset_flag (RECT_IS_REGION_BOUNDARY | RECT_INSIDE_REGION);
            }
        }

        /*!
         * Apply an on-centre/off-surround filter by convolving the data on this
         * cartgrid with a filter whose centre is of size one pixel with the value 1,
         * surrounded by a ring of up to n=8 pixels, set to the value -1/n. The sum of
         * the filter is thus always 0. n<8 if the central pixel is close to the edge of
         * the cartgrid.
         */
        template<typename T>
        void oncentre_offsurround (const std::vector<T>& data, std::vector<T>& result) const
        {
            if (result.size() != this->rects.size()) {
                throw std::runtime_error ("The result vector is not the same size as the cartgrid.");
            }
            if (result.size() != data.size()) {
                throw std::runtime_error ("The data vector is not the same size as the cartgrid.");
            }
            if (&data == &result) {
                throw std::runtime_error ("Pass in separate memory for the result.");
            }
            // For each rect in this cartgrid, compute the convolution kernel
            for (std::list<rect>::const_iterator ri = this->rects.cbegin(); ri != this->rects.cend(); ++ri) {
                result[ri->vi] = data[ri->vi]; // The 'on' part of the filter
                T count = T{0};
                T offpart = T{0};
                offpart += ri->has_ne()  ? count+=T{1}, data[ri->ne->vi]  : T{0};
                offpart += ri->has_nne() ? count+=T{1}, data[ri->nne->vi] : T{0};
                offpart += ri->has_nn()  ? count+=T{1}, data[ri->nn->vi]  : T{0};
                offpart += ri->has_nnw() ? count+=T{1}, data[ri->nnw->vi] : T{0};
                offpart += ri->has_nw()  ? count+=T{1}, data[ri->nw->vi]  : T{0};
                offpart += ri->has_nsw() ? count+=T{1}, data[ri->nsw->vi] : T{0};
                offpart += ri->has_ns()  ? count+=T{1}, data[ri->ns->vi]  : T{0};
                offpart += ri->has_nse() ? count+=T{1}, data[ri->nse->vi] : T{0};
                //std::cout << "subtract " << offpart << "/" << count << std::endl;
                result[ri->vi] -= offpart / count;
            }
        }

        //! Apply a box filter. SLOOOOOW algorithm.
        template<typename T, bool onlysum=false>
        void boxfilter (const std::vector<T>& data, std::vector<T>& result, const std::uint32_t boxside)
        {
            if (result.size() != this->rects.size()) {
                throw std::runtime_error ("The result vector is not the same size as the cartgrid.");
            }
            if (result.size() != data.size()) {
                throw std::runtime_error ("The data vector is not the same size as the cartgrid.");
            }
            if (&data == &result) {
                throw std::runtime_error ("Pass in separate memory for the result.");
            }

            // At each pixel/rect sum up the contributions from a square box of side
            // boxside. This is symmetric if boxside is odd.
            //
            // On either side, walk [(box side - 1) / 2] steps, if boxside is odd.  If
            // boxside is even, then one walk (right/up) is boxside/2, then other
            // (left/down) is (boxside/2)-1
            std::uint32_t neg_steps = boxside%2==0 ? (boxside/2) - 1 : (boxside-1)/2;
            std::uint32_t pos_steps = boxside%2==0 ? (boxside/2) : (boxside-1)/2;
            T oneover_boxa = T{1} / (static_cast<T>(boxside) * static_cast<T>(boxside)); // 1/ square box area

            // Now can go through the rects
            std::list<rect>::iterator ri = this->rects.begin();
            for (; ri != this->rects.end(); ++ri) {
                // On each rect, sum up the contributions from neighbours. This is a
                // naive, slow, but easy to code algorithm. It can be a bit faster if
                // you keep a track of the sum in the box filter.
                std::list<rect>::iterator ri_col = ri;
                std::list<rect>::iterator ri_row = ri;

                // First step down to a starting point, without summing
                std::uint32_t act_neg_steps = 0;
                for (std::uint32_t i = 0; i < neg_steps; ++i) {
                    if (ri_row->has_ns()) {
                        ri_row = ri_row->ns;
                        ++act_neg_steps;
                    }
                }

                result[ri->vi] = T{0};

                // Should now be at the bottom of the square.
                for (std::uint32_t j = 0; j < (act_neg_steps + 1 + pos_steps); ++j) {

                    ri_col = ri_row; // middle of row

                    result[ri->vi] += data[ri_col->vi]; // add value of middle pixel in row

                    // Step left neg_steps, first, summing
                    for (std::uint32_t i = 0; i < neg_steps; ++i) {
                        if (ri_col->has_nw()) { // May wrap, that's ok
                            ri_col = ri_col->nw;
                            result[ri->vi] += data[ri_col->vi];
                        } // else nothing to add.
                    }
                    // Step right pos_steps, summing
                    ri_col = ri_row; // back to middle
                    for (std::uint32_t i = 0; i < pos_steps; ++i) {
                        if (ri_col->has_ne()) {
                            ri_col = ri_col->ne;
                            result[ri->vi] += data[ri_col->vi];
                        }
                    }

                    if (ri_row->has_nn()) {
                        ri_row = ri_row->nn;
                    } else {
                        break;
                    }
                }

                if constexpr (onlysum == false) {
                    result[ri->vi] *= oneover_boxa;
                }
            }
        }

        // Apply a box filter. Be fast. Rectangular cartgrids only. Test to see if boxside is odd and disallow even (not tested)
        template<typename T, std::int32_t boxside, bool onlysum = false>
        void boxfilter_f (const sm::vvec<T>& data, sm::vvec<T>& result) const
        {
            if (result.size() != this->rects.size()) {
                throw std::runtime_error ("The result vector is not the same size as the cartgrid.");
            }
            if (this->domain_shape != griddomainshape::rectangle) {
                throw std::runtime_error ("This method requires a rectangular cartgrid.");
            }
            if (this->domain_wrap != griddomainwrap::horizontal) {
                throw std::runtime_error ("This method ASSUMES the cartgrid is horizontally wrapped.");
            }
            // check w_px >= boxside and h_px >= boxside
            if (this->w_px < boxside || this->h_px < boxside) {
                throw std::runtime_error ("boxfilter_f was not designed for cartgrids smaller than the box filter square");
            }
            // We are a wrapper:
            sm::algo::boxfilter_2d<T, boxside, onlysum> (data, result, this->w_px);
        }

        /*!
         * Using this cartgrid as the domain, convolve the domain data \a data with the
         * kernel data \a kerneldata, which exists on another cartgrid, \a
         * kernelgrid. Return the result in \a result.
         */
        template<typename T>
        void convolve (const cartgrid& kernelgrid, const std::vector<T>& kerneldata,
                       const std::vector<T>& data, std::vector<T>& result)
        {
            if (result.size() != this->rects.size()) {
                throw std::runtime_error ("The result vector is not the same size as the cartgrid.");
            }
            if (result.size() != data.size()) {
                throw std::runtime_error ("The data vector is not the same size as the cartgrid.");
            }
            // Could relax this test...
            if (kernelgrid.get_d() != this->d) {
                throw std::runtime_error ("The kernel cartgrid must have same d as this cartgrid to carry out convolution.");
            }
            if (&data == &result) {
                throw std::runtime_error ("Pass in separate memory for the result.");
            }

            // For each rect in this cartgrid, compute the convolution kernel
            std::list<rect>::iterator ri = this->rects.begin();

            for (; ri != this->rects.end(); ++ri) {
                T sum = T{0};
                // For each kernel rect, sum up.
                for (auto kr : kernelgrid.rects) {
                    std::list<rect>::iterator dri = ri;
                    std::int32_t xx = kr.xi;
                    std::int32_t yy = kr.yi;
                    bool failed = false;
                    bool finished = false;

                    while (!finished) {
                        bool moved = false;
                        // Try to move in x direction
                        if (xx > 0) { // Then kernel rect is to right of 0, so relevant rect on Cartgrid is to east
                            if (dri->has_ne()) {
                                dri = dri->ne;
                                --xx;
                                moved = true;
                            } // Didn't move in +x direction
                        } else if (xx < 0) {
                            if (dri->has_nw()) {
                                dri = dri->nw;
                                ++xx;
                                moved = true;
                            } // Didn't move in -x direction
                        }
                        // Try to move in y direction
                        if (yy > 0) {
                            if (dri->has_nn()) {
                                dri = dri->nn;
                                --yy;
                                moved = true;
                            } // Didn't move in +y direction
                        } else if (yy < 0) {
                            if (dri->has_ns()) {
                                dri = dri->ns;
                                ++yy;
                                moved = true;
                            } // Didn't move in -y direction
                        }

                        if (xx == 0 && yy == 0) {
                            finished = true;
                            break;
                        }

                        if (!moved) {
                            // We're stuck; Can't move in x or y direction, so can't add a contribution
                            failed = true;
                            break;
                        }
                    }

                    if (!failed) {
                        // Can do the sum
                        sum +=  data[dri->vi] * kerneldata[kr.vi];
                    }
                }

                result[ri->vi] = sum;
            }
        }

        /*!
         * What shape domain to set? Set this to the non-default BEFORE calling
         * cartgrid::set_boundary (const bezcurvepath& p) - that's where the domain_shape
         * is applied.
         */
        griddomainshape domain_shape = griddomainshape::rectangle;

        //! Edge wrapping? none, horizontal, vertical or both.
        griddomainwrap domain_wrap = griddomainwrap::none;

        /*!
         * The list of rects that make up this cartgrid.
         */
        std::list<rect> rects;

        /*!
         * Once boundary secured, fill this vector. Experimental - can I do parallel
         * loops with vectors of rects? Ans: Not very well.
         */
        std::vector<rect*> vrects;

        /*!
         * While determining if boundary is continuous, fill this maps container of
         * rects.
         */
        std::list<const rect*> brects; // Not better as a separate list<rect>?

        /*!
         * Store the centroid of the boundary path. The centroid of a read-in
         * bezcurvepath [see void set_boundary (const bezcurvepath& p)] is subtracted
         * from each generated point on the boundary path so that the boundary once it
         * is expressed in the cartgrid will have a (2D) centroid of roughly
         * (0,0). Hence, this is usually roughly (0,0).
         */
        sm::vec<float, 2> boundary_centroid = { 0.0f, 0.0f };

        /*!
         * Holds the centroid of the boundary before all points on the boundary were
         * translated so that the centroid of the boundary would be 0,0
         */
        sm::vec<float, 2> original_boundary_centroid = { 0.0f, 0.0f };

    private:
        /*!
         * Initialise a grid of rects in a raster fashion, setting neighbours as we
         * go. This method populates rects based on the grid parameters set in d, v and
         * x_span.
         */
        void init()
        {
            // Use x_span to determine how many cols
            float half_x = this->x_span/2.0f;
            std::int32_t half_cols = std::abs(std::ceil(half_x/this->d));
            // Use y_span to determine how many rows
            float half_y = this->y_span/2.0f;
            std::int32_t half_rows = std::abs(std::ceil(half_y/this->v));

            if (this->domain_shape == griddomainshape::rectangle) {
                this->w_px = 2 * half_rows + 1;
                this->h_px = 2 * half_cols + 1;
            }

            this->x_minmax = sm::range<float>(-half_cols * this->d, half_cols * this->d);
            this->y_minmax = sm::range<float>(-half_rows * this->v, half_rows * this->v);

            // The "vector iterator" - this is an identity iterator that is added to each rect in the grid.
            std::uint32_t vi = 0;

            std::vector<std::list<sm::rect>::iterator> prev_row_even;
            std::vector<std::list<sm::rect>::iterator> prev_row_odd;

            // Swap pointers between rows.
            std::vector<std::list<sm::rect>::iterator>* prev_row = &prev_row_even;
            std::vector<std::list<sm::rect>::iterator>* next_prev_row = &prev_row_odd;

            // Build grid, raster style.
            for (std::int32_t yi = -half_rows; yi <= half_rows; ++yi) {
                std::uint32_t pri = 0U;
                for (std::int32_t xi = -half_cols; xi <= half_cols; ++xi) {
                    this->rects.emplace_back (vi++, this->d, this->v, xi, yi);

                    auto ri = this->rects.end(); ri--;
                    this->vrects.push_back (&(*ri));

                    //std::cout << "emplaced rect " << ri->output_cart() << std::endl;
                    if (xi > -half_cols) {
                        auto ri_w = ri; ri_w--;
                        ri_w->set_ne (ri);
                        ri->set_nw (ri_w);
                    }
                    if (yi > -half_rows) {
                        //std::cout << "For (xi,yi) = (" << xi << "," << yi << ") set rect (*prev_row)[" << pri << "]"
                        //          << (*prev_row)[pri]->output_cart() << " as S of rect ri = " << ri->output_cart() << std::endl;
                        (*prev_row)[pri]->set_nn (ri);
                        ri->set_ns ((*prev_row)[pri]);
                        if (xi > -half_cols) {
                            //std::cout << "For (xi,yi) = (" << xi << "," << yi << ") set rect (*prev_row)[" << (pri-1) << "]"
                            //          << (*prev_row)[pri-1]->output_cart() << " as SW of rect ri = " << ri->output_cart() << std::endl;
                            (*prev_row)[pri-1]->set_nne (ri);
                            ri->set_nsw ((*prev_row)[pri-1]);
                        }
                        if (xi < half_cols) {
                            (*prev_row)[pri+1]->set_nnw (ri);
                            ri->set_nse ((*prev_row)[pri+1]);
                        }
                    }
                    ++pri;
                    next_prev_row->push_back (ri);
                }
                // Swap prev_row and next_prev_row.
                std::vector<std::list<sm::rect>::iterator>* tmp = prev_row;
                prev_row = next_prev_row;
                next_prev_row = tmp;
                next_prev_row->clear();
            }
        }

        //! Initialize a non-symmetric rectangular grid.
        void init2 (float x1, float y1, float x2, float y2)
        {
            if constexpr (debug_cartgrid) {
                std::cout << __FUNCTION__ << " called for ("<<x1<<","<<y1<<") to ("<<x2<<","<<y2<<")\n";
            }
            this->x_minmax = sm::range<float>(x1, x2);
            this->y_minmax = sm::range<float>(y1, y2);

            std::int32_t _xi = std::round(x1/this->d);
            std::int32_t _xf = std::round(x2/this->d);
            std::int32_t _yi = std::round(y1/this->v);
            std::int32_t _yf = std::round(y2/this->v);

            if (this->domain_shape == griddomainshape::rectangle) {
                this->w_px = _xf-_xi+1;
                this->h_px = _yf-_yi+1;
            }

            // The "vector iterator" - this is an identity iterator that is added to each rect in the grid.
            std::uint32_t vi = 0;

            std::vector<std::list<sm::rect>::iterator> prev_row_even;
            std::vector<std::list<sm::rect>::iterator> prev_row_odd;

            // Swap pointers between rows.
            std::vector<std::list<sm::rect>::iterator>* prev_row = &prev_row_even;
            std::vector<std::list<sm::rect>::iterator>* next_prev_row = &prev_row_odd;

            // Build grid, raster style.
            for (std::int32_t yi = _yi; yi <= _yf; ++yi) { // for each row
                std::uint32_t pri = 0U;
                for (std::int32_t xi = _xi; xi <= _xf; ++xi) { // for each element in row
                    this->rects.emplace_back (vi++, this->d, this->v, xi, yi);

                    auto ri = this->rects.end(); ri--;
                    this->vrects.push_back (&(*ri));

                    if (xi > _xi) {
                        auto ri_w = ri; ri_w--;
                        ri_w->set_ne (ri);
                        ri->set_nw (ri_w);
                    }
                    if (yi > _yi) {
                        (*prev_row)[pri]->set_nn (ri);
                        ri->set_ns ((*prev_row)[pri]);
                        if (xi > _xi) {
                            (*prev_row)[pri-1]->set_nne (ri);
                            ri->set_nsw ((*prev_row)[pri-1]);
                        }
                        if (xi < _xf) {
                            (*prev_row)[pri+1]->set_nnw (ri);
                            ri->set_nse ((*prev_row)[pri+1]);
                        }
                    }
                    ++pri;
                    next_prev_row->push_back (ri);
                }
                // Now row has been created, can complete the wraparound links (if necessary). *next_prev_row is the current row.
                if (this->domain_wrap == sm::griddomainwrap::horizontal || this->domain_wrap == sm::griddomainwrap::both) {
                    (*next_prev_row)[0]->set_nw ((*next_prev_row)[_xf-_xi]);
                    (*next_prev_row)[0]->set_wraps_w();
                    (*next_prev_row)[_xf-_xi]->set_ne ((*next_prev_row)[0]);
                    (*next_prev_row)[_xf-_xi]->set_wraps_e();
                }

                // Swap prev_row and next_prev_row.
                std::vector<std::list<sm::rect>::iterator>* tmp = prev_row;
                prev_row = next_prev_row;
                next_prev_row = tmp;
                next_prev_row->clear();
            }
            if constexpr (debug_cartgrid) {
                std::cout << "init2() end: emplaced " << vi << " rects\n";
            }
        }

        /*!
         * Starting from \a start_from, and following nearest-neighbour relations, find
         * the closest rect in rects to the coordinate point \a point, and set its
         * rect::on_boundary attribute to true.
         *
         * \return An iterator into cartgrid::rects which refers to the closest rect to \a point.
         */
        std::list<sm::rect>::iterator set_boundary (const sm::bezcoord<float>& point,
                                                    std::list<sm::rect>::iterator start_from)
        {
            std::list<sm::rect>::iterator h = this->find_rect_near_point (point, start_from);
            h->set_flag (RECT_IS_BOUNDARY | RECT_INSIDE_BOUNDARY);
            return h;
        }

        // ASSUMING that the boundary is rectangular, is the point inside the rectangle?
        bool is_inside_rectangular_boundary (const sm::vec<float, 2>& point)
        {
            if (point[0] < this->x_minmax.min) { return false; }
            if (point[0] > this->x_minmax.max) { return false; }
            if (point[1] < this->y_minmax.min) { return false; }
            if (point[1] > this->y_minmax.max) { return false; }
            return true;
        }

        /*!
         * Determine whether the boundary is contiguous. Whilst doing so, populate a
         * list<rect> containing just the boundary rects.
         */
        bool boundary_contiguous()
        {
            this->brects.clear();
            std::list<sm::rect>::const_iterator bhi = this->rects.begin();
            if (this->find_boundary_rect (bhi) == false) {
                // Found no boundary rect
                return false;
            }
            std::set<std::uint32_t> seen;
            std::list<sm::rect>::const_iterator hi = bhi;
            return this->boundary_contiguous (bhi, hi, seen, RECT_NEIGHBOUR_POS_E);
        }

        /*!
         * Determine whether the boundary is contiguous, starting from the boundary
         * rect iterator \a bhi.
         *
         * If following the boundary clockwise, then need to search neighbours "starting
         * from the one that's clockwise around from the last one that was on the
         * boundary"
         *
         * The overload with brects takes a list of rect pointers and populates it with
         * pointers to the rects on the boundary.
         */
        bool boundary_contiguous (std::list<rect>::const_iterator bri,
                                  std::list<rect>::const_iterator ri, std::set<std::uint32_t>& seen, std::int32_t dirn)
        {
            bool rtn = false;
            std::list<sm::rect>::const_iterator ri_next;
            seen.insert (ri->vi);
            // Insert into the std::list of rect pointers, too
            this->brects.push_back (&(*ri));

            // increasing direction is an anticlockwise sense
            for (std::int32_t i = 0; i < 8; ++i) {
                if (rtn == false
                    && ri->has_neighbour(dirn+i)
                    && ri->get_neighbour(dirn+i)->test_flags(RECT_IS_BOUNDARY) == true
                    && seen.find(ri->get_neighbour(dirn+i)->vi) == seen.end()) {
                    //std::cout << ri->neighbour_pos(dirn+i) << std::endl;
                    ri_next = ri->get_neighbour(dirn+i);
                    rtn = (this->boundary_contiguous (bri, ri_next, seen, dirn+i));
                }
            }

            if (rtn == false) {
                // Checked all neighbours
                if (ri == bri) {
                    // Back at start, nowhere left to go! return true.
                    rtn = true;
                }
            }

            return rtn;
        }

        /*!
         * Set the rect closest to point as being on the region boundary. Region
         * boundaries are supposed to be temporary, so that client code can find a
         * region, extract the pointers to all the rects in that region and store that
         * information for later use.
         */
        std::list<rect>::iterator set_region_boundary (const sm::bezcoord<float>& point, std::list<rect>::iterator start_from)
        {
            std::list<sm::rect>::iterator h = this->find_rect_near_point (point, start_from);
            h->set_flag (RECT_IS_REGION_BOUNDARY | RECT_INSIDE_REGION);
            return h;
        }

        /*!
         * Set the rect closest to point as being on the region boundary. Region
         * boundaries are supposed to be temporary, so that client code can find a
         * region, extract the pointers to all the rects in that region and store that
         * information for later use.
         */
        std::list<rect>::iterator set_region_boundary (const sm::vec<float, 2>& point, std::list<rect>::iterator start_from)
        {
            std::list<sm::rect>::iterator h = this->find_rect_near_point (point, start_from);
            h->set_flag (RECT_IS_REGION_BOUNDARY | RECT_INSIDE_REGION);
            return h;
        }

        /*!
         * Determine whether the region boundary is contiguous, starting from the
         * boundary rect iterator #bhi.
         */
        bool region_boundary_contiguous (std::list<rect>::const_iterator bhi,
                                         std::list<rect>::const_iterator hi, std::set<std::uint32_t>& seen)
        {
            bool rtn = false;
            std::list<sm::rect>::const_iterator hi_next;
            seen.insert (hi->vi);
            // Insert into the list of rect pointers, too
            this->brects.push_back (&(*hi));

            if (rtn == false && hi->has_ne() && hi->ne->test_flags(RECT_IS_REGION_BOUNDARY) == true && seen.find(hi->ne->vi) == seen.end()) {
                hi_next = hi->ne;
                rtn = (this->region_boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nne() && hi->nne->test_flags(RECT_IS_REGION_BOUNDARY) == true && seen.find(hi->nne->vi) == seen.end()) {
                hi_next = hi->nne;
                rtn = this->region_boundary_contiguous (bhi, hi_next, seen);
            }
            if (rtn == false && hi->has_nnw() && hi->nnw->test_flags(RECT_IS_REGION_BOUNDARY) == true && seen.find(hi->nnw->vi) == seen.end()) {
                hi_next = hi->nnw;
                rtn =  (this->region_boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nw() && hi->nw->test_flags(RECT_IS_REGION_BOUNDARY) == true && seen.find(hi->nw->vi) == seen.end()) {
                hi_next = hi->nw;
                rtn =  (this->region_boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nsw() && hi->nsw->test_flags(RECT_IS_REGION_BOUNDARY) == true && seen.find(hi->nsw->vi) == seen.end()) {
                hi_next = hi->nsw;
                rtn =  (this->region_boundary_contiguous (bhi, hi_next, seen));
            }
            if (rtn == false && hi->has_nse() && hi->nse->test_flags(RECT_IS_REGION_BOUNDARY) == true && seen.find(hi->nse->vi) == seen.end()) {
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
         * Find a rect, any rect, that's on the boundary specified by #boundary. This
         * assumes that set_boundary (const bezcurvepath&) has been called to mark the
         * rects that lie on the boundary.
         */
        bool find_boundary_rect (std::list<rect>::const_iterator& ri) const
        {
            if (ri->test_flags(RECT_IS_BOUNDARY) == true) {
                // No need to change the rect iterator
                return true;
            }

            // On a Cartesian grid should be able to simply go south until we hit a boundary rect
            if (ri->has_ns()) {
                std::list<sm::rect>::const_iterator ci(ri->ns);
                if (this->find_boundary_rect (ci) == true) {
                    ri = ci;
                    return true;
                }
            }

            return false;
        }

        std::list<rect>::iterator find_rect_near_point (const vec<float, 2>& point, std::list<rect>::iterator start_from)
        {
            bool neighbour_nearer = true;

            std::list<sm::rect>::iterator h = start_from;
            float d = h->distance_from (point);
            float d_ = 0.0f;

            while (neighbour_nearer == true) {

                neighbour_nearer = false;
                if (h->has_ne() && (d_ = h->ne->distance_from (point)) < d) {
                    d = d_;
                    h = h->ne;
                    neighbour_nearer = true;

                } else if (h->has_nne() && (d_ = h->nne->distance_from (point)) < d) {
                    d = d_;
                    h = h->nne;
                    neighbour_nearer = true;

                } else if (h->has_nnw() && (d_ = h->nnw->distance_from (point)) < d) {
                    d = d_;
                    h = h->nnw;
                    neighbour_nearer = true;

                } else if (h->has_nw() && (d_ = h->nw->distance_from (point)) < d) {
                    d = d_;
                    h = h->nw;
                    neighbour_nearer = true;

                } else if (h->has_nsw() && (d_ = h->nsw->distance_from (point)) < d) {
                    d = d_;
                    h = h->nsw;
                    neighbour_nearer = true;

                } else if (h->has_nse() && (d_ = h->nse->distance_from (point)) < d) {
                    d = d_;
                    h = h->nse;
                    neighbour_nearer = true;
                }
            }

            return h;
        }

        /*!
         * Find the rect near @point, starting from start_from, which should be as close
         * as possible to point in order to reduce computation time.
         */
        std::list<rect>::iterator find_rect_near_point (const sm::bezcoord<float>& point, std::list<rect>::iterator start_from)
        {
            bool neighbour_nearer = true;

            std::list<sm::rect>::iterator h = start_from;
            float d = h->distance_from (point);
            float d_ = 0.0f;

            while (neighbour_nearer == true) {

                neighbour_nearer = false;
                if (h->has_ne() && (d_ = h->ne->distance_from (point)) < d) {
                    d = d_;
                    h = h->ne;
                    neighbour_nearer = true;

                } else if (h->has_nne() && (d_ = h->nne->distance_from (point)) < d) {
                    d = d_;
                    h = h->nne;
                    neighbour_nearer = true;

                } else if (h->has_nnw() && (d_ = h->nnw->distance_from (point)) < d) {
                    d = d_;
                    h = h->nnw;
                    neighbour_nearer = true;

                } else if (h->has_nw() && (d_ = h->nw->distance_from (point)) < d) {
                    d = d_;
                    h = h->nw;
                    neighbour_nearer = true;

                } else if (h->has_nsw() && (d_ = h->nsw->distance_from (point)) < d) {
                    d = d_;
                    h = h->nsw;
                    neighbour_nearer = true;

                } else if (h->has_nse() && (d_ = h->nse->distance_from (point)) < d) {
                    d = d_;
                    h = h->nse;
                    neighbour_nearer = true;
                }
            }

            return h;
        }

        /*!
         * Mark rects as being inside the boundary given that \a hi refers to a boundary
         * rect and at least one adjacent rect to \a hi has already been marked as inside
         * the boundary (thus allowing the algorithm to know which side of the boundary
         * rect is the inside)
         *
         * \param hi list iterator to starting rect.
         *
         * By changing \a bdry_flag and \a inside_flag, it's possible to use this method
         * with region boundaries.
         */
        void mark_from_boundary (std::list<rect>::iterator hi,
                                 std::uint32_t bdry_flag = RECT_IS_BOUNDARY,
                                 std::uint32_t inside_flag = RECT_INSIDE_BOUNDARY)
        {
            this->mark_from_boundary (&(*hi), bdry_flag, inside_flag);
        }

        /*!
         * Mark rects as being inside the boundary given that \a hi refers to a boundary
         * rect and at least one adjacent rect to \a hi has already been marked as inside
         * the boundary (thus allowing the algorithm to know which side of the boundary
         * rect is the inside)
         *
         * \param hi list iterator to a pointer to the starting rect.
         *
         * By changing \a bdry_flag and \a inside_flag, it's possible to use this method
         * with region boundaries.
         */
        void mark_from_boundary (std::list<rect*>::iterator hi,
                                 std::uint32_t bdry_flag = RECT_IS_BOUNDARY,
                                 std::uint32_t inside_flag = RECT_INSIDE_BOUNDARY)
        {
            this->mark_from_boundary ((*hi), bdry_flag, inside_flag);
        }

        /*!
         * Mark rects as being inside the boundary given that \a hi refers to a boundary
         * rect and at least one adjacent rect to \a hi has already been marked as inside
         * the boundary (thus allowing the algorithm to know which side of the boundary
         * rect is the inside)
         *
         * \param hi pointer to the starting rect.
         *
         * By changing \a bdry_flag and \a inside_flag, it's possible to use this method
         * with region boundaries.
         */
        void mark_from_boundary (sm::rect* hi,
                                 std::uint32_t bdry_flag = RECT_IS_BOUNDARY,
                                 std::uint32_t inside_flag = RECT_INSIDE_BOUNDARY)
        {
            // Find a marked-inside rect next to this boundary rect. This will be the first direction to mark
            // a line of inside rects in.
            std::list<sm::rect>::iterator first_inside = this->rects.begin();
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

            // For each other direction also mark lines. Count direction upwards until we hit a boundary rect:
            short diri = (firsti + 1) % 6;
            // Can debug first *count up* direction with sm::rect::neighbour_pos(diri)
            while (hi->has_neighbour(diri) && hi->get_neighbour(diri)->test_flags(bdry_flag)==false && diri != firsti) {
                first_inside = hi->get_neighbour(diri);
                this->mark_from_boundary_common (first_inside, diri, bdry_flag, inside_flag);
                diri = (diri + 1) % 6;
            }

            // Then count downwards until we hit the other boundary rect
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
        void mark_from_boundary_common (std::list<rect>::iterator first_inside, std::uint16_t firsti,
                                        std::uint32_t bdry_flag = RECT_IS_BOUNDARY,
                                        std::uint32_t inside_flag = RECT_INSIDE_BOUNDARY)
        {
            // From the "first inside the boundary rect" head in the direction specified by firsti until a
            // boundary rect is reached.
            std::list<sm::rect>::iterator straight = first_inside;

#ifdef DO_WARNINGS
            bool warning_given = false;
#endif
            while (straight->test_flags(bdry_flag) == false) {
                // Set inside_boundary true
                straight->set_flag (inside_flag);
                if (straight->has_neighbour(firsti)) {
                    straight = straight->get_neighbour (firsti);
                } else {
                    // no further neighbour in this direction
                    if (straight->test_flags(bdry_flag) == false) {
#ifdef DO_WARNINGS
                        if (!warning_given) {
                            std::cerr << "WARNING: Got to edge of region (dirn " << firsti
                                      << ") without encountering a boundary rect.\n";
                            warning_given = true;
                        }
#endif
                        break;
                    }
                }
            }
        }

        /*!
         * Given the current boundary rect iterator, bhi and the n_recents last boundary
         * rects in recently_seen, and assuming that bhi has had all its adjacent inside
         * rects marked as inside_boundary, find the next boundary rect.
         *
         * \param bhi The boundary rect iterator. From this rect, find the next boundary
         * rect.
         *
         * \param recently_seen a deque containing the recently processed boundary
         * rects. for a boundary which is always exactly one rect thick, you only need a
         * memory of the last boundary rect to keep you going in the right direction
         * around the boundary BUT if your boundary has some "double thickness"
         * sections, then you need to know a few more recent rects to avoid looping
         * around and returning to the start!
         *
         * \param n_recents The number of rects to record in \a recently_seen. The
         * actual number you will need depends on the "thickness" of your boundary -
         * does it have sections that are two rects thick, or sections that are six
         * rects thick? It also depends on the length along which the boundary may be
         * two rects thick. In theory, if you have a boundary section two rects thick
         * for 5 pairs, then you need to store 10 previous rects. However, due to the
         * way that this algorithm tests rects (always testing direction '0' which is
         * East first, then going anti-clockwise to the next direction; North-East and
         * so on), n_recents=2 appears to be sufficient for a thickness 2 boundary,
         * which is what can occur when setting a boundary using the method
         * cartgrid::set_elliptical_boundary. Boundaries that are more than thickness 2
         * shouldn't really occur, whereas a boundary with a short section of thickness
         * 2 can quite easily occur, as in set_elliptical_boundary, where insisting that
         * the boundary was strictly always only 1 rect thick would make that algorithm
         * more complex.
         *
         * \param bdry_flag The flag used to recognise a boundary rect.
         *
         * \param inside_flag The flag used to recognise a rect that is inside the
         * boundary.
         *
         * \return true if a next boundary neighbour was found, false otherwise.
         */
        bool find_next_boundary_neighbour (std::list<rect>::iterator& bhi,
                                           std::deque<std::list<rect>::iterator>& recently_seen,
                                           std::uint32_t n_recents = 2U,
                                           std::uint32_t bdry_flag = RECT_IS_BOUNDARY,
                                           std::uint32_t inside_flag = RECT_INSIDE_BOUNDARY) const
        {
            bool gotnextneighbour = false;

            // From each boundary rect, loop round all 6 neighbours until we get to a new neighbour
            for (std::uint16_t i = 0; i < 6 && gotnextneighbour == false; ++i) {

                // This is "if it's a neighbour and the neighbour is a boundary rect"
                if (bhi->has_neighbour(i) && bhi->get_neighbour(i)->test_flags(bdry_flag)) {

                    // cbhi is "candidate boundary rect iterator", now guaranteed to be a boundary rect
                    std::list<sm::rect>::iterator cbhi = bhi->get_neighbour(i);

                    // Test if the candidate boundary rect is in the 'recently seen' deque
                    bool rect_already_seen = false;
                    for (auto rs : recently_seen) {
                        if (rs == cbhi) {
                            // This candidate rect has been recently seen. continue to next i
                            rect_already_seen = true;
                        }
                    }
                    if (rect_already_seen) { continue; }

                    std::uint16_t i_opp = ((i+3)%6);

                    // Go round each of the candidate boundary rect's neighbours (but j!=i)
                    for (std::uint16_t j = 0; j < 6; ++j) {

                        // Ignore the candidate boundary rect itself. if j==i_opp, then
                        // i's neighbour in dirn sm::rect::neighbour_pos(j) is the
                        // candidate iself, continue to next i
                        if (j == i_opp) { continue; }

                        // What is this logic. If the candidate boundary rect (which is already
                        // known to be on the boundary) has a neighbour which is inside the
                        // boundary and not itself a boundary rect, then cbhi IS the next
                        // boundary rect.
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
         * Mark rects as inside_boundary if they are inside the boundary. Starts from
         * \a hi which is assumed to already be known to refer to a rect lying inside the
         * boundary.
         */
        void mark_rects_inside (std::list<rect>::iterator hi,
                                std::uint32_t bdry_flag = RECT_IS_BOUNDARY,
                                std::uint32_t inside_flag = RECT_INSIDE_BOUNDARY)
        {
            // Run to boundary, marking as we go
            std::list<sm::rect>::iterator bhi(hi);
            while (bhi->test_flags (bdry_flag) == false && bhi->has_nne()) {
                bhi->set_flag (inside_flag);
                bhi = bhi->nne;
            }
            std::list<sm::rect>::iterator bhi_start = bhi;

            // Mark from first boundary rect and across the region
            this->mark_from_boundary (bhi, bdry_flag, inside_flag);

            // a deque to hold the 'n_recents' most recently seen boundary rects.
            std::deque<std::list<sm::rect>::iterator> recently_seen;
            std::uint32_t n_recents = 16U; // 2 should be sufficient for boundaries with double thickness
            // sections. If problems occur, trying increasing this.
            bool gotnext = this->find_next_boundary_neighbour (bhi, recently_seen, n_recents, bdry_flag, inside_flag);
            // Loop around boundary, marking inwards in all possible directions from each boundary rect
            while (gotnext && bhi != bhi_start) {
                this->mark_from_boundary (bhi, bdry_flag, inside_flag);
                gotnext = this->find_next_boundary_neighbour (bhi, recently_seen, n_recents, bdry_flag, inside_flag);
            }
        }

        /*!
         * Mark ALL rects as inside the domain
         */
        void markall_rects_inside_domain()
        {
            std::list<sm::rect>::iterator hi = this->rects.begin();
            while (hi != this->rects.end()) {
                hi->set_inside_domain();
                hi++;
            }
        }

        /*!
         * Discard rects in this->rects that are outside the boundary #boundary.
         */
        void discard_outside_boundary()
        {
            // Mark those rects inside the boundary
            std::list<sm::rect>::iterator centroid_rect = this->find_rect_nearest (this->boundary_centroid);
            this->mark_rects_inside (centroid_rect);
            // Run through and discard those rects outside the boundary:
            auto hi = this->rects.begin();
            while (hi != this->rects.end()) {
                if (hi->test_flags(RECT_INSIDE_BOUNDARY) == false) {
                    // When erasing a Rect, I need to update the neighbours of its
                    // neighbours.
                    hi->disconnect_neighbours();
                    // Having disconnected the neighbours, erase the rect.
                    hi = this->rects.erase (hi);
                } else {
                    ++hi;
                }
            }
            // The rect::vi indices need to be re-numbered.
            this->renumber_vector_indices();
            // Finally, do something about the rectagonal grid vertices; set this to true to mark that the
            // iterators to the outermost vertices are no longer valid and shouldn't be used.
            this->grid_reduced = true;
        }

        /*!
         * Discard rects in this->rects that are outside the rectangular rect domain.
         */
        void discard_outside_domain()
        {
            // Similar to discard_outside_boundary:
            auto hi = this->rects.begin();
            while (hi != this->rects.end()) {
                if (hi->inside_domain() == false) {
                    hi->disconnect_neighbours();
                    hi = this->rects.erase (hi);
                } else {
                    ++hi;
                }
            }
            this->renumber_vector_indices();
            this->grid_reduced = true;
        }

        /*!
         * Find the extents of the boundary rects. Find the xi for the left-most rect and
         * the xi for the right-most rect (elements 0 and 1 of the return array). Find
         * the yi for the top most rect and the yi for the bottom most rect.
         *
         * Return object contains: {xi-left, xi-right, yi-bottom, yi-top}
         */
        std::array<std::int32_t, 4> find_boundary_extents() const
        {
            if constexpr (debug_cartgrid) {
                std::cout << "Called for cartgrid 0x" << (std::uint64_t)this << std::endl;
            }
            // Return object
            std::array<std::int32_t, 4> extents = {{0,0,0,0}};

            // Find the furthest left and right rects and the furthest up and down rects.
            std::array<float, 4> limits = {{0,0,0,0}};
            bool first = true;
            for (auto r : this->rects) {
                if (r.test_flags(RECT_IS_BOUNDARY) == true) {
                    if (first) {
                        limits = {r.x, r.x, r.y, r.y};
                        extents = {r.xi, r.xi, r.yi, r.yi};
                        first = false;
                    }
                    if (r.x < limits[0]) {
                        limits[0] = r.x;
                        extents[0] = r.xi;
                    }
                    if (r.x > limits[1]) {
                        limits[1] = r.x;
                        extents[1] = r.xi;
                    }
                    if (r.y < limits[2]) {
                        limits[2] = r.y;
                        extents[2] = r.yi;
                    }
                    if (r.y > limits[3]) {
                        limits[3] = r.y;
                        extents[3] = r.yi;
                    }
                }
            }

            // Add 'growth buffer'
            extents[0] -= this->d_growthbuffer_horz;
            extents[1] += this->d_growthbuffer_horz;
            extents[2] -= this->d_growthbuffer_vert;
            extents[3] += this->d_growthbuffer_vert;

            return extents;
        }

        /*!
         * Find the rect in the rect grid which is closest to the x,y position given by
         * pos.
         */
        std::list<rect>::iterator find_rect_nearest (const sm::vec<float, 2>& pos)
        {
            std::list<sm::rect>::iterator nearest = this->rects.end();
            std::list<sm::rect>::iterator ri = this->rects.begin();
            float dist = std::numeric_limits<float>::max();
            while (ri != this->rects.end()) {
                sm::vec<float, 2> dx = { pos[0] - ri->x, pos[1] - ri->y };
                float dl = dx.length();
                if (dl < dist) {
                    dist = dl;
                    nearest = ri;
                }
                ++ri;
            }
            return nearest;
        }

        //! Assuming a rectangular cartgrid, find bottom left element
        std::list<rect>::iterator find_bottom_left()
        {
            std::list<sm::rect>::iterator bottomleft = this->rects.begin();
            while (bottomleft->has_ns()) { bottomleft = bottomleft->ns; }
            while (bottomleft->has_nw()) { bottomleft = bottomleft->nw; }
            return bottomleft;
        }

        /*!
         * Does what it says on the tin. Re-number the rect::vi vector index in each
         * rect in the cartgrid, from the start of the list<rect> rects until the end.
         */
        void renumber_vector_indices()
        {
            if constexpr (debug_cartgrid) { std::cout << __FUNCTION__ << " called\n"; }
            std::uint32_t vi = 0;
            this->vrects.clear();
            auto ri = this->rects.begin();
            while (ri != this->rects.end()) {
                ri->vi = vi++;
                this->vrects.push_back (&(*ri));
                ++ri;
            }
        }

        //! The centre to centre horizontal distance.
        float d = 1.0f; // refactor dx?

        //! The centre to centre rect vertical distance
        float v = 1.0f; // refactor dy?

        //! Give the initial rectangular grid a size x_span in the horizontal direction.
        float x_span = 10.0f;

        //! Give the initial rectangular grid a size y_span in the vertical direction.
        float y_span = 10.0f;

        //! The z coordinate of this rect grid layer
        float z;

        //! A boundary to apply to the initial, rectangular grid.
        sm::bezcurvepath<float> boundary;

        /*!
         * Set true when a new boundary or domain has been applied.
         */
        bool grid_reduced = false;

    public:
        // Min/max x and y to record size of domain. Populate during init.
        sm::range<float> x_minmax;
        sm::range<float> y_minmax;
    };

} // namespace sm
