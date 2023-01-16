// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

#pragma once

#include <string>

#include "arguments.h"

void PrintBanner(const Arguments& arguments);

void PrintErrorOnly(const Arguments& arguments, const std::string& errorMessage, const std::string& errorDetailsMessage = "");

void PrintErrorAndUsage(const Arguments& arguments, const std::string& errorMessage, const std::string& errorDetailsMessage = "");
