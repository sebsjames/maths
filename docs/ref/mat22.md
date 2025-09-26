---
layout: page
title: sm::mat22
parent: Reference
nav_order: 5
permalink: /ref/mat22/
---

# sm::mat22
{: .no_toc }
## A 2x2 matrix class
{: .no_toc }
```c++
#include <sm/mat22>
```
Header file: [<sm/mat22>](https://github.com/sebsjames/maths/blob/main/sm/mat22).

**Table of Contents**

- TOC
{:toc}

## Summary

A 2x2 matrix class, useful for two dimensional transformations.

Defined as:
```c++
namespace sm {
    template <typename F>
    struct mat22 {
        // ...
        sm::vec<F, 4> mat;
```
where `F` must be a floating point type. The data is stored in an `sm::vec` in column-major format; the left-most column of the matrix is stored in the first 3 elements of the array.


## Create a mat22

Like mat44, mat22 is initialized as an identity matrix by default.

```c++
sm::mat22<float> m;
std::cout << "Identity matrix:\n" << m << std::endl;
```

As for `mat44`, you can create and assign an initializer list:
```c++
sm::mat22<double> m = { 1, 2, 3, 4 };
std::cout << m << std::endl;
```
Gives:
```
[ 1 , 3 ;
  2 , 4 ]
```
See how the initializer list is placed into the matrix column-by-column.

The list may contain fewer than 4 elements:

```c++
sm::mat22<double> m = { 1, 2 };
std::cout << m << std::endl;
```
Gives:
```
[ 1 , 0 ;
  2 , 0 ]
```

The additional elements are all set to 0. This means you can create a null matrix with

```c++
sm::mat22<double> m = { 0 };
std::cout << m << std::endl;
```
Gives:
```
[ 0 , 0 ;
  0 , 0 ]
```

But **careful**: if you provide an empty brace list, you will cause the default constructor to be called and the matrix will be the identity matrix. If you want a null matrix, use `{ {} }` or `{0}`:

```c++
sm::mat22<double> m1 = {};      // Yields the identity matrix, NOT the null matrix
sm::mat22<double> m2 = { {} };  // Yields a null matrix
sm::mat22<double> m3 = { 0.0 }; // Yields a null matrix
```

## Set data in the mat22

### Raw access to the elements

You can set the data manually with an initializer list:
```c++
sm::mat22<int> m; // initially set up as identity matrix
m = { 0, 1, 2, 3 };
std::cout << "Matrix contains:\n" << m << std::endl;
```
which outputs:
```
Matrix contains:
[ 0 , 2 ;
  1 , 3 ]
```

You can change individual elements of the matrix with the array access operator:
```c++
m[3] = 100;
std::cout << "Matrix updated:\n" << m << std::endl;
```

The updated matrix is:
```
Matrix updated:
[ 0 , 2 ;
  1 , 100 ]
```

You can reset a mat22 to the identity matrix:
```c++
m.setToIdentity();
```

## Access data in a mat22

You can access individual elements:

```c++
sm::mat22<float> m = { 4, 3, 2, 1 };
std::cout << "Element 0 of matrix m is " << m[0] << std::endl; // 4
```

You can access a row or column. These are returned as `sm::vec` objects:

```c++
sm::vec<float, 2> zero_row = m.row(0);
std::cout << zero_row << std::endl; // (4, 2)

sm::vec<float, 2> zero_col = m.col(0);
std::cout << zero_col << std::endl; // (4, 3)
```

## Matrix operations

You can multiply `mat22` instances by other mat22 objects and by scalars:
```c++
sm::mat22<double> m1;
sm::mat22<double> m2;
sm::mat22<double> m5 = m1 * m2;
m2 *= m1;
m1 *= 3;
```

You can obtain the inverse and transposed matrices or carry out these operations in-place:
```c++
sm::mat22<float> m;
sm::mat22<float> mi = m.inverse();   // Returns the inverse.
sm::mat22<float> mt = m.transpose(); // Returns transposed matrix

m.inverse_inplace();   // Invert in-place
m.transpose_inplace(); // Transposes the matrix in place
```
### Matrix operations on vectors

You can multiply 2D vectors by a `mat22`. Multiplication of a
vector by a `mat22` always yields another two dimensional vector.

Use `sm::vec<>` as the vector type:
```c++
sm::mat22<float> m = { 1, 2, 3, 4 };
sm::vec<float, 2> v1 = { 0, 1 };
std::cout << "\n" << m << " * " << v1 << " = " << m * v1 << std::endl;
```
This gives the output:
```
[ 1 , 3 ;
  2 , 4 ]
 * (0,1) = (3,4)
```

## Matrix properties

The determinant, trace and adjugate of the matrix are available via these function calls:
```c++
sm::mat22<float> m;
float d = m.determinant();
float t = m.trace();
std::array<float, 4> a = m.adjugate();
```
The adjugate returns `sm::vec` rather than `mat22` as it is usually used internally during a computation of the inverse.
