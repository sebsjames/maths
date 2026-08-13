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

    namespace internal
    {
        // Return an I integer with sz bits set true/1
        template <typename I> requires std::is_integral_v<I>
        constexpr I n_bits (const std::size_t sz)
        {
            I all_set = I{0};
            std::size_t Ibits = sizeof (I) * 8;
            for (std::size_t i = 0; i < sz && i < Ibits; ++i) { all_set |= 1 << i; }
            return all_set;
        }
    }

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
        // The value of the infimum (the minimum)
        T min = T{0};
        // The value of the supremum (the maximum)
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

        // Pass in a value and return either the value or whichever of min or max it exceeded
        template<typename Ty = T> requires (infimum == interval_endpoint::closed) && (supremum == interval_endpoint::closed)
        constexpr T constrain (const Ty& val) noexcept
        {
            T constrained = val;
            if (val > this->max) { constrained = this->max; }
            else if (val < this->min) { constrained = this->min; }
            return constrained;
        }

        // Is the interval valid? A valid interval specifies a non-empty set. [0, 1] is valid. [1,
        // 0] is not valid. [0, 0] is valid, (0, 0] is invalid. Note that an interval that has been
        // search_initialized will initially be non-valid.
        constexpr bool valid() const noexcept
        {
            if constexpr (sm::number_type<T>::value == 2) {
                // interval is complex. What's the test?
                return true;
            } else if constexpr (sm::number_type<T>::value == 1) {
                // interval is scalar
                // extremum tests here
                if constexpr (infimum == interval_endpoint::open || supremum == interval_endpoint::open) {
                    return this->min < this->max;
                } else { // both suprema are closed min==max is ok
                    return this->min <= this->max;
                }
            } else {
                // interval is vector. What's the test? The test is that this should be an axis
                // aligned bounding box with each dimension have min <(=) max
                bool _valid = true;
                if constexpr (infimum == interval_endpoint::open || supremum == interval_endpoint::open) {
                    for (std::uint32_t i = 0; i < this->min.size(); ++i) {
                        if ((this->min[i] < this->max[i]) == false) { _valid = false; }
                    }
                } else { // both suprema are closed min==max is ok
                    for (std::uint32_t i = 0; i < this->min.size(); ++i) {
                        if ((this->min[i] <= this->max[i]) == false) { _valid = false; }
                    }
                }
                return _valid;
            }
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
        requires (sm::number_type<Ty>::value == 0) // vector elements
        constexpr bool operator== (const interval<Ty, infimum_y, supremum_y>& rhs) const noexcept
        {
            // Assume our vector elements have their own operator==. This will work for sm::interval<sm::vec<>>
            if constexpr (infimum_y != infimum || supremum_y != supremum) {
                return false;
            } else {
                return this->min == rhs.min && this->max == rhs.max;
            }
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
                    std::uint32_t other_inside = 0;
                    other_inside = 1u & (std::real(this->min) <= std::real(other.min) && std::imag(this->min) <= std::imag(other.min)
                                         && std::real(this->max) >= std::real(other.min) && std::imag(this->max) >= std::imag(other.min));
                    other_inside |= (1u & (std::real(this->min) <= std::real(other.max) && std::imag(this->min) <= std::imag(other.max)
                                           && std::real(this->max) >= std::real(other.max) && std::imag(this->max) >= std::imag(other.max))) << 1;
                    return other_inside == 3u;
                } else {
                    []<bool flag = false>() { static_assert(flag, "contains(interval) for T complex not implemented for open/semi-open"); }();
                }
            } else if constexpr (sm::number_type<T>::value == 1) { // interval is scalar
                std::uint32_t other_inside = 0;
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
            // Invalid intervals (those that are an empty set) can't intersect
            if (this->valid() == false || other.valid() == false) { return false; }

            if constexpr (sm::number_type<T>::value == 2) { // interval is complex
                // Does other define a rectangle in the complex plane that intersects the one made by this->min and max?
                bool othermin_inside = std::real(this->min) <= std::real(other.min) && std::imag(this->min) <= std::imag(other.min)
                && std::real(this->max) >= std::real(other.min) && std::imag(this->max) >= std::imag(other.min);
                bool othermax_inside = std::real(this->min) <= std::real(other.max) && std::imag(this->min) <= std::imag(other.max)
                && std::real(this->max) >= std::real(other.max) && std::imag(this->max) >= std::imag(other.max);
                return othermin_inside || othermax_inside;

            } else if constexpr (sm::number_type<T>::value == 1) { // interval is scalar

                // These tests depend on min <= max being true for both *this and other.

                std::uint32_t itest = 0u;
                // TEST 1. Is other's min inside my interval?. Set bit 0.
                // True if both of these are true:
                //  1.1. othermin <(=) max
                if constexpr (supremum == interval_endpoint::closed && infimum_y == interval_endpoint::closed) {
                    itest = other.min <= this->max;
                } else {
                    itest = other.min < this->max;
                }
                //  1.2. othermin >(=) min
                if constexpr (infimum == interval_endpoint::closed && infimum_y == interval_endpoint::closed) {
                    itest &= other.min >= this->min;
                } else {
                    itest &= other.min > this->min;
                }

                // TEST 2. Is other's max inside my interval?
                // True if both of these are true:
                //  2.1. othermax <(=) max
                if constexpr (supremum == interval_endpoint::closed && supremum_y == interval_endpoint::closed) {
                    itest |= (other.max <= this->max) << 1;
                } else {
                    itest |= (other.max < this->max) << 1;
                }
                //  2.2. othermax >(=) min
                if constexpr (infimum == interval_endpoint::closed && supremum_y == interval_endpoint::closed) {
                    itest &= 1u | ((other.max >= this->min) << 1);
                } else {
                    itest &= 1u | ((other.max > this->min) << 1);
                }

                // TEST 3. Is my min AND max inside other? (No constexpr supremum/infimum tests required)
                itest |= ((other.max >= this->max) && (other.min <= this->min)) << 2;

                return itest > 0u;

            } else { // interval is vector (so this test is an aabb bounding box intersection test)

                using T_el=std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;

                constexpr std::size_t sz = sizeof (T) / sizeof (T_el);

                // If you get this error, you could always change idim to std::uint64_t.
                static_assert (sz <= 32,
                               "The vector type in the sm::interval needs to be <= 32D to call sm::interval::intersects");

                // Results of intersection tests for each dimension are collected in this unsigned
                // integer. If sz > num bits in uint32_t, then this code would not work.
                std::uint32_t idim = 0u;

                constexpr std::uint32_t idim_all_dims = internal::n_bits<std::uint32_t> (sz);

                // Test each dimension as a scalar intersection. If there's an intersection on ALL
                // dimensions at once, then the vector intersection is true.
                for (std::size_t i = 0; i < sz; ++i) {

                    std::uint32_t itest = 0u;

                    // TEST 1. Is other's min inside my interval?. Set bit 0.
                    // True if both of these are true:
                    //  1.1. othermin <(=) max
                    if constexpr (supremum == interval_endpoint::closed && infimum_y == interval_endpoint::closed) {
                        itest = (other.min[i] <= this->max[i]) << 0;
                    } else {
                        itest = (other.min[i] < this->max[i]) << 0;
                    }
                    //  1.2. othermin >(=) min
                    if constexpr (infimum == interval_endpoint::closed && infimum_y == interval_endpoint::closed) {
                        itest &= (other.min[i] >= this->min[i]) << 0;
                    } else {
                        itest &= (other.min[i] > this->min[i]) << 0;
                    }

                    // TEST 2. Is other's max inside my interval?
                    // True if both of these are true:
                    //  2.1. othermax <(=) max
                    if constexpr (supremum == interval_endpoint::closed && supremum_y == interval_endpoint::closed) {
                        itest |= (other.max[i] <= this->max[i]) << 1;
                    } else {
                        itest |= (other.max[i] < this->max[i]) << 1;
                    }
                    //  2.2. othermax >(=) min
                    if constexpr (infimum == interval_endpoint::closed && supremum_y == interval_endpoint::closed) {
                        itest &= 1u | ((other.max[i] >= this->min[i]) << 1);
                    } else {
                        itest &= 1u | ((other.max[i] > this->min[i]) << 1);
                    }

                    // TEST 3. Is my min AND max inside other? (No constexpr supremum/infimum tests required)
                    itest |= ((other.max[i] >= this->max[i]) && (other.min[i] <= this->min[i])) << 2;

                    idim |= (itest > 0u) ? (1u << i) : 0u;
                }
                return idim == idim_all_dims;
            }
        }

        // A note on intersection(interval<>& other) and union(interval<>& other) methods
        //
        // I don't think this is possible for an interval class with compile-time defined
        // infimum/supremum. It would not be possible to determine the bounds (whether open or
        // closed) of the result at compile time.

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
