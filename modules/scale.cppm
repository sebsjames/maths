// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * \brief A class for scaling signals
 *
 * scale is a class for scaling (transforming) signals. It has been coded for linear scaling of
 * signals. It could also be used to logarithmically scale signals with suitable extension, and this
 * has been kept in mind (see scaling_function). It has an autoscaling feature which allows a signal which
 * ranges between x and y to be rescaled to range between 0 and 1 (or -1 and 1 or -w and z)
 *
 * Classes created from the template class sm::scale will derive from one of the sm::scale_impl
 * <ntype, T, S> classes, where ntype is the 'number type' (0 means the numbers are mathematical
 * vectors like sm::vec, std::array or std::vector; 1 means that the numbers are scalars like
 * float, double or int)
 *
 * Usage: See \c tests/scale1.cpp \n
 * e.g.:\n
 * \code{.cpp}
 *   sm::scale<float> s;
 *   s.do_autoscale = true;
 *   std::vector<float> vf = { 1, 2, 3 };
 *   std::vector<float> result(vf);
 *   s.transform (vf, result);
 *\endcode
 *
 * If the output type is different from the input type, then specify both as template parameters:
 * \code{.cpp}
 *   sm::scale<int, float> s;
 *   s.do_autoscale = true;
 *   std::vector<int> vi = { -2, -1, 1, 3 };
 *   std::vector<float> result(vi.size());
 *   s.transform (vi, result);
 *\endcode
 *
 * \author Seb James
 * \date April 2020
 */
module;

#include <sm/scale.hpp>

export module sm.scale;

export namespace sm
{
    using sm::scaling_function;
    using sm::scale;
    using sm::operator<<;
}
