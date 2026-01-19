---
title: sm::hdfdata
parent: Reference
layout: page
permalink: /ref/hdf5
nav_order: 14
---
# sm::hdfdata
## HDF5 data storage and retrieval
{: .no_toc}

```c++
#include <sm/hdfdata>
```
Header file: [<sm/hdfdata>](https://github.com/sebsjames/maths/blob/main/sm/hdfdata).

**Table of Contents**

- TOC
{:toc}

## Summary

sm::hdfdata is a wrapper class around the HDF5 C API. It is used for
saving and loading binary data in HDF5 format. It provides a simple
interface for saving and loading data into, and out of containers such
as `sm::vec`, `sm::vvec`, `std::vector` and so on.