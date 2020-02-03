#include "Light.h"

Light::Light() : Package("light", 29, Message::Endianness::LITTLE_ENDIAN)
{
    bworker.addField("MW", 2);
}

Light::Light(std::vector<Package::byte_t> &buffer) : Package("light", 29, Message::Endianness::LITTLE_ENDIAN)
{
    bworker.addField("MW", 2);
    bworker.fromBinary(buffer);
}