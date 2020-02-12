#include "Message.h"


Message::Message(Endianness endianness)
{
	_endianness = endianness;
	size_message = 0;
}

Message::~Message() {}

void Message::addField(Field field) {
	std::string fname = field.name();
	auto itr = index.find(fname);
	if (itr != index.end()) {
		throw std::invalid_argument(std::string("field with this (") +
			fname + std::string(") exists"));
	}
	index[fname] = fields.size();
	fields.push_back(field);
	size_message += field.size();
}

void Message::addField(std::string field_name, size_t num_bytes)
{
	addField(Field(field_name, num_bytes));
}


std::vector<Field>::iterator Message::get(std::string field_name)
{
	auto itr =  index.find(field_name);
	if (itr == index.end()) {
		return fields.end();
	}
	return fields.begin() + itr->second;
	
}
std::vector<Field>::const_iterator Message::get(std::string field_name) const {
	return const_cast<Message*>(this)->get(field_name);
}


std::vector<Field>::iterator Message::begin() {
	return fields.begin();
}

std::vector<Field>::iterator Message::end() {
	return fields.end();
}

std::vector<Field>::const_iterator Message::cbegin() const
{
	return fields.cbegin();
}

std::vector<Field>::const_iterator Message::cend() const
{
	return fields.cend();
}

std::vector<unsigned char> Message::toBinary() const {
	std::vector<unsigned char> result;
	for (auto itr = cbegin(); itr != cend(); itr++) {
		const unsigned char *data_field = itr->rawData();
		//TODO конвертирование BIG-ENDIAN, LITTLE-ENDIAN можно в этот момент
		if (_endianness == LITTLE_ENDIAN) {
			for (size_t i = 0; i < itr->size(); i++) {
				result.push_back(data_field[i]);
			}
		}
		else {
			for (size_t i = itr->size()-1; i >= 0; i++) {//≈лси вдруг размер 0 то ошибка!
				result.push_back(data_field[i]);
			}
		}
		
	}
	return result;
}

void Message::fromBinary(std::vector<unsigned char> bytes, size_t start)
{
	size_t buff_index = 0;
	auto field = fields.begin();
	for (size_t i = 0; field != fields.end(), i < start; field++, i++)
	{	
		buff_index += field->size();
	}

	for (; field != fields.end(); field++) {
		//TODO опасно. примен€етс€ move-семантика,но по интерфесу не очевидно
		unsigned char *tmp = new unsigned char[field->size()];

		if (_endianness == LITTLE_ENDIAN) {
			for (int i = 0; i < field->size(); i++) {
				tmp[i] = bytes[buff_index];
				buff_index++;
			}
		
		}
		else {
			for (int i = field->size()-1; i >= 0 ; i--) {
				tmp[i] = bytes[buff_index];
				buff_index++;
			}
		}
		field->fill(tmp, field->size());//TODO сделать перемещение массива со значением а не его копирование

		if(tmp != nullptr)
			delete[] tmp;
	}
}

size_t Message::size() {
    return size_message;
}



