/* -----------------------------------------------------------------------------
 *
 * Adaptation of healpix_bare.c/h into a single header for simple inclusion (by Seb James). Also
 * adapted to be compiled by a C++ compiler (so that it can be namespaced). Further adapted (May
 * 2026) to compile as a C++ module.
 *
 *  Copyright (C) 1997-2019 Krzysztof M. Gorski, Eric Hivon, Martin Reinecke,
 *                          Benjamin D. Wandelt, Anthony J. Banday,
 *                          Matthias Bartelmann,
 *                          Reza Ansari & Kenneth M. Ganga
 *
 *  Implementation of the Healpix bare bones C library
 *
 *  Licensed under a 3-clause BSD style license - see LICENSE
 *
 *  For more information on HEALPix and additional software packages, see
 *  https://healpix.sourceforge.io/
 *
 *  If you are using this code in your own packages, please consider citing
 *  the original paper in your publications:
 *  K.M. Gorski et al., 2005, Ap.J., 622, p.759
 *  (http://adsabs.harvard.edu/abs/2005ApJ...622..759G)
 *
 *----------------------------------------------------------------------------*/

module;

#include <cmath>
#include <cstdint>
#include <cassert>
#include <algorithm>

export module hp.bare;

import sm.mathconst;

// The healpix namespace contains code from the HEALPix C library, slightly modified.
export namespace hp
{
    // Lookup tables
    constexpr std::int32_t jrll[] = { 2,2,2,2,3,3,3,3,4,4,4,4 };
    constexpr std::int32_t jpll[] = { 1,3,5,7,0,2,4,6,1,3,5,7 };

    /*
     * Admissible values for theta (definition see below)
     * 0 <= theta <= pi
     *
     * Admissible values for phi (definition see below) In principle unconstrained, but best
     * accuracy is obtained for -2*pi <= phi <= 2*pi
     */

    /*!
     * A structure describing a location on the sphere. \a theta is the co-latitude in radians (0 at
     * the North Pole, increasing to pi at the South Pole.  \a phi is the azimuth in radians. It's
     * also correct to describe it as the longitude. With theta on the equator (theta = pi/2), phi =
     * 0 gives the x unit vector (1,0,0) and phi = pi/2 gives the y unit vector (0,1,0). This
     * matches longitude in the Earth-centered Earth-fixed (ECEF) scheme. Test by passing these
     * values to
     *
     * hp::loc2vec (hp::ang2loc (_t_ang))
     */
    typedef struct { double theta, phi; } t_ang;
    /*!
     * A structure describing a 3-vector with coordinates \a x, \a y and \a z.
     */
    typedef struct { double x, y, z; } t_vec;

    /* Discrete coordinate systems */

    /*
     * Admissible values for nside parameters:
     *  any integer power of 2 with 1 <= nside <= 1<<29
     *
     *  Admissible values for pixel indices:
     *  0 <= idx < 12*nside*nside
     */
    /*!
     * A structure describing the discrete Healpix coordinate system.  \a f takes values in [0;11],
     * \a x and \a y lie in [0; nside].
     */
    typedef struct { std::int64_t x, y; std::int32_t f; } t_hpd;

    /* conversions between continuous coordinate systems */
    typedef struct { double z, s, phi; } tloc;

    /*!
     * A structure describing the continuous Healpix coordinate system.  \a f takes values in
     * [0;11], \a x and \a y lie in [0.0; 1.0].
     */
    typedef struct { double x, y; std::int32_t f; } t_hpc;

