// -*- C++ -*-
/*!
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Utility functions
 */
module;

#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>

export module sm.util;

import sm.trait_tests;

export namespace sm::util
{
    /*!
     * Split a string of values into a vector using the separator string (not char)
     * passed in as "separator". If ignore_trailing_empty_val is true, then a trailing
     * separator with nothing after it will NOT cause an additional empty value in
     * the returned vector.
     */
    std::vector<std::string> string_to_vector (const std::string& s,
                                               const std::string& separator,
                                               const bool ignore_trailing_empty_val = true)
    {
        if (separator.empty()) {
            throw std::runtime_error ("Can't split the string; the separator is empty.");
        }
        std::vector<std::string> the_vec;
        std::string entry("");
        std::string::size_type sep_len = separator.size();
        std::string::size_type a = 0, b = 0;
        while (a < s.size() && (b = s.find (separator, a)) != std::string::npos) {
            entry = s.substr (a, b - a);
            the_vec.push_back (entry);
            a = b + sep_len;
        }
        // Last one has no separator
        if (a < s.size()) {
            b = s.size();
            entry = s.substr (a, b - a);
            the_vec.push_back (entry);
        } else {
            if (!ignore_trailing_empty_val) {
                the_vec.push_back ("");
            }
        }

        return the_vec;
    }

    // Strip bracket characters from string
    void strip_brackets (std::string& str)
    {
        static constexpr std::string_view bracket_chars {"{}[]()"};
        std::string::size_type ptr = std::string::npos;
        while ((ptr = str.find_last_of (bracket_chars, ptr)) != std::string::npos) {
            str = str.erase (ptr, 1);
        }
    }

    // Write an item of type T to fout, assumed open. T can be basic types or fixed size containers (sm::vec/std::array)
    template<typename T> requires (std::is_arithmetic_v<T> == true || sm::is_copyable_fixedsize<std::decay_t<T>>::value == true)
    void binary_write (std::ofstream& fout, const T& from)
    {
        if constexpr (sm::is_copyable_fixedsize<std::decay_t<T>>::value == true) {
            for (auto el : from) { fout.write (reinterpret_cast<const char*>(&el), sizeof el); }
        } else {
            // Should be ok for float, double, uint32_t etc
            fout.write (reinterpret_cast<const char*>(&from), sizeof from);
        }
    }

    // Read an item of type T from fin, which is assumed to be open
    template<typename T> requires (std::is_arithmetic_v<T> == true || sm::is_copyable_fixedsize<std::decay_t<T>>::value == true)
    void binary_read (std::ifstream& fin, T& into)
    {
        if constexpr (sm::is_copyable_fixedsize<std::decay_t<T>>::value == true) {
            for (auto& el : into) { fin.read (reinterpret_cast<char*>(&el), sizeof el); }
        } else {
            fin.read (reinterpret_cast<char*>(&into), sizeof into);
        }
    }

} // namespace
