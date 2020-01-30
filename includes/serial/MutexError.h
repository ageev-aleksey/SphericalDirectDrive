#ifndef _LOCK_MUTEX_ERROR_H_
#define _LOCK_MUTEX_ERROR_H_

#include <stdexcept>
#include <cstdint>
#include <string>

class MutexError : std::runtime_error {
public: 
	MutexError(std::string what, uint32_t error_code);
	uint32_t getErrorCode() const;
private:
	uint32_t error;
};


class LockMutexError : MutexError {
public:
	LockMutexError(std::string what, uint32_t error_code);
};

class CreateMutexError : MutexError {
public:
	CreateMutexError(std::string what, uint32_t error_code);
};

class ReleaseMutexError : MutexError {
public:
	ReleaseMutexError(std::string what, uint32_t error_code);
};

#endif
