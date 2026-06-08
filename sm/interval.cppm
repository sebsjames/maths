// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * A (no longer quite so) tiny container class to hold the information defining a mathematical
 * interval. With range, I wanted a common type in which to return minmax values for use in sm::vec
 * and sm::vvec. An option would have been an std::array, but I prefer this, as vvec doesn't
 * otherwise need to include <array>. range can be used in constexpr functions.
 *
 * The class has become a full interval class, covering (for most cases) closed, open and semi-open
 * intervals, both for real numbers, complex numbers and vectors.
 *
 * Extra bonus: You can use this as an implementation of an axis-aligned bounding box!
 *
 * Author: Seb James
 * Date: June 2026
 */

module;

#include <cstdint>
#include <ostream>
#include <sstream>
#include <limits>
#include <complex>
#include <span>

export module sm.interval;

import sm.trait_tests;
export import sm.constexpr_math;

export namespace sm
{
    // Different values to use to initialize a interval object with
    enum class interval_init { zeros, for_search };

    // Is the endpoint closed '[' or ']' or open '(' or ')'?
    enum class interval_endpoint { closed, open };

    // Forward declare the class and stream operator
    template <typename T, interval_endpoint infimum, interval_endpoint supremum> struct interval;

    template <typename T, interval_endpoint infimum, interval_endpoint supremum>
    std::ostream& operator<< (std::ostream&, const interval<T, infimum, supremum>&);

    // interval is a constexpr-friendly literal type defining an interval [min, max], (min, max],
    // [min, max) or (min, max).
    //
    // \tparam T The type representing the values in the interval
    // \tparam infimum How to interpret the min of the interval as an endpoint (closed or open)
    // \tparam supremum How to interpret the max of the interval (closed or open)
    template <typename T,
              interval_endpoint infimum = interval_endpoint::closed,
              interval_endpoint supremum = interval_endpoint::closed>
    struct interval
    {
        // The minimum value in the closed interval
        T min = T{0};
        // The maximum value
        T max = T{0};

        // In the default constructor, min == max == T{0}
        constexpr interval() noexcept {}
        // Interval constructor in which you can specify that the interval should be initialized for search
        constexpr interval (const sm::interval_init _interval_init) noexcept
        {
            if (_interval_init == sm::interval_init::for_search) { this->search_init(); }
        }
        // Interval constructor taking the min and max for a ready-to-go interval
        constexpr interval (const T& _min, const T& _max) noexcept : min(_min), max(_max) {}

        // Set the interval to _min, _max
        constexpr void set (const T& _min, const T& _max) noexcept
        {
            this->min = _min;
            this->max = _max;
        }

        // Output a string representation of the min and max. Rewrite with <format> at some point.
        std::string str() const
        {
            std::stringstream ss;
            if constexpr (infimum == interval_endpoint::closed) {
                ss << "[";
            } else {
                ss << "(";
            }
            ss << this->min << ", " << this->max;
            if constexpr (supremum == interval_endpoint::closed) {
                ss << "]";
            } else {
                ss << ")";
            }
            return ss.str();
        }

        template<typename Ty=T, interval_endpoint infimum_y, interval_endpoint supremum_y>
        requires std::is_floating_point_v<Ty>
        constexpr bool operator== (const interval<Ty, infimum_y, supremum_y>& rhs) const noexcept
        {
            if constexpr (infimum_y != infimum || supremum_y != supremum) {
                return false;
            } else {
                return (sm::cem::abs(this->min - static_cast<T>(rhs.min)) < std::numeric_limits<T>::epsilon()
                        && sm::cem::abs(this->max - static_cast<T>(rhs.max)) < std::numeric_limits<T>::epsilon());
            }
        }

