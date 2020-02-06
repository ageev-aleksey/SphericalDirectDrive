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

using SerialReadHandler = void(std::shared_ptr< std::vector<unsigned char> >);
using SerialWriteHandler = void();

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

    void async_read(size_t size_read, std::function<SerialReadHandler> handler);
    void async_write(std::shared_ptr<std::vector<unsigned char>> buff_ptr, std::function<SerialWriteHandler> handler = nullptr);
private:
    std::shared_ptr<IMessage> async_serial_read_handler(std::shared_ptr<IMessage> msg);
	std::shared_ptr<IMessage> async_serial_write_handler(std::shared_ptr<IMessage> msg);
	
    std::function<SerialReadHandler>  async_read_handler;
	std::function<SerialWriteHandler>  async_write_handler;
    Serial com_port;
    std::shared_ptr<Connector> connector;
    bool set_operation_read;
	bool set_operation_write;

};



#endif //SPPU_SERIAL_ASYNCSERIAL_H
