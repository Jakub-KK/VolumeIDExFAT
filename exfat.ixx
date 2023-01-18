// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

module;

#include <assert.h>
#include <format>
#include <memory>
#include <string>

#include "win.h"

export module exfat;

import array_length;
import error;
import exceptions;
import hex;

using std::string;

// offset in main/backup boot sector subregion with data not participating in calculating checksum, nor comparing main and boot sectors
constexpr uint32_t EXFAT_IGNOREBYTE_VOLUMEFLAGS_L = 106;
// offset in main/backup boot sector subregion with data not participating in calculating checksum, nor comparing main and boot sectors
constexpr uint32_t EXFAT_IGNOREBYTE_VOLUMEFLAGS_H = 107;
// offset in main/backup boot sector subregion with data not participating in calculating checksum, nor comparing main and boot sectors
constexpr uint32_t EXFAT_IGNOREBYTE_PERCENTINUSE = 112;

/// <summary>
/// source: https://learn.microsoft.com/en-us/windows/win32/fileio/exfat-specification
/// (3.4 Main and Backup Boot Checksum Sub-regions)
/// </summary>
uint32_t ExFATBootChecksum(const uint8_t* const sectors, const uint32_t byteCount) {
    uint32_t checksum{ 0 };
    for (uint32_t idx{ 0 }; idx < byteCount; idx++) {
        if ((idx == EXFAT_IGNOREBYTE_VOLUMEFLAGS_L) || (idx == EXFAT_IGNOREBYTE_VOLUMEFLAGS_H) || (idx == EXFAT_IGNOREBYTE_PERCENTINUSE))
            continue;
        checksum = ((checksum & 1) ? 0x80000000 : 0) + (checksum >> 1) + (uint32_t)sectors[idx];
    }
    return checksum;
}

/// <summary>Fill byte array with checksum. Count must be power of 2.</summary>
void ExFATCreateChecksumSector(uint8_t* const sector, uint32_t count, const uint32_t checksum) {
    assert((count & (count - 1)) == 0 && "\"count\" must be power of 2");
    *reinterpret_cast<uint32_t*>(sector) = checksum;
    uint32_t currentCount{ sizeof(uint32_t) };
    count /= currentCount;
    while (count > 1) {
        memcpy(sector + currentCount, sector, currentCount);
        currentCount <<= 1;
        count >>= 1;
    }
}

