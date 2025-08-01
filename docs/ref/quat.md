---
layout: page
title: sm::quaternion
parent: Reference
permalink: /ref/quaternion/
nav_order: 4
---
# sm::quaternion
{: .no_toc }
## Quaternion rotations
{: .no_toc }

```c++
#include <sm/quaternion>
```
Header file: [<sm/quaternion>](https://github.com/sebsjames/maths/blob/main/sm/quaternion). Test and example code:  [tests/testQuaternion](https://github.com/sebsjames/maths/blob/main/tests/testQuaternion.cpp)

**Table of Contents**

- TOC
{:toc}

## Summary

A class for doing quaternion operations. Initially developed for use in [mathplot](https://github.com/sebsjames/mathplot) (`mplot::Visual`) to compute rotations of the scene.

Defined as:
```c++
namespace sm
{
    template <typename F>
    struct quaternion
```
where `F` hints that the template arg is a floating point type. The Hamiltonian convention is adopted for the order of the member elements: `F w, x, y, z;`. These are the only member data attributes.

## Create a quaternion

```c++
sm::quaternion<float> q;             // By default w=1, and x, y, z = 0;
sm::quaternion<float> q(1, 0, 0, 0); // Explicit setting of elements
```
A newly created quaternion will be an identity quaternion (1,0,0,0) unless explicity set otherwise.

You can also create a quaternion with a specified rotation about a given vector axis:
```c++
sm::vec<float, 3> z_axis = { 0, 0, 1 };              // set a rotation about the z axis...
float angle = sm::mathconst<float>::pi_over_2; // ...of pi/2 radians

sm::quaternion<float> q (z_axis, angle);  // The (axis, angle) constructor
```

## Copying
```c++
sm::quaternion<float> q1;
sm::quaternion<float> q2 = q1; // Assignment works as expected
```

You can use **equality** and **inequality operators** on a quaternion: `q1 == q2` and `q1 != q2`.

## Streaming

You can stream a quaternion object:
```c++
sm::quaternion<float> q1(1, 0, 0, 0);
std::cout << q1 << std::endl;
```
Output: `quaternion[wxyz]=(1,0,0,0)`

## Use in `constexpr` functions

`sm::quaternion` is constexpr capabable and may be utilized in your
constexpr functions.

## Operations on a quaternion

Renormalize to magnitude 1 or check if it's a unit quaternion:

```c++
q1.renormalize();
q1.checkunit();
```
The quaternion can be reset to the identity with `reset()`.

## Quaternion multiplication

The most useful feature of quaternions, and the reason for their popularity in computer programs is the fact that the rotations can be combined by **multiplication** and their rotations applied to a vector by multiplication.

### Applying a quaternion rotation to a `sm::vec`

You have a vector (3D or '3+1'D) and you want to apply the rotation specified by a quaternion? You use the multiplication `operator*`:

```c++
using mc = sm::mathconst<float>;
// Create quaternion for a rotation of pi radians
sm::quaternion<float> q(sm::vec<float>{1, 0, 0}, mc::pi);
sm::vec<float> v = { 1, 2, 3 };
sm::vec<float> rotated = q * v; // rotates v by pi about the x axis
```

### Combining rotations by quaternion multiplication If we have 2
quaternions `q1` and `q2` that specify rotations, we can combine them
into a third quaternion by multiplication. In the following code, `q3`
becomes the rotation that would result from first applying rotation
q1, then applying rotation q2.

```c++
using mc = sm::mathconst<float>;
sm::quaternion<float> q1(sm::vec<float>({1,0,0}), mc::pi_over_3);
sm::quaternion<float> q2(sm::vec<float>({0,1,0}), mc::pi_over_4);
sm::quaternion<float> q3 = q2 * q1;
```

To multiply one quaternion by another, it's important to specify the
multiplication order. The `postmultiply` and `premultiply` functions
allow you to control this.

```c++
q1.postmultiply (q2); // Places result of q1 * q2 into q1.
q1.premultiply (q2);  // Places result of q2 * q1 into q1.
```

The quaternion `q3 = q2 * q1` is equivalent to applying an initial rotation of q1, followed by a rotation of q2. For this reason, in the following, `rotated_vec1` and `rotated_vec2` will hold the same result:

```c++
sm::quaternion<float> q3 = q2 * q1;
sm::vec<float> unrotated = { 1, 0, 2 };
sm::vec<float> rotated_vec1 = q3 * unrotated;
sm::vec<float> rotated_vec2 = q2 * (q1 * unrotated); // More computation required
```

Quaternion **division** is also possible: `q3 = q1/q2;` as is division by a scalar. Note that quaternion addition and subtraction are not implemented in this class at present.

### Inversions and conjugates

The rotational inversion, or 'reverse rotation' is obtained with `invert`:
```c++
q.invert(); // Obtains the opposite rotation to the one specified by q
```
For the **algebraic inverse** or **reciprocal**, q<sup>-1</sup>, use `inverse`. This finds the reciprocal, q<sup>-1</sup> which has the property that q<sup>-1</sup> * q = the identity quaternion (1,0,0,0).

```c++
q.inverse(); // q^-1
```
The conjugate is also obtainable with `q.conjugate();` and the magnitude of the quaternion with `q.magnitude();`.

`q.norm()` is an alias for the magnitude and `q.norm_squared()` returns the square of the norm.

## Setting the rotation of the quaternion

The function `set_rotation` sets the values of the quaternion to specify a rotation
(specified in degrees, not radians) about the given three dimensional
axis, starting from no rotation.

```c++
void set_rotation (const sm::vec<F>& axis, const F& angle);
```

### Rotate the quaternion further with these methods:

```c++
void rotate (const sm::vec<F, 3>& axis, const F angle);
void rotate (const std::array<F, 3>& axis, const F angle);
void rotate (const F axis_x, const F axis_y, const F axis_z, const F angle)
```
Each method rotates the quaternion by an angle in radians about a 3D axis specified by the axis array (or by the individual components of the axis).

### Rotation Matrix

You can obtain the equivalent 4x4 **rotation matrix** in column-major format (OpenGL friendly) from the quaternion with
```c++
std::array<F, 16> rotationMatrix() const;           // Returns the rotation matrix
void rotationMatrix (std::array<F, 16>& mat) const  // sets the values in the passed-in matrix
```
If you know that your quaternion can be assumed to be a unit quaternion, you can use
```c++
std::array<F, 16> unitRotationMatrix() const;
void unitRotationMatrix (std::array<F, 16>& mat) const;
```
These involve slightly less computation.
