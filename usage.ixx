// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

module;

#include <iostream>
#include <string>
#include <vector>

export module usage;

import arguments;

using std::string;
using std::cerr;
using std::cout;
using std::endl;

export void PrintBanner(const Arguments& arguments) {
    if (!arguments.optNoBanner) {
        cout
            << "VolumeIdExFAT v1.0 - Set serial number of exFAT volume" << endl
            << "Copyright (C) 2023 Jakub Kopec-Kawka" << endl
            << endl
            ;
    }
}

void PrintError(const Arguments& arguments, const string& errorMessage, const string& errorDetailsMessage) {
    cerr << "Error: " << errorMessage << endl;
    if (arguments.optVerbose && errorDetailsMessage != "") {
        cerr << "Error details: " << errorDetailsMessage << endl;
    }
}

void PrintUsage(const Arguments& arguments) {
    cout
        << endl
        << "Usage: " << arguments.executableName << " -i_accept_license <drive>: <XXXX-XXXX> [-nobanner] [-nowarning] [-verbose]" << endl
        << "  -i_accept_license   required as a statement that you read and accept the license terms (see LICENSE.txt)" << endl
        << "  <drive>:            drive letter of target volume, \":\" must be included" << endl
        << "  <XXXX-XXXX>         new serial number, X is hexadecimal digit" << endl
        << "  -nobanner           optional, do not display the startup banner" << endl
        << "  -nowarning          optional, do not display the warning about potential data loss" << endl
        << "  -verbose            optional, display a detailed progress information" << endl
        << endl
        << "This program will change the serial number of an exFAT volume." << endl
        << "Administrator privileges are not needed when operating on \"ejectable\" volumes (such as USB removable devices)." << endl
        << "Before making any changes the exFAT filesystem boot records will be checked for consistency," << endl
        << "if an invalid state is detected the program will abort (try using 'chkdsk' program to fix the volume)." << endl
        ;
}

void PrintWarningAndDisclaimer(const Arguments& arguments) {
    if (!arguments.optNoLegal) {
        cout
            << endl
            << "WARNING:" << endl
            << "IMPROPER USE OF THIS SOFTWARE MAY RESULT IN DATA LOSS." << endl
            << "DO NOT REMOVE USB DEVICES WHEN USING THIS SOFTWARE." << endl
            << "DO NOT TURN OFF OR CUT THE POWER TO THE DEVICE HOSTING THE VOLUME." << endl
            << "ALWAYS HAVE A BACKUP OF THE DATA ON THE VOLUME YOU OPERATE ON." << endl
            << endl
            << "DISCLAIMER:" << endl
            << "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND," << endl
            << "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF" << endl
            << "MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT." << endl
            << "IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM," << endl
            << "DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR" << endl
            << "OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE" << endl
            << "OR THE USE OR OTHER DEALINGS IN THE SOFTWARE." << endl
            ;
    }
}

export void PrintErrorOnly(const Arguments& arguments, const string& errorMessage, const string& errorDetailsMessage = "") {
    PrintError(arguments, errorMessage, errorDetailsMessage);
    PrintWarningAndDisclaimer(arguments);
}

export void PrintErrorAndUsage(const Arguments& arguments, const string& errorMessage, const string& errorDetailsMessage = "") {
    PrintError(arguments, errorMessage, errorDetailsMessage);
    PrintUsage(arguments);
    PrintWarningAndDisclaimer(arguments);
}
