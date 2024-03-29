// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

export module volumeID;

import std;

import arguments;
import exceptions;
import exfat;
import hex;
import usage;

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::pair;

void ExecuteProgram(const Arguments& arguments) {
    // report parameters
    if (arguments.optVerbose) {
        cout
            << "Drive device name: " << arguments.driveDeviceName << endl
            << "exFAT volume serial number to set: " << hex(arguments.volumeSerialNumber) << endl
            << endl
            ;
    }
    // try to open the drive and change volume serial number if non-corrupted exFAT volume is detected
    ExFATChangeVolumeSerialNumber(arguments.driveDeviceName, arguments.volumeSerialNumber);
    // report success
    if (arguments.optVerbose) {
        cout << "Operation performed successfully." << endl;
    }
}

// defined in stdlib.h, missing in std module
constexpr int EXIT_SUCCESS{ 0 };
constexpr int EXIT_FAILURE{ 1 };

export int ExecuteProgram(const string& executableName, const vector<string>& args) {
    Arguments arguments{ executableName };
    try {
        bool bannerDone{ false };
        try {
            ParseArguments(args, arguments);
            PrintBanner(arguments);
            bannerDone = true;
            ExecuteProgram(arguments);
            return EXIT_SUCCESS;
        } catch (...) {
            if (!bannerDone) PrintBanner(arguments);
            throw;
        }
    } catch (const ProgramArgumentsError& e) {
        PrintErrorAndUsage(arguments, e.what());
    } catch (const ExFATModificationError& e) {
        PrintErrorOnly(arguments, e.what(), e.details());
    } catch (const std::exception& e) {
        PrintErrorOnly(arguments, "Program error.", e.what());
    } catch (...) {
        PrintErrorOnly(arguments, "Program error.", "Error details not provided.");
    }
    return EXIT_FAILURE;
}
