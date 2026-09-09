// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * Defines save and load functions for hexgrid
 *
 * \author: Seb James
 * \date: 2018/07
 */
module;

#include <cstdint>
#include <string>
#include <ios>
#include <list>
#include <stdexcept>

export module sm.hexgrid.hdf;

export import sm.hexgrid;
import sm.hdfdata;

export namespace sm
{
    /*!
     * Save the data for this hex into the already open hdfdata object @h5data in the path
     * @h5path.
     */
    template<typename F>
    void hex_save (const sm::hex<F>& hx, sm::hdfdata& h5data, const std::string& h5path)
    {
        std::string dpath = h5path + "/vi";
        h5data.add_val (dpath.c_str(), hx.vi);
        dpath = h5path + "/di";
        h5data.add_val (dpath.c_str(), hx.di);
        dpath = h5path + "/x";
        h5data.add_val (dpath.c_str(), hx.x);
        dpath = h5path + "/y";
        h5data.add_val (dpath.c_str(), hx.y);
        dpath = h5path + "/z";
        h5data.add_val (dpath.c_str(), hx.z);
        dpath = h5path + "/r";
        h5data.add_val (dpath.c_str(), hx.r);
        dpath = h5path + "/phi";
        h5data.add_val (dpath.c_str(), hx.phi);
        dpath = h5path + "/d";
        h5data.add_val (dpath.c_str(), hx.d);
        dpath = h5path + "/ri";
        h5data.add_val (dpath.c_str(), hx.ri);
        dpath = h5path + "/gi";
        h5data.add_val (dpath.c_str(), hx.gi);
        dpath = h5path + "/bi";
        h5data.add_val (dpath.c_str(), hx.bi);
        dpath = h5path + "/dist_to_boundary";
        h5data.add_val (dpath.c_str(), hx.dist_to_boundary);
        dpath = h5path + "/flags";
        h5data.add_val (dpath.c_str(), hx.flags);
    }

    //! Load the data for this hex from a sm::hdfdata file
    template<typename F>
    void hex_load (sm::hex<F>& hx, hdfdata& h5data, const std::string& h5path)
    {
        std::string dpath = h5path + "/vi";
        h5data.read_val (dpath.c_str(), hx.vi);
        dpath = h5path + "/di";
        h5data.read_val (dpath.c_str(), hx.di);
        dpath = h5path + "/x";
        h5data.read_val (dpath.c_str(), hx.x);
        dpath = h5path + "/y";
        h5data.read_val (dpath.c_str(), hx.y);
        dpath = h5path + "/z";
        h5data.read_val (dpath.c_str(), hx.z);
        dpath = h5path + "/r";
        h5data.read_val (dpath.c_str(), hx.r);
        dpath = h5path + "/phi";
        h5data.read_val (dpath.c_str(), hx.phi);
        dpath = h5path + "/d";
        h5data.read_val (dpath.c_str(), hx.d);
        dpath = h5path + "/ri";
        h5data.read_val (dpath.c_str(), hx.ri);
        dpath = h5path + "/gi";
        h5data.read_val (dpath.c_str(), hx.gi);
        dpath = h5path + "/bi";
        h5data.read_val (dpath.c_str(), hx.bi);
        dpath = h5path + "/dist_to_boundary";
        h5data.read_val (dpath.c_str(), hx.dist_to_boundary);
        std::uint32_t flgs = 0;
        dpath = h5path + "/flags";
        h5data.read_val (dpath.c_str(), flgs);
        hx.flags = flgs;
    }
    /*!
     * Save this hexgrid (and all the hexes in it) into the HDF5 file at the
     * location @path.
     */
    template<typename F>
    void hexgrid_save (const sm::hexgrid<F>& hg, const std::string& path)
    {
        sm::hdfdata hgdata (path, std::ios::out | std::ios::trunc);
        hgdata.add_val ("/d", hg.d);
        hgdata.add_val ("/v", hg.v);
        hgdata.add_val ("/x_span", hg.x_span);
        hgdata.add_val ("/z", hg.z);
        hgdata.add_val ("/d_rowlen", hg.d_rowlen);
        hgdata.add_val ("/d_numrows", hg.d_numrows);
        hgdata.add_val ("/d_size", hg.d_size);
        hgdata.add_val ("/d_growthbuffer_horz", hg.d_growthbuffer_horz);
        hgdata.add_val ("/d_growthbuffer_vert", hg.d_growthbuffer_vert);

        // sm::vec<float, 2>
        hgdata.add_contained_vals ("/boundary_centroid", hg.boundary_centroid);

        // Don't save bezcurvepath boundary - limit this to the ability to
        // save which hexes are boundary hexes and which aren't

        // Don't save vertex_e, vertex_ne etc. Make sure to set grid_reduced
        // = true when calling load()

        // vector<float>
        hgdata.add_contained_vals ("/d_x", hg.d_x);
        hgdata.add_contained_vals ("/d_y", hg.d_y);
        hgdata.add_contained_vals ("/d_dist_to_boundary", hg.d_dist_to_boundary);
        // vector<int32_t>
        hgdata.add_contained_vals ("/d_ri", hg.d_ri);
        hgdata.add_contained_vals ("/d_gi", hg.d_gi);
        hgdata.add_contained_vals ("/d_bi", hg.d_bi);

        hgdata.add_contained_vals ("/d_n0", hg.d_n0);
        hgdata.add_contained_vals ("/d_n1", hg.d_n1);
        hgdata.add_contained_vals ("/d_n2", hg.d_n2);
        hgdata.add_contained_vals ("/d_n3", hg.d_n3);
        hgdata.add_contained_vals ("/d_n4", hg.d_n4);
        hgdata.add_contained_vals ("/d_n5", hg.d_n5);

        // vector<uint32_t>
        hgdata.add_contained_vals ("/d_flags", hg.d_flags);

        // The transform matrix
        hgdata.add_contained_vals ("/tfm", hg.tfm.arr);

        // list<hex> hexen
        // for i in list, save hex
        typename std::list<sm::hex<F>>::const_iterator h = hg.hexen.begin();
        std::uint32_t hcount = 0;
        while (h != hg.hexen.end()) {
            // Make up a path
            std::string h5path = "/hexen/" + std::to_string(hcount);
            sm::hex_save<F> (*h, hgdata, h5path);
            ++h;
            ++hcount;
        }
        hgdata.add_val ("/hcount", hcount);

        // What about vhexen? Probably don't save and re-call method to populate.
        //hg.renumber_vector_indices();

        // What about bhexen? Probably re-run/test hg.boundary_contiguous() on load.
        //hg.boundary_contiguous();
    }