        template<typename Ty=T, interval_endpoint infimum_y, interval_endpoint supremum_y>
        requires std::is_floating_point_v<Ty>
        constexpr bool operator!= (const interval<Ty, infimum_y, supremum_y>& rhs) const noexcept
        {
            if constexpr (infimum_y != infimum || supremum_y != supremum) {
                return true;
            } else {
                return (sm::cem::abs(this->min - static_cast<T>(rhs.min)) > std::numeric_limits<T>::epsilon()
                        || sm::cem::abs(this->max - static_cast<T>(rhs.max)) > std::numeric_limits<T>::epsilon());
            }
        }

        template<typename Ty=T, interval_endpoint infimum_y, interval_endpoint supremum_y>
        requires std::is_integral_v<Ty>
        constexpr bool operator== (const interval<Ty, infimum_y, supremum_y>& rhs) const noexcept
        {
            if constexpr (infimum_y != infimum || supremum_y != supremum) {
                return false;
            } else {
                return (this->min == static_cast<T>(rhs.min) && this->max == static_cast<T>(rhs.max));
            }
        }

        template<typename Ty=T, interval_endpoint infimum_y, interval_endpoint supremum_y>
        requires std::is_integral_v<Ty>
        constexpr bool operator!= (const interval<Ty, infimum_y, supremum_y>& rhs) const noexcept
        {
            if constexpr (infimum_y != infimum || supremum_y != supremum) {
                return true;
            } else {
                return (this->min != static_cast<T>(rhs.min) || this->max != static_cast<T>(rhs.max));
            }
        }

        // Adding to a interval means adding to both min and max
        constexpr sm::interval<T, infimum, supremum> operator+ (const T& s) const noexcept
        {
            return sm::interval<T, infimum, supremum>{ this->min + s, this->max + s };
        }
        constexpr void operator+= (const T& s) noexcept
        {
            this->min += s;
            this->max += s;
        }

        // Subtracting from a interval
        constexpr sm::interval<T, infimum, supremum> operator- (const T& s) const noexcept
        {
            return sm::interval<T, infimum, supremum>{ this->min - s, this->max - s };
        }
        constexpr void operator-= (const T& s) noexcept
        {
            this->min -= s;
            this->max -= s;
        }

        // Return a interval that is initialized to participate in a search for the max and
        // min through a interval of data.
        //
        // Interval can then be part of a loop through data with code like:
        //
        // sm::vvec<T> data;
        // data.randomize();
        // sm::interval<T> r = sm::interval<T>::search_initialized();
        // for (auto d : data) { r.update (d); }
        // std::cout << "The interval of values in data was: " << r << std::endl;
        static constexpr sm::interval<T, infimum, supremum> search_initialized() noexcept
        {
            sm::interval<T> si;
            if constexpr (sm::number_type<T>::value == 2) { // interval is complex
                si.min = { std::numeric_limits<typename T::value_type>::max(), std::numeric_limits<typename T::value_type>::max() };
                si.max = { std::numeric_limits<typename T::value_type>::lowest(), std::numeric_limits<typename T::value_type>::lowest() };
            } else if constexpr (sm::number_type<T>::value == 1) { // interval is scalar
                si.min = std::numeric_limits<T>::max();
                si.max = std::numeric_limits<T>::lowest();
            } else {
                si.min = sm::interval<T, infimum, supremum>::max_vector<T>();
                si.max = sm::interval<T, infimum, supremum>::lowest_vector<T>();
            }
            return si;
        }

        // Initialise the interval to participate in a search for the max and min through a interval of data.
        //
        // Interval can then be part of a loop through data with code like:
        //
        // sm::vvec<T> data;
        // data.randomize();
        // sm::interval<T> r;
        // r.search_init();
        // for (auto d : data) { r.update (d); }
        // std::cout << "The interval of values in data was: " << r << std::endl;
        constexpr void search_init() noexcept { *this = sm::interval<T, infimum, supremum>::search_initialized(); }

