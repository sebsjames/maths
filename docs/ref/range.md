---
title: sm::range
parent: Reference
layout: page
permalink: /ref/range
nav_order: 5
---
# sm::range
{: .no_toc}
## Minimum and maximum values
{: .no_toc}

```c++
#include <sm/range>
```
Header file: [<sm/range>](https://github.com/sebsjames/maths/blob/main/sm/range).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::range` is a class for specifying a mathematical closed interval [min, max]. Its data consists of just two numbers indicating the minimum and maximum of the interval.

It is used as a return object for the `vec::range` and `vvec::range` methods and gives semantic meaning to the two values `min` and `max`, which are public and accessible directly by client code (if a 2 element array were used for a range, the client coder would have to remember if element 0 was min or max).

The range object can participate in the process of determining the range of values in a data container and it can test for a value being within the interval.

## Design

`sm::range` takes one template argument, specifying the type of the values.
```c++
namespace sm
{
    template <typename T>
    struct range
    {
```

`sm::range` is `constexpr` capable and may be used with scalar,
`std::complex` or vector (`sm::vec` and `std::array`) values. Used
with vector values, `sm::range` may be used as a *bounding box*
in graphics applications.

## Construct

```c++
sm::range<T> r;                              // Default range has min == max == T{0}
sm::range<T> r = { T{0}, T{10} };            // Construct with initializer list
sm::range<T> r(T{0}, T{10});                 // Construct with a defined interval [0, 10]
sm::range<T> r (sm::range_init::for_search); // Construct ready for search...
sm::range<T> r = sm::range<T>::search_initialized() // ...or initialize from a static function
```

## Set

**Set** the range manually in a single function call
```c++
sm::range<int> r;  // range initially [0, 0]
r.set (-100, 100); // range now [-100, 100]
```
or use initializer braces
```c++
r = { -100, 100 };
```

**Update** the range to include a value
```c++
sm::range<int> r; // range initially 0 to 0
bool changed1 = r.update (100);      // range now 0 to 100
bool changed2 = r.update (-100);     // range now -100 to 100
bool changed3 = r.update (50);       // range unchanged; still -100 to 100
```

`update` returns a `bool` which will be true if the range was changed and false if the range is not changed. In the example above, `changed1` and `changed2` will both be `true`, but `changed3` will contain `false`.

You can **add** to and **subtract** from a range
```c++
sm::range<int> r = { -10, 10 };
r += 3;
std::cout << r << std::endl;  // [-7, 13]
sm::range<int> r2 = r - 1;
std::cout << r2 << std::endl; // [-8, 12]
```
This works also for vector ranges:
```c++
sm::range<sm::vec<float>> rv = { {0,0,0}, {1,1,1} };
std::cout << "range:      " << rv << std::endl;                   // [(0,0,0), (1,1,1)]
std::cout << "range + uz: " << rv + sm::vec<>::uz() << std::endl; // [(0,0,1), (1,1,2)]
```

## Query

To query the max or min of the range, just access the `max` or `min` members:
```c++
std::cout << "range maximum is " << r.max << " and its minimum is " << r.min << std::endl;
```
You can **stream** the range to get both at once:
```c++
std::cout << r << std::endl;
```
This would output `[-100,100]` in our previous example.

There's a helper function to get `range.max - range.min`:
```c++
std::cout << "The range 'spans': " << r.span() << std::endl;
```

The mid-point of the range [(max - min) / 2] also has a helper:
```c++
std::cout << "The range mid is " << r.mid() << std::endl;
```

### Contains a value?

Test a value to see if the range includes this value:
```c++
r.contains (45);     // would return bool true, following on from previous example
r.contains (-450);   // would return bool false
```
### Contains a range?

You can determine if one range fits inside another with `range::contains()`:
```c++
sm::range<int> r1 = { 1, 100 };
sm::range<int> r2 = { 10, 90 };
sm::range<int> r3 = { -1, 2 };
std::cout << "range " << r1 << (r1.contains(r2) ? " contains " : " doesn't contain ") << r2 << std::endl;
std::cout << "range " << r1 << (r1.contains(r3) ? " contains " : " doesn't contain ") << r3 << std::endl;
```

### Intersection

```c++
sm::range<int> r1 = { 1, 100 };
sm::range<int> r2 = { 90, 200 };

// Prints "r1 intersects r2":
std::cout << (r1.intersects (r2) ? "r1 intersects r2" : "no intersection") << std::endl;
```

### Comparison

You can compare ranges to be equal or not equal to each other
```c++
sm::range<int> r1 = { 1, 100 };
sm::range<int> r2 = { 10, 90 };
bool equality_test = (r1 == r2);
bool nonequality_test = (r1 != r2);
```

## Determine the range from data

Determine a range from data. Here, we initialize a range with min taking the *maximum* possible value for the type and max taking the *minimum* possible value. This is done with a call to `range::search_init`. We then run through the data container, calling `update` for each element. For example:

```c++
sm::vvec<double> data (10, 0.0);
data.randomize();
sm::range<double> r; // Default constructed range is [ 0, 0 ]
r.search_init();     // prepare range for search
for (auto d : data) { r.update (d); } // update on each element of data
std::cout << "The range of values in data was: " << r << std::endl;
```

To save a line of code, use the constructor for setting up a range ready-configured for search:
```c++
sm::vvec<double> data;
data.randomize();
range<double> r (sm::range_init::for_search); // avoids need for r.search_init()
for (auto d : data) { r.update (d); }
```
or use the static `range<>::search_initialized`, which returns a suitable range:
```c++
sm::vvec<double> data;
data.randomize();
range<double> r = sm::range<double>search_initialized();
for (auto d : data) { r.update (d); }
```
