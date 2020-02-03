#include "PackageFactory.h"
#include "PackageError.h"
#include "State.h"
#include "Serial.h"
#include "SerialError.h"
#include "Event.h"
#include "EventLoop.h"
#include "Timer.h"
#include <functional>

class ComData : public IMessage {
public:
    std::shared_ptr<State> _pack;
    ComData(std::shared_ptr<State> pack) : IMessage("ComData") , _pack(pack){
    }
};



class ComWorker {
public:
    ComWorker(size_t com_num, Connector loop) : _com_port(com_num, Serial::Mode::ASYNC), _loop(std::move(loop))
    {
        _com_port.setBaudRate(Serial::BaudRate::BR9600)
        .disableParityControll()
        .setStopBits(Serial::StopBits::ONE)
        .setTimeout(50)
        .open();
        _loop.registerEvent(std::bind(&ComWorker::readFromComPort, this, std::placeholders::_1), Timer(0));
        _loop.registerEvent(std::bind(&ComWorker::writeToComPort, this, std::placeholders::_1), "WriteToComPort");
    }

    std::shared_ptr<IMessage> readFromComPort(std::shared_ptr<IMessage> msg) {
        PackageFactory packFactory;
        try{
           std::shared_ptr<State> package =
                   std::dynamic_pointer_cast<State>(
                           packFactory.createPackage(
                                   _com_port.read(36)));
           _loop.sendMessage(std::make_shared<ComData>(package));



        } catch (SerialBufferEmptyError &exp) {

        }
        return msg;
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
    Serial _com_port;
    Connector _loop;

};


void test_ComPort() {
    Serial com(1);
	com.setBaudRate(Serial::BaudRate::BR9600).open();
	
    com.write(std::vector<unsigned char>{ 'H', 'e', 'l', 'l', 'o' });

}

int main() {
  //  std::shared_ptr<EventLoop> loop;
    //ComWorker com(1, loop->createConnector());
    test_ComPort();


	/*Serial comport(1);
	comport.setBaudRate(Serial::BaudRate::BR9600)
		   .setStopBits(Serial::StopBits::ONE)
		   .disableParityControll();
	try {
		comport.open();
		auto res = comport.read(20);
	}
	catch (SerialError &exp) {
		std::cout << "error code: " << exp.getCode() << std::endl;
		//std::cout << "what: " << exp.what() << std::endl;
	}*/
	
	/*
	for (auto itr = res.begin(); itr != res.end(); itr++) {
		std::cout << *itr;
	}
	std::cout << std::endl;*/
	return 0;
}