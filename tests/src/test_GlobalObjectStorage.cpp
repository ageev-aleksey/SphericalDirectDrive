//
// Created by Aleksey on 17.03.2020.
//

#include "system/GlobalObjectBase.h"
#include  "system/GlobalObjectStorage.h"
#include "system/SystemExceptions.h"
#include "gtest/gtest.h"

class TestGlobalObject : public GlobalObjectBase {
public:
    TestGlobalObject() {
        v =  100;
    }
    int get() {
        return v;
    }
private:
    int v;
};

TEST(TestGlobalObject, UsingGlobalStorageWithoutInitialization) {
	
	ASSERT_THROW(GlobalStorage::instance(), NotInitializedGlobalStorage);
}


TEST(TestGlobalObject, GetDontExistsObject) {
	GlobalStorage::init();
	ASSERT_EQ(GlobalStorage::instance().get<TestGlobalObject>(), nullptr);
}


TEST(TestGlobalObject, AddAndGetObject) {
    GlobalStorage::init();
	GlobalStorage::instance().add<TestGlobalObject>();
	TestGlobalObject *ptr = GlobalStorage::instance().get<TestGlobalObject>();
	ASSERT_TRUE(ptr != nullptr);
    ASSERT_EQ(ptr->get(), 100);
}




