---
layout: page
title: sm::pca
parent: Reference
nav_order: 10
permalink: /ref/pca/
---
# sm::pca
{: .no_toc}
## Principal Component Analysis
{: .no_toc}
```c++
import sm.pca;
```
**Table of contents**

- TOC
{:toc}

## Summary

`sm::pca` is a Principal Component Analysis namespace. It provides:

`sm::pca::compute` A function that finds the principal components in a dataset.

`sm::pca::transform` A function to project your data onto the principal components.

`sm::pca::result` A class that holds the result returned by `pca::compute`.

## Example usage

To use `pca::compute`, you need to pack your data either as a 'vvec of
vecs' or a 'vec of vvecs'. Suppose you have 3 arrays of data, then
first ensure each is an `sm::vvec` (with a `float` or `double` element
type) and then pack them in an `sm::vec`:

```c++
sm::vvec<float> d1 = {1,  2,   3,   2};
sm::vvec<float> d2 = {1,  0.9, 0.8, 2};
sm::vvec<float> d3 = {0,  8,   7,   1};

sm::vec<sm::vvec<float>, 3> x;

x[0] = d1;
x[1] = d2;
x[2] = d3;

sm::pca::result<float, 3> my_pca = sm::pca::compute<float, 3> (x);

if (my_pca.error != sm::pca::error_code::no_error) { /* Something went wrong */ }
```

The return object contains a centered copy of the input data x (but
not an original copy of x) along with the principal components, which are stored in

```c++
struct result
{
    // ...
    // The principal component unit vectors
    sm::vec<sm::vec<T, N>, N> pc_vectors
    // ...
};
```

You can output the components like this:
```c++
for (std::uint32_t i = 0; i < 3; ++i) {
    std::cout << "PC " << (i + 1) << " = " << my_pca.pc_vectors[i]
              << " accounts for " << my_pca.pc_proportions[i] << " of the variability\n";
}
```
Note that they are ordered in size; `pc_vector[0]` is the first
principal component, which accounts for the most variability.
The magnitude of the component is stored in `sm::vec<T, N>
pc_magnitudes` and the proportions in `sm::vec<T, N> pc_proportions`.

You use a second function call to project your data onto the principal
components. The `pca::transform` function takes a reference to your
result object and computes (and stores, but maybe this design is
wrong) the projected points:

```c++
sm::pca::transform (my_pca); // Perform the projection
```
You now have access to the projected data (in my_pca.x_proj).

## The result struct

```c++
    // Return struct for a principal component analysis with N dimensions of data
    template<typename T, std::uint32_t N> requires std::is_floating_point_v<T>
    struct result
    {
        // A status or error code
        pca::error_code error = pca::error_code::uncomputed; // changes to x_cols_unequal, x_empty or no_error
        // The length of each column of z
        std::uint32_t dsz = 0u;
        // The mean and standard deviation of each dimension of the input data
        sm::vec<sm::vvec<T>, N> mu_sig_x;
        // The input data, after it has been zero-shifted
        sm::vec<sm::vvec<T>, N> z;
        // The covariance matrix of the zero-shifted data
        sm::mat<T, N, N> covariance;
        // The principal component unit vectors
        sm::vec<sm::vec<T, N>, N> pc_vectors;
        // Principal component magnitudes (like the SD of the data in each dimension)
        sm::vec<T, N> pc_magnitudes = {};
        // Principal component proportions (of the variability each component accounts for)
        sm::vec<T, N> pc_proportions = {};
        // This holds the centred input data, projected onto the principal components
        sm::vec<sm::vvec<T>, N> x_proj;
        //... some code omitted
    };
```