#ifndef _SERIAL_ERROR_H_
#define _SERIAL_ERROR_H_

#include <stdexcept>

class SerialError : std::runtime_error {
public:
	SerialError(const std::string &what, uint32_t error_code);
	virtual uint32_t getCode();
	static constexpr size_t NOT_SYSTEM_CALL_ERROR = 0;
private:
	uint32_t error;
};


class SerialOpenError : public SerialError {
public:
	SerialOpenError(const std::string &what, uint32_t error_code);
};

class SerialReadError : public SerialError {
public:
    SerialReadError(const std::string &what, uint32_t error_code);
};

class SerialWriteError: public SerialError {
public:
    SerialWriteError(const std::string &what, uint32_t error_code);
};
#endif