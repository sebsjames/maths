// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * A (no longer quite so) tiny container class to hold the information defining a mathematical
 * interval. With the earlier incarnation of this code, which was called sm::range, I wanted a
 * common type in which to return minmax values for use in sm::vec and sm::vvec. An option would
 * have been an std::array, but I preferred this, as vvec doesn't otherwise need to include
 * <array>.
 *
 * The class has since become an interval class, covering (for most cases) closed, open and
 * semi-open intervals, both for real numbers, complex numbers and even vectors (though our
 * sm::interval<vector_type> is really an axis-aligned bounding box rather than a proper
 * mathematical interval).
 *
 * The design is a struct containing two values (the min and max), with the nature of the endpoints
 * defined with template parameters. This means that there are some limitations. While we can write
 * a function that returns whether a value falls within the interval or not, we can't write a
 * function that returns the union or intersection of two intervals, because the runtime values
 * would have to determine the compile-time type of the return object (i.e., its endpoint template
 * parameters). If intersection and union methods become a requirement in the future, it will be
 * necessary to write a new interval class containing a runtime-variable encoding of the endpoint
 * types. This could cost only 2 bits of storage space (or more practically a single
 * std::uint32_t). At present this functionality is not required. In fact, sm::interval is almost
 * always used with the default closed extrema (with the interval being notated [min, max]).
 *
 * Extra bonus: You can also use sm::interval<> in constexpr functions.
 *
 * Author: Seb James
 * Date: June 2026
 */

module;

#include <sm/interval.hpp>
#include <sm/trait_tests.hpp>
#include <sm/constexpr_math.hpp>

export module sm.interval;

export namespace sm
{
    using sm::interval_init;
    using sm::interval_endpoint;
    using sm::operator<<;
    using sm::interval;
}
