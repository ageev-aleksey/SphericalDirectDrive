//#include "BinaryBuffer.h"
//
//
//
//
//
//BinaryBuffer::BinaryBuffer() : next_position(0)
//{}
//
//void BinaryBuffer::append(byte_t byte) {
//	data.push_back(byte);
//}
//
//Buffer::byte_t BinaryBuffer::next_byte() {
//	if (has_next()) {
//		return data[next_position++];		
//	}
//	throw std::out_of_range("end of buffer was be reached");
//}
//
//unsigned char BinaryBuffer::before_byte() {
//	if (has_before()) {
//		size_t p = next_position - 2;
//		next_position--;
//		return data[p];
//	}
//	throw std::out_of_range("begin of buffer was be reached");
//}
//
//bool BinaryBuffer::has_next() {
//	return next_position < data.size();
//}
//
//bool BinaryBuffer::has_before() {
//	return next_position >= 2;
//}
//
//std::vector<Buffer::byte_t> BinaryBuffer::get_by_range(int start, int stop) {
//	if ((start >= 0) && (start < data.size())) {
//		if ((stop > start) && (stop < data.size())) {
//			std::vector<Buffer::byte_t> res(stop-start);
//			for (size_t i = 0; i < res.size(); i++) {
//				res[i] = data[start + i];
//			}
//			return res;
//		}
//	}
//	throw std::out_of_range("given range don't match with size buffer");
//}