    t_hpc loc2hpc (tloc loc)
    {
        double za = std::fabs (loc.z);
        double x = loc.phi * sm::mathconst<double>::one_over_two_pi;
        if (x < 0.0) {
            x += (std::int64_t)x + 1.0;
        } else if (x >= 1.0) {
            x -= (std::int64_t)x;
        }
        double tt = 4.0 * x;

        if (za <= 2.0 / 3.0) { // Equatorial region
            double temp1 = 0.5 + tt;     // [0.5; 4.5)
            double temp2 = loc.z * 0.75; // [-0.5; +0.5]
            double jp = temp1 - temp2;   // index of  ascending edge line // [0; 5)
            double jm = temp1 + temp2;   // index of descending edge line // [0; 5)
            std::int32_t ifp = (std::int32_t)jp;           // in {0,4}
            std::int32_t ifm = (std::int32_t)jm;
            return hp::t_hpc{ jm - ifm, 1 + ifp - jp, (ifp == ifm) ? (ifp | 4) : ((ifp < ifm) ? ifp : (ifm + 8)) };
        }
        std::int32_t ntt = (std::int32_t)tt;
        if (ntt >= 4) { ntt = 3; }
        double tp = tt - ntt; // [0;1)
        double tmp = loc.s / std::sqrt( (1.0 + za) / 3.0); // FIXME optimize!

        double jp = tp * tmp;            // increasing edge line index
        double jm = (1.0 - tp) * tmp;    // decreasing edge line index
        if (jp > 1.0) { jp = 1.0; }      // for points too close to the boundary
        if (jm > 1.0) { jm = 1.0; }
        return (loc.z >= 0) ? hp::t_hpc{ 1.0 - jm, 1.0 - jp, ntt } : hp::t_hpc{ jp, jm, ntt + 8 };
    }

    tloc hpc2loc (t_hpc hpc)
    {
        double jr = jrll[hpc.f] - hpc.x - hpc.y;
        if (jr < 1.0) {
            double tmp = jr * jr * (1.0 / 3.0);
            double z = 1.0 - tmp;
            double s = std::sqrt (tmp * (2.0 - tmp));
            double phi = sm::mathconst<double>::pi_over_4 * (jpll[hpc.f] + (hpc.x - hpc.y) / jr);
            return hp::tloc{ z, s, phi };
        } else if (jr > 3.0) {
            jr = 4.0 - jr;
            double tmp = jr * jr * (1.0 / 3.0);
            double z = tmp - 1.0;
            double s = std::sqrt(tmp * (2.0 - tmp));
            double phi = sm::mathconst<double>::pi_over_4 * (jpll[hpc.f] + (hpc.x - hpc.y) / jr);
            return tloc{ z, s, phi };
        } else {
            double z = (2.0 - jr) * (2.0 / 3.0);
            double s = std::sqrt((1.0 + z) * (1.0 - z));
            double phi = sm::mathconst<double>::pi_over_4 * (jpll[hpc.f] + hpc.x - hpc.y);
            return tloc{ z, s, phi };
        }
    }

    tloc ang2loc (t_ang ang)
    {
        double cth = std::cos (ang.theta), sth = std::sin (ang.theta);
        if (sth < 0.0) { sth = -sth; ang.phi += sm::mathconst<double>::pi; }
        return tloc{ cth, sth, ang.phi };
    }

    t_ang loc2ang (tloc loc)
    {
        return t_ang{ std::atan2 (loc.s, loc.z), loc.phi };
    }

