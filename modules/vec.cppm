// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 * \brief An N dimensional vector class template which derives from std::array.
 *
 * \author Seb James (with thanks to Konrad Rudolph and Miguel Avila for code review)
 * \date April 2020
 */
module;

#include <sm/vec.hpp>

export module sm.vec;

export namespace sm
{
    using sm::operator<<;
    using sm::vec;
    using sm::operator*;
    using sm::operator/;
    using sm::operator+;
    using sm::operator-;
}
