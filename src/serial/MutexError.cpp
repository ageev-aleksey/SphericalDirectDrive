#include "MutexError.h"

MutexError::MutexError(std::string what, uint32_t error_code) : std::runtime_error(what) {
	error = error_code;
}

uint32_t MutexError::getErrorCode() const {
	return error;
}


LockMutexError::LockMutexError(std::string what, uint32_t error_code) : MutexError(what, error_code)
{}

CreateMutexError::CreateMutexError(std::string what, uint32_t error_code) : MutexError(what, error_code)
{}


ReleaseMutexError::ReleaseMutexError(std::string what, uint32_t error_code) : MutexError(what, error_code)
{}




