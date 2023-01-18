// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

#pragma once

#include "targetver.h"

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <windows.h>

#include <winioctl.h> // for FSCTL_LOCK_VOLUME, excluded when WIN32_LEAN_AND_MEAN

#include <assert.h>
