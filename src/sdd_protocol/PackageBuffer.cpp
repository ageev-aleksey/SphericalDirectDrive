//
// Created by Aleksey on 15.03.2020.
//

#include "PackageBuffer.h"
#include "PackageError.h"

PackageBuffer::PackageBuffer(size_t size) : buffer(size) {
}

void PackageBuffer::addBytes(const std::vector<unsigned char> &bytes) {
    buffer.insert(buffer.end(), bytes.begin(), bytes.end());
}

State PackageBuffer::formPackage() {
	boost::circular_buffer<unsigned char>::iterator begin = buffer.begin();
    boost::circular_buffer<unsigned char>::iterator end = buffer.end();
    State res;
    while(begin != end) {
        if(*begin == res.id()) {
            if((end - begin) >= res.size()) {
                if(*(begin + res.size()-1) == Package::hash(begin, begin + (res.size()-2))) {
                    std::vector<unsigned char> tmp(begin, begin + res.size());
                    begin = begin + res.size();
                    return State(std::move(tmp));
                } 
            } else {
				break;
			} 
		}
        ++begin;
    }
   throw PackageError("Don't find in buffer correct segments bytes which represented State package");
}

void PackageBuffer::flush() {
    buffer.clear();
}

std::ostream &operator<<(std::ostream &stream, const PackageBuffer &buff) {
	for (const auto &el : buff.buffer) {
		stream << (int)el << "-";
	}
	return stream;
}


