//
// Created by Aleksey on 05.02.2020.
//

#ifndef SPPU_SERIAL_ASYNCSERIAL_H
#define SPPU_SERIAL_ASYNCSERIAL_H
#include "Serial.h"
#include "EventLoop.h"
#include "IMessage.h"
#include <memory>
#include <functional>
class AsyncSerial;

using SerialEventHandler = void(std::shared_ptr< std::vector<unsigned char> >);

class AsyncSerial {
public:
    AsyncSerial(size_t port_mumber, std::shared_ptr<Connector> conn);

	AsyncSerial& setTimeout(size_t ms);
    AsyncSerial& setBaudRate(Serial::BaudRate baudrate);
    AsyncSerial& setPort(size_t number_port);
    AsyncSerial& setStopBits(Serial::StopBits stop_bits);
    AsyncSerial& enableParityControll(Serial::ParityControll pc);
    AsyncSerial& disableParityControll();
	AsyncSerial& setInternalBufferSize(size_t size);
    void open();
    void close();
	void flush();

    void async_read(size_t size_read, std::function<SerialEventHandler> &&handler);
    //TODO void async_write(IEventHandler handler);
private:
    std::shared_ptr<IMessage> async_serial_read_handler(std::shared_ptr<IMessage> msg);
    std::function<SerialEventHandler>  async_read_handler;
    Serial com_port;
    std::shared_ptr<Connector> connector;
    bool set_operation_read;

};



#endif //SPPU_SERIAL_ASYNCSERIAL_H