    /*!
     * Populate hexgrid hg from the HDF5 file at the location @path.
     */
    template<typename F>
    void hexgrid_load (sm::hexgrid<F>& hg, const std::string& path)
    {
        sm::hdfdata hgdata (path, std::ios::in);
        hgdata.read_val ("/d", hg.d);
        hgdata.read_val ("/v", hg.v);
        hgdata.read_val ("/x_span", hg.x_span);
        hgdata.read_val ("/z", hg.z);
        hgdata.read_val ("/d_rowlen", hg.d_rowlen);
        hgdata.read_val ("/d_numrows", hg.d_numrows);
        hgdata.read_val ("/d_size", hg.d_size);
        hgdata.read_val ("/d_growthbuffer_horz", hg.d_growthbuffer_horz);
        hgdata.read_val ("/d_growthbuffer_vert", hg.d_growthbuffer_vert);

        hgdata.read_contained_vals ("/boundary_centroid", hg.boundary_centroid);
        hgdata.read_contained_vals ("/d_x", hg.d_x);
        hgdata.read_contained_vals ("/d_y", hg.d_y);
        hgdata.read_contained_vals ("/d_dist_to_boundary", hg.d_dist_to_boundary);
        hgdata.read_contained_vals ("/d_ri", hg.d_ri);
        hgdata.read_contained_vals ("/d_gi", hg.d_gi);
        hgdata.read_contained_vals ("/d_bi", hg.d_bi);
        hgdata.read_contained_vals ("/d_n0", hg.d_n0);
        hgdata.read_contained_vals ("/d_n1", hg.d_n1);
        hgdata.read_contained_vals ("/d_n2", hg.d_n2);
        hgdata.read_contained_vals ("/d_n3", hg.d_n3);
        hgdata.read_contained_vals ("/d_n4", hg.d_n4);
        hgdata.read_contained_vals ("/d_n5", hg.d_n5);
        hgdata.read_contained_vals ("/d_flags", hg.d_flags);

        hgdata.read_contained_vals ("/tfm", hg.tfm.arr);

        // Assume a boundary has been applied so set this true. Also, the hexgrid::save method doesn't
        // save hexgrid::vertex_e, etc
        hg.grid_reduced = true;

        std::uint32_t hcount = 0;
        hgdata.read_val ("/hcount", hcount);
        for (std::uint32_t i = 0; i < hcount; ++i) {
            std::string h5path = "/hexen/" + std::to_string(i);
            sm::hex<F> h;
            sm::hex_load<F> (h, hgdata, h5path);
            hg.hexen.push_back (h);
        }

        // After creating hexen list, need to set neighbour relations in each hex, as loaded in d_ne,
        // etc.
        for (sm::hex<F>& _h : hg.hexen) {
            // For each hex, six loops through hexen:
            if (_h.has_n0() == true) {
                bool matched = false;
                std::uint32_t neighb_it = (std::uint32_t) hg.d_n0[_h.vi];
                typename std::list<sm::hex<F>>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.n0 = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour 0 relation...");
                }
            }

            if (_h.has_n1() == true) {
                bool matched = false;
                std::uint32_t neighb_it = (std::uint32_t) hg.d_n1[_h.vi];
                typename std::list<sm::hex<F>>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.n1 = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour 1 relation...");
                }
            }

            if (_h.has_n2() == true) {
                bool matched = false;
                std::uint32_t neighb_it = (std::uint32_t) hg.d_n2[_h.vi];
                typename std::list<sm::hex<F>>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.n2 = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour 2 relation...");
                }
            }

            if (_h.has_n3() == true) {
                bool matched = false;
                std::uint32_t neighb_it = (std::uint32_t) hg.d_n3[_h.vi];
                typename std::list<sm::hex<F>>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.n3 = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour 3 relation...");
                }
            }

            if (_h.has_n4() == true) {
                bool matched = false;
                std::uint32_t neighb_it = (std::uint32_t) hg.d_n4[_h.vi];
                typename std::list<sm::hex<F>>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.n4 = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour 4 relation...");
                }
            }

            if (_h.has_n5() == true) {
                bool matched = false;
                std::uint32_t neighb_it = (std::uint32_t) hg.d_n5[_h.vi];
                typename std::list<sm::hex<F>>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.n5 = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour 5 relation...");
                }
            }
        }
    }

} // namespace
