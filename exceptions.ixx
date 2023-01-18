// VolumeIDExFAT - command-line tool for changing serial number of exFAT volume
// Copyright (C) 2023 Jakub Kopec-Kawka (MIT License)
//
// Please refer to LICENSE.txt for details about distribution and modification

module;

#include <stdexcept>

export module exceptions;

class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& message) : _message(message), std::runtime_error(message) {}
private:
    const std::string _message; // copy of std::string to prevent removing its data from memory if temporary
};

export class ProgramArgumentsError : public RuntimeError {
public:
    explicit ProgramArgumentsError(const std::string& message) : RuntimeError(message) {}
};

export class ExFATModificationError : public RuntimeError {
public:
    explicit ExFATModificationError(const std::string& message, const std::string& details) : RuntimeError(message), _details(details) {}
    explicit ExFATModificationError(const std::string& message) : RuntimeError(message), _details("") {}

    virtual const char* details() const { return _details.c_str(); }

private:
    const std::string _details; // copy of std::string to prevent removing its data from memory if temporary
};
