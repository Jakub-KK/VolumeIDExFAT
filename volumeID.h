// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

#pragma once

#include <string>
#include <vector>

int ExecuteProgram(const std::string& executableName, const std::vector<std::string>& args);
