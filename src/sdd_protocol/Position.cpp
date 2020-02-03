
#include "Position.h"

void Position::create_all_fields() {
    bworker.addField("POSX", 2);
    bworker.addField("POSY", 2);
}


Position::Position() : Package("position", 22, Message::Endianness::LITTLE_ENDIAN) {
    create_all_fields();
}

Position::Position(std::vector<Package::byte_t > &buffer) : Package("position", 22, Message::Endianness::LITTLE_ENDIAN){
    create_all_fields();
    fromBinary(buffer);
}

short Position::posX(){
    return findField("POSX")->to<short>()[0];
}

void Position::setPosX(short x) {
    findField("POSX")->fill(reinterpret_cast<const unsigned char*>(&x), 2);
}


short Position::posY(){
    return findField("POSY")->to<short>()[0];
}

void Position::setPosY(short x) {
    findField("POSY")->fill(reinterpret_cast<const unsigned char*>(&x), 2);
}

