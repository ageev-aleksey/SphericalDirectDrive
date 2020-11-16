#include "EventLoop.h"

class ConnectorImpl {
public:
	ConnectorImpl(std::weak_ptr<EventLoop> loop_ptr, int id) : event_loop(loop_ptr), id(id)
	{}
	bool sendMessage(std::shared_ptr<IMessage> message) {
		if (message != nullptr) {
			std::shared_ptr<EventLoop> loop = event_loop.lock();
			if (loop == nullptr) {
				return false;
			}
			loop->sendMessage(message);
			return true;
		}
		return false;
	}

	bool registerEvent(std::function<IEventHandler> handler, std::string &&event_type) {
		std::shared_ptr<EventLoop> loop = event_loop.lock();
		if (loop == nullptr) {
			return false;
		}
		Event new_event({ handler }, std::move(event_type));
		loop->addEvent(std::move(new_event), id);
		return true;
	}

	bool registerEvent(std::function<IEventHandler> handler, Timer timer) {
		std::shared_ptr<EventLoop> loop = event_loop.lock();
		if (loop == nullptr) {
			return false;
		}
		Event new_event({ handler }, "EventTime");
		loop->addEvent(std::move(new_event), timer, id);
		return true;
	}


	bool resetEvent(const std::string &type) {
		std::shared_ptr<EventLoop> loop = event_loop.lock();
		if (loop == nullptr) {
			return false;
		}
		loop->delEvent(type, id);
	}

private:
	std::weak_ptr<EventLoop> event_loop;
	int id;
};

ConnectorImpl* Connector::Pimpl() {
	return data;
}
Connector::Connector(std::weak_ptr<EventLoop> loop_ptr, int id)
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