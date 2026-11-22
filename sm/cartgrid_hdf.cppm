// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * Defines save and load functions for cartgrid
 *
 * This will need finishing - it is code transferred out of cartgrid/rect and left for possible
 * future use.
 *
 * \author: Seb James
 * \date: 2026/11
 */
module;

#include <cstdint>
#include <string>
#include <ios>
#include <list>
#include <stdexcept>
#include <iostream>

export module sm.cartgrid.hdf;

export import sm.cartgrid;
import sm.hdfdata;

export namespace sm
{
    /*!
     * Save the data for this rect into the already open hdfdata object @h5data in the path
     * @h5path.
     */
    template<typename F>
    void rect_save (const sm::rect& rct, sm::hdfdata& h5data, const std::string& h5path)
    {
        std::string dpath = h5path + "/vi";
        h5data.add_val (dpath.c_str(), rct.vi);
        dpath = h5path + "/di";
        h5data.add_val (dpath.c_str(), rct.di);
        dpath = h5path + "/x";
        h5data.add_val (dpath.c_str(), rct.x);
        dpath = h5path + "/y";
        h5data.add_val (dpath.c_str(), rct.y);
        dpath = h5path + "/z";
        h5data.add_val (dpath.c_str(), rct.z);
        dpath = h5path + "/r";
        h5data.add_val (dpath.c_str(), rct.r);
        dpath = h5path + "/phi";
        h5data.add_val (dpath.c_str(), rct.phi);
        dpath = h5path + "/dx";
        h5data.add_val (dpath.c_str(), rct.dx);
        dpath = h5path + "/dy";
        h5data.add_val (dpath.c_str(), rct.dy);
        dpath = h5path + "/xi";
        h5data.add_val (dpath.c_str(), rct.xi);
        dpath = h5path + "/yi";
        h5data.add_val (dpath.c_str(), rct.yi);
        dpath = h5path + "/dist_to_boundary";
        h5data.add_val (dpath.c_str(), rct.dist_to_boundary);
        dpath = h5path + "/flags";
        h5data.add_val (dpath.c_str(), rct.flags);
    }

    //! Load the data for this rect from a sm::hdfdata file
    template<typename F>
    void rect_load (sm::rect& rct, hdfdata& h5data, const std::string& h5path)
    {
        std::string dpath = h5path + "/vi";
        h5data.read_val (dpath.c_str(), rct.vi);
        dpath = h5path + "/di";
        h5data.read_val (dpath.c_str(), rct.di);
        dpath = h5path + "/x";
        h5data.read_val (dpath.c_str(), rct.x);
        dpath = h5path + "/y";
        h5data.read_val (dpath.c_str(), rct.y);
        dpath = h5path + "/z";
        h5data.read_val (dpath.c_str(), rct.z);
        dpath = h5path + "/r";
        h5data.read_val (dpath.c_str(), rct.r);
        dpath = h5path + "/phi";
        h5data.read_val (dpath.c_str(), rct.phi);
        dpath = h5path + "/dx";
        h5data.read_val (dpath.c_str(), rct.dx);
        dpath = h5path + "/dy";
        h5data.read_val (dpath.c_str(), rct.dy);
        dpath = h5path + "/xi";
        h5data.read_val (dpath.c_str(), rct.xi);
        dpath = h5path + "/yi";
        h5data.read_val (dpath.c_str(), rct.yi);
        dpath = h5path + "/dist_to_boundary";
        h5data.read_val (dpath.c_str(), rct.dist_to_boundary);
        std::uint32_t flgs = 0;
        dpath = h5path + "/flags";
        h5data.read_val (dpath.c_str(), flgs);
        rct.flags = flgs;
    }
    /*!
     * Save the cartgrid (and all the hexes in it) into the HDF5 file at the location @path.
     */
    template<typename F>
    void cartgrid_save (const sm::cartgrid& cg, const std::string& path)
    {
        sm::hdfdata cgdata (path, std::ios::out | std::ios::trunc);
        cgdata.add_val ("/d", cg.d);
        cgdata.add_val ("/v", cg.v);
        cgdata.add_val ("/x_span", cg.x_span);
        cgdata.add_val ("/y_span", cg.y_span);
        cgdata.add_val ("/z", cg.z);
        cgdata.add_val ("/d_growthbuffer_horz", cg.d_growthbuffer_horz);
        cgdata.add_val ("/d_growthbuffer_vert", cg.d_growthbuffer_vert);

        // sm::vec<float, 2>
        cgdata.add_contained_vals ("/boundary_entroid", cg.boundary_centroid);

        // Don't save bezcurvepath boundary - limit this to the ability to
        // save which elements are boundary elements and which aren't

        // vector<float>
        cgdata.add_contained_vals ("/d_x", cg.d_x);
        cgdata.add_contained_vals ("/d_y", cg.d_y);
        cgdata.add_contained_vals ("/d_dist_to_boundary", cg.d_dist_to_boundary);
        // vector<std::int32_t>
        cgdata.add_contained_vals ("/d_xi", cg.d_xi);
        cgdata.add_contained_vals ("/d_yi", cg.d_yi);

        cgdata.add_contained_vals ("/d_ne", cg.d_ne);
        cgdata.add_contained_vals ("/d_nne", cg.d_nne);
        cgdata.add_contained_vals ("/d_nn", cg.d_nn);
        cgdata.add_contained_vals ("/d_nnw", cg.d_nnw);
        cgdata.add_contained_vals ("/d_nw", cg.d_nw);
        cgdata.add_contained_vals ("/d_nsw", cg.d_nsw);
        cgdata.add_contained_vals ("/d_ns", cg.d_ns);
        cgdata.add_contained_vals ("/d_nse", cg.d_nse);

        // vector<std::uint32_t>
        cgdata.add_contained_vals ("/d_flags", cg.d_flags);

        // list<rect> rects
        // for i in list, save rect
        std::list<sm::rect>::const_iterator r = cg.rects.begin();
        std::uint32_t rcount = 0;
        while (r != cg.rects.end()) {
            // Make up a path
            std::string h5path = "/rects/" + std::to_string(rcount);
            r->save (cgdata, h5path);
            ++r;
            ++rcount;
        }
        cgdata.add_val ("/rcount", rcount);
    }

