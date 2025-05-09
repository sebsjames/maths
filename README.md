# Seb's maths library

## A simple mathematics library for C++20 projects

This header-only library is intended to help you to write maths into
your C++ in simple, readable and comprehensible code.

It provides:

* Static and dynamically sized vector classes
* Transform matrices
* A quaternion class
* Scaling and range (or interval) classes
* Basic statistics including a histo class and a number of bootstrap methods
* Random number, and string generation
* A compatible HDF5 wrapper class
* Classes for working with 2D grids of data (Cartesian and hexagonal)
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
    sm::quaternion<float> q1 (sj::vec<float, 3>{1, 0, 0}, mc::pi_over_2);

    // Rotate the vector with the quaternion
    sm::vec<float, 3> v1_rotated = q1 * v1;

    std::cout << v1 << " rotated pi/2 about x-axis is " << v1_rotated << "\n";
}
```
