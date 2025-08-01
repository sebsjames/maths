---
layout: page
title: sm::mat44
parent: Reference
nav_order: 7
permalink: /ref/mat44/
---
# sm::mat44
{: .no_toc }
## A 4x4 transformation matrix class
{: .no_toc }
```c++
#include <sm/mat44>
```

Header file: [<sm/mat44>](https://github.com/sebsjames/maths/blob/main/sm/mat44). Test and example code:  [tests/testTransformMatrix](https://github.com/sebsjames/maths/blob/main/tests/testTransformMatrix.cpp) [tests/testTranslatePretranslate](https://github.com/sebsjames/maths/blob/main/tests/testTranslatePretranslate.cpp) [tests/testTransMat_constexpr](https://github.com/sebsjames/maths/blob/main/tests/testTransMat_constexpr.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary
A 4x4 matrix class with a templated element type. `constexpr` capable.

Defined as:
```c++
namespace sm {
    template <typename F>
    struct mat44 {
        // ...
        std::array<F, 16> mat;
```
where `F` hints that the template arg is usually a floating point type (although it may be any signed arithmetic type). The data is stored in an `std::array` in column-major format; the left-most column of the matrix is stored in the first 4 elements of the array.

Note that this class template (along with `mat22` and `mat33`) is not designed with template parameters for rows and columns; it retains the simplicity of providing just a square transform matrix.

## Create a mat44

When you create a `mat44` with no constructor arguments, the matrix is initialized as the identity matrix

```c++
sm::mat44<double> m;
std::cout << "Initialized as the identity matrix:\n" << m << std::endl;
```

You can create and assign an initializer list
```c++
sm::mat44<double> m = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
std::cout << m << std::endl;
```
Gives:
```
[ 1, 5, 9, 13,
  2, 6, 10, 14,
  3, 7, 11, 15,
  4, 8, 12, 16 ]
```
See how the initializer list is placed into the matrix column-by-column.

The list may contain fewer than 16 elements:

```c++
sm::mat44<double> m = { 1, 2, 3, 4 };
std::cout << m << std::endl;
```
Gives:
```
[ 1, 0, 0, 0,
  2, 0, 0, 0,
  3, 0, 0, 0,
  4, 0, 0, 0 ]
```

The additional elements are all set to 0. This means you can create a null matrix with

```c++
sm::mat44<double> m = { 0 };
std::cout << m << std::endl;
```
Gives:
```
[ 0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0 ]
```

But **careful**: if you provide an empty brace list, you will cause the default constructor to be called and the matrix will be the identity matrix. If you want a null matrix, use `{ {} }` or `{0}`:

```c++
sm::mat44<double> m1 = {};      // Yields the identity matrix, NOT the null matrix
sm::mat44<double> m2 = { {} };  // Yields a null matrix
sm::mat44<double> m3 = { 0.0 }; // Yields a null matrix
```

## Set data in the mat44

### Raw access to the elements

You can set the data manually with an initializer list:
```c++
sm::mat44<int> m; // initially set up as identity matrix
m = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
std::cout << "Matrix contains:\n" << m << std::endl;
```
which outputs:
```
Matrix contains:
[ 0 , 4 , 8 , 12 ;
  1 , 5 , 9 , 13 ;
  2 , 6 , 10 , 14 ;
  3 , 7 , 11 , 15 ]
```

You can change individual elements of the matrix with the array access operator:
```c++
m[3] = 100;
std::cout << "Matrix updated:\n" << m << std::endl;
```

The updated matrix is:
```
Matrix updated:
[ 0 , 4 , 8 , 12 ;
  1 , 5 , 9 , 13 ;
  2 , 6 , 10 , 14 ;
  100 , 7 , 11 , 15 ]
```

You can reset a mat44 to the identity matrix:
```c++
m.setToIdentity();
```

### Setting elements by transforms

Often, you want to set the values in the matrix by defining translations and rotations.

```c++
sm::mat44<float> m;
sm::vec<float, 3> t = { 1, 2, 3 };
m.translate (t); // t could also be std::array<float, 3>
// or equivalently:
sm::mat44<float> m2;
m2.translate (1.0f, 2.0f, 3.0f);
```

`translate` applies the translation *after* any rotation that is
specified in the linear part of the matrix (the top left 3x3 sub-matrix).

The `rotate` methods apply a rotation into the linear part of the matrix:

```c++
sm::mat44<float> m;
// Rotate by axis and angle
sm::vec<float> axis = { 0, 1, 0 }; // You can also use std::array<float, 3>
m.rotate (axis, sm::mathconst<float>::pi_over_2);
// Rotate using a sm::quaternion
sm::mat44<float> m2;
sm::quaternion<double> qd (axis.as_double(), sm::mathconst<double>::pi_over_4);
m2.rotate (qd); // Note that the quaternion does not have to have the same
                // element type as the mat44
```

The `rotate` methods pre-multipy the 'this' matrix by a pure rotation matrix.

After applying a rotation with `rotate`, you can apply a translation that occurs *before* the rotation with `pretranslate`:

```c++
sm::mat44<float> m;
sm::vec<float> axis = { 0, 1, 0 };
m.rotate (axis, sm::mathconst<float>::pi_over_2);
sm::vec<float> t = { 2, 0, 0 }; // a translation
m.pretranslate (t); // m now encodes a translation followed by a rotation about (0,1,0)
```

Finally, there are the `prerotate` methods, which apply a rotation
before any existing rotations and translations that are already
encoded in the matrix. These methods post-multiply the 'this' matrix
with a pure rotation matrix.

```c++
sm::mat44<float> m;
sm::vec<float> t = { 2, 0, 0 };
m.translate (t);
sm::vec<float> axis = { 0, 1, 0 };
m.prerotate (axis, sm::mathconst<float>::pi_over_2);
// m now encodes a rotation about (0,1,0) followed by a translation of (2,0,0)
```

The rotate, prerotate, translate and pretranslate methods have equivalent names to similar functions in Eigen.

In addition to rotate and translate functions, `mat44` provides
`scale` functions:

```c++
sm::mat44<float> m;
sm::vec<float> t = { 2, 0, 0 };
m.translate (t);
sm::vec<float> scaling = { 0.5, 1, 1.5 };
m.scale (scaling);
```
Here, the 3D scaling can be provided as a `sm::vec<float, 3>`,
`std::array<float, 3>` or as three floats.

It's also possible to specify an equal scaling in all elements:

```c++
sm::mat44<float> m;
sm::vec<float> t = { 2, 0, 0 };
m.translate (t);
float scaling = 1.2f;
m.scale (scaling);
```

### Special setters

`frombasis` `perspective` and `orthographic`

## Matrix properties

```c++
sm::mat44<float> m;
float d = m.determinant();
float t = m.trace();
```

## Matrix operations

```c++
sm::mat44<float> m;
sm::mat44<float> the_inverse = m.inverse(); // Returns the inverse.
m.transpose(); // Transposes the matrix in place
```