#include "SerialError.h"


SerialError::SerialError(const std::string &what, uint32_t error_code) : std::runtime_error(what) {
	error = error_code;
}

uint32_t SerialError::getCode() {
	return error;
}

SerialOpenError::SerialOpenError(const std::string &what, uint32_t error_code) : SerialError(what, error_code)
{

}

SerialReadError::SerialReadError(const std::string &what, uint32_t error_code)  : SerialError(what, error_code)
{}

SerialWriteError::SerialWriteError(const std::string &what, uint32_t error_code) : SerialError(what, error_code)
{}