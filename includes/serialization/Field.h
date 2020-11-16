#ifndef _FIELD_H_
#define _FIELD_H_
#include <vector>
#include <string>

class Field {
public:
	Field(std::string field_name, size_t size);
	Field(const Field &field);
	Field(Field &&field);
	~Field();
	//void fill(unsigned char *&&d, size_t len);
	void fill(const unsigned char *d, size_t len);

	size_t size() const;

	Field& operator=(const Field &field);
	Field& operator=(Field &&field);

	std::string name() const;
	template<typename T>
	std::vector<T> to() const {
		size_t length_result_vector = 0;
		if (_size > sizeof(T)) {
			length_result_vector = (_size - 1) / sizeof(T) + 1;
		}
		else {
			length_result_vector = 1;
		}
		std::vector<T> result(length_result_vector);
		size_t data_field_index = 0;
		
		for (size_t i = 0; i < length_result_vector; i++) {
			//TODO ни чего не трогал, возникла ошибка 
			//result[i] = 0;
			unsigned char *byte_ptr = reinterpret_cast<unsigned char*> (&result[i]);
			for (size_t j = 0; (j < sizeof(T)) && (data_field_index < _size); j++) {
				byte_ptr[j] = _data[data_field_index];
				data_field_index++;
			}
		}
		return result;
	}	
	const unsigned char* rawData() const;

private:
	unsigned char *_data;
	size_t _size;
	std::string _name;

};

#endif