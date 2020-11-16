//
// Created by Aleksey on 17.03.2020.
//

#ifndef SPPU_SERIAL_SYSTEMEXCEPTIONS_H
#define SPPU_SERIAL_SYSTEMEXCEPTIONS_H

#include <stdexcept>

class GlobalStorageException : public std::runtime_error {
public:
    GlobalStorageException(const std::string &what);
};

class ObjectInStorageExists : public GlobalStorageException {
public:
    ObjectInStorageExists (const std::string &what);
};

class NotInitializedGlobalStorage : public GlobalStorageException {
public:
    NotInitializedGlobalStorage(const std::string &what);
};

class NotFoundException : public GlobalStorageException {
public:
    NotFoundException(const std::string &what);
};

#endif //SPPU_SERIAL_SYSTEMEXCEPTIONS_H
