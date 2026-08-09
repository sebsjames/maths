---
layout: page
title: sm::hexyhisto
parent: Reference
nav_order: 35
permalink: /ref/hexyhisto/
---
# sm::hexyhisto
{: .no_toc}
## A 2D histogram of point data on a hexgrid
{: .no_toc}
```c++
import sm.hexyhisto;
```

Module file: [sm/hexyhisto.cppm](https://github.com/sebsjames/maths/blob/main/sm/hexyhisto.cppm).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::hexyhisto<T>` bins a cloud of 2D points onto an existing [`sm::hexgrid`](/maths/ref/hexgrid/), counting how many points land in (or near) each hex - a 2D histogram for hex-tiled data, useful for plotting a density map of e.g. crossing points or events over a spatial domain.

**Note:** at the time of writing, `sm.hexyhisto` has no test or example in this repository.

## Creating a hexyhisto

Here's an example where we create a circular `sm::hexgrid`, then

```c++
sm::hexgrid hg (0.1f, 2.0f, 0.0f);
hg.set_circular_boundary (0.5f);

sm::vvec<sm::vec<float>> data; // sm::vec<float> defaults to 3 elements: {x, y, flag}
data.push_back ({ 0.0f, 0.0f, 0.0f });  // a point at the origin
data.push_back ({ 0.3f, 0.1f, 0.0f });  // a point not at the origin
data.push_back ({ 0.2f, 0.2f, -1.0f }); // flag < 0 means "skip this point"
// ... etc

// Pass data and hexgrid to hexyhisto
sm::hexyhisto<float> hh (data, &hg);
```
Each `data` entry is a 3-element `sm::vec<T>`: `{x, y, flag}`. A negative `flag` marks a point that should be skipped/ignored.

## Reading the result

```c++
T total = hh.datacount;         // how many input points were actually counted
sm::vvec<T> counts = hh.counts; // raw count per hex, indexed by each hex's vi
sm::vvec<T> proportions = hh.proportions; // counts, normalized to sum to 1
```
`proportions` is exactly what you'd plot on the `sm::hexgrid` to visualize the histogram as a density map.

*This page was authored with AI, based on human written code in hexyhisto.cppm and reviewed by Seb James*
