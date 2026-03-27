# Seb's maths library

![GCC 14](https://github.com/sebsjames/maths/actions/workflows/ubuntu-cmakeninja-gcc14.yml/badge.svg)
![GCC 15](https://github.com/sebsjames/maths/actions/workflows/ubuntu-cmakeninja-gcc15.yml/badge.svg)
![Clang 18](https://github.com/sebsjames/maths/actions/workflows/ubuntu-cmakeninja-clang18.yml/badge.svg)
![Clang 20](https://github.com/sebsjames/maths/actions/workflows/ubuntu-cmakeninja-clang20.yml/badge.svg)
![Clang 22](https://github.com/sebsjames/maths/actions/workflows/ubuntu-cmakeninja-clang22.yml/badge.svg)
![Windows 2022](https://github.com/sebsjames/maths/actions/workflows/cmake-windows-2022.yml/badge.svg)

## A modules-native mathematics library for modern C++

This C++ modules-native library is intended to help you to write maths into
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

You can visualize the data in [mathplot](https://github.com/sebsjames/mathplot) (this repo was developed alongside mathplot when they were both 'morphologica').

The namespace is just `sm` (I like short namespaces for frequently used types).

Here's an example

```c++
#include <iostream> // possibly import std; in the future.
import sm.mathconst;
import sm.vec;
import sm.quaternion;
import sm.mat;

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

    // Make a rotation matrix that encodes the rotation from the quaternion
    sm::mat<float, 4, 4> rmat (q1);

    std::cout << "Rotation matrix:\n" << rmat << "\n";
}
```

## Can I use the code header-only?

I decided to go fully modules-native with this code. However, there is a `header-only` branch that contains the code in its last header-only form.

## Build requirements

Minimum compilers: g++-14*, clang++-18, Visual Studio 2022.
You will need cmake version 3.28.5 as a minimum.
Ninja is required as a partner for cmake because GNU make does not yet support C++-20 modules.

*gcc 14 can only compile a subset of the test programs. gcc 15 compiles all the tests. gcc 16 may be required to compile the modules version of [mathplot](https://github.com/sebsjames/mathplot).*

### Installing Clang on Ubuntu 24

Clang-18, Clang-19 and Clang-20 are available in the default repositories:

```bash
sudo apt install clang-18 clang-tools-18 # need both.
```

### Installing GCC on Ubuntu 24

gcc 14 is available in the default repositories, but to install the recommended gcc 15, you can add an extra repository:

```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test # provides gcc-15
sudo apt install gcc-15 g++-15
```

### Building with Clang

```bash
mkdir build
cd build
# A cmake call something like (if you want to use clang and your default libc++/libstdc++:
CC=clang-20 CXX=clang++-20 cmake .. -GNinja
# Optionally add -DCMAKE_CXX_FLAGS="-stdlib=libc++" to compile with libc++ if it not your default
# possibly also -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi"
ninja
```

### Building with GCC

```bash
mkdir build
cd build
# A cmake call something like (if you want to use gcc and your default c++ runtime library
CC=gcc-15 CXX=g++-15 cmake .. -GNinja
ninja
```
