// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

// precompiled headers
#include "pch.h"

#include <vector>
#include <string>
#include <filesystem>

#include "volumeID.h"

using std::string;
using std::vector;
using std::filesystem::path;

int main(const int argc, const char* const* const argv) {
    const path pathExecutable{ argv[0] };
    const string executableName{ pathExecutable.filename().string() };

    const vector<string> args{ argv + 1, argv + argc };

    return ExecuteProgram(executableName, args);
}
