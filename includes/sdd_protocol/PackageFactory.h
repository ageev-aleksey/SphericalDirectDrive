#ifndef SPPU_SERIAL_PACKAGEFACTORY_H
#define SPPU_SERIAL_PACKAGEFACTORY_H
#include "Package.h"
#include <memory>
class PackageFactory {
public:
    PackageFactory(Message::Endianness endianness = Message::Endianness::LITTLE_ENDIAN);
    std::shared_ptr<Package> createPackage(const std::vector<unsigned char> &bin_buffer);
private:
	Message::Endianness _endianness;
};



#endif //SPPU_SERIAL_PACKAGEFACTORY_H