        // Extend the interval to include the given datum. Return true if the interval changed.
        template<typename Ty = T> requires (infimum == interval_endpoint::closed) && (supremum == interval_endpoint::closed)
        constexpr bool update (const Ty& d) noexcept
        {
            bool changed = false;
            if constexpr (sm::number_type<T>::value == 2) { // interval is complex
                // Does d 'extend the rectangle in the complex plane that defines the complex interval'?
                this->min = std::real(d) < std::real(this->min) || std::imag(d) < std::imag(this->min) ? changed = true, d : this->min;
                this->max = std::real(d) > std::real(this->max) || std::imag(d) > std::imag(this->max) ? changed = true, d : this->max;
            } else if constexpr (sm::number_type<T>::value == 1) { // interval is scalar
                this->min = d < this->min ? changed = true, d : this->min;
                this->max = d > this->max ? changed = true, d : this->max;
            } else {
                // Does d extend the volume defined by min and max?
                changed = sm::interval<T>::vec_min (this->min, d) ? true : changed;
                changed = sm::interval<T>::vec_max (this->max, d) ? true : changed;
            }
            return changed;
        }

        // Does the interval contain the value v?
        constexpr bool contains (const T& v) const noexcept
        {
            if constexpr (sm::number_type<T>::value == 2) { // interval is complex
                if constexpr (infimum == interval_endpoint::closed && supremum == interval_endpoint::closed) {
                    // Is v inside the rectangle in the complex plane made by min and max?
                    return (std::real(v) <= std::real(this->max) && std::real(v) >= std::real(this->min)
                            && std::imag(v) <= std::imag(this->max) && std::imag(v) >= std::imag(this->min));
                } else {
                    []<bool flag = false>() { static_assert(flag, "contains() for T complex not implemented for open/semi-open"); }();
                }
            } else {
                // Same logic for vector and scalar T
                if constexpr (infimum == interval_endpoint::closed && supremum == interval_endpoint::closed) {
                    return (v <= this->max && v >= this->min);
                } else if constexpr (infimum == interval_endpoint::open && supremum == interval_endpoint::closed) {
                    return (v <= this->max && v > this->min);
                } else if constexpr (infimum == interval_endpoint::closed && supremum == interval_endpoint::open) {
                    return (v < this->max && v >= this->min);
                } else if constexpr (infimum == interval_endpoint::open && supremum == interval_endpoint::open) {
                    return (v < this->max && v > this->min);
                }
            }
        }

        // If the interval other 'fits inside' this interval, then this interval contains (or encompasses) the interval other.
        template<typename Ty=T, interval_endpoint infimum_y, interval_endpoint supremum_y>
        constexpr bool contains (const sm::interval<Ty, infimum_y, supremum_y>& other) const noexcept
        {
            if constexpr (sm::number_type<T>::value == 2) { // interval is complex
                if constexpr (infimum == interval_endpoint::closed && supremum == interval_endpoint::closed
                              && infimum_y == interval_endpoint::closed && supremum_y == interval_endpoint::closed) {
                    // Does other define a rectangle in the complex plane that fits inside the one made by this->min and max?
                    unsigned int other_inside = 0;
                    other_inside = 1u & (std::real(this->min) <= std::real(other.min) && std::imag(this->min) <= std::imag(other.min)
                                         && std::real(this->max) >= std::real(other.min) && std::imag(this->max) >= std::imag(other.min));
                    other_inside |= (1u & (std::real(this->min) <= std::real(other.max) && std::imag(this->min) <= std::imag(other.max)
                                           && std::real(this->max) >= std::real(other.max) && std::imag(this->max) >= std::imag(other.max))) << 1;
                    return other_inside == 3u;
                } else {
                    []<bool flag = false>() { static_assert(flag, "contains(interval) for T complex not implemented for open/semi-open"); }();
                }
            } else if constexpr (sm::number_type<T>::value == 1) { // interval is scalar
                unsigned int other_inside = 0;
                // min is inside other.min?
                if constexpr (infimum == interval_endpoint::open && infimum_y == interval_endpoint::closed) {
                    other_inside = 1u & (this->min < other.min);
                } else {
                    other_inside = 1u & (this->min <= other.min);
                }

                if constexpr (supremum == interval_endpoint::open && supremum_y == interval_endpoint::closed) {
                    other_inside |= (1u & (this->max > other.max)) << 1;
                } else {
                    other_inside |= (1u & (this->max >= other.max)) << 1;
                }
                return other_inside == 3u;
            } else {
                return (this->contains (this->min) && this->contains (this->max));
            }
        }

