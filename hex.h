// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

#pragma once

#include <iostream>
#include <iomanip>
#include <limits>

void HexDump(std::ostream& os, const uint8_t* const data, const uint32_t count);

// easier method to output zero-padded hex values to streams
// source: https://stackoverflow.com/a/21417465
// modifications: added specialization for char/signed char/unsigned char to prevent writing
//     "cout << hex(unsigned(u8), 2)" instead of "cout << hex(u8)" when u8 is char type
//     (otherwise it would be treated as char and printed a character to the stream)
// example: cout << hex((std::uint8_t)1) << ", " << hex((std::uint16_t)1) << ", " << hex((std::uint32_t)1) << endl

template <typename T>
struct Hex
{
    static constexpr int Width = (std::numeric_limits<T>::digits + 1) / 4; // C++11, +1 because of signed types
    //enum { Width = (std::numeric_limits<T>::digits + 1) / 4 }; // Otherwise

    const T& value;
    const bool oX;
    const int width;

    Hex(const T& value, const bool oX = false, int width = Width)
        : value(value), oX(oX), width(width)
    {}

    void write(std::ostream& stream) const {
        if (std::numeric_limits<T>::radix != 2) {
            stream << value;
        } else {
            std::ios_base::fmtflags flags = stream.setf(std::ios_base::hex, std::ios_base::basefield);
            char fill = stream.fill('0');
            if (oX) {
                stream << "0x";
            }
            stream << std::setw(width) << value;
            stream.fill(fill);
            stream.setf(flags, std::ios_base::basefield);
        }
    }
};

template <typename T>
inline std::ostream& operator << (std::ostream& stream, const Hex<T>& value) {
    value.write(stream);
    return stream;
}

template <typename T>
inline Hex<T> hex(const T& value, int width = Hex<T>::Width) {
    return Hex<T>(value, false, width);
}

template <typename T>
inline Hex<T> hex0x(const T& value, int width = Hex<T>::Width) {
    return Hex<T>(value, true, width);
}

// specialization for "unsigned char"/"uint8_t", otherwise it would print character instead of value
inline Hex<unsigned int> hex(const unsigned char& value) {
    return Hex<unsigned int>(unsigned(value), false, 2);
}

// specialization for "unsigned char"/"uint8_t", otherwise it would print character instead of value
inline Hex<unsigned int> hex0x(const unsigned char& value) {
    return Hex<unsigned int>(unsigned(value), true, 2);
}

// specialization for "signed char"/"int8_t", otherwise it would print character instead of value
inline Hex<signed int> hex(const signed char& value) {
    return Hex<signed int>(value, false, 2);
}

// specialization for "signed char"/"int8_t", otherwise it would print character instead of value
inline Hex<signed int> hex0x(const signed char& value) {
    return Hex<signed int>(value, true, 2);
}

// specialization for "char", otherwise it would print character instead of value
inline Hex<int> hex(const char& value) {
    return Hex<int>(value, false, 2);
}

// specialization for "char", otherwise it would print character instead of value
inline Hex<int> hex0x(const char& value) {
    return Hex<int>(value, true, 2);
}
