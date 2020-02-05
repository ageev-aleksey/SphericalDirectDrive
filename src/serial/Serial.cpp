#include <cassert>
#include "Serial.h"
#include <vector>
#include "SerialError.h"
#include <string>
#include <Windows.h>
#include <iostream>

DWORD WINAPI ThredProc(_In_ LPVOID lpParametr);

class SerialImpl {
public:
	SerialImpl(size_t number_port, Serial::Mode mode) {
		internal_buffer_size = 256;
		internal_buffer = nullptr;
		port = number_port;
        hWinSerial = INVALID_HANDLE_VALUE;
        timeout = 0xFFFFFFFF;
		boundrate = Serial::BaudRate::BR9600;
		stop_bits = Serial::StopBits::ONE;
		parity = Serial::ParityControll::NOT_CONTROLL;
		buffer_write_when_read_com = nullptr;
		sync = { 0 };
		sync.hEvent = INVALID_HANDLE_VALUE;
		read_from_last_check = 0;
        if(mode == Serial::Mode::SYNC) {
            _mode = FILE_ATTRIBUTE_NORMAL;
            func_read = &SerialImpl::read_sync;
//                func_write = &SerialImpl::write_sync;
        } else {
            _mode = FILE_FLAG_OVERLAPPED;
            func_read = &SerialImpl::read_async;
           // func_write = &SerialImpl::write_async;
        }


	}
	~SerialImpl() {
		close();
	}
	void setTimeout(size_t ms) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then change timeout of read operation.",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
	    timeout = ms;
	}
	void setBaudRate(Serial::BaudRate _boundrate) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then change baud rate.", 
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		boundrate = _boundrate;
	}

	void setPort(size_t num_port) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then change number of port.", 
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		port = num_port;
	}

	void setStopBits(Serial::StopBits _stop_bits) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then change number of stop bits.", 
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		stop_bits = _stop_bits;
	}

	void enableParityControll(Serial::ParityControll controll) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then change parity controll.", 
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		parity = controll;
	}

	void setInternalBufferSize(size_t value) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then change size of internal buffer.", 
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		if (value == 0) {
			throw SerialError("Size of internal buffer can not be equal zero", 
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		internal_buffer_size = value;

	}



	void open() {
		internal_buffer = new unsigned char[internal_buffer_size];

		std::string str_port = std::string("COM") + std::to_string(port);
		hWinSerial = CreateFile(TEXT(str_port.c_str()), GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, _mode, NULL);
		if (hWinSerial == INVALID_HANDLE_VALUE) {
			throw SerialOpenError(std::string("failed open port: ") + str_port,
								GetLastError());
		}

		COMMCONFIG com_config;
		DWORD com_config_size = 0;
		if (GetCommConfig(hWinSerial, &com_config, &com_config_size)) {
			throw SerialOpenError(std::string("failed get com port default configuration"),
								 GetLastError());
		}
		com_config.dcb.BaudRate = _to(boundrate);
		com_config.dcb.StopBits = _to(stop_bits);
		com_config.dcb.Parity = _to(parity);
		if (SetCommConfig(hWinSerial, &com_config, sizeof(com_config))) {
			throw SerialOpenError(std::string("failed set com port configuration"),
				GetLastError());
		}

		// Now set the timeouts ( we control the timeout overselves using WaitForXXX()
		COMMTIMEOUTS timeouts;
		timeouts.ReadIntervalTimeout = timeout; //максимальное время ожидания получения очередного нового символа
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.ReadTotalTimeoutConstant = 0xFFFFFFFF;
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 0xFFFFFFFF;
		if (!SetCommTimeouts(hWinSerial, &timeouts)) {
			throw SerialOpenError(std::string("failed set timeouts for com port"),
				GetLastError());
		}

		if (_mode = Serial::Mode::ASYNC) {
			sync.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (sync.hEvent == INVALID_HANDLE_VALUE) {
				_mode = FILE_ATTRIBUTE_NORMAL;
				func_read = &SerialImpl::read_sync;
				throw SerialOpenError("error create event object (windows) for async work", GetLastError());
			}

			if (!SetCommMask(hWinSerial, EV_RXCHAR)) {
				CloseHandle(hWinSerial);
				hWinSerial = INVALID_HANDLE_VALUE;
				CloseHandle(sync.hEvent);
				sync.hEvent = INVALID_HANDLE_VALUE;
				throw SerialOpenError("Error configure Serial for asynchrone work",
					GetLastError());
			}
			DWORD last_error = 0;
			DWORD state = 0;
			if (!WaitCommEvent(hWinSerial, &state, &sync) && (last_error = GetLastError()) != ERROR_IO_PENDING) {
				CloseHandle(hWinSerial);
				hWinSerial = INVALID_HANDLE_VALUE;
				CloseHandle(sync.hEvent);
				sync.hEvent = INVALID_HANDLE_VALUE;
				throw SerialOpenError("Error binding of system object with com-port",
					last_error);
			}

		}
	}

	bool isOpen() {
        return hWinSerial != INVALID_HANDLE_VALUE;
	}

	void read(std::shared_ptr< std::vector<unsigned char> > &buff_ptr) {
		return (this->*func_read)(buff_ptr);
	}

	void write(const std::vector<unsigned char> &buff) {
        if(hWinSerial == INVALID_HANDLE_VALUE) {
            throw SerialWriteError("com port was not opened", 
				SerialError::NOT_SYSTEM_CALL_ERROR);
        }
        DWORD feedback = 0;
		size_t copy= 0;
		if (!WriteFile(hWinSerial, &buff[0], buff.size(), &feedback, 0) || feedback != buff.size()) {
            CloseHandle(hWinSerial);
            hWinSerial = INVALID_HANDLE_VALUE;
            throw SerialWriteError("error, writing data in com port", GetLastError());
        }
	}

    std::shared_ptr< std::vector<unsigned char> > isReadAlready() {
		if (!isOpen()) {
			throw SerialReadError("Serial not opened",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}

		if (_mode == Serial::Mode::ASYNC) {

			if (buffer_write_when_read_com == nullptr) {
				throw SerialReadError("asynchrone operation of read don't started",
					SerialError::NOT_SYSTEM_CALL_ERROR);
			}

			DWORD wait = WaitForSingleObject(sync.hEvent, 0);
			DWORD read = 0;
			DWORD k = 0;
			if (wait == WAIT_OBJECT_0) {
				if (GetOverlappedResult(hWinSerial, &sync, &read, FALSE)) {
					for (size_t i = 0; i < read; i++) {
						assert(read_from_last_check < buffer_write_when_read_com->size());
						(*buffer_write_when_read_com)[read_from_last_check] = internal_buffer[i];
						read_from_last_check++;
					}
					if (read_from_last_check == buffer_write_when_read_com->size()) {
						std::shared_ptr< std::vector<unsigned char> > tmp = std::move( buffer_write_when_read_com);
						read_from_last_check = 0;
						return std::move(tmp);
					}
					if ((buffer_write_when_read_com->size() - read_from_last_check) < internal_buffer_size) {
						k = internal_buffer_size - (buffer_write_when_read_com->size() - read_from_last_check);
					}
					ReadFile(hWinSerial, internal_buffer, internal_buffer_size - k, &read, &sync);
				}
			}
			return nullptr;
		}
		return nullptr;
	}

	void close() {
		if (hWinSerial != hWinSerial) {
			CloseHandle(hWinSerial);
			hWinSerial = hWinSerial;
		}
		if (sync.hEvent != INVALID_HANDLE_VALUE) {
			CloseHandle(sync.hEvent);
			sync.hEvent = INVALID_HANDLE_VALUE;
		}
		if (internal_buffer != nullptr) {
			delete[] internal_buffer;
			internal_buffer = nullptr;
		}
	}

	void flush() {
		if (isOpen()) {
			PurgeComm(hWinSerial, PURGE_RXCLEAR);
		}
	}

private:

	int _to(Serial::BaudRate br) {
		return br;
	}
	int _to(Serial::StopBits sb) {
		switch (sb) {
		case Serial::StopBits::ONE:
			return ONESTOPBIT;
		case Serial::StopBits::ONE_AND_HALF:
			return ONE5STOPBITS;
		case Serial::StopBits::TWO:
			return TWOSTOPBITS;
		}
	}
	int _to(Serial::ParityControll pc) {
		switch (pc)
		{
		case Serial::ParityControll::APPEND_TO_EVEN:
			return EVENPARITY;
		case Serial::ParityControll::APPEND_TO_ODD:
			return ODDPARITY;
		case Serial::ParityControll::ZERO_BIT:
			return SPACEPARITY;
		case Serial::ParityControll::UNIT_BIT:
			return MARKPARITY;
		case Serial::ParityControll::NOT_CONTROLL:
			return NOPARITY;
		}
	}

	void read_sync(std::shared_ptr< std::vector<unsigned char> > &buffer_ptr){
	    if(!isOpen()) {
	        throw SerialError("Com port is not openned", 0);
	    }
	    DWORD read = 0;
		DWORD all_read = 0;
	    unsigned char tmp_buffer[256] = {0};
		/*while (all_read != read_size) {
			if ((!ReadFile(hWinSerial, &buff[0] + all_read, read_size - all_read, &read, NULL)) && (read != read_size)) {
				CloseHandle(hWinSerial);
				hWinSerial = INVALID_HANDLE_VALUE;
				throw SerialReadError("was less readed than necessary", GetLastError());
			}
			all_read += read;

		}*/
    }

	void read_async(std::shared_ptr< std::vector<unsigned char> > &buffer_ptr) {
		if (!isOpen()) {
			throw SerialReadError("Serial not opened",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		
		if (buffer_write_when_read_com != nullptr) {
			throw SerialReadError("asynchrone operation of read already started",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		buffer_write_when_read_com = std::move(buffer_ptr);
		DWORD must_read = internal_buffer_size;
		DWORD read = 0;
		if (internal_buffer_size > buffer_write_when_read_com->size()) {
			must_read = buffer_write_when_read_com->size();
		}
		//ReadFile(hWinSerial, (&buff[0] + result_read), read_size - result_read, &read, &sync);
		ReadFile(hWinSerial, internal_buffer, must_read, &read, &sync);
    }




   // void (SerialImpl::*func_write)(const std::vector<unsigned char>&);
	void (SerialImpl::*func_read)(std::shared_ptr< std::vector<unsigned char> >&);
	HANDLE hWinSerial;
	size_t port;
	Serial::BaudRate boundrate;
	Serial::StopBits stop_bits;
	Serial::ParityControll parity;
	size_t timeout;//таймаут синхронного чтения.
	DWORD _mode; //режим. Асинхронный или синхронный
	unsigned char *internal_buffer;//временный буфер, в который записывает ОС
	size_t internal_buffer_size; // размер временного буфера, в который записывает ОС
	std::shared_ptr<std::vector<unsigned char>> buffer_write_when_read_com;//буфер в который записывать при асинхронном чтении
	size_t read_from_last_check;//индекс заполняемости буфера, в который записывются данные при асинхронном чтении
	OVERLAPPED sync; //Структура, с помощью которой ОС сигнализирует о возможности чтения из порта

	static constexpr size_t TEMP_BUFFER_SIZE = 256;
};

//DWORD WINAPI ThredProc(_In_ LPVOID lpParametr) {
//	HANDLE com_port = (HANDLE)lpParametr;
//	OVERLAPPED o;
//	DWORD dvEvntMask;
//
//	if (SetCommMask(com_port, EV_RXCHAR) == 0) {
//		//TODO Ошибка настройки уведомления об том, что данные пришли
//	}
//
//	o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//	o.Internal = 0;
//	o.InternalHigh = 0;
//	o.Offset = 0;
//	o.OffsetHigh = 0;
//	if (o.hEvent == INVALID_HANDLE_VALUE) {
//		//TODO ошибка создания события
//	}
//	bool bContinue = true;
//	char buffer[256] = { 0 };
//	while (bContinue) {
//		/*Поскольку мы работаем с портом в асинхронном режиме, 
//		то функция немедленно верноет управления с ошибкой:
//		ERROR_IO_PENDING*/
//		BOOL SetResult = WaitCommEvent(com_port, &dvEvntMask, &o);
//		if (!((SetResult == FALSE) && (GetLastError() == ERROR_IO_PENDING))) {
//			//TODO ошибка регистрации события
//		}
//		DWORD wait = WaitForSingleObject(o.hEvent, INFINITE);
//		if (wait == WAIT_OBJECT_0) {
//			do {
//				DWORD readed = 0;
//				ReadFile(com_port, buffer, 256, &readed, o);
//				wait = WaitForSingleObject(sync.hEvent, INFINITE);
//				//TODO операция чтения из ком порта
//			} while ();
//		}
//	}
//	
//}


Serial::Serial(size_t port_mumber, Mode mode) {
	pimpl = new SerialImpl(port_mumber, mode);
}

void Serial::read(std::shared_ptr< std::vector<unsigned char> > buff_ptr) {
	pimpl->read(buff_ptr);
}

Serial& Serial::setBaudRate(BaudRate baudrate) {
	pimpl->setBaudRate(baudrate);
	return *this;
}

Serial& Serial::setPort(size_t number_port) {
	pimpl->setPort(number_port);
	return *this;
}

Serial& Serial::setStopBits(StopBits stop_bits) {
	pimpl->setStopBits(stop_bits);
	return *this;
}

Serial& Serial::enableParityControll(ParityControll controll) {
	pimpl->enableParityControll(controll);
	return *this;
}

Serial& Serial::disableParityControll() {
	pimpl->enableParityControll(NOT_CONTROLL);
	return *this;
}

Serial& Serial::setInternalBufferSize(size_t size) {
	pimpl->setInternalBufferSize(size);
	return *this;
}

void Serial::open() {
	pimpl->open();
}

Serial& Serial::setTimeout(size_t ms) {
    pimpl->setTimeout(ms);
    return *this;
}

void Serial::write(const std::vector<unsigned char> &buffer) {
    pimpl->write(buffer);
}

Serial::~Serial() {
	delete pimpl;
}

std::shared_ptr< std::vector<unsigned char> > Serial::isReadAlready() {
	return pimpl->isReadAlready();
}

void Serial::close() {
	pimpl->close();
}

void Serial::flush() {
	pimpl->flush();
}