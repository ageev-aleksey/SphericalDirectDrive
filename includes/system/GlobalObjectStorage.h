//
// Created by Aleksey on 17.03.2020.
//

#ifndef SPPU_SERIAL_GLOBALOGBJECTSTORAGE_H
#define SPPU_SERIAL_GLOBALOGBJECTSTORAGE_H
#include "system/GlobalObjectBase.h"
#include <memory>
#include <vector>


//Don't thread-save
class GlobalStorage {
private:
    void add_object(GlobalObjectBase *obj);
    GlobalObjectBase* find_object(size_t hash);
public:
	GlobalStorage();
    static GlobalStorage& instance();
    std::unique_ptr<GlobalStorage> static init(std::unique_ptr<GlobalStorage> storage = nullptr);
    template<class ObjectType>
    void add() {
        GlobalObjectBase* tmp = new ObjectType();
        tmp->reCalcHashCode();
        add_object(tmp);
    }
    template<class ObjectType>
    ObjectType* get() {
        return static_cast<ObjectType*> (find_object(typeid(ObjectType).hash_code()));
    }
private:
    static std::unique_ptr<GlobalStorage> ptr;
    std::vector<GlobalObjectBase*> storage;
};

#endif //SPPU_SERIAL_GLOBALOGBJECTSTORAGE_H
