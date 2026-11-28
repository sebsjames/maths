// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * Type traits template incantations.
 *
 * This file contains numerous classes which can be used to test for features in class
 * types. It's useful for if constexpr () tests, and was initially
 * written/collected/adapted from internet examples for sm::Winder.
 *
 * \author Seb James
 * \date May 2020
 */
module;

#include <sm/trait_tests.hpp>

export module sm.trait_tests;

export namespace sm
{
    using sm::has_subtraction;
    using sm::has_addition;
    using sm::has_xy_methods;
    using sm::has_resize_method;
    using sm::has_xy_members;
    using sm::has_firstsecond_members;
    using sm::array_access_possible;
    using sm::is_copyable_container;
    using sm::is_constexpr_constructible;
    using sm::call_size_const;
    using sm::has_size_const_method;
    using sm::is_copyable_fixedsize;
    using sm::is_complex;
    using sm::value_type;
    using sm::value_type_t;
    using sm::number_type;
}
