#include "Mutex.h"
#include "MutexError.h"
#include <windows.h>


class MutexImpl {
public:
	MutexImpl() {
		hWinMutex = CreateMutex(NULL, FALSE, NULL);
		if (hWinMutex == NULL) {
			throw CreateMutexError("failed to create mutex", GetLastError());
		}
	}
	void lock() {
		DWORD res = WaitForSingleObject(hWinMutex, INFINITE);
		if (res == WAIT_FAILED) {
			throw  LockMutexError("failed locking mutex", GetLastError());
		}
	}

	void release() {
		DWORD res = ReleaseMutex(hWinMutex);
		if (res == MUTEX_ERROR) {
			throw ReleaseMutexError("failed release mutex", GetLastError());
		}
	}

	~MutexImpl() 
	{
		CloseHandle(hWinMutex);
	}

private:
	static constexpr BOOL MUTEX_ERROR = 0;
	HANDLE hWinMutex;
};



Mutex::Mutex() {
	pimpl = new MutexImpl();
}

Mutex::~Mutex() {
	delete pimpl;
}

void Mutex::lock() {
	pimpl->lock();
}

void Mutex::release() {
	pimpl->release();
}