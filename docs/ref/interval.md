---
title: sm::interval
parent: Reference
layout: page
permalink: /ref/interval
nav_order: 5
---
# sm::interval
{: .no_toc}
## Minimum and maximum values
{: .no_toc}

```c++
import sm.interval;
```
Module file: [sm/interval.cppm](https://github.com/sebsjames/maths/blob/main/sm/interval.cppm).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::interval` is a class for specifying a mathematical interval between an infimum, `min` and a supremum, `max`. Its data consists of just two numbers indicating the minimum and maximum of the interval. By default the interval is closed [min, max], but you can use template parameters to define semi-open and open intervals.

It is used as a return object for the `vec::range` and `vvec::range` methods and gives semantic meaning to the two values `min` and `max`, which are public and accessible directly by client code (if a 2 element array were used for an interval, the client coder would have to remember if element 0 was min or max).

The interval object can participate in the process of determining the range of values in a data container and it can test for a value being within the interval.

## Design

`sm::interval` takes one template argument, specifying the type of the values and two arguments defining the extrema
```c++
export namespace sm
{
    template <typename T,
              interval_endpoint infimum = interval_endpoint::closed,
              interval_endpoint supremum = interval_endpoint::closed>
    struct interval
    {
```

`sm::interval` is `constexpr` capable and may be used with scalar,
`std::complex` or vector (`sm::vec` and `std::array`) values. Used
with vector values, `sm::interval` may be used as a *bounding box*
in graphics applications.

## Construct

```c++
sm::interval<T> r;                              // Default (closed) interval has min == max == T{0}
sm::interval<T> r = { T{0}, T{10} };            // Construct with initializer list
sm::interval<T> r(T{0}, T{10});                 // Construct with a defined interval [0, 10]
sm::interval<T> r (sm::interval_init::for_search); // Construct ready for search...
sm::interval<T> r = sm::interval<T>::search_initialized(); // ...or initialize from a static function
```
I usually write this:
```c++
auto r = sm::interval<T>::search_initialized();
```

## Set

**Set** the interval manually in a single function call
```c++
sm::interval<int> r;  // interval initially [0, 0]
r.set (-100, 100); // interval now [-100, 100]
```
or use initializer braces
```c++
r = { -100, 100 };
```

**Update** the interval to include a value
```c++
sm::interval<int> r; // interval initially 0 to 0
bool changed1 = r.update (100);      // interval now 0 to 100
bool changed2 = r.update (-100);     // interval now -100 to 100
bool changed3 = r.update (50);       // interval unchanged; still -100 to 100
```

`update` returns a `bool` which will be true if the interval was changed and false if the interval is not changed. In the example above, `changed1` and `changed2` will both be `true`, but `changed3` will contain `false`.

You can **add** to and **subtract** from an interval
```c++
sm::interval<int> r = { -10, 10 };
r += 3;
std::cout << r << std::endl;  // [-7, 13]
sm::interval<int> r2 = r - 1;
std::cout << r2 << std::endl; // [-8, 12]
```
This works also for vector intervals:
```c++
sm::interval<sm::vec<float>> rv = { {0,0,0}, {1,1,1} };
std::cout << "interval:      " << rv << std::endl;                   // [(0,0,0), (1,1,1)]
std::cout << "interval + uz: " << rv + sm::vec<>::uz() << std::endl; // [(0,0,1), (1,1,2)]
```

## Query

To query the max or min of the interval, just access the `max` or `min` members:
```c++
std::cout << "interval maximum is " << r.max << " and its minimum is " << r.min << std::endl;
```
You can **stream** the interval to get both at once:
```c++
std::cout << r << std::endl;
```
This would output `[-100,100]` in our previous example.

There's a helper function to get `interval.max - interval.min`:
```c++
std::cout << "The interval 'spans': " << r.span() << std::endl;
```

The mid-point of the interval [(max - min) / 2] also has a helper:
```c++
std::cout << "The interval mid is " << r.mid() << std::endl;
```

### Contains a value?

Test a value to see if the interval includes this value:
```c++
r.contains (45);     // would return bool true, following on from previous example
r.contains (-450);   // would return bool false
```
### Contains another interval?

You can determine if one interval fits inside another with `interval::contains()`:
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

Note that there is no `interval<>::intersection (sm::interval<>& other)` method to return the intersection between two intervals.
This limitation results from the extrema types being defined at compile time (as template parameters), but runtime information is required to determine the extrema of the intersection result.
For the same reason, there is no `interval<>::union (sm::interval<>& other)` method.

### Comparison

You can compare intervals to be equal or not equal to each other
```c++
sm::interval<int> r1 = { 1, 100 };
sm::interval<int> r2 = { 10, 90 };
bool equality_test = (r1 == r2);
bool nonequality_test = (r1 != r2);
```

## Determine the interval from data

Determine an interval from data. Here, we initialize an interval with min taking the *maximum* possible value for the type and max taking the *minimum* possible value. This is done with a call to `interval::search_init`. We then run through the data container, calling `update` for each element. For example:

```c++
sm::vvec<double> data (10, 0.0);
data.randomize();
sm::interval<double> r; // Default constructed interval is [ 0, 0 ]
r.search_init();     // prepare interval for search
for (auto d : data) { r.update (d); } // update on each element of data
std::cout << "The range of values in data was: " << r << std::endl;
```

To save a line of code, use the constructor for setting up an interval ready-configured for search:
```c++
sm::vvec<double> data;
data.randomize();
sm::interval<double> r (sm::interval_init::for_search); // avoids need for r.search_init()
for (auto d : data) { r.update (d); }
```
or use the static `interval<>::search_initialized`, which returns a suitable interval:
```c++
sm::vvec<double> data;
data.randomize();
auto r = sm::interval<double>::search_initialized();
for (auto d : data) { r.update (d); }
```
