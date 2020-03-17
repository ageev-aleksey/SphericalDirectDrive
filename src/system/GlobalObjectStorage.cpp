//
// Created by Aleksey on 17.03.2020.
//

#include "system/GlobalObjectStorage.h"
#include "system/SystemExceptions.h"
#include <sstream>

std::unique_ptr<GlobalStorage> GlobalStorage::ptr = nullptr;

GlobalStorage::GlobalStorage() {
}

std::unique_ptr<GlobalStorage> GlobalStorage::init(std::unique_ptr<GlobalStorage> str) {
	std::unique_ptr<GlobalStorage>  tmp = std::move(str);
	if (str == nullptr) {
		ptr = std::make_unique<GlobalStorage>();
	}
	else {
		ptr = std::move(str);
	}
    return std::move(tmp);
}

GlobalStorage& GlobalStorage::instance() {
    if(ptr == nullptr) {
        throw NotInitializedGlobalStorage("Global object don't initialized");
    }
    return *ptr;
}

GlobalObjectBase* GlobalStorage::find_object(size_t hash) {
    for(auto &el : storage) {
        if(el->getHashCode() == hash) {
            return el;
        }
    }
    //throw NotFoundException("Element in storage not found");
	return nullptr;
}

void GlobalStorage::add_object(GlobalObjectBase *obj) {
	if (find_object(obj->getHashCode()) == nullptr) {
		storage.push_back(obj);
	}
	else {
		std::stringstream msg;
		msg << "object with hash: " << obj->getHashCode()
			<< "already exists.";
		throw ObjectInStorageExists(msg.str());
	}
}

