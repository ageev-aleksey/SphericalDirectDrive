#ifndef _SERIAL_H_
#define _SERIAL_H_
#include <vector>
#include <memory>

class SerialImpl;

class Serial {
public:
	enum BaudRate {
		BR110 = 110, BR300 = 300, BR600 = 600, BR1200 = 1200, BR2400 = 2400, 
		BR4800 = 4800, BR9600 = 9600, BR14400 = 14400, BR19200 = 19200,
		BR38400 = 38400, BR56000 = 56000, BR57600 = 57600, BR128000 = 128000, 
		BR256000 = 256000
	};
	enum ParityControll {
		APPEND_TO_EVEN, APPEND_TO_ODD, ZERO_BIT, UNIT_BIT, NOT_CONTROLL
	};

	enum StopBits {
		ONE, ONE_AND_HALF, TWO
	};

	enum Mode {
	   SYNC, ASYNC
	};

	Serial(size_t port_mumber, Mode mode = SYNC);
	Serial();
	~Serial();

	Serial& setTimeout(size_t ms);
	Serial& setBaudRate(BaudRate baudrate);
	Serial& setPort(size_t number_port);
	Serial& setStopBits(StopBits stop_bits);
	Serial& enableParityControll(ParityControll pc);
	Serial& disableParityControll();
	Serial& setInternalBufferSize(size_t size);
	void open();
	void close();
	void flush();
    std::shared_ptr< std::vector<unsigned char> > isReadAlready();
	bool isWriteAlready();
	void read(std::shared_ptr< std::vector<unsigned char> > buff_ptr);
	void write(std::shared_ptr< std::vector<unsigned char> > buff_ptr);

private:
	SerialImpl *pimpl;
};

#endif