# sj::maths: A small mathematics library for C++20 projects

This header-only library is intended to help you to write maths into
your C++ code in simple, readable and comprehensible code.

It provides static and dynamically sized vector classes, transform
matrices, a quaternion and a scaling and range (or interval) to
support your data. Simple statistics are available, including a histo
class and a number of bootstrap methods.

The vector classes are compatible with C++ algorithms, and have their
own built-in methods.

You can stream objects to debug or observe their values.

Here's an example

```c++
#include <sj/mathconst.h>
#include <sj/vec.h>
#include <sj/quaternion.h>

int main()
{
    using mc = sj::mathconst<float>;
    sj::vec<float, 3> v1 = { 1, 2, 3 };
    sj::quaternion<float> q1 (sj::vec<float, 3>{1, 0, 0}, mc::pi_over_2);
    // Rotate v1:

    auto v1_rotated = q1 * v1;
    std::cout << v1 << " rotated pi/2 about x-axis is " << v1_rotated << "\n";
}
```