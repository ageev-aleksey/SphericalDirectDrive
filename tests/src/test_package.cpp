#include "Package.h"
#include "PackageError.h"
#include "gtest/gtest.h"


class TestPackage : public Package {
public:
	TestPackage() : Package("test", 1, Message::Endianness::LITTLE_ENDIAN) {
		bworker.addField("field1", sizeof(char));
		bworker.addField("field2", sizeof(short));
		bworker.addField("field3", sizeof(int));
		bworker.addField("field4", sizeof(char));
	}

	//Getters
	char field1() {
		return findField("field1")->to<char>()[0];
	}
	short field2() {
		return findField("field2")->to<short>()[0];
	}
	int field3() {
		return findField("field3")->to<int>()[0];
	}
	int field4() {
		return findField("field4")->to<int>()[0];
	}

	//Setters
	void field1(char value) {
		//TODO ОШИБКА, переноситя автоматическая переменная!
		findField("field1")->fill(reinterpret_cast<unsigned char*>(&value), sizeof(char));
	}
	void field2(short value) {
		findField("field2")->fill(reinterpret_cast<unsigned char*>(&value), sizeof(short));
	}
	void field3(int value) {
		findField("field3")->fill(reinterpret_cast<unsigned char*>(&value), sizeof(int));
	}
	void field4(int value) {
		if ((value > -128) && (value < 128))
			findField("field4")->fill(reinterpret_cast<unsigned char*>(&value), 1);
		else
			throw std::invalid_argument("value this field must be in range from -127 to 127");
	}


};


TEST(PackageTest, TestSubClass) {
	TestPackage p;
	p.field1(1);
	p.field2(154);
	p.field3(12354);
	p.field4(41);
	
	ASSERT_EQ(p.field1(), 1);
	ASSERT_EQ(p.field2(), 154);
	ASSERT_EQ(p.field3(), 12354);
	ASSERT_EQ(p.field4(), 41);
	ASSERT_EQ(p.hash(), 55);//p.hash := (field1+field2+field3+field4)mod(255)
	std::vector<unsigned char> bin{1, 1, 154, 0, 66, 48, 0, 0, 41, 55 };
	//ASSERT_EQ(p.toBinary(), bin);
	std::vector<unsigned char> res = p.toBinary();
    /*std::cout << "bin : res\n";
    for (size_t i = 0; i < bin.size(); i++) {
        std::cout  << (unsigned int)bin[i] << " : "  << (unsigned int)res[i] << std::endl;
    }
    std::cout << std::endl;*/
     ASSERT_EQ(res, bin);
}

TEST(TestPackage, TestParseBinaryBuffer) {
    TestPackage p;
    std::vector<unsigned char> bin{1, 1, 154, 0, 66, 48, 0, 0, 41, 55 };
    p.fromBinary(bin);
    ASSERT_EQ(p.field1(), 1);
    ASSERT_EQ(p.field2(), 154);
    ASSERT_EQ(p.field3(), 12354);
    ASSERT_EQ(p.field4(), 41);
    ASSERT_EQ(p.hash(), 55);
}

TEST(TestPaclage, TestRaiseExceptionControlSumError) {
    TestPackage p;
    std::vector<unsigned char> bin{1, 1, 154, 0, 66, 48, 0, 0, 41, 56 };
    ASSERT_THROW(p.fromBinary(bin), PackageControlSumError);
}

TEST(TestPaclage, TestRaiseExceptionBinaryBufferLengthError) {
    TestPackage p;
    std::vector<unsigned char> bin{1, 1, 154, 0, 66, 48, 0, 0, 41};
    ASSERT_THROW(p.fromBinary(bin), PackageError);
}