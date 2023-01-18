// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

export module array_length;

/// <summary>Compile-time computation of array length</summary>
export
template <typename T, size_t N>
constexpr size_t array_length(T(&)[N]) {
    return N;
}
