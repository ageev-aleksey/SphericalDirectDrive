#include "Serial.h"
#include "State.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>
#include "Position.h"
using namespace std::chrono_literals;
int main() {
    Serial com(2, Serial::Mode::ASYNC);
    com.setBaudRate(Serial::BR38400);
    com.open();
	State pack;
	pack.setOX(10);
	pack.setOY(20);
	pack.setPositionX(115);
	pack.setPositionY(0);
	pack.setPWMX(255);
	pack.setPWMY(-115);
	auto ptr = std::make_shared<std::vector<unsigned char>>(pack.toBinary());
	com.write(ptr);
	auto ptr_for_read = std::make_shared<std::vector<unsigned char>>(1);
	std::vector<unsigned char> buffer;
	com.read(std::move(ptr_for_read));
	unsigned char command_code = 0;

	for(int i = 0; i < 3;) {
	    if(com.isWriteAlready()) {
            i++;
	        auto data = pack.toBinary();
	        std::vector<unsigned char> half1;
            std::vector<unsigned char> half2;
            for(int i = 0; i < data.size()/2; i++) {
                half1.push_back(data[i]);
            }
            for(int i = data.size()/2; i < data.size(); i++) {
                half2.push_back(data[i]);
            }
	        auto ptr = std::make_shared<std::vector<unsigned char>>(half1);
            com.write(ptr);
            std::cout << "1 ";
            while(!com.isWriteAlready());
            _sleep(100);
            ptr = std::make_shared<std::vector<unsigned char>>(half2);
            com.write(ptr);
            std::cout << "2\n";
	    }
	}


	/*while (true) {
		if (com.isWriteAlready()) {
			com.write(ptr);
		}
	
		std::cout << std::endl;
		if ((ptr_for_read = com.isReadAlready()) != nullptr) {
			if (ptr_for_read->size() == 1) {
				command_code = (*ptr_for_read)[0];
				com.read(std::move(std::make_shared<std::vector<unsigned char>>(6)));
			}
			else {
				buffer.push_back(command_code);
				for (auto &el : *ptr_for_read) {
					buffer.push_back(el);
				}
				switch (command_code) {
				case 22: {
					Position pos(buffer);
					std::cout << "OX: " << pos.posX() << "\n"
						<< "OY: " << pos.posY() << std::endl;
				}
				}
			}
			
		}
	}*/
       

}