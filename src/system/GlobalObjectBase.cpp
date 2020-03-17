//
// Created by Aleksey on 17.03.2020.
//

#include "system/GlobalObjectBase.h"
#include <typeinfo>

GlobalObjectBase::GlobalObjectBase() {
    hash_code_object = 0;
}

GlobalObjectBase::~GlobalObjectBase() {}

size_t GlobalObjectBase::getHashCode(){
    return  hash_code_object;
}

void GlobalObjectBase::reCalcHashCode() {
    hash_code_object = typeid(*this).hash_code();
}

