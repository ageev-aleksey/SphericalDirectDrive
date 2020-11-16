#include "Field.h"
#include "Message.h"
#include <vector>
#include "gtest/gtest.h"

TEST(TestField, CreateField) {
	Field f("TestField", 1);
	char v = 5;
	f.fill(reinterpret_cast<unsigned char*>(&v), 1);
	ASSERT_EQ(f.to<char>()[0], 5);
	std::vector<Field> d;
	d.push_back(f);
}

TEST(TestField, conver_to_int) {
	Field f("test", 1);
	unsigned char *value = new unsigned char[1];
	value[0] = 118;
	f.fill(value, 1);
	std::vector<int> v = f.to<int>();
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0], 118);
}


TEST(TestField, conver_to_int_two) {
	Field f("test", 2);
	unsigned char *value = new unsigned char[2];
	value[0] = 255;
	value[1] = 255;
	f.fill(value, 2);
	std::vector<int> v = f.to<int>();
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0], 65535);
}

TEST(TestField, conver_to_short_two) {
	Field f("test", 2);
	unsigned char *value = new unsigned char[2];
	value[0] = 255;
	value[1] = 255;
	f.fill(value, 2);
	std::vector<unsigned short> v = f.to<unsigned short>();
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0], 65535);
}

TEST(TestField, conver_to_short_three) {
	Field f("test", 3);
	unsigned char *value = new unsigned char[3];
	value[0] = 255;
	value[1] = 255;
	value[2] = 255;
	f.fill(value, 3);
	std::vector<unsigned short> v = f.to<unsigned short>();
	ASSERT_EQ(v.size(), 2);
	ASSERT_EQ(v[0], 65535);
	ASSERT_EQ(v[1], 255);
}

TEST(TestMessage, CreateMessage) {
	Message msg;
	msg.addField(Field("test_field", 1));
	msg.get("test_field")->fill(new unsigned char[1]{ 255 }, 1);
	std::vector<int> v = msg.get("test_field")->to<int>();
	ASSERT_EQ(v.size(), 1);
	ASSERT_EQ(v[0], 255);
}

TEST(Endianness, WhoEndianness)
{
	int const Value = 0x1234abcd;
	unsigned char const *pByte = (unsigned char const *)&Value;

	for (size_t i = 0; i < sizeof(Value); ++i)
	{
		std::cout << std::hex << static_cast<int>(pByte[i]);
	}

	std::cout << std::endl;
}