#include "PackageFactory.h"
#include "PackageError.h"
#include "State.h"
#include "Serial.h"
#include "SerialError.h"
#include "Event.h"
#include "EventLoop.h"
#include "Timer.h"
#include <functional>
#include <iostream>
#include <AsyncSerial.h>

class ComData : public IMessage {
public:
    std::shared_ptr<State> _pack;
    ComData(std::shared_ptr<State> pack) : IMessage("ComData") , _pack(pack){
    }
};



class ComWorker {
public:
    ComWorker(size_t com_num, std::shared_ptr<Connector> loop) : _com_port(com_num, loop)
    {
        _com_port.setBaudRate(Serial::BaudRate::BR9600)
			.disableParityControll()
			.setStopBits(Serial::StopBits::ONE)
			.setTimeout(50)
			.setInternalBufferSize(36)
			.open();
		_com_port.async_read(36, std::bind(&ComWorker::readFromComPort, this, std::placeholders::_1));
    }

    void  readFromComPort(std::shared_ptr< std::vector<unsigned char> > data) {
        PackageFactory packFactory;

        try{
           std::shared_ptr<State> package =
                   std::dynamic_pointer_cast<State>(packFactory.createPackage(*data));
		   std::cout << "Position OX:   "<< package->OX()          << "\n"
					 << "Position OY:   "<< package->OX()          << "\n"
					 << "PWM X:         "<< package->PWMX()        << "\n"
					 << "PWM Y:         "<< package->PWMY()        << "\n"
					 << "Task to X:     "<< package->positionX()   << "\n"
					 << "Task to Y:     "<< package->positionY()   << "\n"
					 << "Random value:  "<< package->randomValue() << "\n"
					 << "Hash:          "<< package->randomValue() << std::endl;
        } catch (PackageParseError &exp) {
			std::cout << "package parse error";
        }

    }

    std::shared_ptr<IMessage> writeToComPort(std::shared_ptr<IMessage> msg){
        try{
            std::shared_ptr<Package> pack = std::dynamic_pointer_cast<Package>(msg);
            _com_port.write(pack->toBinary());
        } catch(...) {

        }
    return msg;
    }

private:
    AsyncSerial _com_port;

};



int main() {
	std::shared_ptr<EventLoop> loop = EventLoop::create();
	ComWorker com(1, loop->createConnector());

	return 0;
}