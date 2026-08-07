---
title: sm::hdfdata
parent: Reference
layout: page
permalink: /ref/hdf5
nav_order: 23
---
# sm::hdfdata
## HDF5 data storage and retrieval
{: .no_toc}

```c++
import sm.hdfdata;
```
Module file: [sm/hdfdata.cppm](https://github.com/sebsjames/maths/blob/main/sm/hdfdata.cppm).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::hdfdata` is a wrapper class around the HDF5 C API. It is used for
saving and loading binary data in HDF5 format. It provides a simple
interface for saving and loading data into, and out of, containers such
as `sm::vec`, `sm::vvec`, `std::vector`, `std::list`, `std::deque` and so
on, as well as simple scalar values, `std::string` and `std::bitset`.

## Opening and closing a file

Construct an `hdfdata` object with a path and a file access mode. You can
specify the access mode using the `sm::file_access_mode` enum class:

```c++
enum class file_access_mode
{
    read_only,
    truncate_write, // Default. Creates a new file, truncating any existing file of the same name
    read_write      // Opens an existing file for read/write, without truncating it
};
```

```c++
import sm.hdfdata;
sm::hdfdata data ("myfile.h5", sm::file_access_mode::truncate_write);
```

or by using the more familiar `std::ios_base::openmode` flags (which I prefer):

```c++
sm::hdfdata data ("myfile.h5", std::ios::out | std::ios::trunc); // truncate_write
sm::hdfdata data2 ("myfile.h5", std::ios::in);                   // read_only
sm::hdfdata data3 ("myfile.h5", std::ios::out);                  // read_write
```

You can also default-construct and call `init` later, using either of the
two argument styles above.

The file is closed automatically when the `hdfdata` object goes out of
scope, so a common pattern is to wrap each read or write in its own scope:

```c++
{
    sm::hdfdata data ("myfile.h5", std::ios::out | std::ios::trunc);
    data.add_contained_vals ("/myvals", myvals);
} // file is written and closed here
```

## Writing data

There are three families of "add" methods, all of which take a `path`
(HDF5 dataset path, e.g. `/myvar` or `/somegroup/myvar`) as their first
argument:

* `add_val (path, val)` &mdash; write a single scalar value. Works for
  `double`, `float`, `int`, `long long int`, `unsigned int`, `unsigned
  long long int`, `bool` and `std::bitset<N>`.
* `add_string (path, str)` &mdash; write a `std::string`.
* `add_contained_vals (path, container)` &mdash; write a whole container of
  values in one call. Works with `std::vector`, `std::list` and
  `std::deque` of scalars, `std::array<T, N>` or `std::pair<T, T>`; with
  `std::array<T, N>` itself; with `sm::vvec<sm::vec<T, N>>` and
  `sm::vvec<sm::vvec<T>>`; and with a lone `std::pair<T, T>`.

If you give a nested path such as `/somegroup/myvar`, `sm::hdfdata`
creates any necessary intermediate groups for you &mdash; you don't need to
create `/somegroup` yourself first.

```c++
#include <vector>
#include <string>
import sm.hdfdata;
std::vector<float> vf = { 1.0f, 2.0f, 3.0f, 4.0f };
{
    sm::hdfdata data ("test.h5", std::ios::out | std::ios::trunc);
    data.add_contained_vals ("/testvectorfloat", vf);
    data.add_val ("/somegroup/scalar", 42);
    data.add_string ("/somegroup/label", std::string("an experiment"));
}
```

### sm::vec and sm::vvec

Containers of `sm::vec<T, N>` (N-dimensional coordinates) and nested
`sm::vvec` are both supported directly:

```c++
sm::vvec<sm::vec<float, 3>> pts (4);
pts[0] = sm::vec<float, 3>{1, 2, 3};
// ...

sm::vvec<sm::vvec<float>> vvv;
vvv.push_back (sm::vvec<float>{1, 2, 3});
// ...

{
    sm::hdfdata data ("test.h5", std::ios::out | std::ios::trunc);
    data.add_contained_vals ("/pts", pts);
    data.add_contained_vals ("/vvv", vvv);
}
```

### Overwriting and appending

Opening a file in `read_write` mode (`std::ios::out` without `trunc`) lets
you add new datasets to an existing file, or overwrite a dataset that's
already there, without destroying the rest of the file's content:

```c++
{
    sm::hdfdata data ("test.h5", std::ios::out | std::ios::trunc); // truncate_write
    data.add_contained_vals ("/testvecarray", va);
}
// Add another dataset to the same file, leaving /testvecarray alone
{
    sm::hdfdata data ("test.h5", std::ios::out); // read_write
    data.add_contained_vals ("/testvecarray2", va);
}
// Overwrite /testvecarray2 in place
va[0][0] = 100.0f;
{
    sm::hdfdata data ("test.h5", std::ios::out); // read_write
    data.add_contained_vals ("/testvecarray2", va);
}
```

When overwriting a dataset in `read_write` mode, the new data must fit
within (or exactly match) the existing dataset's dimensions; `sm::hdfdata`
throws a `std::runtime_error` if the existing space is too small for what
you're about to write.

## Reading data

The reading API mirrors the writing API:

* `read_val (path, val)` &mdash; read a single scalar (or `std::bitset<N>`)
  into `val`.
* `read_string (path, str)` &mdash; read a string into `str`.
* `read_contained_vals (path, container)` &mdash; read a whole container,
  resizing it as necessary. Supports the same set of container/element
  type combinations as `add_contained_vals`.

```c++
#include <vector>
import sm.hdfdata;

std::vector<float> vfread;
{
    sm::hdfdata data ("test.h5", std::ios::in);
    data.read_contained_vals ("/testvectorfloat", vfread);
}
```

### Handling missing data

By default, if you try to read a path that doesn't exist in the file,
`sm::hdfdata` prints an informational message to `stdout` and leaves your
variable unmodified. You can change this behaviour by setting the public
`on_read_error_action` member, which takes a `read_error_action`:

```c++
enum class read_error_action
{
    exception, // Throw a std::runtime_error
    warning,   // Print a message to stderr, then carry on
    info,      // Print a message to stdout, then carry on (the default)
    carry_on   // Say nothing; carry on, leaving your variable unmodified
};
```

```c++
sm::hdfdata data ("test.h5", std::ios::in);
data.on_read_error_action = sm::read_error_action::exception;
data.read_contained_vals ("/might_not_exist", myvals); // throws if the path really does not exist
```

## Complete example

Putting reading and writing together, using `std::ios` flags to select the
access mode (as in [tests/hdfdata1.cpp](https://github.com/sebsjames/maths/blob/main/tests/hdfdata1.cpp)):

```c++
#include <vector>
import sm.hdfdata;

int main()
{
    std::vector<float> vf = { 1.0f, 2.0f, 3.0f, 4.0f };
    {
        sm::hdfdata data ("test.h5", std::ios::out | std::ios::trunc);
        data.add_contained_vals ("/testvectorfloat", vf);
    } // data is written to test.h5 and closed here

    std::vector<float> vfread;
    {
        sm::hdfdata data ("test.h5", std::ios::in);
        data.read_contained_vals ("/testvectorfloat", vfread);
    } // test.h5 is closed here

    // vfread now equals vf
}
```

*This page was authored with AI, based on human written code in hdfdata.cppm, and reviewed by Seb James*
