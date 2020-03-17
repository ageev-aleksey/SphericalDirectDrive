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
		boudrate = Serial::BaudRate::BR9600;
		stop_bits = Serial::StopBits::ONE;
		parity = Serial::ParityControll::NOT_CONTROLL;
		DtrControll = Serial::DTR_DISABLE;
		RtsControll = Serial::RTS_DISABLE;
		CtsFlow = false;
		DsrFlow = false;
		buffer_write_when_read_com = nullptr;
		async_write_started = false;
		sync_read = { 0 };
		sync_read.hEvent = INVALID_HANDLE_VALUE;
		sync_write = { 0 };
		sync_write.hEvent = INVALID_HANDLE_VALUE;
		read_from_last_check = 0;
        if(mode == Serial::Mode::SYNC) {
            _mode = FILE_ATTRIBUTE_NORMAL;
            func_read = &SerialImpl::read_sync;
            func_write = &SerialImpl::write_sync;
        } else {
            _mode = FILE_FLAG_OVERLAPPED;
            func_read = &SerialImpl::read_async;
           func_write = &SerialImpl::write_async;
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
	void setBaudRate(Serial::BaudRate _boudrate) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then change baud rate.", 
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		boudrate = _boudrate;
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

	
	void dtrControll(Serial::DtrControll dtr) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then disable DTR controll.",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		DtrControll = dtr;
	}
	Serial::DtrControll getDtrControll() {
		return DtrControll;
	}

	void rtsControll(Serial::RtsControll rts) {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then enable RTS controll.",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		RtsControll = rts;
	}

	Serial::RtsControll getRtsControll() {
		return RtsControll;
	}

	void enableCtsFlow() {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then disable RTS controll.",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		CtsFlow = true;
	}
	void disableCtsFlow() {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then disable RTS controll.",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		CtsFlow = false;
	}
	bool isCtsFlow() {
		return CtsFlow;
	}

	void enableDsrFlow() {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then disable RTS controll.",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		DsrFlow = true;
	}
	void disableDsrFlow() {
		if (isOpen()) {
			throw SerialError("Serial port is already opened."
				"Close port and then disable RTS controll.",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		DsrFlow = false;
	}
	bool isDsrFlow() {
		return DsrFlow;
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

		DCB dcb = { 0 };
		dcb.DCBlength = sizeof(DCB);
		if (!::GetCommState(hWinSerial, &dcb))
		{
			close();
			throw SerialOpenError(
				"error get config of serial port (windows DCB struct)",
				GetLastError());
		}
		dcb.BaudRate = _to(boudrate);
		dcb.ByteSize = 8;
		dcb.Parity = _to(parity);
		dcb.StopBits = _to(stop_bits);
		dcb.fOutxCtsFlow = _to(CtsFlow);
		dcb.fOutxDsrFlow = _to(DsrFlow);
		dcb.fDtrControl = _to(DtrControll);//DTR_CONTROL_DISABLE; 
		dcb.fDsrSensitivity = 0;
		dcb.fRtsControl = _to(RtsControll);//RTS_CONTROL_DISABLE;
		if (!::SetCommState(hWinSerial, &dcb))
		{
			close();
			throw SerialOpenError(
				"error set config of serial port (windows DCB struct)",
				GetLastError());
		}
		

		// Now set the timeouts ( we control the timeout overselves using WaitForXXX()
		//TODO Разобраться с таймаутами. При текущих настройках таймаут определяется 
		//при вызове функции WaitForXXX() в которые и передается таймаут
		COMMTIMEOUTS timeouts;
		timeouts.ReadIntervalTimeout = MAXDWORD;//timeout; //максимальное время ожидания получения очередного нового символа
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.ReadTotalTimeoutConstant = 0;//0xFFFFFFFF;
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 0;//0xFFFFFFFF;
		if (!SetCommTimeouts(hWinSerial, &timeouts)) {
			throw SerialOpenError(std::string("failed set timeouts for com port"),
				GetLastError());
		}

		if (_mode == FILE_FLAG_OVERLAPPED) {
			sync_read.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			sync_write.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if ((sync_read.hEvent == INVALID_HANDLE_VALUE) ||
				(sync_write.hEvent == INVALID_HANDLE_VALUE)) 
			{
				_mode = FILE_ATTRIBUTE_NORMAL;
				func_read = &SerialImpl::read_sync;
				throw SerialOpenError("error create event object (windows) for async work", GetLastError());
			}

			if (!SetCommMask(hWinSerial, EV_RXCHAR | EV_TXEMPTY)) {
				CloseHandle(hWinSerial);
				hWinSerial = INVALID_HANDLE_VALUE;
				CloseHandle(sync_read.hEvent);
				sync_read.hEvent = INVALID_HANDLE_VALUE;
				throw SerialOpenError("Error configure Serial for asynchrone work",
					GetLastError());
			}
			DWORD last_error = 0;
			DWORD state = 0;
			if (!WaitCommEvent(hWinSerial, &state, &sync_read) && (last_error = GetLastError()) != ERROR_IO_PENDING) {
				CloseHandle(hWinSerial);
				hWinSerial = INVALID_HANDLE_VALUE;
				CloseHandle(sync_read.hEvent);
				sync_read.hEvent = INVALID_HANDLE_VALUE;
				throw SerialOpenError("Error binding of system object with com-port",
					last_error);
			}

		}
	}

	bool isOpen() {
        return hWinSerial != INVALID_HANDLE_VALUE;
	}

	void read(std::shared_ptr< std::vector<unsigned char> > &buff_ptr) {
		if (!isOpen()) {
			throw SerialReadError("Serial not opened",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		if ((buff_ptr == nullptr) || (buff_ptr->size() == 0)) {
			throw SerialWriteError("invalid buffer",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}

		return (this->*func_read)(buff_ptr);
	}

	void write(const std::shared_ptr< std::vector<unsigned char> > &buff) {
		if (!isOpen()) {
			throw SerialWriteError("Serial not opened",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		if ((buff == nullptr) || (buff->size() == 0)) {
			throw SerialWriteError("invalid buffer",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		return (this->*func_write)(buff);
	}

    std::shared_ptr< std::vector<unsigned char> > isReadAlready() {
		if (!isOpen()) {
			throw SerialReadError("Serial not opened",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}

		if (_mode == FILE_FLAG_OVERLAPPED) {

			if (buffer_write_when_read_com == nullptr) {
				throw SerialReadError("asynchrone operation of read don't started",
					SerialError::NOT_SYSTEM_CALL_ERROR);
			}

			DWORD wait = WaitForSingleObject(sync_read.hEvent, 0);
			DWORD read = 0;
			DWORD k = 0;
			if (wait == WAIT_OBJECT_0) {
				if (GetOverlappedResult(hWinSerial, &sync_read, &read, FALSE)) {
					//if (read != 0) {
						for (size_t i = 0; i < read; i++) {
							//assert(read_from_last_check < buffer_write_when_read_com->size());
							if (read_from_last_check >= buffer_write_when_read_com->size()) {
								std::cout << "ERROR!";
							}
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
						ReadFile(hWinSerial, internal_buffer, internal_buffer_size - k, NULL, &sync_read);
					//}
				}
			}
			return nullptr;
		}
		return nullptr;
	}

	bool isWriteAlready() {
		if (!isOpen()) {
			throw SerialReadError("Serial not opened",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		if (_mode == FILE_FLAG_OVERLAPPED) {

			if (!async_write_started) {
				throw SerialReadError("asynchrone operation of read don't started",
					SerialError::NOT_SYSTEM_CALL_ERROR);
			}

			DWORD wait = WaitForSingleObject(sync_write.hEvent, 0);
			DWORD write = 0;
			DWORD k = 0;
			if (wait == WAIT_OBJECT_0) {
				if (GetOverlappedResult(hWinSerial, &sync_write, &write, FALSE)) {
					async_write_started = false;
					return true;
				}
			}
			return false;
		}
		return false;
	}

	void close() {
		if (hWinSerial != hWinSerial) {
			CloseHandle(hWinSerial);
			hWinSerial = hWinSerial;
		}
		if (sync_read.hEvent != INVALID_HANDLE_VALUE) {
			CloseHandle(sync_read.hEvent);
			sync_read.hEvent = INVALID_HANDLE_VALUE;
		}
		if (sync_write.hEvent != INVALID_HANDLE_VALUE) {
			CloseHandle(sync_write.hEvent);
			sync_write.hEvent = INVALID_HANDLE_VALUE;
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
	int _to(bool v) {
		if (v)
			return TRUE;
		else
			return FALSE;
	}
	int _to(Serial::DtrControll v) {
		switch (v) {
		case Serial::DTR_DISABLE: 
			return DTR_CONTROL_DISABLE;
		case Serial::DTR_ENABLE:
			return DTR_CONTROL_ENABLE;
		case Serial::DTR_HANDSHAKE:
			return DTR_CONTROL_HANDSHAKE;
		}
	}

	int _to(Serial::RtsControll v) {
		switch (v) {
		case Serial::RTS_DISABLE:
			return RTS_CONTROL_DISABLE;
		case Serial::RTS_ENABLE:
			return RTS_CONTROL_ENABLE;
		case Serial::RTS_HANDSHAKE:
			return RTS_CONTROL_HANDSHAKE;
		case Serial::RTS_TOGGLE:
			return RTS_CONTROL_TOGGLE;
		}
	}

	void read_sync(std::shared_ptr< std::vector<unsigned char> > &buffer_ptr){
	    if(!isOpen()) {
	        throw SerialError("Com port is not openned", 0);
	    }
	    DWORD read = 0;
		DWORD all_read = 0;
		DWORD read_size = buffer_ptr->size();
	    unsigned char tmp_buffer[256] = {0};
		while (all_read != read_size) {
			if ((!ReadFile(hWinSerial, &tmp_buffer, read_size - all_read, &read, NULL)) && (read != read_size)) {
				CloseHandle(hWinSerial);
				hWinSerial = INVALID_HANDLE_VALUE;
				throw SerialReadError("was less readed than necessary", GetLastError());
			}
			all_read += read;

		}
    }

	void read_async(std::shared_ptr< std::vector<unsigned char> > &buffer_ptr) {		
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
		ReadFile(hWinSerial, internal_buffer, must_read, &read, &sync_read);
    }
	void write_sync(const std::shared_ptr< std::vector<unsigned char> > &buffer_src) {
		DWORD written = 0;
		if((!WriteFile(hWinSerial, &((*buffer_src)[0]), 
			buffer_src->size(), &written, NULL)) && (written != buffer_src->size())) {
			throw SerialWriteError("Error fo write data in com port",
				GetLastError());
		}
	}
	void write_async(const std::shared_ptr< std::vector<unsigned char> > &buffer_src) {
		if (async_write_started) {
			throw SerialWriteError("asynchrone operation of write already started",
				SerialError::NOT_SYSTEM_CALL_ERROR);
		}
		DWORD written = 0;
		if ((!WriteFile(hWinSerial, &((*buffer_src)[0]),
			buffer_src->size(), &written, &sync_write)))	
		{
			DWORD error = GetLastError();
			if (error != ERROR_IO_PENDING) {
				throw SerialWriteError("Error fo write data in com port",
					error);
			}
			
		}
		async_write_started = true;
	}




   // void (SerialImpl::*func_write)(const std::vector<unsigned char>&);
	void (SerialImpl::*func_read)(std::shared_ptr< std::vector<unsigned char> >&);
	void (SerialImpl::*func_write)(const std::shared_ptr< std::vector<unsigned char> >&);
	HANDLE hWinSerial;
	size_t port;
	Serial::BaudRate boudrate;
	Serial::StopBits stop_bits;
	Serial::ParityControll parity;
	Serial::DtrControll DtrControll;
	Serial::RtsControll RtsControll;
	bool CtsFlow;
	bool DsrFlow;
	size_t timeout;//таймаут синхронного чтения.
	DWORD _mode; //режим. Асинхронный или синхронный
	unsigned char *internal_buffer;//временный буфер, в который записывает ОС
	size_t internal_buffer_size; // размер временного буфера, в который записывает ОС
	std::shared_ptr<std::vector<unsigned char>> buffer_write_when_read_com;//буфер в который записывать при асинхронном чтении
	size_t read_from_last_check;//индекс заполняемости буфера, в который записывются данные при асинхронном чтении
	OVERLAPPED sync_read; //Структура, с помощью которой ОС сигнализирует о возможности чтения из порта
	OVERLAPPED sync_write;
	bool async_write_started;

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

void Serial::write(const std::shared_ptr< std::vector<unsigned char> > buffer) {
    pimpl->write(buffer);
}

Serial::~Serial() {
	delete pimpl;
}

std::shared_ptr< std::vector<unsigned char> > Serial::isReadAlready() {
	return pimpl->isReadAlready();
}

bool Serial::isWriteAlready() {
	return pimpl->isWriteAlready();
}

void Serial::close() {
	pimpl->close();
}

void Serial::flush() {
	pimpl->flush();
}


Serial& Serial::dtrControll(Serial::DtrControll dtr) {
	pimpl->dtrControll(dtr);
	return *this;
}

Serial::DtrControll Serial::getDtrControll() {
	return pimpl->getDtrControll();
}

Serial& Serial::rtsControll(Serial::RtsControll rts) {
	pimpl->rtsControll(rts);
	return *this;
}
Serial::RtsControll Serial::getRtsControll() {
	return pimpl->getRtsControll();
}
Serial& Serial::enableCtsFlow() {
	pimpl->enableCtsFlow();
	return *this;
}
Serial& Serial::disableCtsFlow() {
	pimpl->disableCtsFlow();
	return *this;
}
Serial& Serial::enableDsrFlow() {
	pimpl->enableDsrFlow();
	return *this;
}
Serial& Serial::disableDsrFlow() {
	pimpl->disableDsrFlow();
	return *this;
}
