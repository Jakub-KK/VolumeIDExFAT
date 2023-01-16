// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

#pragma once

#include <string>
#include <vector>

struct Arguments {
    const std::string executableName;
    bool isLicenseAccepted;
    bool optNoLegal;
    bool optNoBanner;
    bool optVerbose;
};

void ParseArguments(const std::vector<std::string>& args, Arguments& arguments);
