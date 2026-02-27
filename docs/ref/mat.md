---
layout: page
title: sm::mat
parent: Reference
nav_order: 2
permalink: /ref/mat/
---
# sm::mat
{: .no_toc }
## A matrix class
{: .no_toc }
```c++
#include <sm/mat>
```

Header file: [<sm/mat>](https://github.com/sebsjames/maths/blob/main/sm/mat). Test and example code:
[tests/mat_2x2_general](https://github.com/sebsjames/maths/blob/main/tests/mat_2x2_general.cpp)
[tests/mat_3x3_general](https://github.com/sebsjames/maths/blob/main/tests/mat_3x3_general.cpp)
[tests/mat_4x4](https://github.com/sebsjames/maths/blob/main/tests/mat_4x4.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary
A general matrix class with a templated element type and dimensions. `constexpr` capable.

Defined as:
```c++
namespace sm
{
    template <typename F, uint32_t Nr, uint32_t Nc = Nr> requires std::is_floating_point_v<F>
    struct mat
    {
        // ...
        sm::vec<F, Nr * Nc> arr;
```
where `F` must be a floating point type, `Nr` is the number or rows in the matrix and `Nc` the number of columns.
The data is stored in an `sm::vec` array in column-major format; the left-most column of the matrix is stored in the first 4 elements of the array.

This class is capable of transformation matrix operations, and it is often used to create 2x2, 3x3 and 4x4 matrices.

## Create a mat

When you create a `mat` with no constructor arguments, the matrix is
initialized as the identity matrix if it is square or a null matrix
otherwise.

```c++
sm::mat<double, 4, 4> m;
std::cout << "Initialized as the 4x4 identity matrix:\n" << m << std::endl;
```

You can create a square `mat` by omitting the `Nc` template argument, which is then set equal to `Nr`:
```c++
sm::mat<double, 4> m;
std::cout << "Initialized as the 4x4 identity matrix:\n" << m << std::endl;
```

You can create and assign an initializer list
```c++
sm::mat<double, 4, 4> m = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
std::cout << m << std::endl;
```
Gives:
```
[ 1, 5, 9, 13 ;
  2, 6, 10, 14 ;
  3, 7, 11, 15 ;
  4, 8, 12, 16 ]
```
See how the initializer list is placed into the matrix column-by-column.

The list may contain fewer than `Nr` * `Nc` = 16 elements:

```c++
sm::mat<double, 4> m = { 1, 2, 3, 4 };
std::cout << m << std::endl;
```
Gives:
```
[ 1, 0, 0, 0 ;
  2, 0, 0, 0 ;
  3, 0, 0, 0 ;
  4, 0, 0, 0 ]
```

The additional elements are all set to 0. This means you can create a null matrix with

```c++
sm::mat<double, 4> m = { 0 };
std::cout << m << std::endl;
```
Gives:
```
[ 0, 0, 0, 0 ;
  0, 0, 0, 0 ;
  0, 0, 0, 0 ;
  0, 0, 0, 0 ]
```

But **careful**: if you provide an empty brace list, you will cause the default constructor to be called and (if it is square) the matrix will be the identity matrix. If you want a null matrix, use `{ {} }` or `{0}`:

```c++
sm::mat<double, 4> m1 = {};      // Yields the identity matrix, NOT the null matrix
sm::mat<double, 4> m2 = { {} };  // Yields a null matrix
sm::mat<double, 4> m3 = { 0.0 }; // Yields a null matrix
```

## Set data in the sm::mat

### Raw access to the elements

You can set the data manually with an initializer list:
```c++
sm::mat<int, 4> m; // initially set up as identity matrix
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

You can reset an sm::mat to the identity matrix:
```c++
m.set_identity();
```

### Setting elements by transforms

Often, you want to set the values in the matrix by defining translations and rotations.

#### Translation

```c++
sm::mat<float, 4> m;
sm::vec<float, 3> t = { 1, 2, 3 };
m.translate (t); // t could also be std::array<float, 3>
// or equivalently:
sm::mat<float, 4> m2;
m2.translate (1.0f, 2.0f, 3.0f);
```

`translate` applies the translation as a *post-multiplication* to the
existing matrix (`m` in the code above). `m.translate (t)` is
equivalent to the mathematical notation:

**M** = **M** * **T**.

**M** becomes a matrix that first applies the translation (because
**T** is the right-most matrix in the multiplication) and then applies
whatever transformation was originally in **M**.

`pretranslate` applies a translation as a *pre-multiplication*, making
`m.pretranslate (t)` equivalent to the following maths:

**M** = **T** * **M**.

With `pretranslate`, the requested translation is applied *following* any other
transformations that were specfied in **M**.

`translate` and `pretranslate` have the same effect as the functions of
the same names in [`Eigen::Transform`](https://libeigen.gitlab.io/eigen/docs-nightly/classEigen_1_1Transform.html).

#### Rotation

The `rotate` methods apply a rotation into the linear part of the matrix:

```c++
sm::mat<float, 4> m;
// Rotate by axis and angle
sm::vec<float> axis = { 1, 1, 0 }; // You can also use std::array<float, 3>
m.rotate (axis, sm::mathconst<float>::pi_over_2);
```

The `rotate` methods *post-multiply* the 'this' matrix by a pure
rotation matrix (again this is the same behaviour as Eigen), with the
equivalent mathematical notation for a rotation matrix **R** being:

**M** = **M** * **R**

You can also rotate by passing a quaternion to `rotate`:

```c++
// Rotate using a sm::quaternion
sm::mat<float, 4> m2;
sm::quaternion<double> qd (axis.as_double(), sm::mathconst<double>::pi_over_4);
m2.rotate (qd); // Note that the quaternion does not have to have the same
                // element type as the mat
```

The axis in the quaternion constructor is always renormalized before
being used to define a rotation.

Finally, there are the `prerotate` methods, which apply the rotation
as a pre-multiplication.

`prerotate` is equivalent to

**M** = **R** * **M**

with the rotation pre-multiplying the existing matrix and therefore
*following* any transformations already encoded in *M*.

##### Rotation axis

Note that the `axis` passed to `rotate` (or `prerotate`) will be
automatically renormalized. If you *know* that your rotation axis is
already normalized and you want to save a few computations, or you
have another reason for rotating about an unnormalized axis vector,
you can use:

```c++
sm::vec<float> normalized_axis = { 0, 1, 0 };
constexpr bool renorm = false;
m.rotate<renorm> (normalized_axis, sm::mathconst<float>::pi_over_2);
// or
m.prerotate<renorm> (normalized_axis, sm::mathconst<float>::pi_over_2);
```

You just have to override the `renorm` template parameter whose
default is `true`.

##### 2D rotations

If your mat is 2x2, then there is a specific `rotate` method that takes a scalar for the angle of rotation.
```c++
//! Set this matrix up so that it would rotate a 2D vector by rot_rad radians, anticlockwise.
template <typename T = F> requires std::is_arithmetic_v<T> && (Nr == 2) && (Nc == 2)
constexpr void rotate (const T rot_rad) noexcept
{ ... }
```

#### Scale

In addition to rotate and translate functions, `sm::mat` provides
`scale` functions:

```c++
sm::mat<float, 4> m;
sm::vec<float> t = { 2, 0, 0 };
m.translate (t);
sm::vec<float> scaling = { 0.5, 1, 1.5 };
m.scale (scaling);
```
Here, the 3D scaling can be provided as a `sm::vec<float, 3>`,
`std::array<float, 3>` or as three floats.

It's also possible to specify an equal scaling in all elements:

```c++
sm::mat<float, 4> m;
sm::vec<float> t = { 2, 0, 0 };
m.translate (t);
float scaling = 1.2f;
m.scale (scaling);
```

### Special setter `frombasis`

```c++
static constexpr mat<F, Nr, Nc> frombasis (const sm::vec<F> bx, const sm::vec<F> by, const sm::vec<F> bz);
constexpr void frombasis_inplace (const sm::vec<F> bx, const sm::vec<F> by, const sm::vec<F> bz);
```

The static function `frombasis` sets up a coordinate transformation, using a set of three basis vectors, which it returns.
`frombasis_inplace` is the non-static counterpart.

```c++
sm::vec<float> bx = { 0.707f, 0.707f, 0.0f };
sm::vec<float> by = { -0.707f, 0.707f, 0.0f };
sm::vec<float> bz = { 0, 0, 1 };

sm::mat<float, 4> mfb = sm::mat<float, 4>::frombasis (bx, by, bz);
```
The matrix now encodes a transformation of a vector from the right handed Cartesian coordinate frame into the frame specified by the vectors bx, by and bz.
```c++
    std::cout << "With matrix\n\n" << mfb << ",\n\n" << sm::vec<float>::ux() << " transforms to "
              << mfb * sm::vec<float>::ux() << std::endl << sm::vec<float>::uy() << " transforms to "
              << mfb * sm::vec<float>::uy() << std::endl << sm::vec<float>::uz() << " transforms to "
              << mfb * sm::vec<float>::uz() << std::endl << " and (1,2,3) transforms to "
              << mfb * sm::vec<>{1,2,3} << std::endl;
```
gives output:
```
With matrix

[ 0.707 , -0.707 , 0 , 0 ;
  0.707 , 0.707 , 0 , 0 ;
  0 , 0 , 1 , 0 ;
  0 , 0 , 0 , 1 ],

(1,0,0) transforms to (0.707000017,0.707000017,0,1)
(0,1,0) transforms to (-0.707000017,0.707000017,0,1)
(0,0,1) transforms to (0,0,1,1)
 and (1,2,3) transforms to (-0.707000017,2.12100005,3,1)
```

### Special setter  `perspective`

```c++
static constexpr sm::mat<F, Nr, Nc> perspective (F fovDeg, F aspect, F zNear, F zFar) noexcept;
constexpr void perspective_inplace (F fovDeg, F aspect, F zNear, F zFar) noexcept;
```

`perspective` and `perspective_inplace` set up a perspective (or frustrum) projection, for use in computer graphics applications.

The field of view for the projection is given in degrees, measured from the top of the field to the bottom of the field (rather than from the left to the right).

The aspect ratio is "the number of multiples of the height that the width is". Greater than 1 for a wide-screen; less than 1 for a portrait screen.

The near and far z values specify near and far projection planes and should not be the same.

```c++
float field_of_view_degrees = 30.0f;
float aspect_ratio = 1.5f;
float z_near = 0.01f;
float z_far = 100.0f;
sm::mat<float, 4> mpers = sm::mat<float, 4>::perspective (field_of_view_degrees, aspect_ratio, z_near, z_far);
// Often, mpers is then pushed to the GPU as an 'OpenGL uniform' or similar.
```

### Special setter `orthographic`

```c++
static constexpr sm::mat<F, Nr, Nc> orthographic (const sm::vec<F, 2>& lb, const sm::vec<F, 2>& rt,
                                                  const F zNear, const F zFar) noexcept;
constexpr void orthographic_inplace (const sm::vec<F, 2>& lb, const sm::vec<F, 2>& rt,
                                     const F zNear, const F zFar) noexcept;
```

`orthographic` and `orthographic_inplace` set up an orthographic projection, for use in computer graphics applications.

An orthographic projection requires that you specify a *viewing volume*. This function takes a pair of 2D vectors to specify the left-bottom and right-top of the volume, along with two scalars representing near and far z values.

```c++
sm::vec<float, 2> left_bottom = { -1, -1 };
sm::vec<float, 2> right_top = { 1, 1 };
float z_near = 0.01f;
float z_far = 100.0f;
sm::mat<float, 4> mo;
mo.orthographic_inplace (left_bottom, right_top, z_near, z_far);
```

### Special setter `pure_rotation`

```c++
template <typename T> requires std::is_arithmetic_v<T> && (Nr == 4) && (Nc == 4)
static constexpr mat<F, Nr> pure_rotation (const sm::quaternion<T>& q) noexcept
```

This static method returns a rotation matrix. It places the rotation q
into a mat<F, 4> and returns the matrix.

## Matrix operations

You can add, subtract and multiply `mat` instances, and also add, subtract and multiply by scalars
```c++
sm::mat<double, 4> m1;
sm::mat<double, 4> m2;
sm::mat<double, 4> m3 = m1 + m2;
sm::mat<double, 4> m4 = m1 - m2;
sm::mat<double, 4> m5 = m1 * m2;
m5 += m2;
m4 -= m3;
m4 *= m1;
m4 *= 3;
sm::mat<double, 4> m6 = m4 - 7.45;
```

You can also multiple valid non-square matrices:
```c++
sm::mat<float, 2, 4> twobyfour = { 1, 2, 3, 4, 5, 6, 7, 8 };
sm::mat<float, 4, 6> fourbysix = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
std::cout << "twobyfour * fourbysix =\n" << twobyfour * fourbysix << std::endl;
```

The compiler will prevent invalid shaped matrices from being multiplied in this way.

You can obtain the inverse and transposed square matrices or carry out these operations in-place:
```c++
sm::mat<float, 4> m;
sm::mat<float, 4> mi = m.inverse();   // Returns the inverse.
sm::mat<float, 4> mt = m.transpose(); // Returns transposed matrix

m.inverse_inplace();   // Invert in-place
m.transpose_inplace(); // Transposes the matrix in place
```
### Matrix operations on vectors

You can, of course, multiply vectors by a `sm::mat`. Multiplication of a
vector by a `sm::mat<F, Nr, Nc>` always yields an `Nr` dimensional vector.
However, if you are working in three dimensions, using 4x4 matrices, there are overloads that allows you to multiply a 3D vector by a 4x4 matrix, returning a 4D vector.
You simply ignore the last element of the returned vector.

You should always use `sm::vec<>` as the vector type. The following will work fine:
```c++
// A perspective projection so that our matrix is not empty
sm::mat<float, 4> m = sm::mat<float, 4>::perspective (50, 1.4, 0.1, 100);

sm::vec<float, 3> v1 = { 1, 0, 0 };
std::cout << "\n" << m << " * " << v1 << " = " << m * v1 << std::endl;

sm::vec<float, 4> v2 = { 1, 1, 0, 0 };
std::cout << "\n" << m << " * " << v2 << " = " << m * v2 << std::endl;
```
If you want the output to be a three dimensional vector, use `vec<>::less_one_dim()`:
```c++
sm::vec<float, 3> v3d = (m * v1).less_one_dim();
```
There *is* a definition of vector multiplication for a Nr element `std::array`, though I'd recommend using `sm::vec`. Nevertheless, you can do:
```c++
std::array<float, 4> a2 = { 1, 1, 0, 0 };
std::array<float, 4> ares2 = (m * a2); // Note return object is also std::array
```

## Matrix properties

The determinant, trace, adjugate and cofactor of square `sm::mat` objects are available via these function calls:
```c++
sm::mat<float, 4> m;
float d = m.determinant();
float t = m.trace();
sm::vec<float, 16> a = m.adjugate();
sm::vec<float, 16> c = m.cofactor();
```
The adjugate and cofactor return `sm::vec` rather than `mat` as they are usually used internally during a computation of the inverse.
