/*
 * HexGrid algorithms. These are algorithms that require an sm::hexgrid to be used, such as
 * convolving over a hexgrid or resampling over a hexgrid.
 *
 * \author Seb James
 * \date Sept. 2026
 */

module;

#include <cstdint>
#include <vector>
#include <list>
#include <stdexcept>
#include <cmath>

export module sm.algo.hexgrid;

import sm.hex;
import sm.interval;

export import sm.hexgrid;
export import sm.vec;
export import sm.vvec;

export namespace sm::algo::hexgrid
{
    /*!
     * Using hexgrid hg as the domain, convolve the domain data \a data with the
     * kernel data \a kerneldata, which exists on another hexgrid, \a
     * kernelgrid. Return the result in \a result.
     *
     * \tparam F the floating point type for the hexgrid coords
     * \tparam t the type for the data
     */
    template<typename F, typename T>
    void convolve (const sm::hexgrid<F>& hg,
                   const sm::hexgrid<F>& kernelgrid, const std::vector<T>& kerneldata,
                   const std::vector<T>& data, std::vector<T>& result)
    {
        if (result.size() != hg.hexen.size()) {
            throw std::runtime_error ("The result vector is not the same size as the hexgrid.");
        }
        if (result.size() != data.size()) {
            throw std::runtime_error ("The data vector is not the same size as the hexgrid.");
        }
        if (kernelgrid.get_d() != hg.d) {
            throw std::runtime_error ("The kernel hexgrid must have same d as this hexgrid to carry out convolution.");
        }
        if (&data == &result) {
            throw std::runtime_error ("Pass in separate memory for the result.");
        }

        // For each hex in this hexgrid, compute the convolution kernel
        typename std::list<sm::hex<F>>::const_iterator hi = hg.hexen.begin();
        for (; hi != hg.hexen.end(); ++hi) {
            T sum = T{0};
            // For each kernel hex, sum up.
            for (auto kh : kernelgrid.hexen) {
                typename std::list<sm::hex<F>>::const_iterator dhi = hi;
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
     *
     * \tparam F the floating point type for the hexgrid coords and data
     */
    template <typename F>
    sm::vvec<F> resample_regular_data (const sm::hexgrid<F>& hg,
                                       const sm::vvec<F>& _data,
                                       const sm::vvec<sm::vec<F, 2>>& _coords,
                                       const F g_sigma)
    {
        std::uint32_t csz = _data.size();

        // Return data object for the resampled result
        sm::vvec<F> expr_resampled(hg.num(), 0.0f);

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
        for (typename std::vector<F>::size_type xi = 0u; xi < hg.d_x.size(); ++xi) {
            F expr = 0.0f;
            for (std::uint32_t i = 0; i < csz; ++i) {
                // Get x/y pixel coords:
                // sm::vec<std::uint32_t, 2> idx = {(i % image_pixelsz[0]), (i / image_pixelsz[0])};
                // Get the coordinates of the pixel at index i (in hexgrid units):
                // Distance from input pixel to output hex:
                const F _d_x = hg.d_x[xi] - _coords[i][0];
                const F _d_y = hg.d_y[xi] - _coords[i][1];
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
     *
     * \tparam F the floating point type for the hexgrid coords and data
     */
    template<typename F>
    sm::vvec<F> resample_image (const sm::hexgrid<F>& hg,
                                const sm::vvec<F>& image_data,
                                const std::uint32_t image_pixelwidth,
                                const sm::vec<F, 2>& image_scale,
                                const sm::vec<F, 2>& image_offset)
    {
        std::uint32_t csz = image_data.size();
        sm::vec<std::uint32_t, 2> image_pixelsz = {image_pixelwidth, csz / image_pixelwidth};

        // Return data object for the resampled result
        sm::vvec<F> expr_resampled(hg.num(), 0.0f);

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
        for (typename std::vector<F>::size_type xi = 0u; xi < hg.d_x.size(); ++xi) {
            F expr = 0.0f;
            for (std::uint32_t i = 0; i < csz; ++i) {
                // Get x/y pixel coords:
                sm::vec<std::uint32_t, 2> idx = {(i % image_pixelsz[0]), (i / image_pixelsz[0])};
                // Get the coordinates of the pixel at index i (in hexgrid units):
                sm::vec<F, 2> posn = (dist_per_pix * idx) - input_centering_offset + image_offset;
                // Distance from input pixel to output hex:
                F _d_x = hg.d_x[xi] - posn[0];
                F _d_y = hg.d_y[xi] - posn[1];
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
}
