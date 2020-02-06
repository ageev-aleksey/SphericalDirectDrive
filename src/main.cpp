#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "PackageFactory.h"
#include "PackageError.h"
#include "State.h"
#include "Position.h"
#include "Light.h"
#include "Mode.h"
#include "Serial.h"
#include "SerialError.h"
#include "Event.h"
#include "EventLoop.h"
#include "Timer.h"
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <AsyncSerial.h>
#include <windows.h>


class Command {
public:
	Command(const std::string &str) {
		std::stringstream stream;
		stream << str;
		std::string value;
		while (stream >> value) {
			comm.push_back(std::move(value));
		}
	}
	const std::string& command() const{
		return comm[0];
	}

	const std::string& arg(int index) const {
		return comm[1 + index];
	}
	int arg_int(int index) const {
		return std::stoi(comm[1 + index]);
	}
	int arg_count() const {
		return comm.size() - 1;
	}
private:
	std::vector<std::string> comm;
};


class ComWorker {
public:
    ComWorker(size_t com_num, std::shared_ptr<Connector> loop) : _com_port(com_num, loop)
    {
        _com_port.setBaudRate(Serial::BaudRate::BR9600)
			.disableParityControll()
			.setStopBits(Serial::StopBits::ONE)
			.setTimeout(50)
			.setInternalBufferSize(36)
			.open();
		_com_port.flush();
		_com_port.async_read(36, std::bind(&ComWorker::readFromComPort, this, std::placeholders::_1));
		loop->registerEvent(std::bind(&ComWorker::consoleCommands, this, std::placeholders::_1), Timer(0));
		//TODO сделать обертку над системными вызовами для работы с конослью
		hConsoleInputBuffer = GetStdHandle(STD_INPUT_HANDLE);
		hConsoleOutputBuffer = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleMode(hConsoleInputBuffer, &fdwSaveOldMode);
        SetConsoleMode(hConsoleInputBuffer, ENABLE_WINDOW_INPUT);
    }

    void  readFromComPort(std::shared_ptr< std::vector<unsigned char> > data) {
		//system("cls");
		int a = 1;
		SetConsoleCursorPosition(hConsoleOutputBuffer, { 0, 0 });
		PackageFactory packFactory;
        try{
           //std::shared_ptr<State> package =
                 //  std::dynamic_pointer_cast<State>(packFactory.createPackage(*data));
			auto package = std::make_shared<State>(*data);
		   std::cout << "Position OX:   "<< (int)package->OX()          << "\n"
					 << "Position OY:   "<< (int)package->OY()          << "\n"
					 << "PWM X:         "<< (int)package->PWMX()        << "\n"
					 << "PWM Y:         "<< (int)package->PWMY()        << "\n"
					 << "Task to X:     "<< (int)package->positionX()   << "\n"
					 << "Task to Y:     "<< (int)package->positionY()   << "\n"
					 << "Random value:  "<< (int)package->randomValue() << "\n"
					 << "Hash:          "<< (int)package->hash() << std::endl;
        } catch (PackageParseError &exp) {
			std::cout << "package parse error";
        }
		_com_port.async_read(36, std::bind(&ComWorker::readFromComPort, this, std::placeholders::_1));
    }


	std::shared_ptr<IMessage> consoleCommands(std::shared_ptr<IMessage> msg) {
		DWORD numEvents = 0;
		INPUT_RECORD irInBuf = { 0 };
		//GetNumberOfConsoleInputEvents(hConsoleInputBuffer, &numEvents)
		if (WaitForSingleObject(hConsoleInputBuffer, 10) == WAIT_OBJECT_0) {
			ReadConsoleInput(hConsoleInputBuffer, &irInBuf, 1, &numEvents);
			if (!(irInBuf.EventType == KEY_EVENT && irInBuf.Event.KeyEvent.uChar.AsciiChar == 'c')) return msg;
		}
		else {
			return msg;
		}

		system("cls");//TODO заменить на нормальный системный вызов
		std::string command_str;
		std::cout << ">>";
		std::getline(std::cin, command_str);
		Command comm(command_str);
		const std::string &c = comm.command();
		if ("help" == c) {
			command_help();
		}
		else if ("pos" == c) {
			command_position(comm);
		}
		else if ("light" == c) {
			command_light(comm);
		}
		else if ("on" == c) {
			command_on();
		}
		else if ("off" == c) {
			command_off();
		}
		_com_port.flush();
		return msg;
	}

	void command_help() {
		std::cout << "Help:\n"
			<< "pos x y - set task for SSD of positioning to point (x, y). x,y - integer from -500 to 500\n"
			<< "light x - turn state luminodiode of SDD. x - (optional) integer. Setting speed blink as half period from 3 * x\n"
			<< "on - turn on current of coils.\n"
			<< "off - turn off current of coils" << std::endl;
	}

	void command_position(const Command &comm) {
		if (comm.arg_count() != 2) {
			std::cout << "ERROR: Command must have two integer arguments" << std::endl;
			return;
		}
		try {
			Position pos;
			pos.setPosX(comm.arg_int(0));
			pos.setPosY(comm.arg_int(1));
			_com_port.async_write(std::make_shared<std::vector<unsigned char>>(pos.toBinary()));
		}
		catch (...) {
			std::cout << "ERROR: arguments must have integer type" << std::endl;
			return;
		}
	}

	void command_light(const Command &comm) {
		static bool light_state = true;
		Light light;
		if (comm.arg_count() == 1) {
			
			if (light_state) {
				light_state = false;
				light.lightOff();
			}
			else {
				light_state = true;
				light.lightOn();
			}
		}
		else {
			try {
				light.blinking(comm.arg_int(0));//TODO возможно переполнение типа!!
			}
			catch (...) {
				std::cout << "ERROR: arguments must have integer type" << std::endl;
				return;
			}
		}
		_com_port.async_write(std::make_shared<std::vector<unsigned char>>(light.toBinary()));
	}

	void command_on() {
		Mode mode;
		mode.coilOn();
		_com_port.async_write(std::make_shared<std::vector<unsigned char>>(mode.toBinary()));
	}

	void command_off() {
		Mode mode;
		mode.coilOff();
		_com_port.async_write(std::make_shared<std::vector<unsigned char>>(mode.toBinary()));
	}
private:
    AsyncSerial _com_port;
	HANDLE hConsoleInputBuffer;
	HANDLE hConsoleOutputBuffer;
    DWORD fdwSaveOldMode;


};



int main() {
	std::shared_ptr<EventLoop> loop = EventLoop::create();
	ComWorker com(1, loop->createConnector());
	loop->run();
	_CrtDumpMemoryLeaks();
	return 0;
}