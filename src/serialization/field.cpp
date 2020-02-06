#include "Field.h"
#include <stdexcept>

Field::Field(std::string field_name, size_t size_field) {
	_data = new unsigned char[size_field];
	_size = size_field;
	_name = field_name;
	for(size_t i = 0; i < size_field; i++) {
	    _data[i] = 0;
	}
}

Field::~Field() {
	if (_data != nullptr) {
		/*if (_size > 1)
			delete[] _data;
		else if (_size == 1)
			delete _data;*/
		delete[] _data;
	}
}

/*
void Field::fill(unsigned char *&&d, size_t len) {
	if (len == _size) {
		data = d;
		d = nullptr;
		return;
	}
	throw std::length_error("length of data don't equal length this field");
}*/
void Field::fill(const unsigned char* d, size_t len) {
	if (len == _size) {
		//_data = new unsigned char[_size];
		for (int i = 0; i < _size; i++) {
			_data[i] = d[i];
		}
		return;
	}
	throw std::length_error("length of data don't equal length this field");
}

size_t Field::size() const
{
	return _size;
}

std::string Field::name() const {
	return _name;
}

const unsigned char* Field::rawData() const
{
	return _data;
}


Field::Field(const Field &field) {
	_name = field._name;
	_size = field._size;
	_data = new unsigned char[_size];
	fill(field._data, field._size);
}


Field::Field(Field &&field) {
	operator=(std::move(field));
}

Field& Field::operator=(const Field &field) {
	_name = field._name;
	_size = field._size;
	if (field._data != nullptr) {
		fill(field._data, field._size);
	}
	return *this;
}

Field& Field::operator=(Field &&field) {
	_name = field._name;
	_size = field._size;
	_data = field._data;
	field._data = nullptr;
	return *this;
}