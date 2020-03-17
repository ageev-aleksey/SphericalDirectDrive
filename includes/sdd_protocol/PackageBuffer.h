//
// Created by Aleksey on 15.03.2020.
//

#ifndef SPPU_SERIAL_PACKAGEBUFFER_H
#define SPPU_SERIAL_PACKAGEBUFFER_H

#include "State.h"
#include <boost/circular_buffer.hpp>
#include <ostream>

//TODO Класс реализован строкго для одного типа пакетов - State. /
// необходимо переделать, чтобы класс работал для любых пакетов. /
// для этого необходимо реализовать патер фабрики
class PackageBuffer {
public:
    PackageBuffer(size_t size);
    void addBytes(const std::vector<unsigned char> &bytes);
    State formPackage();
    void flush();
	friend std::ostream &operator<<(std::ostream &stream, const PackageBuffer &buff);
//private:
    boost::circular_buffer<unsigned char> buffer;
   };

std::ostream &operator<<(std::ostream &stream, const PackageBuffer &buff);

#endif //SPPU_SERIAL_PACKAGEBUFFER_H