        // If the interval other intersects with this interval, return true
        template<typename Ty=T, interval_endpoint infimum_y, interval_endpoint supremum_y>
        constexpr bool intersects (const sm::interval<Ty, infimum_y, supremum_y>& other) const noexcept
        {
            if constexpr (sm::number_type<T>::value == 2) { // interval is complex
                // Does other define a rectangle in the complex plane that intersects the one made by this->min and max?
                bool othermin_inside = std::real(this->min) <= std::real(other.min) && std::imag(this->min) <= std::imag(other.min)
                && std::real(this->max) >= std::real(other.min) && std::imag(this->max) >= std::imag(other.min);
                bool othermax_inside = std::real(this->min) <= std::real(other.max) && std::imag(this->min) <= std::imag(other.max)
                && std::real(this->max) >= std::real(other.max) && std::imag(this->max) >= std::imag(other.max);
                return othermin_inside || othermax_inside;

            } else if constexpr (sm::number_type<T>::value == 1) { // interval is scalar
                // If othermin is inside this or othermax is inside this then intersects is true:
                unsigned int other_inside = 0;
                // othermin inside? True if both of these are true (then set bit 0 of other_inside)
                //  1. othermin inside max?
                if constexpr (supremum == interval_endpoint::closed && infimum_y == interval_endpoint::closed) {
                    other_inside = 1u & (other.min <= this->max);
                } else {
                    other_inside = 1u & (other.min < this->max);
                }
                //  2. othermin inside min?
                if constexpr (infimum == interval_endpoint::closed && infimum_y == interval_endpoint::closed) {
                    other_inside &= 1u & (other.min >= this->min);
                } else {
                    other_inside &= 1u & (other.min > this->min);
                }

                // othermax inside? True if both of these are true:
                //  1. othermax inside max? then set bit 1 of other_inside
                if constexpr (supremum == interval_endpoint::closed && supremum_y == interval_endpoint::closed) {
                    other_inside |= (1u & (other.max <= this->max)) << 1;
                } else {
                    other_inside |= (1u & (other.max < this->max)) << 1;
                }
                //  1. othermax inside min? then set bit 2 of other_inside
                if constexpr (infimum == interval_endpoint::closed && supremum_y == interval_endpoint::closed) {
                    other_inside |= (1u & (other.max >= this->min)) << 2;
                } else {
                    other_inside |= (1u & (other.max > this->min)) << 2;
                }

                // return true for:
                //
                //    2 1 0
                //    -----
                //    0 0 0  F
                //    0 0 1  T
                //    0 1 0  F
                //    0 1 1  T
                //    1 0 0  F
                //    1 0 1  T
                //    1 1 0  T
                //    1 1 1  T
                return (other_inside > 4 || (other_inside % 2 /* != 0 */));

            } else {
                using T_el=std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
                std::size_t sz = sizeof (T) / sizeof (T_el);
                for (std::size_t i = 0; i < sz; ++i) {
                    if (other.min[i] > this->max[i] || other.max[i] < this->min[i]) { return false; }
                }
                return true;
            }
        }

        // Could add intersection and union methods
        //
        // Actually, not sure if I can, as I will not be able to determine the bounds (whether open
        // or closed) of the result at compile time.

        // What's the 'span of the interval'? Whether scalar or complex (or vector), it's max - min
        constexpr T span() const noexcept { return this->max - this->min; }

