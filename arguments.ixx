// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

module;

#include <format>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

export module arguments;

import exceptions;

using std::string;
using std::vector;
using std::format;

export struct Arguments {
    const string executableName;
    bool isLicenseAccepted;
    bool optNoLegal;
    bool optNoBanner;
    bool optVerbose;
    string driveDeviceName;
    uint32_t volumeSerialNumber;
};

string ParseDrive(const string& arg) {
    using std::regex;
    using std::smatch;

    const regex driveStringPattern{ "([a-zA-Z]{1}):" };
    smatch driveStringParts;
    if (!regex_match(arg, driveStringParts, driveStringPattern)) {
        throw ProgramArgumentsError(format("Drive \"{}\" invalid, must use format \"X:\" where X is drive letter.", arg));
    }
    return format("\\\\.\\{}:", driveStringParts[1].str());
}

uint32_t ParseVolumeSerialNumber(const string& arg) {
    using std::regex;
    using std::smatch;
    using std::stringstream;
    using std::hex;

    const regex volumeSerialNumberPattern{ "([a-fA-F0-9]{4})-([a-fA-F0-9]{4})" };
    smatch volumeSerialNumberParts;
    if (!regex_match(arg, volumeSerialNumberParts, volumeSerialNumberPattern)) {
        throw ProgramArgumentsError(format("Volume serial number \"{}\" invalid, required format is \"XXXX-XXXX\" where X is hexadecimal digit.", arg));
    }
    stringstream ss;
    ss << hex << volumeSerialNumberParts[1] << volumeSerialNumberParts[2];
    uint32_t result;
    ss >> result;
    return result;
}

export void ParseArguments(const vector<string>& args, Arguments& arguments) {
    using namespace std::string_literals;

    if (args.size() == 0) {
        throw ProgramArgumentsError("Required arguments missing."s);
    }
    bool hasDriveString{ false };
    bool hasVolumeSerialNumber{ false };
    for (auto i{ args.cbegin() }; i != args.cend(); ++i) {
        const string& argIth{ *i };
        if (argIth[0] == '-' || argIth[0] == '/') {
            if (argIth == "-I_accept_license" || argIth == "-i_accept_license" || argIth == "/I_accept_license" || argIth == "/i_accept_license") {
                arguments.isLicenseAccepted = true;
            } else if (argIth == "-nowarning" || argIth == "/nowarning" || argIth == "/nw" || argIth == "/NW") {
                arguments.optNoLegal = true;
            } else if (argIth == "-nobanner" || argIth == "/nobanner" || argIth == "/nb" || argIth == "/NB") {
                arguments.optNoBanner = true;
            } else if (argIth == "-verbose" || argIth == "/verbose" || argIth == "/v" || argIth == "/V") {
                arguments.optVerbose = true;
            } else {
                throw ProgramArgumentsError(format("Flag \"{}\" unknown, aborting.", argIth));
            }
        } else {
            if (!hasDriveString) {
                // read drive argument
                arguments.driveDeviceName = ParseDrive(argIth);
                hasDriveString = true;
            } else if (!hasVolumeSerialNumber) {
                // read volume serial number argument
                arguments.volumeSerialNumber = ParseVolumeSerialNumber(argIth);
                hasVolumeSerialNumber = true;
            } else {
                throw ProgramArgumentsError(format("Argument \"{}\" unknown, aborting.", *i));
            }
        }
    }
    if (!arguments.isLicenseAccepted) {
        throw ProgramArgumentsError("Required argument missing: license agreement statement (\"-i_accept_license\" argument)."s);
    }
    if (!hasDriveString) {
        throw ProgramArgumentsError("Required argument missing: drive."s);
    }
    if (!hasVolumeSerialNumber) {
        throw ProgramArgumentsError("Required argument missing: new serial number."s);
    }
}
