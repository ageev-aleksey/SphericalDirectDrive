
#ifndef SPPU_SERIAL_PACKAGEERROR_H
#define SPPU_SERIAL_PACKAGEERROR_H

#include <stdexcept>

class PackageError : public std::runtime_error {
public:
    PackageError(const std::string &what);
};

class PackageParseError : public PackageError {
public:
    PackageParseError(const std::string &what);
};

class PackageControlSumError : public PackageParseError {
public:
    PackageControlSumError(const std::string &what);
};


#endif //SPPU_SERIAL_PACKAGEERROR_H
