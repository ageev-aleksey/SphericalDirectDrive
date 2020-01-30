#include "Serial.h"
#include <vector>
#include "SerialError.h"
#include <string>
#include <Windows.h>

DWORD WINAPI ThredProc(_In_ LPVOID lpParametr);

class SerialImpl {
public:
	SerialImpl(size_t number_port, Serial::Mode mode) {
		port = number_port;
        hWinSerial = INVALID_HANDLE_VALUE;
        if(mode == Serial::Mode::SYNC) {
                func_read = &SerialImpl::read_sync;
                func_write = &SerialImpl::write_sync;
        } else {
            func_read = &SerialImpl::read_async;
            func_write = &SerialImpl::write_async;
        }


	}

	void setBaudRate(Serial::BaudRate _boundrate) {
		boundrate = _boundrate;
	}

	void setPort(size_t num_port) {
		port = num_port;
	}

	void setStopBits(Serial::StopBits _stop_bits) {
		stop_bits = _stop_bits;
	}

	void enableParityControll(Serial::ParityControll controll) {
		parity = controll;
	}



	void open() {
		std::string str_port = std::string("COM") + std::to_string(port);
		hWinSerial = CreateFile(TEXT(str_port.c_str()), GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
		timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.ReadTotalTimeoutConstant = 0xFFFFFFFF;
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 0xFFFFFFFF;
		if (!SetCommTimeouts(hWinSerial, &timeouts)) {
			throw SerialOpenError(std::string("failed set timeouts for com port"),
				GetLastError());
		}


		//открытие потока, для записи всех входящих пакетов в собственный буффер
		/*hWinThread = CreateThread(NULL, 0, ThredProc, this, 0, NULL);
		if (hWinThread == NULL) {
			throw SerialOpenError(std::string("failed create thread for buffering arriving packages"),
				GetLastError());
		}*/

	}

	bool isOpen() {
        return hWinSerial != INVALID_HANDLE_VALUE;
	}

	std::vector<unsigned char> read(size_t size_read) {
		return (this->*func_read)(size_read);
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

	std::vector <unsigned char> read_sync(size_t read_size){
	    if(!isOpen()) {
	        throw SerialError("Com port is not openned", 0);
	    }
	    DWORD readed = 0;
	    unsigned char tmp_buffer[256] = {0};
	    DWORD bytes_read = read_size < 256 ? read_size : 256;
	    std::vector<unsigned char> res(read_size);
	    do {
	        DWORD current_readed = 0;
	        BOOL read_result = ReadFile(hWinSerial, tmp_buffer, bytes_read, &current_readed, NULL);
	        if(read_result == FALSE) {
	            throw SerialReadError("error read", GetLastError());
	        }
            for(size_t i = readed; i < current_readed; i++) {
                res[i] = tmp_buffer[i-readed];
            }
	        readed += current_readed;

	    }while(readed != read_size);

	    return res;
    }

    std::vector <unsigned char> read_async(size_t read_size){
        return std::vector<unsigned char>();
    }

    void write_sync(std::vector<unsigned char> &buff) {

	}

    void write_async(std::vector<unsigned char> &buff) {

    }


    void (SerialImpl::*func_write)(std::vector<unsigned char>&);
    std::vector <unsigned char> (SerialImpl::*func_read)(size_t);
	HANDLE hWinSerial;
	size_t port;
	Serial::BaudRate boundrate;
	Serial::StopBits stop_bits;
	Serial::ParityControll parity;
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

std::vector<unsigned char> Serial::read(size_t num_read) {
	return pimpl->read(num_read);
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

void Serial::open() {
	pimpl->open();
}