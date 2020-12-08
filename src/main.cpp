#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
// #include <crtdbg.h> // memory leak finder

//
//#include "system/GlobalObjectStorage.h"
//#include "log4cpp/category.hh"
//#include "log4cpp/fileappender.hh"
//#include "log4cpp/basiclayout.hh"
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
#include <string>
#include <fstream>
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
		if (comm.size() == 0) {
			comm.push_back(std::string(""));
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
	ComWorker(size_t com_num, std::shared_ptr<EventLoop> loop) : _com_port(com_num, loop->createConnector()), _loop(loop)
    {
       /* _com_port.setBaudRate(Serial::BaudRate::BR38400)
			.disableParityControll()
			.setStopBits(Serial::StopBits::ONE)
			.setTimeout(50)
			.setInternalBufferSize(36)
			.open();*/
		//Serial com_port(5, Serial::ASYNC);
		_com_port.setBaudRate(Serial::BR38400)
			.setTimeout(50)
			.disableParityControll()
			.disableCtsFlow()
			.disableDsrFlow()
			.setStopBits(Serial::ONE)
			.open();
		_com_port.flush();
		_com_port.async_read(36, std::bind(&ComWorker::readFromComPort, this, std::placeholders::_1));
		//TODO сделать обертку над системными вызовами для работы с конослью
		hConsoleInputBuffer = GetStdHandle(STD_INPUT_HANDLE);
		hConsoleOutputBuffer = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleMode(hConsoleInputBuffer, &fdwSaveOldMode);
        SetConsoleMode(hConsoleInputBuffer, ENABLE_WINDOW_INPUT);
		//loop->registerEvent(std::bind(&ComWorker::consoleCommands, this, std::placeholders::_1), Timer(0));
		loop->addEvent(Event({ std::bind(&ComWorker::consoleCommands, this, std::placeholders::_1) }, "t"), Timer(0), 0);
    }

    void  readFromComPort(std::shared_ptr< std::vector<unsigned char> > data) {
		//system("cls");
		int a = 1;
		SetConsoleCursorPosition(hConsoleOutputBuffer, { 0, 0 });
		//ClearConsole();

		PackageFactory packFactory;
        try{
           //std::shared_ptr<State> package =
                 //  std::dynamic_pointer_cast<State>(packFactory.createPackage(*data));
			auto package = std::make_shared<State>(*data);
			if (file.is_open()) {
				std::cout << "print to file\n";
				file << package->OX() << ' ' << package->OY() << std::endl;
			}
			for (auto &el : *data) {
				std::cout << (unsigned int)el << "-";
			}
			std::cout << std::endl;
			
			for (auto &el : *data) {
						std::cout << (unsigned int)el << "-";
			}
			std::cout << std::endl;
			short ox = 0;
			ox |= (*data)[2];
			ox |= (*data)[3] << 8;
			std::cout << "OX: " << ox << "\n";
		   std::cout << "Position OX:   "<< (int)package->OX()          << "   \n"
					 << "Position OY:   "<< (int)package->OY()          << "   \n"
					 << "PWM X:         "<< (int)package->PWMX()        << "   \n"
					 << "PWM Y:         "<< (int)package->PWMY()        << "   \n"
					 << "Task to X:     "<< (int)package->positionX()   << "   \n"
					 << "Task to Y:     "<< (int)package->positionY()   << "   \n"
					 << "Random value:  "<< (int)package->randomValue() << "   \n"
					 << "Hash:          "<< (int)package->hash() << "   "<<std::endl;
        } catch (PackageParseError &exp) {
			_com_port.flush();
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
		else if ("toFile" == c) {
			command_file(comm);
		}
		else if ("exit" == c) {
			command_off();
			_loop->stop();
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
		if (comm.arg_count() == 0) {
			
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

	void command_file(const Command &comm) {
		if (comm.arg_count() != 1) {
			std::cout << "must have one argumet - file path\n";
			std::getchar();
		}
		else {
			file.open(comm.arg(0), std::ofstream::out);
		}
		
	}
	~ComWorker() {
		if (file.is_open()) {
			file.close();
		}
	}
private:
    AsyncSerial _com_port;
	std::shared_ptr<EventLoop> _loop;
	HANDLE hConsoleInputBuffer;
	HANDLE hConsoleOutputBuffer;
    DWORD fdwSaveOldMode;
	std::ofstream file;


};



int main() {
    // GlobalStorage::init();
    //TODO нужна обертка над логгером, так как глобальное хранилище не поддерживает данный способ инициализации объекта
//    log4cpp::Appender *logMehod = new log4cpp::FileAppender("FriteToFile", "log.txt");
//    logMehod->setLayout(new log4cpp::BasicLayout());
//    log4cpp::Category *logMain= &log4cpp::Category::getInstance("Main");
//    logMain->setAdditivity(false);
//    logMain->setAppender(logMehod);
//    logMain->setPriority(log4cpp::Priority::DEBUG);

	system("echo %CD%");
	std::getchar();
	std::shared_ptr<EventLoop> loop = EventLoop::create();
	ComWorker com(5, loop);
	loop->run();
	//_CrtDumpMemoryLeaks();

#pragma region --===binary_copy_test===--
	//long long value = -256;
	//std::vector<unsigned char> bytes(sizeof(value));
	//unsigned char *bptr = reinterpret_cast<unsigned char*>(&value);
	//for (size_t i = 0; i < sizeof(value); i++) {
	//    bytes[i] = bptr[i];
	//}
	//for(auto &el : bytes) {
	//    std::cout << (unsigned int)el << "-";
	//}
	//long long value2 = 0;
	//bptr = reinterpret_cast<unsigned char*>(&value2);
	//for (size_t i = 0; i < sizeof(long long); i++) {
	//	bptr[i] = bytes[i];
	//}
	//std::cout << std::endl;
	//std::cout << value2 << std::endl;
#pragma endregion

#pragma region --===field_test===--

	/*long long value = -256;
	long long value2 = 0;
	Field field("test", 8);
	field.fill(reinterpret_cast<unsigned char*>(&value), sizeof(value));
	value2 = field.to<long long>()[0];
	std::cout << value2 << std::endl;*/

#pragma endregion

#pragma region --===FromBinaryTest===--
	Serial com_port(1, Serial::ASYNC);
	com_port.setBaudRate(Serial::BR38400)
		//.setTimeout(50)
		.setTimeout(MAXDWORD)
		.disableParityControll()
		.disableCtsFlow()
		.disableDsrFlow()
		.setStopBits(Serial::ONE)
		.open();
	std::shared_ptr<std::vector<unsigned char>> buff = std::make_shared<std::vector<unsigned char>>(36);
	HANDLE hConsoleOutputBuffer = GetStdHandle(STD_OUTPUT_HANDLE);
	com_port.flush();
	int k = 0;
	for (int i = 0; i < 10000; i++) {
		//com_port.flush();
		com_port.read(buff);
		while(com_port.isReadAlready() == nullptr) {
			if (k == 10) {
				break;
			}
		}
		k++;
		//SetConsoleCursorPosition(hConsoleOutputBuffer, { 0, 0 });
		
		/*if ((*buff)[0] != 20) {
			com_port.flush();
			continue;
		}*/
		for (auto &el : *buff) {
			std::cout << (unsigned int)el << "-";
		}
		std::cout << "\n";
		/*std::cout << "\n";
		State pack(*buff);
		short ox = 0;
		ox |= (*buff)[2];
		ox |= (*buff)[3] << 8;
		std::cout << "OX: " << ox << "\n";
		std::cout << "Position OX:   " << pack.OX() << "\n"
			<< "Position OY:   " << pack.OY() << "\n"
			<< "PWM X:         " << pack.PWMX() << "\n"
			<< "PWM Y:         " << pack.PWMY() << "\n"
			<< "Task to X:     " << pack.positionX() << "\n"
			<< "Task to Y:     " << pack.positionY() << "\n"
			<< "Random value:  " << (unsigned int)pack.randomValue() << "\n"
			<< "Hash:          " << (unsigned int)pack.hash() << "\n";
		std::cout << "===================" << std::endl;*/
	}


#pragma endregion
	return 0;
}