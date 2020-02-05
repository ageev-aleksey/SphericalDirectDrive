#include "AsyncSerial.h"
#include <functional>


AsyncSerial::AsyncSerial(size_t port_number, std::shared_ptr<Connector> conn)  :
                    com_port(port_number, Serial::Mode::ASYNC), connector(conn) {
    connector->registerEvent(std::bind(&AsyncSerial::async_serial_read_handler, this, std::placeholders::_1), Timer(0));
    set_operation_read = false;
}

AsyncSerial& AsyncSerial::setBaudRate(Serial::BaudRate baudrate){
    com_port.setBaudRate(baudrate);
    return *this;
}
AsyncSerial& AsyncSerial::setPort(size_t number_port) {
    com_port.setPort(number_port);
    return *this;
}
AsyncSerial& AsyncSerial::setStopBits(Serial::StopBits stop_bits) {
	com_port.setStopBits(stop_bits);
    return *this;
}
AsyncSerial& AsyncSerial::enableParityControll(Serial::ParityControll pc) {
    com_port.enableParityControll(pc);
    return *this;
}
AsyncSerial& AsyncSerial::disableParityControll() {
    com_port.disableParityControll();
    return *this;
}

AsyncSerial& AsyncSerial::setInternalBufferSize(size_t size) {
	com_port.setInternalBufferSize(size);
	return *this;
}




void AsyncSerial::async_read(size_t size_read, std::function<SerialEventHandler> &&handler) {
    set_operation_read = true;
    async_read_handler = std::move(handler);
    std::shared_ptr< std::vector<unsigned char> > buff_ptr = std::make_shared< std::vector<unsigned char> >(size_read);
    com_port.read(std::move(buff_ptr));
}

std::shared_ptr<IMessage> AsyncSerial::async_serial_read_handler(std::shared_ptr<IMessage> msg) {
   if(!set_operation_read) return msg;
    std::shared_ptr < std::vector <unsigned char>> buff;
    if((buff = com_port.isReadAlready()) != nullptr) {
		set_operation_read = false;
        async_read_handler(std::move(buff));
    }
    return msg;
}

void AsyncSerial::open() {
    com_port.open();
}

void AsyncSerial::close() {
	com_port.close();

}

void AsyncSerial::flush() {
	com_port.flush();
}

AsyncSerial& AsyncSerial::setTimeout(size_t ms) {
	com_port.setTimeout(ms);
	return *this;
}