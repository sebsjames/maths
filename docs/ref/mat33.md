---
layout: page
title: sm::mat33
parent: Reference
nav_order: 6
permalink: /ref/mat33/
---
# sm::mat33
{: .no_toc }
## A 3x3 matrix class
{: .no_toc }
```c++
#include <sm/mat33>
```
Header file: [<sm/mat33>](https://github.com/sebsjames/maths/blob/main/sm/mat33). Test and example code:
[examples/mat33.cpp](https://github.com/sebsjames/maths/blob/main/examples/mat33.cpp)
[tests/mat33_1.cpp](https://github.com/sebsjames/maths/blob/main/tests/mat33_1.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

A 3x3 matrix class.

Defined as:
```c++
namespace sm {
    template <typename F> requires std::is_floating_point_v<F>
    struct mat33 {
        // ...
        std::array<F, 9> mat;
```
where `F` must be a floating point type. The data is stored in an `std::array` in column-major format; the left-most column of the matrix is stored in the first 3 elements of the array.


## Create a mat33

Like mat44, mat33 is initialized as an identity matrix by default.

```c++
sm::mat33<float> m;
std::cout << "Identity matrix:\n" << m << std::endl;
```

As for `mat44`, you can create and assign an initializer list:
```c++
sm::mat33<double> m = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
std::cout << m << std::endl;
```
Gives:
```
[ 1 , 4 , 7 ;
  2 , 5 , 8 ;
  3 , 6 , 9 ]
```
See how the initializer list is placed into the matrix column-by-column.

The list may contain fewer than9 elements:

```c++
sm::mat33<double> m = { 1, 2, 3 };
std::cout << m << std::endl;
```
Gives:
```
[ 1 , 0 , 0 ;
  2 , 0 , 0 ;
  3 , 0 , 0 ]
```

The additional elements are all set to 0. This means you can create a null matrix with

```c++
sm::mat33<double> m = { 0 };
std::cout << m << std::endl;
```
Gives:
```
[ 0 , 0 , 0 ;
  0 , 0 , 0 ;
  0 , 0 , 0 ]
```

But **careful**: if you provide an empty brace list, you will cause the default constructor to be called and the matrix will be the identity matrix. If you want a null matrix, use `{ {} }` or `{0}`:

```c++
sm::mat33<double> m1 = {};      // Yields the identity matrix, NOT the null matrix
sm::mat33<double> m2 = { {} };  // Yields a null matrix
sm::mat33<double> m3 = { 0.0 }; // Yields a null matrix
```

## Set data in the mat33

### Raw access to the elements

You can set the data manually with an initializer list:
```c++
sm::mat33<int> m; // initially set up as identity matrix
m = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
std::cout << "Matrix contains:\n" << m << std::endl;
```
which outputs:
```
Matrix contains:
[ 0 , 3 , 6 ;
  1 , 4 , 7 ;
  2 , 5 , 8 ]
```

You can change individual elements of the matrix with the array access operator:
```c++
m[3] = 100;
std::cout << "Matrix updated:\n" << m << std::endl;
```

The updated matrix is:
```
Matrix updated:
[ 0 , 100 , 6 ;
  1 , 4 , 7 ;
  2 , 5 , 8 ]
```

You can reset a mat33 to the identity matrix:
```c++
m.setToIdentity();
```

## Access data in a mat33

You can access individual elements:

```c++
sm::mat33<float> m = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
std::cout << "Element 0 of matrix m is " << m[0] << std::endl; // 9
```

You can access a row or column. These are returned as `sm::vec` objects:

```c++
sm::vec<float, 3> zero_row = m.row(0);
std::cout << zero_row << std::endl; // (9, 6, 3)

sm::vec<float, 3> zero_col = m.col(0);
std::cout << zero_col << std::endl; // (9, 8, 7)
```

You can access a rotation quaternion that represents the rotation encoded in the 3x3 `mat33`:
```c++
sm::mat33<float> m = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
sm::quaternion<float> q = m.rotation();
```

## Matrix operations

You can add, subtract and multiply `mat33` instances, and also add, subtract and multiply by scalars
```c++
sm::mat33<double> m1;
sm::mat33<double> m2;
sm::mat33<double> m3 = m1 + m2;
sm::mat33<double> m4 = m1 - m2;
sm::mat33<double> m5 = m1 * m2;
m5 += m2;
m4 -= m3;
m4 *= m1;
m4 *= 3;
sm::mat33<double> m6 = m4 - 7.45;
```

You can obtain the inverse and transposed matrices or carry out these operations in-place:
```c++
sm::mat33<float> m;
sm::mat33<float> mi = m.inverse(); // Returns the inverse.
sm::mat33<float> mt = m.transpose();        // Returns transposed matrix

m.inverse_inplace();   // Invert in-place
m.transpose_inplace(); // Transposes the matrix in place
```
### Matrix operations on vectors

You can multiply 3D vectors by a `mat33`. Multiplication of a
vector by a `mat33` always yields another three dimensional vector.

You should always use `sm::vec<>` as the vector type:
```c++
sm::mat33<float> m = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
sm::vec<float, 3> v1 = { 0, 1, 0 };
std::cout << "\n" << m << " * " << v1 << " = " << m * v1 << std::endl;
```
This gives the output:
```
[ 1 , 4 , 7 ;
  2 , 5 , 8 ;
  3 , 6 , 9 ]
 * (0,1,0) = (4,5,6)
```

## Matrix properties

The determinant, trace, adjugate and cofactor of the matrix are available via these function calls:
```c++
sm::mat33<float> m;
float d = m.determinant();
float t = m.trace();
std::array<float, 9> a = m.adjugate();
std::array<float, 9> c = m.cofactor();
```
The adjugate and cofactor return `std::array` rather than `mat33` as they are usually used internally during a computation of the inverse.