    /*!
     * Populate cartgrid hg from the HDF5 file at the location @path.
     */
    template<typename F>
    void cartgrid_load (sm::cartgrid& cg, const std::string& path)
    {
        sm::hdfdata cgdata (path, std::ios::in);
        cgdata.read_val ("/d", cg.d);
        cgdata.read_val ("/v", cg.v);
        cgdata.read_val ("/x_span", cg.x_span);
        cgdata.read_val ("/y_span", cg.y_span);
        cgdata.read_val ("/z", cg.z);
        cgdata.read_val ("/d_growthbuffer_horz", cg.d_growthbuffer_horz);
        cgdata.read_val ("/d_growthbuffer_vert", cg.d_growthbuffer_vert);

        cgdata.read_contained_vals ("/boundary_centroid", cg.boundary_centroid);
        cgdata.read_contained_vals ("/d_x", cg.d_x);
        cgdata.read_contained_vals ("/d_y", cg.d_y);
        cgdata.read_contained_vals ("/d_dist_to_boundary", cg.d_dist_to_boundary);
        cgdata.read_contained_vals ("/d_xi", cg.d_xi);
        cgdata.read_contained_vals ("/d_yi", cg.d_yi);
        cgdata.read_contained_vals ("/d_ne", cg.d_ne);
        cgdata.read_contained_vals ("/d_nne", cg.d_nne);
        cgdata.read_contained_vals ("/d_nnw", cg.d_nnw);
        cgdata.read_contained_vals ("/d_nw", cg.d_nw);
        cgdata.read_contained_vals ("/d_nsw", cg.d_nsw);
        cgdata.read_contained_vals ("/d_nse", cg.d_nse);
        cgdata.read_contained_vals ("/d_flags", cg.d_flags);

        // Assume a boundary has been applied so set this true.
        cg.grid_reduced = true;

        std::uint32_t rcount = 0;
        cgdata.read_val ("/rcount", rcount);
        for (std::uint32_t i = 0; i < rcount; ++i) {
            std::string h5path = "/rects/" + std::to_string(i);
            sm::rect r(cgdata, h5path);
            cg.rects.push_back (r);
        }

        // After creating rects list, need to set neighbour relations in each rect, as loaded in
        // d_ne, etc.
        for (sm::rect& _r : cg.rects) {
            // For each rect, six loops through rects:
            if (_r.has_ne() == true) {
                bool matched = false;
                std::uint32_t neighb_it = (std::uint32_t) cg.d_ne[_r.vi];
                std::list<sm::rect>::iterator ri = cg.rects.begin();
                while (ri != cg.rects.end()) {
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
                std::uint32_t neighb_it = (std::uint32_t) cg.d_nne[_r.vi];
                std::list<sm::rect>::iterator ri = cg.rects.begin();
                while (ri != cg.rects.end()) {
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
                std::uint32_t neighb_it = (std::uint32_t) cg.d_nn[_r.vi];
                std::list<sm::rect>::iterator ri = cg.rects.begin();
                while (ri != cg.rects.end()) {
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
                std::uint32_t neighb_it = (std::uint32_t) cg.d_nnw[_r.vi];
                std::list<sm::rect>::iterator ri = cg.rects.begin();
                while (ri != cg.rects.end()) {
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
                std::uint32_t neighb_it = (std::uint32_t) cg.d_nw[_r.vi];
                std::list<sm::rect>::iterator ri = cg.rects.begin();
                while (ri != cg.rects.end()) {
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
                std::uint32_t neighb_it = (std::uint32_t) cg.d_nsw[_r.vi];
                std::list<sm::rect>::iterator ri = cg.rects.begin();
                while (ri != cg.rects.end()) {
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
                std::uint32_t neighb_it = (std::uint32_t) cg.d_ns[_r.vi];
                std::list<sm::rect>::iterator ri = cg.rects.begin();
                while (ri != cg.rects.end()) {
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
                std::uint32_t neighb_it = (std::uint32_t) cg.d_nse[_r.vi];
                std::list<sm::rect>::iterator ri = cg.rects.begin();
                while (ri != cg.rects.end()) {
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

} // namespace
