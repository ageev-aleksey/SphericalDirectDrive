#include "State.h"


State::State() : Package("state", 20, Message::Endianness::LITTLE_ENDIAN) {
    create_all_fields();
}



State::State(const std::vector<Package::byte_t> &bin_buff) : Package("state", 20, Message::Endianness::LITTLE_ENDIAN) {
    create_all_fields();
    fromBinary(bin_buff);
}

State::~State() {

}

/*
void State::fromBinary(std::vector<Package::byte_t> bin_buff) {//TODO копирование
   if((bin_buff.size() == 36) && (bin_buff[0] == id())) {
        bworker.fromBinary(bin_buff, 1); //TODO копирование
   }

}*/

Package::byte_t State::state() {
    return findField("TW")->to<Package::byte_t>()[0];
}

void State::setState(Package::byte_t s) {
    findField("TW")->fill(&s, 1);
}

short State::OX() {
    return findField("OX")->to<short>()[0];
}

void State::setOX(short x) {
    findField("OX")->fill(reinterpret_cast<const unsigned char*>(&x), 2);
}

short State::OY() {
    return  findField("OY")->to<short>()[0];
}

void State::setOY(short y) {
    findField("OY")->fill(reinterpret_cast<const unsigned char*>(&y), 2);
}

short State::PWMX() {
    return findField("PWMX")->to<short>()[0];
}

void State::setPWMX(short pwmx){
    findField("PWMX")->fill(reinterpret_cast<const unsigned char*>(&pwmx), 2);
}

short State::PWMY() {
    return findField("PWMY")->to<short>()[0];
}

void State::setPWMY(short pwmy){
    findField("PWMY")->fill(reinterpret_cast<const unsigned char*>(&pwmy), 2);
}

short State::positionX() {
    return findField("POSX")->to<short>()[0];
}

void  State::setPositionX(short pos) {
    findField("POSX")->fill(reinterpret_cast<const unsigned char*>(&pos), 2);
}

short State::positionY() {
    return findField("POSY")->to<short>()[0];
}

void  State::setPositionY(short pos) {
    findField("POSY")->fill(reinterpret_cast<const unsigned char*>(&pos), 2);
}

Package::byte_t State::randomValue() {
    return findField("R")->to<Package::byte_t>()[0];
}



void State::create_all_fields() {
	bworker.addField("TW", 1);
	bworker.addField("OX", 2);
	bworker.addField("OY", 2);
	bworker.addField("-1", 3);//Байты, значения которых не удалось определить
	bworker.addField("PWMX", 2);
	bworker.addField("PWMY", 2);
	bworker.addField("POSX", 2);
	bworker.addField("POSY", 2);
	bworker.addField("-2", 17);//Байты, значения которых не удалось определить
	bworker.addField("R", 1);
}