#ifndef _SERIAL_ERROR_H_
#define _SERIAL_ERROR_H_

#include <stdexcept>

class SerialError : std::runtime_error {
public:
	SerialError(std::string what, uint32_t error_code);
	virtual uint32_t getCode();
private:
	uint32_t error;
};


class SerialOpenError : public SerialError {
public:
	SerialOpenError(std::string what, uint32_t error_code);
};

class SerialReadError : public SerialError {
public:
    SerialReadError(std::string what, uint32_t error_code);
};

#endif