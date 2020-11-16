#include "Message.h"
#include "gtest/gtest.h"

TEST(MESSAGE, TestDecodeLittleEndian) {

	Message msg;
	msg.addField("first", 2);
	msg.addField("second", 2);
	msg.addField("third", 1);
	std::vector<unsigned char> buff = { 0,1, 1,0, 15 };
	msg.fromBinary(buff);
	ASSERT_EQ(msg.get("first")->to<int>()[0], 256);
	ASSERT_EQ(msg.get("second")->to<int>()[0], 1);
	ASSERT_EQ(msg.get("third")->to<int>()[0], 15);
}


TEST(MESSAGE, TestDecodeBigEndian) {

	Message msg(Message::BIG_ENDIAN);
	msg.addField("first", 2);
	msg.addField("second", 2);
	msg.addField("third", 1);
	std::vector<unsigned char> buff = { 0,1, 1,0, 15 };
	msg.fromBinary(buff);
	ASSERT_EQ(msg.get("first")->to<int>()[0], 1);
	ASSERT_EQ(msg.get("second")->to<int>()[0], 256);
	ASSERT_EQ(msg.get("third")->to<int>()[0], 15);
}