    tloc vec2loc (t_vec vec)
    {
        double vlen = std::sqrt (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
        double cth = vec.z / vlen;
        double sth = std::sqrt (vec.x * vec.x + vec.y * vec.y) / vlen;
        return tloc{ cth, sth, std::atan2 (vec.y, vec.x) };
    }

    t_vec loc2vec (tloc loc)
    {
        return t_vec{ loc.s * std::cos (loc.phi), loc.s * std::sin (loc.phi), loc.z };
    }

    /*!
     * PUBLIC INTERFACE.
     * Returns a normalized 3-vector pointing in the same direction as \a ang.
     */
    t_vec ang2vec (t_ang ang) { return hp::loc2vec (hp::ang2loc (ang)); }

    /*!
     * PUBLIC INTERFACE.
     * Returns a t_ang describing the same direction as the 3-vector \a vec.
     * \a vec need not be normalized.
     */
    t_ang vec2ang (t_vec vec)
    {
        return t_ang{ std::atan2( std::sqrt(vec.x * vec.x + vec.y * vec.y), vec.z), std::atan2 (vec.y, vec.x) };
    }

    std::int64_t isqrt (std::int64_t v)
    {
        std::int64_t res = std::sqrt (v + 0.5);
        if (v < ((std::int64_t)(1) << 50)) { return res; }
        if (res * res > v) {
            --res;
        } else if ((res + 1) * (res + 1) <= v) {
            ++res;
        }
        return res;
    }

    std::int64_t spread_bits (std::int64_t v)
    {
        std::int64_t res = v & 0xffffffff;
        res = (res^(res<<16)) & 0x0000ffff0000ffff;
        res = (res^(res<< 8)) & 0x00ff00ff00ff00ff;
        res = (res^(res<< 4)) & 0x0f0f0f0f0f0f0f0f;
        res = (res^(res<< 2)) & 0x3333333333333333;
        res = (res^(res<< 1)) & 0x5555555555555555;
        return res;
    }

    std::int64_t compress_bits (std::int64_t v)
    {
        std::int64_t res = v & 0x5555555555555555;
        res = (res^(res>> 1)) & 0x3333333333333333;
        res = (res^(res>> 2)) & 0x0f0f0f0f0f0f0f0f;
        res = (res^(res>> 4)) & 0x00ff00ff00ff00ff;
        res = (res^(res>> 8)) & 0x0000ffff0000ffff;
        res = (res^(res>>16)) & 0x00000000ffffffff;
        return res;
    }

    std::int64_t hpd2nest (std::int64_t nside, t_hpd hpd)
    {
        return (hpd.f * nside * nside) + hp::spread_bits(hpd.x) + (hp::spread_bits(hpd.y) << 1);
    }

    t_hpd nest2hpd (std::int64_t nside, std::int64_t pix)
    {
        std::int64_t npface_ = nside * nside;
        std::int64_t p2 = pix & (npface_ - 1);
        std::int32_t pix_over_npface = pix / npface_;
        return t_hpd{ hp::compress_bits(p2), hp::compress_bits(p2>>1), pix_over_npface };
    }

    std::int64_t hpd2ring (std::int64_t nside_, t_hpd hpd)
    {
        std::int64_t nl4 = 4 * nside_;
        std::int64_t jr = (jrll[hpd.f] * nside_) - hpd.x - hpd.y - 1;

        if (jr<nside_) {
            std::int64_t jp = (jpll[hpd.f] * jr + hpd.x - hpd.y + 1) / 2;
            jp = (jp > nl4) ? jp - nl4 : ((jp < 1) ? jp + nl4 : jp);
            return 2 * jr * (jr - 1) + jp - 1;
        } else if (jr > 3 * nside_) {
            jr = nl4 - jr;
            std::int64_t jp = (jpll[hpd.f] * jr + hpd.x - hpd.y + 1) / 2;
            jp = (jp > nl4) ? jp - nl4 : ((jp < 1) ? jp + nl4 : jp);
            return 12 * nside_ * nside_ - 2 * (jr + 1) * jr + jp - 1;
        } else {
            std::int64_t jp = (jpll[hpd.f] * nside_ + hpd.x - hpd.y + 1 + ((jr - nside_) & 1)) / 2;
            jp = (jp > nl4) ? jp - nl4 : ((jp < 1) ? jp + nl4 : jp);
            return 2 * nside_ * (nside_ - 1) + (jr - nside_) * nl4 + jp - 1;
        }
    }

    t_hpd ring2hpd (std::int64_t nside_, std::int64_t pix)
    {
        std::int64_t ncap_ = 2 * nside_ * (nside_ - 1);
        std::int64_t npix_ = 12 * nside_ * nside_;

        if (pix < ncap_) { /* North Polar cap */
            std::int64_t iring = (1 + hp::isqrt(1 + 2 * pix)) >> 1;  /* counted from North pole */
            std::int64_t iphi  = (pix + 1) - 2 * iring * (iring-1);
            std::int32_t face = (iphi - 1) / iring;
            std::int64_t irt = iring - (jrll[face] * nside_) + 1;
            std::int64_t ipt = 2 * iphi - jpll[face] * iring - 1;
            if (ipt >= 2 * nside_) { ipt -= 8 * nside_; }
            return t_hpd{ (ipt - irt) >> 1, (-(ipt + irt)) >> 1, face };

        } else if (pix < (npix_ - ncap_)) { /* Equatorial region */
            std::int64_t ip = pix - ncap_;
            std::int64_t iring = (ip / (4 * nside_)) + nside_;  /* counted from North pole */
            std::int64_t iphi  = (ip % (4 * nside_)) + 1;
            std::int64_t kshift = (iring + nside_) & 1;
            std::int64_t ire = iring - nside_ + 1;
            std::int64_t irm = 2 * nside_ + 2 - ire;
            std::int64_t ifm = (iphi - ire / 2 + nside_ - 1) / nside_;
            std::int64_t ifp = (iphi - irm / 2 + nside_ - 1) / nside_;
            std::int32_t face = (ifp == ifm) ? (ifp | 4) : ((ifp < ifm) ? ifp : (ifm + 8));
            std::int64_t irt = iring - (jrll[face] * nside_) + 1;
            std::int64_t ipt = 2*iphi- jpll[face] * nside_ - kshift - 1;
            if (ipt >= 2 * nside_) { ipt -= 8 * nside_; }
            return t_hpd{ (ipt - irt) >> 1, (-(ipt + irt)) >> 1, face };

        } else { /* South Polar cap */
            std::int64_t ip = npix_ - pix;
            std::int64_t iring = (1 + hp::isqrt(2 * ip - 1)) >> 1;  /* counted from South pole */
            std::int64_t iphi  = 4 * iring + 1 - (ip - 2 * iring * (iring - 1));
            std::int32_t face = 8 + (iphi - 1) / iring;
            std::int64_t irt = 4 * nside_ - iring - (jrll[face] * nside_) + 1;
            std::int64_t ipt = 2 * iphi - jpll[face] * iring - 1;
            if (ipt >= 2 * nside_) { ipt -= 8 * nside_; }
            return t_hpd{ (ipt - irt) >> 1, (-(ipt + irt)) >> 1, face };
        }
    }

    /*!
     * PUBLIC INTERFACE
     * Returns the RING pixel index of pixel \a ipnest at resolution \a nside.
     * On error, returns -1.
     */
    std::int64_t nest2ring (std::int64_t nside, std::int64_t ipnest)
    {
        if ((nside & (nside-1)) != 0) { return -1; }
        return hp::hpd2ring (nside, hp::nest2hpd (nside, ipnest));
    }

    /*!
     * PUBLIC INTERFACE
     * Returns the NEST pixel index of pixel \a ipring at resolution \a nside.
     * On error, returns -1.
     */
    std::int64_t ring2nest (std::int64_t nside, std::int64_t ipring)
    {
        if ((nside & (nside - 1)) != 0) { return -1; }
        return hp::hpd2nest (nside, hp::ring2hpd (nside, ipring));
    }

    /* mixed conversions */

    t_hpd loc2hpd (std::int64_t nside_, tloc loc)
    {
        t_hpc tmp = hp::loc2hpc (loc);
        std::int32_t _f = tmp.f;
        std::int64_t _x = tmp.x * nside_;
        std::int64_t _y = tmp.y * nside_;
        return t_hpd{ _x, _y, _f };
    }

    tloc hpd2loc (std::int64_t nside_, t_hpd hpd)
    {
        double xns = 1.0 / nside_;
        t_hpc tmp = t_hpc{ (hpd.x + 0.5) * xns, (hpd.y + 0.5) * xns, hpd.f };
        return hp::hpc2loc (tmp);
    }

    /* Miscellaneous utility routines */

    /*! Returns \a sqrt(npix/12) if this is an integer number, otherwise \a -1. */
    std::int64_t npix2nside (std::int64_t npix)
    {
        std::int64_t res = hp::isqrt (npix / 12);
        return (res * res * 12 == npix) ? res : -1;
    }

    /*!
     * PUBLIC INTERFACE
     * Returns \a 12*nside*nside.
     */
    std::int64_t nside2npix (std::int64_t nside) { return 12 * nside * nside; }

    /*!
     * PUBLIC INTERFACE
     * Returns the angle (in radians) between the vectors \a v1 and \a v2.
     * The result is accurate even for angles close to 0 and pi.
     */
    double vec_angle (t_vec v1, t_vec v2)
    {
        t_vec cross = { v1.y * v2.z - v1.z * v2.y,    v1.z * v2.x - v1.x * v2.z,   v1.x * v2.y - v1.y * v2.x };
        double len_cross = std::sqrt (cross.x * cross.x + cross.y * cross.y + cross.z * cross.z);
        double dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        return std::atan2 (len_cross, dot);
    }

    /* Conversions between continuous and discrete coordinate systems */

    /*!
     * PUBLIC INTERFACE
     * Returns the pixel number in RING scheme at resolution \a nside,
     * which contains the position \a ang.
     */
    std::int64_t ang2ring (std::int64_t nside, t_ang ang)
    {
        return hp::hpd2ring (nside, hp::loc2hpd (nside, hp::ang2loc (ang)));
    }
    /*!
     * PUBLIC INTERFACE
     * Returns the pixel number in NEST scheme at resolution \a nside,
     * which contains the position \a ang.
     */
    std::int64_t ang2nest (std::int64_t nside, t_ang ang)
    {
        return hp::hpd2nest (nside, hp::loc2hpd (nside, hp::ang2loc (ang)));
    }
    /*!
     * PUBLIC INTERFACE
     * Returns a t_ang corresponding to the angular position of the center of
     * pixel \a ipix in RING scheme at resolution \a nside.
     */
    t_ang ring2ang (std::int64_t nside, std::int64_t ipix)
    {
        return hp::loc2ang (hp::hpd2loc(nside, hp::ring2hpd (nside, ipix)));
    }
    /*!
     * PUBLIC INTERFACE
     * Returns a t_ang corresponding to the angular position of the center of
     * pixel \a ipix in NEST scheme at resolution \a nside.
     */
    t_ang nest2ang (std::int64_t nside, std::int64_t ipix)
    {
        return hp::loc2ang (hp::hpd2loc(nside, hp::nest2hpd (nside, ipix)));
    }

    /*!
     * PUBLIC INTERFACE
     * Returns the pixel number in RING scheme at resolution \a nside,
     * which contains the direction described the 3-vector \a vec.
     */
    std::int64_t vec2ring (std::int64_t nside, t_vec vec)
    {
        return hp::hpd2ring (nside, hp::loc2hpd (nside, hp::vec2loc (vec)));
    }
    /*!
     * PUBLIC INTERFACE
     * Returns the pixel number in NEST scheme at resolution \a nside,
     * which contains the direction described the 3-vector \a vec.
     */
    std::int64_t vec2nest (std::int64_t nside, t_vec vec)
    {
        return hp::hpd2nest (nside, hp::loc2hpd (nside, hp::vec2loc (vec)));
    }

    /*!
     * PUBLIC INTERFACE
     * Returns a normalized 3-vector pointing in the direction of the center
     * of pixel \a ipix in RING scheme at resolution \a nside.
     */
    t_vec ring2vec (std::int64_t nside, std::int64_t ipix)
    {
        return hp::loc2vec (hp::hpd2loc (nside, hp::ring2hpd (nside, ipix)));
    }
    /*!
     * PUBLIC INTERFACE
     * Returns a normalized 3-vector pointing in the direction of the center
     * of pixel \a ipix in NEST scheme at resolution \a nside.
     */
    t_vec nest2vec (std::int64_t nside, std::int64_t ipix)
    {
        return hp::loc2vec (hp::hpd2loc (nside, hp::nest2hpd (nside, ipix)));
    }

} // namespace hp (for healpix)
