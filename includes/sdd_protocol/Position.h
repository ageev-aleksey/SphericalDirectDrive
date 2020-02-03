//
// Created by Aleksey on 27.01.2020.
//

#ifndef SPPU_SERIAL_POSITION_H
#define SPPU_SERIAL_POSITION_H
#include "Package.h"

class Position :  Package{
public:
    Position();
    Position(std::vector<Package::byte_t> &bin_buff);

    short posX();
    void setPosX(short x);

    short posY();
    void setPosY(short x);
private:
    void create_all_fields();
};


#endif //SPPU_SERIAL_POSITION_H
