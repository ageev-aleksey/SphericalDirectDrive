#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include <vector>
#include <unordered_map>
#include "field.h"

//TODO ��� ������������� �� ����������� �������� ���������� ����������� ��������� ��� ����� �������
class Message {
public:
	enum Endianness : char {
		BIG_ENDIAN,
		LITTLE_ENDIAN
	};
	Message(Endianness endianness = LITTLE_ENDIAN);
	void addField(Field field);
	void addField(std::string field_name, size_t num_bytes);
	std::vector<Field>::iterator get(std::string field_name);
	std::vector<Field>::const_iterator get(std::string field_name) const;
	std::vector<Field>::iterator begin();
	std::vector<Field>::iterator end();
	std::vector<Field>::const_iterator cbegin() const;
	std::vector<Field>::const_iterator cend() const;
	std::vector<unsigned char> toBinary() const;
	void fromBinary(std::vector<unsigned char> bytes, size_t start = 0);
	size_t size();
private:
	std::vector<Field> fields;
	std::unordered_map<std::string, size_t> index;
	Endianness _endianness;
    size_t size_message;
	
};
#endif