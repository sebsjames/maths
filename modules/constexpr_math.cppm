// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Constexpr math in the sm namespace as sm::cem
 *
 * This file is based on the GCE-Math C++ library by Keith O'Hara.
 *
 * But! Seb has cherry picked the functions he wanted and ruined/modified Keith's
 * code to suit his needs. It's all now in this one file, and should work for C++20 or C++23
 * compilers. If you're up to C++26, you'll no longer need this, as all the std::: versions of the
 * functions here will be constexpr compliant.
 *
 * If you need to understand it, then refer back to Keith's original, I stripped many of the
 * comments out of his code for brevity.
 *
 * Many thanks to Keith for writing the code!  (copied from GCEM at commit
 * 012ae73c6d0a2cb09ffe86475f5c6fba3926e200)
 */

/*################################################################################
  ##
  ##   Copyright (C) 2016-2024 Keith O'Hara
  ##
  ##   This file is part of the GCE-Math C++ library.
  ##
  ##   Licensed under the Apache License, Version 2.0 (the "License");
  ##   you may not use this file except in compliance with the License.
  ##   You may obtain a copy of the License at
  ##
  ##       http://www.apache.org/licenses/LICENSE-2.0
  ##
  ##   Unless required by applicable law or agreed to in writing, software
  ##   distributed under the License is distributed on an "AS IS" BASIS,
  ##   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ##   See the License for the specific language governing permissions and
  ##   limitations under the License.
  ##
  ################################################################################*/
module;

#include <sm/constexpr_math.hpp>

export module sm.constexpr_math;

export namespace sm::cem // ConstExpr Math
{
    using sm::cem::abs;
    using sm::cem::isinf;
    using sm::cem::isfinite;
    using sm::cem::isnan;
    using sm::cem::sgn;
    using sm::cem::signbit;
    using sm::cem::copysign;
    using sm::cem::neg_zero;
    using sm::cem::log;
    using sm::cem::log10;
    using sm::cem::ceil;
    using sm::cem::floor;
    using sm::cem::trunc;
    using sm::cem::round;
    using sm::cem::exp;
    using sm::cem::pow;
    using sm::cem::atan;
    using sm::cem::atan2;
    using sm::cem::sqrt;
    using sm::cem::tan;
    using sm::cem::cos;
    using sm::cem::sin;
    using sm::cem::acos;
    using sm::cem::asin;
}
