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
    void hex_save (const sm::hex& hx, sm::hdfdata& h5data, const std::string& h5path)
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
        dpath = h5path + "/distToBoundary";
        h5data.add_val (dpath.c_str(), hx.distToBoundary);
        dpath = h5path + "/flags";
        h5data.add_val (dpath.c_str(), hx.flags);
    }

    //! Load the data for this hex from a sm::hdfdata file
    void hex_load (sm::hex& hx, hdfdata& h5data, const std::string& h5path)
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
        dpath = h5path + "/distToBoundary";
        h5data.read_val (dpath.c_str(), hx.distToBoundary);
        uint32_t flgs = 0;
        dpath = h5path + "/flags";
        h5data.read_val (dpath.c_str(), flgs);
        hx.flags = flgs;
    }
    /*!
     * Save this hexgrid (and all the hexes in it) into the HDF5 file at the
     * location @path.
     */
    void hexgrid_save (const sm::hexgrid& hg, const std::string& path)
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
        hgdata.add_contained_vals ("/boundaryCentroid", hg.boundaryCentroid);

        // Don't save bezcurvepath boundary - limit this to the ability to
        // save which hexes are boundary hexes and which aren't

        // Don't save vertexE, vertexNE etc. Make sure to set gridReduced
        // = true when calling load()

        // vector<float>
        hgdata.add_contained_vals ("/d_x", hg.d_x);
        hgdata.add_contained_vals ("/d_y", hg.d_y);
        hgdata.add_contained_vals ("/d_distToBoundary", hg.d_distToBoundary);
        // vector<int32_t>
        hgdata.add_contained_vals ("/d_ri", hg.d_ri);
        hgdata.add_contained_vals ("/d_gi", hg.d_gi);
        hgdata.add_contained_vals ("/d_bi", hg.d_bi);

        hgdata.add_contained_vals ("/d_ne", hg.d_ne);
        hgdata.add_contained_vals ("/d_nne", hg.d_nne);
        hgdata.add_contained_vals ("/d_nnw", hg.d_nnw);
        hgdata.add_contained_vals ("/d_nw", hg.d_nw);
        hgdata.add_contained_vals ("/d_nsw", hg.d_nsw);
        hgdata.add_contained_vals ("/d_nse", hg.d_nse);

        // vector<uint32_t>
        hgdata.add_contained_vals ("/d_flags", hg.d_flags);

        // list<hex> hexen
        // for i in list, save hex
        std::list<sm::hex>::const_iterator h = hg.hexen.begin();
        uint32_t hcount = 0;
        while (h != hg.hexen.end()) {
            // Make up a path
            std::string h5path = "/hexen/" + std::to_string(hcount);
            sm::hex_save (*h, hgdata, h5path);
            ++h;
            ++hcount;
        }
        hgdata.add_val ("/hcount", hcount);

        // What about vhexen? Probably don't save and re-call method to populate.
        //hg.renumberVectorIndices();

        // What about bhexen? Probably re-run/test hg.boundaryContiguous() on load.
        //hg.boundaryContiguous();
    }

    /*!
     * Populate hexgrid hg from the HDF5 file at the location @path.
     */
    void hexgrid_load (sm::hexgrid& hg, const std::string& path)
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

        hgdata.read_contained_vals ("/boundaryCentroid", hg.boundaryCentroid);
        hgdata.read_contained_vals ("/d_x", hg.d_x);
        hgdata.read_contained_vals ("/d_y", hg.d_y);
        hgdata.read_contained_vals ("/d_distToBoundary", hg.d_distToBoundary);
        hgdata.read_contained_vals ("/d_ri", hg.d_ri);
        hgdata.read_contained_vals ("/d_gi", hg.d_gi);
        hgdata.read_contained_vals ("/d_bi", hg.d_bi);
        hgdata.read_contained_vals ("/d_ne", hg.d_ne);
        hgdata.read_contained_vals ("/d_nne", hg.d_nne);
        hgdata.read_contained_vals ("/d_nnw", hg.d_nnw);
        hgdata.read_contained_vals ("/d_nw", hg.d_nw);
        hgdata.read_contained_vals ("/d_nsw", hg.d_nsw);
        hgdata.read_contained_vals ("/d_nse", hg.d_nse);
        hgdata.read_contained_vals ("/d_flags", hg.d_flags);

        // Assume a boundary has been applied so set this true. Also, the hexgrid::save method doesn't
        // save hexgrid::vertexE, etc
        hg.gridReduced = true;

        uint32_t hcount = 0;
        hgdata.read_val ("/hcount", hcount);
        for (uint32_t i = 0; i < hcount; ++i) {
            std::string h5path = "/hexen/" + std::to_string(i);
            sm::hex h;
            sm::hex_load (h, hgdata, h5path);
            hg.hexen.push_back (h);
        }

        // After creating hexen list, need to set neighbour relations in each hex, as loaded in d_ne,
        // etc.
        for (sm::hex& _h : hg.hexen) {
            // Set neighbours for hex " << _h.outputRG()
            // For each hex, six loops through hexen:
            if (_h.has_ne() == true) {
                bool matched = false;
                uint32_t neighb_it = (uint32_t) hg.d_ne[_h.vi];
                std::list<sm::hex>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.ne = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour E relation...");
                }
            }

            if (_h.has_nne() == true) {
                bool matched = false;
                uint32_t neighb_it = (uint32_t) hg.d_nne[_h.vi];
                std::list<sm::hex>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.nne = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour NE relation...");
                }
            }

            if (_h.has_nnw() == true) {
                bool matched = false;
                uint32_t neighb_it = (uint32_t) hg.d_nnw[_h.vi];
                std::list<sm::hex>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.nnw = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour NW relation...");
                }
            }

            if (_h.has_nw() == true) {
                bool matched = false;
                uint32_t neighb_it = (uint32_t) hg.d_nw[_h.vi];
                std::list<sm::hex>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.nw = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour W relation...");
                }
            }

            if (_h.has_nsw() == true) {
                bool matched = false;
                uint32_t neighb_it = (uint32_t) hg.d_nsw[_h.vi];
                std::list<sm::hex>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.nsw = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour SW relation...");
                }
            }

            if (_h.has_nse() == true) {
                bool matched = false;
                uint32_t neighb_it = (uint32_t) hg.d_nse[_h.vi];
                std::list<sm::hex>::iterator hi = hg.hexen.begin();
                while (hi != hg.hexen.end()) {
                    if (hi->vi == neighb_it) {
                        matched = true;
                        _h.nse = hi;
                        break;
                    }
                    ++hi;
                }
                if (!matched) {
                    throw std::runtime_error ("Failed to match hexen neighbour SE relation...");
                }
            }
        }
    }

} // namespace