        // What's the middle of the interval?
        constexpr T mid() const noexcept
        {
            if constexpr (sm::number_type<T>::value == 0) {
                using T_el=std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
                return (this->max + this->min) / T_el{2};
            } else {
                return (this->max + this->min) / T{2};
            }
        }

        // Static member to get a closed interval from any old container of values.
        template <typename C> requires sm::is_copyable_container<C>::value
        static sm::interval<T> get_from (const C& values)
        {
            using Tc = typename C::value_type;

            // The container element type must match T
            if constexpr (std::is_same_v<T, Tc>) {
                if constexpr (sm::number_type<T>::value == 0) { // vector elements
                    // Example to get the type of the container T.
                    // See https://stackoverflow.com/questions/44521991/type-trait-to-get-element-type-of-stdarray-or-c-style-array
                    using T_el = std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
                    sm::interval<Tc> r (std::numeric_limits<T>::max(), std::numeric_limits<T>::min());
                    T_el maxlen = 0;
                    T_el minlen = std::numeric_limits<T_el>::max();

                    for (auto v : values) {
                        // (Vector version compares sqrt (v[0]*v[0] + v[1]*v[1] +...))
                        T_el vlen = 0;
                        for (auto vi : v) { vlen += vi*vi; }
                        vlen = std::sqrt (vlen);
                        if (vlen > maxlen) {
                            maxlen = vlen;
                            r.max = v;
                        }
                        if (vlen < minlen) {
                            minlen = vlen;
                            r.min = v;
                        }
                    }
                    return r; // interval of vector elements

                } else if constexpr (sm::number_type<T>::value == 2) { // complex elements
                    using T_el = typename T::value_type; // If T is std::complex<float>, T_el will be float
                    // Note that there's no specialization of numeric_limits for std::complex, so set it up manually
                    sm::interval<T> r ({std::numeric_limits<T_el>::max(), std::numeric_limits<T_el>::max() }, T{0, 0});
                    for (auto v : values) {
                        // comparison operations on complex numbers commonly consider their
                        // modulus - how far the number is from the origin.
                        r.max = std::abs(v) > std::abs(r.max) ? v : r.max;
                        r.min = std::abs(v) < std::abs(r.min) ? v : r.min;
                    }
                    return r; // interval of complex elements

                } else if constexpr (sm::number_type<T>::value == 1) { // scalar elements
                    sm::interval<T> r (std::numeric_limits<T>::max(), std::numeric_limits<T>::lowest());
                    for (auto v : values) {
                        r.max = v > r.max ? v : r.max;
                        r.min = v < r.min ? v : r.min;
                    }
                    return r; // interval of scalar elements
                } else {
                    []<bool flag = false>() { static_assert(flag, "Can't find interval for that number_type"); }();
                }
            } else {
                []<bool flag = false>() { static_assert(flag, "Container element type should match interval type T"); }();
            }
        }

        // Static member to get a closed interval from a span of values
        static sm::interval<T> get_from (const std::span<T> values)
        {
            if constexpr (sm::number_type<T>::value == 0) { // vector elements
                // Example to get the type of the container T.
                // See https://stackoverflow.com/questions/44521991/type-trait-to-get-element-type-of-stdarray-or-c-style-array
                using T_el = std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
                sm::interval<T> r (std::numeric_limits<T>::max(), std::numeric_limits<T>::min());
                T_el maxlen = 0;
                T_el minlen = std::numeric_limits<T_el>::max();

                for (auto v : values) {
                    // (Vector version compares sqrt (v[0]*v[0] + v[1]*v[1] +...))
                    T_el vlen = 0;
                    for (auto vi : v) { vlen += vi*vi; }
                    vlen = std::sqrt (vlen);
                    if (vlen > maxlen) {
                        maxlen = vlen;
                        r.max = v;
                    }
                    if (vlen < minlen) {
                        minlen = vlen;
                        r.min = v;
                    }
                }
                return r; // interval of vector elements

            } else if constexpr (sm::number_type<T>::value == 2) { // complex elements
                using T_el = typename T::value_type; // If T is std::complex<float>, T_el will be float
                // Note that there's no specialization of numeric_limits for std::complex, so set it up manually
                sm::interval<T> r ({std::numeric_limits<T_el>::max(), std::numeric_limits<T_el>::max() }, T{0, 0});
                for (auto v : values) {
                    // comparison operations on complex numbers commonly consider their
                    // modulus - how far the number is from the origin.
                    r.max = std::abs(v) > std::abs(r.max) ? v : r.max;
                    r.min = std::abs(v) < std::abs(r.min) ? v : r.min;
                }
                return r; // interval of complex elements

            } else if constexpr (sm::number_type<T>::value == 1) { // scalar elements
                sm::interval<T> r (std::numeric_limits<T>::max(), std::numeric_limits<T>::lowest());
                for (auto v : values) {
                    r.max = v > r.max ? v : r.max;
                    r.min = v < r.min ? v : r.min;
                }
                return r; // interval of scalar elements
            } else {
                []<bool flag = false>() { static_assert(flag, "Can't find interval for that number_type"); }();
            }
        }

