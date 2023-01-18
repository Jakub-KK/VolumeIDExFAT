// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

module;

#include <iostream>
#include <iomanip>
#include <limits>

export module hex;

using std::endl;
using std::ios_base;
using std::numeric_limits;
using std::ostream;
using std::setw;

// easier method to output zero-padded hex values to streams
// source: https://stackoverflow.com/a/21417465
// modifications: added specialization for char/signed char/unsigned char to prevent writing
//     "cout << hex(unsigned(u8), 2)" instead of "cout << hex(u8)" when u8 is char type
//     (otherwise it would be treated as char and printed a character to the stream)
// example: cout << hex((std::uint8_t)1) << ", " << hex((std::uint16_t)1) << ", " << hex((std::uint32_t)1) << endl

export
template <typename T>
struct Hex
{
    static constexpr int Width = (numeric_limits<T>::digits + 1) / 4; // C++11, +1 because of signed types
    //enum { Width = (numeric_limits<T>::digits + 1) / 4 }; // Otherwise

    const T& value;
    const bool oX;
    const int width;

    Hex(const T& value, const bool oX = false, int width = Width)
        : value(value), oX(oX), width(width)
    {}

    void write(ostream& stream) const {
        if (numeric_limits<T>::radix != 2) {
            stream << value;
        } else {
            ios_base::fmtflags flags = stream.setf(ios_base::hex, ios_base::basefield);
            char fill = stream.fill('0');
            if (oX) {
                stream << "0x";
            }
            stream << setw(width) << value;
            stream.fill(fill);
            stream.setf(flags, ios_base::basefield);
        }
    }
};

export
template <typename T>
inline ostream& operator << (ostream& stream, const Hex<T>& value) {
    value.write(stream);
    return stream;
}

export
template <typename T>
inline Hex<T> hex(const T& value, int width = Hex<T>::Width) {
    return Hex<T>(value, false, width);
}

export
template <typename T>
inline Hex<T> hex0x(const T& value, int width = Hex<T>::Width) {
    return Hex<T>(value, true, width);
}

// specialization for "unsigned char"/"uint8_t", otherwise it would print character instead of value
export
inline Hex<unsigned int> hex(const unsigned char& value) {
    return Hex<unsigned int>(unsigned(value), false, 2);
}

// specialization for "unsigned char"/"uint8_t", otherwise it would print character instead of value
export
inline Hex<unsigned int> hex0x(const unsigned char& value) {
    return Hex<unsigned int>(unsigned(value), true, 2);
}

// specialization for "signed char"/"int8_t", otherwise it would print character instead of value
export
inline Hex<signed int> hex(const signed char& value) {
    return Hex<signed int>(value, false, 2);
}

// specialization for "signed char"/"int8_t", otherwise it would print character instead of value
export
inline Hex<signed int> hex0x(const signed char& value) {
    return Hex<signed int>(value, true, 2);
}

// specialization for "char", otherwise it would print character instead of value
export
inline Hex<int> hex(const char& value) {
    return Hex<int>(value, false, 2);
}

// specialization for "char", otherwise it would print character instead of value
export
inline Hex<int> hex0x(const char& value) {
    return Hex<int>(value, true, 2);
}

// TODO: create helpers so that "cout << hexdump(data, 20) << endl" works 
export void HexDump(ostream& os, const uint8_t* const data, const uint32_t count) {
    os << hex0x((uint16_t)0) << " ";
    for (uint32_t i{ 0 }; i < count; i++) {
        os << hex(data[i]) << " ";
        if ((i + 1) % 16 == 0) {
            os << endl;
            if (i != count - 1) {
                os << hex0x((uint16_t)(i + 1)) << " ";
            }
        }
    }
}
