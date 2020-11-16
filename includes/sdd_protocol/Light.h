//
// Created by Aleksey on 03.02.2020.
//

#ifndef SPPU_SERIAL_LIGHT_H
#define SPPU_SERIAL_LIGHT_H

#include "Package.h"

class Light : public Package {
public:
    Light();
    Light(std::vector<Package::byte_t> &buff);

    void lightOn();
    void lightOff();
    void blinking(short n);
};

#endif //SPPU_SERIAL_LIGHT_H
