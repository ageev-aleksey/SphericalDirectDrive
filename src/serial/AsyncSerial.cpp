#include "AsyncSerial.h"
#include <functional>


AsyncSerial::AsyncSerial(size_t port_number, std::shared_ptr<Connector> conn)  :
                    com_port(port_number, Serial::Mode::ASYNC), connector(conn) {
    connector->registerEvent(std::bind(&AsyncSerial::async_serial_read_handler, this, std::placeholders::_1), Timer(0));
	connector->registerEvent(std::bind(&AsyncSerial::async_serial_write_handler, this, std::placeholders::_1), Timer(0));
    set_operation_read = false;
	set_operation_write = false;
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




void AsyncSerial::async_read(size_t size_read, std::function<SerialReadHandler> handler) {
    async_read_handler = std::move(handler);
    std::shared_ptr< std::vector<unsigned char> > buff_ptr = std::make_shared< std::vector<unsigned char> >(size_read);
    com_port.read(std::move(buff_ptr));
	set_operation_read = true;
}

void AsyncSerial::async_write(std::shared_ptr<std::vector<unsigned char>> buff, std::function<SerialWriteHandler> handler) {
	async_write_handler = std::move(handler);
	com_port.write(std::move(buff));
	set_operation_write = true;
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

std::shared_ptr<IMessage> AsyncSerial::async_serial_write_handler(std::shared_ptr<IMessage> msg) {
	if (!set_operation_write) return msg;
	if (com_port.isWriteAlready()) {
		set_operation_write = false;
		if (!!async_write_handler) {
			async_write_handler();
		}
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

AsyncSerial& AsyncSerial::dtrControll(Serial::DtrControll dtr) {
	com_port.dtrControll(dtr);
	return *this;
}
Serial::DtrControll AsyncSerial::getDtrControll() {
	return com_port.getDtrControll();
}

AsyncSerial& AsyncSerial::rtsControll(Serial::RtsControll rts) {
	com_port.rtsControll(rts);
	return *this;
}
Serial::RtsControll AsyncSerial::getRtsControll() {
	return com_port.getRtsControll();
}

AsyncSerial& AsyncSerial::enableCtsFlow() {
	com_port.enableCtsFlow();
	return *this;
}
AsyncSerial& AsyncSerial::disableCtsFlow() {
	com_port.disableCtsFlow();
	return *this;
}

AsyncSerial& AsyncSerial::enableDsrFlow() {
	com_port.enableDsrFlow();
	return *this;
}
AsyncSerial& AsyncSerial::disableDsrFlow() {
	com_port.disableDsrFlow();
	return *this;
}