#include "Serial.h"
#include "State.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
int main() {
    Serial com(2);
    com.setBaudRate(Serial::BaudRate::BR9600);
    com.open();
    while(true) {
        State pack;
        pack.setOX(10);
        pack.setOY(20);
        pack.setPositionX(115);
        pack.setPositionY(0);
        pack.setPWMX(255);
        pack.setPWMY(-115);
        com.write(pack.toBinary());
        std::this_thread::sleep_for(10ms);
    }
}