export void ExFATChangeVolumeSerialNumber(const string driveDeviceName, const uint32_t volumeSerialNumber) {
    using std::unique_ptr;
    using std::remove_pointer;
    using std::format;
    using namespace std::string_literals;

    // CreateFile is Windows API function that is being used to open a raw volume and read/write filesystem boot records
    // Documentation: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
    // Latest change to this doc (2022-09-23): https://github.com/MicrosoftDocs/sdk-api/commit/080b6c05e7f35ee5427e1ef481c22fea6b0f4aa2
    // GitHub: https://github.com/MicrosoftDocs/sdk-api/blob/docs/sdk-api-src/content/fileapi/nf-fileapi-createfilea.md
    //
    // Restrictions and discrepancies:
    // - DOCS: direct disk/volume access is restricted and requires administrative privileges (WinXP/WinServer2003 and older not restricted)
    // - TESTS: on Windows 10 show that admin is NOT required for USB removable[*] volume manipulation (reads and writes), no mention of this in docs
    // - [*] "removable volume" as in "shows 'eject ...' and drive letter in tray icon 'safely remove hardware and eject media'"
    //
    // Windows editions tested: Windows 10 Pro 22H2 19045.2251 and 21H2 19044.2364
    //
    // CONCLUSION: on some(/all?) Windows editions there's no need for admin when operating on volume filesystem, docs are wrong/misleading

    // RAII for HANDLE (source: https://stackoverflow.com/questions/24611215/one-liner-for-raii-on-non-pointer)
    const unique_ptr<remove_pointer<HANDLE>::type, decltype(&CloseHandle)> hDrive(
        CreateFile( // open the volume referenced by drive letter
            driveDeviceName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        ),
        &CloseHandle
    );

    if (hDrive.get() == INVALID_HANDLE_VALUE) {
        // this could happen when process is not elevated to administrator and tries to open non-removable disk volume
        throw ExFATModificationError("Unable to open the volume. Run program as administrator for fixed disk exFAT volumes."s, GetLastErrorString("CreateFile"));
    }

    DWORD dwTmpBytesReturned{ 0 };

    // lock the volume
    if (!DeviceIoControl(hDrive.get(), FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &dwTmpBytesReturned, nullptr)) {
        // this could happen when volume is currently in use, for example while copying large file to usb removable volume
        throw ExFATModificationError("Unable to lock the volume. Make sure the volume is not in use."s, GetLastErrorString("DeviceIoControl"));
    }

    // TODO: dismount using FSCTL_DISMOUNT_VOLUME? it may be needed for fixed disk volumes

    constexpr uint32_t BYTES_IN_SECTOR = 512;
    constexpr uint32_t SECTORS_IN_SUBREGION_BOOT = 1;
    constexpr uint32_t SECTORS_IN_SUBREGION_BOOTEXTENDED = 8;
    constexpr uint32_t SECTORS_IN_SUBREGION_OEMPARAMETERS = 1;
    constexpr uint32_t SECTORS_IN_SUBREGION_RESERVED = 1;
    constexpr uint32_t SECTORS_IN_SUBREGION_CHECKSUM = 1;
    constexpr uint32_t SECTORS_IN_REGION_FOR_CHEKSUM_CALCULATION = SECTORS_IN_SUBREGION_BOOT + SECTORS_IN_SUBREGION_BOOTEXTENDED + SECTORS_IN_SUBREGION_OEMPARAMETERS + SECTORS_IN_SUBREGION_RESERVED;
    constexpr uint32_t BYTES_IN_REGION_FOR_CHEKSUM_CALCULATION = SECTORS_IN_REGION_FOR_CHEKSUM_CALCULATION * BYTES_IN_SECTOR;
    constexpr uint32_t SECTORS_IN_REGION = SECTORS_IN_REGION_FOR_CHEKSUM_CALCULATION + SECTORS_IN_SUBREGION_CHECKSUM;
    constexpr uint32_t BYTES_IN_REGION = SECTORS_IN_REGION * BYTES_IN_SECTOR;
    constexpr uint32_t BYTES_IN_MAIN_AND_BACKUP_REGION = 2 * BYTES_IN_REGION;

    // read volume main boot region (and backup boot region which should be the same, placed directly after main boot region):
    // - boot sector (aka partition boot sector, contains volume serial number)
    // - 10 more sectors (boot extended, oem paramaters, reserved sectors, used only for calculating checksum)
    // - 1 cheksum sector (32-bit checksum is repeated in whole sector)
    uint8_t pSectorBytes[BYTES_IN_MAIN_AND_BACKUP_REGION]{ { 0 } };
    if (!ReadFile(hDrive.get(), pSectorBytes, BYTES_IN_MAIN_AND_BACKUP_REGION, &dwTmpBytesReturned, nullptr) || dwTmpBytesReturned != BYTES_IN_MAIN_AND_BACKUP_REGION) {
        throw ExFATModificationError("Unable to read volume boot sector."s, GetLastErrorString("ReadFile"));
    }

    // source: rufus GitHub src/drive.c GetFsName function
    // The beginning of a superblock for FAT/exFAT/NTFS/ReFS is pretty much always the same:
    // There are 3 bytes potentially used for a jump instruction, and then are 8 bytes of
    // OEM Name which, even if *not* technically correct, we are going to assume hold an
    // immutable file system magic for exFAT/NTFS/ReFS [...].
    //
    // verify exFAT filesystem
    constexpr uint8_t magic_exfat[]{ 'E', 'X', 'F', 'A', 'T', ' ', ' ', ' ' };
    assert(array_length(magic_exfat) == 8 && "Invalid length of exFAT magic OEM name, expected 8 bytes");
    constexpr uint32_t SUBREGION_BOOT_OFFSET_OEMNAME = 3;
    if (memcmp(pSectorBytes + SUBREGION_BOOT_OFFSET_OEMNAME, magic_exfat, array_length(magic_exfat)) != 0) {
        // HexDump(std::cout, pSectorBytes, BYTES_IN_SECTOR);
        throw ExFATModificationError("Volume filesystem is not exFAT."s);
    }

    // volume integrity verification: compare main and backup boot regions...
    // ... but ignoring bytes at offset 106, 107 (VolumeFlags) and 112 (PercentInUse), the same that are ignored in checksum calculations,
    // because in backup boot region those are considered "stale" according to spec
    bool regionsEqualish{ true };
    for (uint32_t idx{ 0 }; idx < BYTES_IN_REGION; idx++) {
        if ((idx == EXFAT_IGNOREBYTE_VOLUMEFLAGS_L) || (idx == EXFAT_IGNOREBYTE_VOLUMEFLAGS_H) || (idx == EXFAT_IGNOREBYTE_PERCENTINUSE))
            continue;
        if (*(pSectorBytes + idx) != *(pSectorBytes + BYTES_IN_REGION + idx)) {
            regionsEqualish = false;
            break;
        }
    }
    if (!regionsEqualish) {
        throw ExFATModificationError("exFAT filesystem is corrupted (main and boot region are different), aborting. Run \"chkdsk /f\" to fix."s);
    }
    // volume integrity verification: main boot region checksum
    // calculate current checksum
    const uint32_t checksumBeforeChange{ ExFATBootChecksum(pSectorBytes, BYTES_IN_REGION_FOR_CHEKSUM_CALCULATION) };
    // verify checksum subregion of main boot region
    uint8_t pChecksumSectorBeforeChange[BYTES_IN_SECTOR]{ { 0 } };
    ExFATCreateChecksumSector(pChecksumSectorBeforeChange, BYTES_IN_SECTOR, checksumBeforeChange);
    if (memcmp(pChecksumSectorBeforeChange, pSectorBytes + BYTES_IN_REGION_FOR_CHEKSUM_CALCULATION, BYTES_IN_SECTOR) != 0) {
        throw ExFATModificationError("exFAT filesystem is corrupted (checksum check failed), aborting. Run \"chkdsk /f\" to fix."s);
    }

    constexpr uint32_t SUBREGION_BOOT_OFFSET_VOLUMEID = 100;

    // check if volume serial number is the same, if it is then abort
    uint32_t* const volSerialNumerPtr{ reinterpret_cast<uint32_t*>(pSectorBytes + SUBREGION_BOOT_OFFSET_VOLUMEID) };
    if (*volSerialNumerPtr == volumeSerialNumber) {
        throw ExFATModificationError(format("Volume serial number is already {:8x}, nothing to do, aborting.", volumeSerialNumber));
    }

    // change volume ID (serial number)
    *volSerialNumerPtr = volumeSerialNumber;

    // calculate new checksum
    const uint32_t checksum{ ExFATBootChecksum(pSectorBytes, BYTES_IN_REGION_FOR_CHEKSUM_CALCULATION) };
    // create new checksum sector (in place)
    ExFATCreateChecksumSector(pSectorBytes + BYTES_IN_REGION_FOR_CHEKSUM_CALCULATION, BYTES_IN_SECTOR, checksum);
    // HexDump(std::cout, pSectorBytes + BYTES_IN_REGION_FOR_CHEKSUM_CALCULATION, BYTES_IN_SECTOR);

    // duplicate whole VBR main region to backup region
    memcpy(pSectorBytes + BYTES_IN_REGION, pSectorBytes, BYTES_IN_REGION);

    // seek to start of volume boot record before writing changes
    if (SetFilePointer(hDrive.get(), 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        throw ExFATModificationError("Unable to move file pointer to beginning of the device."s, GetLastErrorString("SetFilePointer"));
    }
    // write modified volume boot record main and backup region in one operation
    if (!WriteFile(hDrive.get(), pSectorBytes, BYTES_IN_MAIN_AND_BACKUP_REGION, &dwTmpBytesReturned, nullptr) || dwTmpBytesReturned != BYTES_IN_MAIN_AND_BACKUP_REGION) {
        throw ExFATModificationError("Write error. Volume may be corrupted! Run \"chkdsk /f\" to fix."s, GetLastErrorString("WriteFile"));
    }
}
