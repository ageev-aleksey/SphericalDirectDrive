#include "Package.h"
#include "PackageError.h"
#include <exception>
#include <string>

Package::Package(std::string pname, unsigned char id, Message::Endianness end) : bworker(end) {
	bworker.addField("CC", 1);
	bworker.get("CC")->fill(&id, 1);
	package_name = std::move(pname);
}

int Package::id() const {
	return bworker.get("CC")->to<int>()[0];
}


class FieldsIterator : public std::iterator <std::input_iterator_tag,
	char,
	char,
	const char*,
	char> {
public:
	explicit FieldsIterator(std::vector<Field>::const_iterator itr_begin, std::vector<Field>::const_iterator itr_end) {
		_current = itr_begin;
		_end = itr_end;
		position_in_current_bytes = 0;

	}
	FieldsIterator(const FieldsIterator &obj) {
		*this = obj;
	}

	FieldsIterator& operator=(const FieldsIterator& obj) {
		_current = obj._current;
		_end = obj._end;
		position_in_current_bytes = obj.position_in_current_bytes;
		return *this;
	}

	bool operator==(const FieldsIterator& obj) {
		return ((_current == obj._current) && (position_in_current_bytes == obj.position_in_current_bytes));
	}

	bool operator!=(const FieldsIterator& obj) {
		return !(*this == obj);
	}

	unsigned char operator*() {
		if (_current != _end) {
			const unsigned char *bptr= _current->rawData();
			return bptr[position_in_current_bytes];
		}
		throw std::out_of_range("error dereferencing of ending iterator");
	}
	FieldsIterator& operator++() {
		if (_current != _end) {
			if ((position_in_current_bytes + 1) == _current->size()) {
				position_in_current_bytes = 0;
				++_current;
			}
			else {
				++position_in_current_bytes;
			}
			return *this;
		}
		throw std::out_of_range("error incrementing of ending iterator");
	}
	FieldsIterator& operator--() {
		if (position_in_current_bytes > 0) {
			--position_in_current_bytes;
		}
		else {
			--_current;
			position_in_current_bytes = _current->size()-1;
		}
		return *this;
	}
	FieldsIterator operator++(int) {
		FieldsIterator old = *this;
		++*this;
		return old;
	}
	FieldsIterator operator--(int) {
		FieldsIterator old = *this;
		--*this;
		return old;
	}
private:
private:
	std::vector<Field>::const_iterator _current;
	std::vector<Field>::const_iterator _end;
	size_t position_in_current_bytes;
};


unsigned char Package::hash() const {
	FieldsIterator start(bworker.cbegin(), bworker.cend());
	FieldsIterator stop(bworker.cend(), bworker.cend());
	return hash<FieldsIterator>(start, stop);
}

std::vector<Field>::iterator Package::findField(std::string field_name) {
	auto itr = bworker.get(field_name);
	if (itr == bworker.cend()) {
		throw std::invalid_argument(std::string("field with name (") + field_name +std::string(") not found"));
	}
	return itr;
}

std::vector<Field>::const_iterator Package::findField(std::string field_name) const {
	return const_cast<Package*>(this)->findField(field_name);
}



Package::~Package() {}


std::string Package::name() const {
	return package_name;
}

/*
static char Package::calc_controll_sum(char *bytes, size_t size) {
	char sum = 0;
	for (size_t i = 0; i < size; i++) {
		sum += bytes[i];
	}
	return sum;
}*/


//TODO ВОЗМОЖНО обобщенные пакет не может иметь возможности заполнять поля!
/*void Package::set(std::string field_name, std::vector<int> value) {
	auto itr = bworker.get(field_name);
	if (itr == bworker.cend()) {
		throw std::invalid_argument(std::string("field with name (") + field_name + std::string(") not found"));
	}
	if (itr->size() != value.size()) {
		throw std::invalid_argument(std::string("field have saze a ") +
			std::to_string(itr->size()) + std::string(". youre value has size ") +
			std::to_string(value.size()));
	}
}*/


std::vector<unsigned char> Package::toBinary() const{ 
	std::vector<unsigned char> bin = bworker.toBinary();
	bin.push_back(hash());
	return bin;
}

void Package::fromBinary(const std::vector<byte_t> &bin_buff) {
    if((bin_buff.size() == (bworker.size() + 1)) && (bin_buff[0] == id())) {
        if(*(--bin_buff.cend()) == hash(bin_buff.cbegin(), (--bin_buff.cend()))) {
            bworker.fromBinary(bin_buff, 1); //TODO копирование
        } else {
            throw PackageControlSumError("control sum in package don't equal with calculated checksum");
        }
    } else {
        throw PackageParseError(std::string("length of buffer dont't equal with binary representation package ") + name() +
                                std::string(". Thsi package have length a " ) + std::to_string(bworker.size() + 1) +
                                std::string(". This binary buffer have langth a ") +std::to_string(bin_buff.size()));
    }
}

size_t Package::size() {
    return bworker.size() + 1;
}
