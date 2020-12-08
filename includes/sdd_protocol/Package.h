#ifndef _PACKAGE_H_
#define _PACKAGE_H_
#include "Message.h"
#include <list>
#include <iterator>





class Package {
public:
    using byte_t = unsigned char;
	//using Itr = std::iterator<std::input_iterator_tag, byte_t, byte_t, const byte_t*, byte_t&>;
	std::string name() const;
	int id() const;
    byte_t hash() const;

    template<typename Itr>
     static byte_t hash(Itr itr_begin, Itr itr_end) {
        // return hash<Itr>(itr_begin, itr_end, std::iterator_traits<Itr>::iterator_category());
        byte_t sum = 0;
        for (Itr itr = itr_begin; itr != itr_end; itr++) {
            sum -= *itr;
        }
        return sum;
    }
	virtual ~Package();
	std::vector<unsigned char> toBinary() const;
	void fromBinary(const std::vector<byte_t > &bin_buff);
	size_t size();


	//virtual std::vector<int> get(std::string field_name) const;
	//virtual void set(std::string field_name, std::vector<int> value);
protected:
	std::vector<Field>::iterator findField(std::string name);
	std::vector<Field>::const_iterator findField(std::string name) const;
	Package(std::string pname, unsigned char id, Message::Endianness end);

	//static char calc_controll_sum(char *bytes, size_t size);
	Message bworker;//TODO реализовать кеширование
	std::string package_name;
private:
	/*template<typename Itr>
	static byte_t hash(Itr itr_begin, Itr itr_end, std::input_iterator_tag) {
		byte_t sum = 0;
		for (Itr itr = itr_begin; itr != itr_end; itr++) {
			sum -= *itr;
		}
		return sum;
	}*/

};
#endif