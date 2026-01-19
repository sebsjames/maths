---
title: Welcome
layout: home
permalink: /
has_children: true
nav_order: 0
---
# Welcome to Seb's maths
{: .no_toc}
This is a maths library for your modern C++ projects. It was created for [mathplot](https://github.com/sebsjames/mathplot) and has been used in all of my recent scientific modelling work. I wanted a set of classes that would be *convenient* and *pleasurable* to use when coding straightforward mathematics. On this page, I'll give you a taste of what you can do with the library. For full details, refer to the Reference pages.

- TOC
{:toc}

## Mathematical constants
`mathconst` provides a set of templated, type-correct mathmatical constants. Stop using M_PI!
```c++
#include <sm/mathconst>
sm::mathconst<float>::pi      // A static constexpr float representation of pi
sm::mathconst<double>::root_2 // Square root of two, double precision
```

## Closed intervals
The `sm::range` class specifies a closed interval [a, b] to get min/max pairs. It's a small class, but it gets frequent use.
```c++
#include <sm/vvec>
#include <sm/range>
sm::vvec<int> v = { -1, 2, 3, 10 };
sm::range<int> r = v.range();
std::cout << r;  // Outputs: [ -1, 10 ]; r.min == -1; r.max == 10
```

## Numerical scaling
`sm::scale` provides scaling between containers of numbers, whether or not they are of the same type.
```c++
#include <sm/scale>
sm::scale<float, double> s; // Input is float, output is double
s.do_autoscale = true;      // We want to autoscale based on the input
sm::vvec<float> input = {1,2,3,4,5,8,9,18};  // Input data
sm::vvec<double> output(vf.size());          // Output/transformed data
s.transform (input, output); // Output will now be in the range [0, 1]
```

## Random numbers
I wrap the modern C++ random number generation from `#include <random>` in `<sm/random>`. Currently the following distributions are provided: uniform, normal, lognormal, Poisson, exponential, Pareto and Von Mises.
```c++
#include <sm/random>
sm::rand_uniform<unsigned int> rng (0, 100);      // Uniform distribution in [0, 100]
unsigned int sample = rng.get()                   // Sample once from the RNG
std::vector<unsigned int> samples = rng.get(100); // Sample 100 times
```
## Random strings
Also provided by `<sm/random>` is a random string generator:
```c++
sm::rand_string sgen;
sgen.setCharGroup (sm::CharGroup::AlphaNumeric);
std::string rstring = sgen.get(16); // Gets 16 random alpha-numeric characters
```

## Mathematical vectors

`sm::vec` and `sm::vvec` provide fixed-size (`constexpr` capable) and dynamically resizable vectors for your programs.
```c++
#include <sm/vec>
#include <sm/vvec>
constexpr sm::vec<float, 3> v1 = { 0, 1, 2 };   // 3D fixed size vectors
sm::vec<float, 3> v2 = { -2, 0, 7 };            //
sm::vec<float, 3> v3 = v1 + v2;                 // Operators work naturally
sm::vec<float, 3> v4 = -v2;                     //
v3.renormalize();                               // There are lots of functions
float max = v1.max();                           //
sm::vvec<double> var_size = { 1, 2, 3, 4, 5 };  // Variable size vectors
std::cout << var_size;                          // Streamable for debug
double thesum = var_size.sum();                 // vvec also has functions
sm::vvec<sm::vec<float, 3>> vector_of_vecs = {} // vvec of vecs? No problem.
```

## Quaternions and transform matrices

The classes `sm::quaternion`, `sm::mat44`, `sm::mat33` and `sm::mat22` fulfil your vector transformation needs.
```c++
#include <sm/quaternion>
sm::quaternion<float> q1 (sm::vec<float>{0, 0, 1}, sm::mathconst<float>::pi_over_2);
sm::quaternion<float> q2 (sm::vec<float>{0, 1, 1}, sm::mathconst<float>::pi_over_4);
sm::quaternion<float> q3 = q1 * q2;

#include <sm/mat44>
sm::mat44<float> m;
m.prerotate (q3)                     // Set a (pre)rotation from a quaternion
m.translate (sm::vec<float>{0,1,2}); // Set a translation into the matrix
sm::vec<float, 3> v = {0,0,1};
sm::vec<float, 4> vout = m * v;      // Transform a 3D vector, always get a 4D return
```

## Constexpr maths

`sm::vec`, `sm::quaternion` and `sm::mat44` are all `constexpr` capable in C++20. For this I had to incorporate a constexpr capable set of the basic math functions to stand in place of those from the `std` namespace. Find these in the `sm::cem` namespace with `#include <constexpr_maths>`. Adapted from Keith O'Hara's GCE Math library with thanks.

## Two dimensional grids

`sm::grid` allows you to manage rectangular grids of coordinates in programs that do 2D maths. `sm::cartgrid` is similar, but allows the boundary to be of an arbitrary shape. `sm::hexgrid` allows you to manage hexagonal grids! These objects have corresponding visualization classes in mathplot such as in this [hexgrid image example](https://github.com/sebsjames/mathplot/tree/main/examples#hexgrid_image)

## Histograms
See `<sm/histo>` for 1D histograms and `<sm/hexyhisto>` for hexagonal 2D histograms.
```c++
#include <sm/vvec>
#include <sm/histo>
sm::vvec<double> numbers = { 1, 1.5, 2, 3, 4.1, 4.4, 4.9 };
sm::histo<double, float> h(numbers, 3);
std::cout << "Bin edges are: " << h.binedges << std::endl;
std::cout << "Proportions are: " << h.proportions << std::endl;
```

## Optimization
Implementations of the Nelder-Mead (`sm::nm_simplex`) and Simulated Annealing (`sm::anneal`) algorithms are provided.

## Bezier curves

The grid classes `sm::hexgrid` and `sm::cargrid` make use of Bezier curves to specify arbitrary boundaries. see `<sm/bezcurve>` and friends.

## Algorithms

There are a number of well known algorithms that I've had need of. These include:

* An efficient implementation of the boxfilter algorithm (`<sm/boxfilter>`)
* A winding number computation (`<sm/winder>`)
* Image resampling methods in `sm::hexgrid` and `sm::cartgrid`
* Bootstrap statistical analyses (`<sm/bootstrap>`)
* The crc32 checksum (`<sm/crc32>`)
* base64 encoding (`<sm/base64>`)
* Line segment crossing, linear regression, covariance and several other miscellaneous algos in `<sm/algo`>
