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
import sm.mat;
```

Module file: [sm/mat.cppm](https://github.com/sebsjames/maths/blob/main/sm/mat.cppm). Test and example code:
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
export namespace sm
{
    template <typename F, std::uint32_t Nr, std::uint32_t Nc = Nr>
    requires (std::is_floating_point_v<F> || sm::is_complex<F>::value == true)
    struct mat
    {
        // ...
        sm::vec<F, Nr * Nc> arr;
```
where `F` must be a floating point type, or a complex type such as `std::complex<float>` (see [Complex matrices](#complex-matrices)), `Nr` is the number of rows in the matrix and `Nc` the number of columns.
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
|      1           5           9           13      |
|      2           6           10          14      |
|      3           7           11          15      |
|      4           8           12          16      |
```
See how the initializer list is placed into the matrix column-by-column. (The exact padding you see depends on the precision `str()` is called with - see [Printing and formatting](#printing-and-formatting).)

The list may contain fewer than `Nr` * `Nc` = 16 elements:

```c++
sm::mat<double, 4> m = { 1, 2, 3, 4 };
std::cout << m << std::endl;
```
Gives:
```
|      1           0           0           0       |
|      2           0           0           0       |
|      3           0           0           0       |
|      4           0           0           0       |
```

The additional elements are all set to 0. This means you can create a null matrix with

```c++
sm::mat<double, 4> m = { 0 };
std::cout << m << std::endl;
```
Gives:
```
|      0           0           0           0       |
|      0           0           0           0       |
|      0           0           0           0       |
|      0           0           0           0       |
```

But **careful**: if you provide an empty brace list, you will cause the default constructor to be called and (if it is square) the matrix will be the identity matrix. If you want a null matrix, use `{ {} }` or `{0}`:

```c++
sm::mat<double, 4> m1 = {};      // Yields the identity matrix, NOT the null matrix
sm::mat<double, 4> m2 = { {} };  // Yields a null matrix
sm::mat<double, 4> m3 = { 0.0 }; // Yields a null matrix
```

If you'd rather avoid the initializer-list gotcha altogether, `set_zero()` (a method) and `zero()` (a static factory, for symmetry with the pre-existing `set_identity()`/`identity()`) give you a null matrix explicitly:
```c++
sm::mat<double, 4> m4;
m4.set_zero();                                      // m4 is now the null matrix
sm::mat<double, 4> m5 = sm::mat<double, 4>::zero(); // equivalent, as a fresh object
```

### Constructing a 4x4 from a rotation

If you already have a 3x3 rotation matrix or a quaternion, you can construct a 4x4 transform matrix directly from it. The rotation goes into the top-left 3x3 block, with an identity translation and homogeneous row:
```c++
sm::mat<float, 3> rot3; // ... some 3x3 rotation ...
sm::mat<float, 4> m6 (rot3);

sm::quaternion<float> q (sm::vec<float>::uz(), sm::mathconst<float>::pi_over_2);
sm::mat<float, 4> m7 (q); // equivalent to: m7.set_identity(); m7.rotate (q);
```

### Converting element type

`as<T>()` returns a copy of the matrix with every element `static_cast` to a new type `T`:
```c++
sm::mat<float, 4> mf;
sm::mat<double, 4> md = mf.as<double>();
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

You can set every element to the same value with `set_from`:
```c++
sm::mat<float, 4> m;
m.set_from (2.0f); // every element of m is now 2.0f
```

### Accessing by row and column

As well as the flat `operator[]`, you can read or write an element by its row and column with `operator()`:
```c++
sm::mat<float, 4> m;
float el = m (1, 2); // row 1, column 2
m (1, 2) = 5.0f;
```
Reading out of range returns NaN; writing out of range clamps to the matrix's last element, rather than being undefined behaviour.

You can also get or set a whole row or column at once, as an `sm::vec`:
```c++
sm::vec<float, 4> r = m.row (1);
m.set_row (1, r);
sm::vec<float, 4> c = m.col (2);
m.set_col (2, c);
```
`rows()` and `cols()` return `Nr` and `Nc` respectively, useful in generic code where the matrix's dimensions aren't otherwise in scope.

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
static constexpr sm::mat<F, 4> perspective (F fov_deg, F aspect, F z_near, F z_far) noexcept;
constexpr void perspective_inplace (F fov_deg, F aspect, F z_near, F z_far) noexcept;
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
static constexpr sm::mat<F, 4> orthographic (const sm::vec<F, 2>& lb, const sm::vec<F, 2>& rt,
                                             const F z_near, const F z_far) noexcept;
constexpr void orthographic_inplace (const sm::vec<F, 2>& lb, const sm::vec<F, 2>& rt,
                                     const F z_near, const F z_far) noexcept;
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

### Special setter `reflection`

```c++
template <typename T> requires std::is_arithmetic_v<T> && (Nr == 3) && (Nc == 3)
static constexpr mat<F, 3> reflection (const sm::vec<T, 3>& n) noexcept;

template <typename T> requires std::is_arithmetic_v<T> && (Nr == 4) && (Nc == 4)
static constexpr mat<F, Nr> reflection (const sm::vec<T, 3>& p, const sm::vec<T, 3> n) noexcept;
```

The 3x3 overload returns a reflection matrix for the plane through the origin with normal `n` (computed as `I - 2*n*n^T`). The 4x4 overload reflects in a plane through an arbitrary point `p` with normal `n`, by composing a translate-to-origin, reflect, translate-back sequence:

```c++
sm::vec<float> p = { 0.25f, 0.0f, 0.0f }; // a point on the plane
sm::vec<float> n = sm::vec<float>::ux();  // the plane's normal
sm::vec<float> v = { 0.5f, 0.0f, 0.5f };

sm::vec<float> reflected = (sm::mat<float, 4>::reflection (p, n) * v).less_one_dim();
// reflected == (0, 0, 0.5) -- v=(0.5,0,0.5) is reflected in the plane x=0.25
```

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

The compiler will prevent invalid shaped matrices from being multiplied in this way. It will also prevent you multiplying a real-valued `mat` by a complex-valued one (or vice-versa) - both operands of `mat * mat` (and `mat * scalar`) must either both be arithmetic or both be complex; see [Complex matrices](#complex-matrices).

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

## Decomposing a 4x4 transformation matrix

If you have a 4x4 matrix that encodes some combination of translation, rotation and scaling, a family of methods let you pull those components back out.

`linear()` returns the top-left 3x3 block (the "linear part" of the transform, i.e. everything except translation); `translation()` returns the translation as an `sm::vec<F, 3>` (the top three elements of the last column):
```c++
sm::mat<float, 4> m; // some transform...
sm::mat<float, 3> rot3 = m.linear();
sm::vec<float, 3> t = m.translation();
```
Note that these are *getters* on an existing matrix, distinct from the pre-existing `translate`/`pretranslate` *instance methods* (which post/pre-multiply a translation into a matrix you already have) and from the static factory `mat<F, 4>::translation (const sm::vec<F, 3>& t)` (which builds a *fresh* translation matrix from scratch, equivalent to `pretranslate` on a default-constructed matrix). There's also an overload of the static factory taking an `sm::vec<F, 4>`.

Similarly, the instance method `rotation()` decomposes a 3x3 or 4x4 matrix's rotational part into an `sm::quaternion`. For a 4x4 matrix this only looks at the top-left 3x3 block, so `m.rotation()` and `m.linear().rotation()` are equivalent:
```c++
sm::quaternion<float> q = m.rotation();
```
Again, don't confuse this with the pre-existing instance method `rotate (axis, theta)` (which rotates a matrix you already have) or with the static factory `mat<F, 4>::rotation (const sm::vec<Fy, 3>& axis, const Fy& theta)` (which builds a fresh rotation matrix, equivalent to calling `rotate` on a default-constructed matrix). **`rotation()` (the getter) assumes the matrix encodes rotation *without* scaling** - it doesn't check that the determinant is 1, and it doesn't normalize the quaternion it returns, so if your matrix has scaling baked in, strip it first (see below) or normalize the result yourself.

If your matrix *does* have scaling baked in, `rotation_mat33()`/`rotation_mat44()` return just the rotational part with that scaling divided back out (each column of the linear block is renormalized to unit length), and `scaling_vec()`/`scaling_mat33()`/`scaling_mat44()` return the scaling itself, as a vector of the three column lengths, or as a diagonal matrix:
```c++
sm::vec<float, 3> scale = m.scaling_vec();       // {|col0|, |col1|, |col2|}
sm::mat<float, 3> rot33 = m.rotation_mat33();    // linear() with columns renormalized to unit length
sm::mat<float, 4> rot44 = m.rotation_mat44();    // same, as a full 4x4 with an identity 4th row/col
```

## Solving linear systems

For a matrix that isn't 2x2, 3x3 or 4x4 (where `inverse()` isn't implemented), or simply as an alternative to computing a full inverse, you can solve `Ax = b` directly by Gaussian elimination. Build an *augmented matrix* `[A | b]` - `A`'s columns followed by one more column for `b` - then reduce it to row-echelon form and back-substitute:
```c++
// Solve: x + 2z = 6, x + 2y + 5z = -4, x + 5y - z = 27
sm::mat<float, 3, 4> aug = { 1, 0, 2,   1, 2, 5,   1, 5, -1,   6, -4, 27 }; // [A | b], 3 rows x 4 cols
aug.row_echelon_form_inplace();
sm::vec<float, 3> x = aug.back_substitution();
// x == (5, 3, -2)
```
This gives the same answer as the more familiar `A.inverse() * b`, but works for any size of (square) `A`, not just 2x2 - 4x4:
```c++
sm::mat<float, 3> A = { 1, 0, 2,   1, 2, 5,   1, 5, -1 };
sm::vec<float, 3> b = { 6, -4, 27 };
sm::vec<float, 3> x2 = A.inverse() * b; // also (5, 3, -2)
```
`row_echelon_form_inplace` (`row_echelon_form` returns a copy instead of mutating) requires `Nc >= Nr`, and performs Gaussian elimination with partial pivoting (by largest column magnitude, so it works for complex `F` too). `divide_rows_by_diagonals_inplace` divides each row by its own diagonal element, and `reduced_row_echelon_form_inplace`/`reduced_row_echelon_form` combine the two steps. `back_substitution` requires `Nc == Nr + 1` (i.e. `*this` must be an augmented matrix already in row-echelon form) and returns `NaN` in any position where no solution could be found - it does not currently have a way to represent an underdetermined (free) variable.

## Eigenvalues and eigenvectors

For a square matrix of real (non-complex) numbers, `eigenvalues()` returns all `Nr` eigenvalues, as `sm::vec<std::complex<F>, Nr>` - a real matrix can have genuinely complex eigenvalues (a rotation matrix, for example), so they're always returned as complex, sorted the same way [`sm::polysolve`](/maths/ref/polysolve/) sorts its roots (real eigenvalues first, ascending, then complex-conjugate pairs):
```c++
sm::mat<double, 4> A; A.set_identity();
A[0] = 2.0; A[5] = 3.0; A[10] = 5.0; A[15] = 7.0; // diag(2, 3, 5, 7)
sm::vec<std::complex<double>, 4> ev = A.eigenvalues(); // (2,0), (3,0), (5,0), (7,0)
```
Internally, this uses the Faddeev-LeVerrier algorithm to build the characteristic polynomial's coefficients, then hands them to `sm::polysolve::solve` to find the roots - so it's exact for degree &le; 4 and numerical (Durand-Kerner) above that. **`eigenvalues()` requires `Nr == Nc` and `Nr >= 2`, and despite `sm::mat` generally being `constexpr`-capable, it is not itself `constexpr`** (it builds `std::vector`s and calls into `sm::polysolve` at runtime).

Given a single eigenvalue, `eigenvector (lambda)` finds a normalized eigenvector for it, by row-reducing the augmented matrix `[A - lambda*I | 0]` and back-substituting:
```c++
std::complex<double> lambda = ev[3]; // one of the eigenvalues found above
sm::vec<std::complex<double>, 4> v = A.eigenvector (lambda);
```
To get every eigenvalue paired with its eigenvector in one call, use `eigenpairs()`, which returns an `sm::vec` of the nested `mat::eigenpair` type (`{ std::complex<F> eigenvalue; sm::vec<std::complex<F>, Nr> eigenvector; }`):
```c++
sm::vec<sm::mat<double, 4>::eigenpair, 4> pairs = A.eigenpairs();
for (const auto& p : pairs) {
    // p.eigenvalue, p.eigenvector -- satisfies A * p.eigenvector ~= p.eigenvalue * p.eigenvector
}
```

**Careful:** `eigenvalues()`, `eigenvector()` and `eigenpairs()` are only implemented for real (floating-point) `F` - calling any of them on a `sm::mat<std::complex<...>, ...>` is a compile error (`static_assert(false, ...)`). Also, `eigenpairs()`'s square-matrix guard is written as `(Nr == Nc) || (Nr < 2u)`, unlike the `(Nr == Nc) && (Nr >= 2u)` guard on the other eigen-methods - this looks like it should be `&&`, though calling it on an invalid shape will still eventually fail via `eigenvalues()`'s own (correct) guard.

## Complex matrices

`sm::mat`'s element type `F` no longer has to be a floating point type - it can also be a complex type such as `std::complex<float>`, detected via the `sm::is_complex<F>` trait (which just checks for `.real()`/`.imag()` members, so it works for `std::complex` or any similarly-shaped type):
```c++
sm::vec<std::complex<float>, 16> fourfour = { 2, 7, 5, 6,  8, 1, 3, 6,  2, 8, -1, 7,  7, 0, 1, 7 };
std::complex<float> det = sm::mat<std::complex<float>, 4>::determinant (fourfour); // (816, 0)
sm::mat<std::complex<float>, 4> m (fourfour);
sm::mat<std::complex<float>, 4> minv = m.inverse();
```
Determinant, trace, adjugate, cofactor, inverse, transpose, `row_echelon_form`/`back_substitution` and multiplication all work generically for complex `F`, exactly as they do for real matrices. Two things don't:
* [Eigenvalues and eigenvectors](#eigenvalues-and-eigenvectors) are not yet implemented for complex matrices (a compile error, as noted above).
* `mat * mat` and `mat * scalar` require **both** operands to be the same "kind" - both real/arithmetic, or both complex. You can't multiply a real `sm::mat` by a complex one (or a complex `mat` by a real scalar) directly with `operator*`.

## Printing and formatting

`std::cout << m` (via `operator<<`) prints `m.str()`, formatting each element into a fixed-width, centered field, with rows delimited by `|`:
```c++
std::cout << m1 << std::endl;
```
```
|   3.14159        4           7           10      |
|      1        3.14159        8           11      |
|      2           5        3.14159        12      |
|      3           6           9          45.5     |
```
`str` takes an optional runtime `prec` argument (number of significant figures) and a template `approximate_zero` flag (default `true`):
```c++
std::cout << m1.str (4);  // fewer significant figures
```
```
|   3.142       4         7         10     |
|     1       3.142       8         11     |
|     2         5       3.142       12     |
|     3         6         9        45.5    |
```
**Note:** the default `prec` for the (non-static) `str()` member is `std::numeric_limits<float>::digits10` (i.e. `6`) *regardless of what F is* - so `std::cout << m` always prints with 6-figure precision, even for a `mat<long double, ...>`. Pass an explicit `prec` if you want more.

When `approximate_zero` is `true` (the default), any element that isn't exactly zero but whose magnitude is below `20 * std::numeric_limits<F>::epsilon()` - e.g. a stray `-1.99349e-07` left over from a floating-point computation that should really have been zero - is printed as `~0` instead of its noisy tiny value. Pass `false` (as the template argument, e.g. `m.str<false>()`) if you want to see the raw value instead.

`str_arr` prints the same data as a flat, comma-separated list (the underlying column-major array), rather than row-by-row:
```c++
std::cout << m1.str_arr (4) << std::endl;
```
```
[ 3.142, 1, 2, 3, 4, 3.142, 5, 6, 7, 8, 3.142, 9, 10, 11, 12, 45.5 ]
```
For a complex-valued matrix, both `str` and `str_arr` render each element as `(real, imag)`:
```
|    (0, 0)      (1, 0)      (0, 0)      (0, 0)    |
|    (1, 0)      (0, 0)      (0, 0)      (0, 0)    |
|    (0, 0)      (0, 0)     (-1, -0)     (0, 0)    |
|    (0, 0)      (0, 0)      (0, 0)      (1, 0)    |
```

*This page was updated with AI assistance, based on human written code in mat.cppm. Reviewed by Seb James*
