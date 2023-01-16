// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

// precompiled headers
#include "pch.h"

#include <format>
#include <string>
#include <vector>

#include "exception.h"

#include "arguments.h"

using std::string;
using std::vector;
using std::format;

void ParseArguments(const vector<string>& args, Arguments& arguments) {
    using namespace std::string_literals;

    if (args.size() == 0) {
        throw ProgramArgumentsError("Required arguments missing."s);
    }
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
            throw ProgramArgumentsError(format("Argument \"{}\" unknown, aborting.", *i));
        }
    }
    if (!arguments.isLicenseAccepted) {
        throw ProgramArgumentsError("Required argument missing: license agreement statement (\"-i_accept_license\" argument)."s);
    }
}
