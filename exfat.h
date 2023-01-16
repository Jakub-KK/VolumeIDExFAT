// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

#pragma once

#include <string>

void ExFATChangeVolumeSerialNumber(const std::string driveDeviceName, const std::uint32_t volumeSerialNumber);
