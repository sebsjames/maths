# Seb's maths library

![cmake ubuntu 24 gcc 11 build test](https://github.com/sebsjames/maths/actions/workflows/cmake-ubu24-gcc12.yml/badge.svg)
![cmake ubuntu 24 default (gcc 13) build test](https://github.com/sebsjames/maths/actions/workflows/cmake-ubuntu-2404.yml/badge.svg)
![cmake ubuntu 24 gcc 14 build test](https://github.com/sebsjames/maths/actions/workflows/cmake-ubu24-gcc14.yml/badge.svg)
![cmake ubuntu 24 clang 16 build test](https://github.com/sebsjames/maths/actions/workflows/cmake-ubu24-clang16.yml/badge.svg)
![cmake ubuntu 24 clang 17 build test](https://github.com/sebsjames/maths/actions/workflows/cmake-ubu24-clang16.yml/badge.svg)
![cmake ubuntu 24 clang 18 build test](https://github.com/sebsjames/maths/actions/workflows/cmake-ubu24-clang16.yml/badge.svg)
![cmake ubuntu 22 default (gcc 11?) build test](https://github.com/sebsjames/maths/actions/workflows/cmake-ubuntu-2204.yml/badge.svg)
![cmake mac 14 build test](https://github.com/sebsjames/maths/actions/workflows/cmake-mac-14.yml/badge.svg)
![cmake mac 15 build test](https://github.com/sebsjames/maths/actions/workflows/cmake-mac-15.yml/badge.svg)
![cmake windows 22 build test](https://github.com/sebsjames/maths/actions/workflows/cmake-windows-2022.yml/badge.svg)

## A mathematics library for modern C++

This header-only library is intended to help you to write maths into
your C++ in simple, readable and comprehensible code.

It provides:

* Static and dynamically sized vector classes
* Transform matrices
* A quaternion class
* Scaling and range (or interval) classes
* Random number, and string generation
* Classes for working with 2D grids of data (Cartesian and hexagonal)
* Bezier curves
* A variety of algorithms (Nelder-Mead, simulated annealing, winding numbers, box filter)
* Basic statistics including a histo class and a number of bootstrap methods
* A compatible HDF5 wrapper class
* A set of constexpr maths methods, derived from Keith O'Hara's GCEM project

The vector classes are compatible with C++ algorithms, and have their
own built-in methods.

You can stream objects to debug or observe their values.

The namespace is just `sm` (I like short namespaces for frequently used types).

Here's an example

```c++
#include <sm/mathconst>
#include <sm/vec>
#include <sm/quaternion>

int main()
{
    // Mathematical constants are provided by mathconst
    using mc = sm::mathconst<float>;

    // Create a fixed-size mathematical 3D vector object
    sm::vec<float, 3> v1 = { 1, 2, 3 };

    // Create and intialize a quaternion rotation
    sm::quaternion<float> q1 (sm::vec<float, 3>{1, 0, 0}, mc::pi_over_2);

    // Rotate the vector with the quaternion
    sm::vec<float, 3> v1_rotated = q1 * v1;

    std::cout << v1 << " rotated pi/2 about x-axis is " << v1_rotated << "\n";
}
```