        // Overload the stream output operator
        friend std::ostream& operator<< <T> (std::ostream& os, const interval<T>& r);

    private:
        // constexpr method to fill a vector-type T with the lowest possible value elements
        template<typename Ty=T>
        requires (sm::number_type<Ty>::value == 0)
        static constexpr T lowest_vector() noexcept
        {
            using T_el=std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
            T lowest_vec = {};
            std::size_t sz = sizeof (T) / sizeof (T_el);
            for (std::size_t i = 0; i < sz; ++i) {
                lowest_vec[i] = std::numeric_limits<T_el>::lowest();
            }
            return lowest_vec;
        }

        // constexpr method to fill a vector-type T with the max possible value elements
        template<typename Ty=T>
        requires (sm::number_type<Ty>::value == 0)
        static constexpr T max_vector() noexcept
        {
            using T_el=std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
            T max_vec = {};
            std::size_t sz = sizeof (T) / sizeof (T_el);
            for (std::size_t i = 0; i < sz; ++i) {
                max_vec[i] = std::numeric_limits<T_el>::max();
            }
            return max_vec;
        }

        // Compare contender with current minimum, cmin. If any element is less than
        // element in cmin, change cmin and return true.
        template<typename Ty=T>
        requires (sm::number_type<Ty>::value == 0)
        static constexpr bool vec_min (T& cmin, const T& contender) noexcept
        {
            using T_el=std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
            std::size_t sz = sizeof (T) / sizeof (T_el);
            bool changed = false;
            for (std::size_t i = 0; i < sz; ++i) {
                cmin[i] = contender[i] < cmin[i] ? changed = true, contender[i] : cmin[i];
            }
            return changed;
        }

        // Compare contender with current maximum, cmax. If any element is greater than
        // element in cmax, change cmax and return true.
        template<typename Ty=T>
        requires (sm::number_type<Ty>::value == 0)
        static constexpr bool vec_max (T& cmax, const T& contender) noexcept
        {
            using T_el=std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;
            std::size_t sz = sizeof (T) / sizeof (T_el);
            bool changed = false;
            for (std::size_t i = 0; i < sz; ++i) {
                cmax[i] = contender[i] > cmax[i] ? changed = true, contender[i] : cmax[i];
            }
            return changed;
        }
    };

    // Output a string with notation "[min, max]" to indicate a closed interval
    template <typename T, interval_endpoint infimum, interval_endpoint supremum>
    std::ostream& operator<< (std::ostream& os, const interval<T, infimum, supremum>& r)
    {
        if constexpr (infimum == interval_endpoint::closed) {
            os << "[";
        } else {
            os << "(";
        }
        os << r.min << ", " << r.max;
        if constexpr (supremum == interval_endpoint::closed) {
            os << "]";
        } else {
            os << ")";
        }
        return os;
    }

} // namespace sm
