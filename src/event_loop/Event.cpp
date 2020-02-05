#include  "Event.h"
#include <iostream>


Event::Event(std::initializer_list<std::function<IEventHandler>> hlist, std::string type) : type(type) {
	for (auto &v : hlist) {
		handlers.push_back(v);
	}
}

Event::Event(std::string type) : handlers(), type(type)
{}


Event::~Event()
{}


void Event::addHandler(std::function<IEventHandler> handler) {
	handlers.push_back(handler);
}

void Event::removeHandler(std::function<IEventHandler> handler) {
	//TODO переделать метод. Скорее всего надо просто сделать сброс всех обработчиков
	/*for (auto itr = handlers.begin(); itr != handlers.end(); itr++) {
		if (*itr == handler) {
			handlers.erase(itr);
		}
	}*/

}

std::string Event::getType() const{
	return type;
}

std::shared_ptr<IMessage> Event::execute(std::shared_ptr<IMessage> message) const
{
	std::shared_ptr<IMessage> msg = message;
	for (auto &func : handlers) {
		try {
			 func(msg);
		}
		catch (...) {
			std::cout << "error";
		}
		
	}
	return msg;
}