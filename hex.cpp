// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

// precompiled headers
#include "pch.h"

#include "hex.h"

// TODO: create helpers so that "cout << hexdump(data, 20) << endl" works 
void HexDump(std::ostream& os, const uint8_t* const data, const uint32_t count) {
    using std::endl;

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
