//#include "BinaryBuffer.h"
#include "Buffer.h"
#include <string>
#include "gtest/gtest.h"
#include <iostream>

/*TEST(TestBinaryBuffer, TestIterrations) {
	Buffer *buff = new BinaryBuffer();
	Buffer::byte_t d[3] = {0, 1, 2};
	buff->append(d[0]);
	buff->append(d[1]);
	buff->append(d[2]);
	int i = 0;
	for(; (i < 3) && buff->has_next(); i++) {
		Buffer::byte_t b = buff->next_byte();
		ASSERT_EQ(b, d[i]);
	}
	ASSERT_EQ(i, 3);
	ASSERT_THROW(buff->next_byte(), std::out_of_range);
	i -= 2;
	for (; (i >= 0) && buff->has_before(); i--) {
		Buffer::byte_t b = buff->before_byte();
		ASSERT_EQ(b, d[i]);
	}
	ASSERT_EQ(i, -1);
	ASSERT_THROW(buff->before_byte(), std::out_of_range);

}*/
/*

TEST(TestBuffer, TestInOneThread) {
	Buffer buf;
	for (int i = 0; i <= 10; i++) {
		buf.push_back(std::string("value: " + std::to_string(i)));
	}

	for (int i = 0; i <= 10; i++) {
		ASSERT_EQ(buf.pop_front(), std::string("value: " + std::to_string(i)));
	}

}*/