---
layout: page
title: sm::hexfft
parent: Reference
nav_order: 11
permalink: /ref/hexfft/
---
# sm::hexfft
{: .no_toc}
## The hexagonal fast Fourier transform
{: .no_toc}
```c++
import sm.hexfft;
```

Module file: [sm/hexfft.cppm](https://github.com/sebsjames/maths/blob/main/sm/hexfft.cppm). Test code:
[tests/hexfft1](https://github.com/sebsjames/maths/blob/main/tests/hexfft1.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

The Hexagonal FFT algorithm, following Nicholas I. Rummelt's PhD thesis *Array set addressing: Enabling efficient hexagonally sampled image processing*, University of Florida, 2010.

With this code, you can present data arranged over a hexgrid (specifically, a `sm::hexgrid<F, hexalign::point_up>`) with an arbitrary boundary, and compute the two dimensional spatial Fourier transform. A hexgrid in the frequency space (a `sm::hexgrid<F, hexalign::flat_up>`) is created alongside a data container with the FFT result in it.

Defined as:
```c++
template<typename F = double, bool construct_hg_asa = false>
struct fft
```
Floating point type `F` is used for hexgrid coordinates and as the element type for `std::complex<F>` values. `construct_hg_asa` is a boolean which may be set true to create an optional hexgrid that is useful for debugging, but not required for the forward or inverse transforms.

The algorithm is made fast by splitting the hexgrid into two rectangular grids of alternating rows. For this reason, the input hexgrid must be enclosed by a perfect rectangular hexgrid (with zero-padding of new elements). Alternating rows are placed in two Array Set Addressing (ASA) grids, then the standard two dimensional, rectangular FFT can be applied.

For a practical, visualized example implementation, you can see the [hex_fft](https://github.com/sebsjames/hex_fft) repository.

## Quick usage guide

Create a hexgrid. The hexgrid constructor args are hex-hex distance, grid width and grid 'z' value (usually set to 0).

```c++
import sm.hexgrid;

sm::hexgrid<float sm::hexalign::point_up> hg(0.01f, 4.0f, 0.0f);
hg.set_circular_boundary (1.0f); // or any other boundary setting function in hexgrid
```

Create some data. The order of the data is defined by the hexgrid indexing. Each hexgrid element has a 'vector iterator', `vi` and provides access to the location of the hex.
```c++
import sm.vvec;

sm::vvec<float> data (hg.num(), 0.0f);
for (auto h : hg.hexen) {
    data[h.vi] = some_function_of (h.x, h.y);
}
```

Create an `sm::hexfft::fft` object and perform a forward transform. The result is stored in `hfft.X_hexgrid`, which is a `sm::vvec` of `std::complex<>` values.

```c++
import sm.hexfft;

sm::hexfft::fft<float> hfft (&hg); // construct and initialize
hfft.forward (data); // Perform forward FFT transform
```
You can modify the values in `X_hexgrid` to make filters. The values in `X_hexgrid` are associated with a frequency hexgrid, `hexfft::fft::hgf`, which is created when hfft is initialized.
```c++
for (auto h : hfft.hgf->hexen) { // hgf is a unique_ptr to a hexgrid
    std::cout << "FFT Frequency " << h.x << ", " << h.y
              << " has magnitude " << std::real(hfft.X_hexgrid[h.vi]) << std::endl;
}

```
After changing values in `hfft.X_hexgrid` (perhaps by masking) you can then inverse transform from frequency space to image space

```c++
sm::vvec<std::complex<float>> invimg = hfft.inverse();
```
The returned data is defined over your original hexgrid, `hg`.

## fft members

### Attributes populated during initialization

`hexfft::asa_rows` and `hexfft::asa_cols` (both `uint32_t` are populated with the dimensions of the two ASA grids.

`hexfft::ri_min` and `hexfft::gi_min` are configured with the `hex::ri`, `hex::gi` of the padded rectangular hexgrid's (a=0, r=0, c=0) corner.

The hexgrid pointer `hexfft::hg` is the point your provide at construction/init.

The hexgrid `hexfft::hgf` is constructed to match `hg` for the frequency space. It has alignment `hexalign::flat_up`.

`hexfft::Uscale` is a scaling factor (computed from image data hexgrid spacing) for the frequency hexgrid, hgf. Uscale allows the user to visualize the frequency hexgrid on a similar size scale to the input data, regardless of the spacing on the input data.

### Data attributes

`hexfft::d0` and `hexfft::d1` hold copies of the input data (or inverse-transformed data) in ASA format. d0 holds even rows, d1, odd rows. They are both `sm::vmat` containers of `std::complex<F>` values.

`hexfft::X0` and `hexfft::X1` are similar containers that hold the (ASA format) result of the forward transformation of `d0` and `d1`.

The data in `X0` and `X1` are manipulated, and then rearranged into a `vvec` of `std::complex<F>` values, `hexfft::X_hexgrid`. `X_hexgrid` is spatially defined by the frequency hexgrid `hexfft::hgf`.
