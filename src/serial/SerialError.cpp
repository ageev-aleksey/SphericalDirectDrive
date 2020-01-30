#include "SerialError.h"


SerialError::SerialError(std::string what, uint32_t error_code) : std::runtime_error(what) {
	error = error_code;
}

uint32_t SerialError::getCode() {
	return error;
}

SerialOpenError::SerialOpenError(std::string what, uint32_t error_code) : SerialError(what, error_code)
{

}

SerialReadError::SerialReadError(std::string what, uint32_t error_code)  : SerialError(what, error_code)
{}