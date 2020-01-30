#include "EventLoop.h"

class ConnectorImpl {
public:
	ConnectorImpl(std::shared_ptr<EventLoop> loop_ptr, int id) : event_loop(loop_ptr), id(id)
	{}
	bool sendMessage(std::shared_ptr<IMessage> message) {
		if (message != nullptr) {
			event_loop->sendMessage(message);
			return true;
		}
		return false;
	}

	bool registerEvent(std::function<IEventHandler> handler, std::string &&event_type) {
		Event new_event({ handler }, std::move(event_type));
		event_loop->addEvent(std::move(new_event), id);
		return false;
	}

	bool registerEvent(std::function<IEventHandler> handler, Timer timer) {
		Event new_event({ handler }, "EventTime");
		event_loop->addEvent(std::move(new_event), timer, id);
		return true;
	}


	bool resetEvent(const std::string &type) {
		return false;
	}

private:
	std::shared_ptr<EventLoop> event_loop;
	int id;
};

ConnectorImpl* Connector::Pimpl() {
	return data;
}
Connector::Connector(std::shared_ptr<EventLoop> loop_ptr, int id)
{
	data = new ConnectorImpl(loop_ptr, id);
}

bool Connector::sendMessage(std::shared_ptr<IMessage> message) {
	return Pimpl()->sendMessage(message);
}

bool Connector::registerEvent(std::function<IEventHandler> handler, std::string event_type) {
	return Pimpl()->registerEvent(handler, std::move(event_type));
}

bool Connector::registerEvent(std::function<IEventHandler> handler, Timer time) {
	return Pimpl()->registerEvent(handler, time);
}

bool Connector::resetEvent(const std::string & event_type) {
	return Pimpl()->resetEvent(event_type);
}

bool Connector::resetEvent(std::string && event_type) {
	return resetEvent(event_type);
}

Connector::Connector(Connector && obj) {
	data = obj.data;
	obj.data = nullptr;
}

Connector&& Connector::operator=(Connector &&obj)
{
	if (data != nullptr)
		delete data;
	data = obj.data;
	obj.data = nullptr;
	return std::move(*this);
}