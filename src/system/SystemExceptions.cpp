//
// Created by Aleksey on 17.03.2020.
//

#include "system/SystemExceptions.h"

GlobalStorageException::GlobalStorageException(const std::string &what) : std::runtime_error(what)
{}

ObjectInStorageExists::ObjectInStorageExists(const std::string &what) : GlobalStorageException(what)
{}


NotInitializedGlobalStorage::NotInitializedGlobalStorage(const std::string &what) :
        GlobalStorageException(what)
{}


NotFoundException::NotFoundException(const std::string &what) :
        GlobalStorageException(what)
{}