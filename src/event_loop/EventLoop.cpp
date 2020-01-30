#include  "EventLoop.h"
#include <iostream>
#include <queue>
#include<list>
#include <unordered_map>
#include <mutex>
#include<thread>
#include<chrono>

using namespace std::chrono;






class EventLoopImpl {
public:
	EventLoopImpl() : queue(), time_events(), events_map(),
		mutex_global(), isRun(false)
	{}

	void addEvent(const Event &event, int register_id = 0) {
		while (!mutex_global.try_lock()) {}

		events_map.insert({ event.getType(), { std::move(event), register_id } });

		mutex_global.unlock();
	}

	void addEvent(const Event &event,const Timer &timer, int registrar_id = 0) {
		while (!mutex_global.try_lock()) {}

		time_events.push_back({ {event, registrar_id}, timer });

		mutex_global.unlock();
	}

	void delEvent(std::string event_type, int registrar_id = 0) {
		mutex_global.lock(); //TODO так как поиск может затянуться, то лучше усыпить процесс.

		for (auto itr = time_events.begin(); itr != time_events.end(); itr++) {
			if ((itr->e.id_conn == registrar_id) && (itr->e.evnt.getType() == event_type)) {
				time_events.erase(itr);
				break;
			}
		}

		auto elist = events_map.equal_range(event_type);
		for (auto itr = elist.first; itr != elist.second; itr++) {
			if (itr->second.id_conn == registrar_id) {
				events_map.erase(itr);
				break;
			}
		}

		mutex_global.unlock();
	}

	void sendMessage(std::shared_ptr<IMessage> message) {
		while (!mutex_global.try_lock()) {}

		queue.push(message);

		mutex_global.unlock();
	}

	void run() {
		isRun = true;//нужна ли блокировка?
		while (isRun) {
			while (!mutex_global.try_lock()) {}

			for (auto &v : time_events) {
				if (v.timer.isRang()) {
					std::shared_ptr<IMessage> msg(new IMessage("t"));
					v.e.evnt.execute(msg);
				}
			}
			if (!queue.empty()) {
				std::shared_ptr<IMessage> message = queue.front();
				queue.pop();
				auto elist = events_map.equal_range(message->getType());
				for (auto itr = elist.first; itr != elist.second; itr++) {
					message = itr->second.evnt.execute(message);
				}
			}
			mutex_global.unlock();

			std::this_thread::sleep_for(CYCLE_TYME_SLEEP);
		}
	}

	void stop() {
		isRun = false; //нужна ли блокировка?
	}


private:
	const std::chrono::milliseconds CYCLE_TYME_SLEEP = 2ms; //TODO Определиться с константой усыпления бесконечного цикла в EventLoop 

	struct event_desc {
		Event evnt;
		int id_conn;
	};
	struct time_event {
		struct event_desc e;
		Timer timer;
	};
	std::queue< std::shared_ptr<IMessage> > queue;
	std::list<struct time_event> time_events;
	std::unordered_multimap<std::string, struct event_desc> events_map;
	std::mutex mutex_global;
	bool isRun;
};



EventLoopImpl* EventLoop::Pimpl() {
	return data;
}

EventLoop::EventLoop() : data(new EventLoopImpl), last_id_connector(0)
{}

EventLoop::~EventLoop()
{
	delete data;
}

void EventLoop::addEvent(Event &&event, int registrar_id) {
	Pimpl()->addEvent(event, registrar_id);
}
void EventLoop::addEvent(Event &&event, Timer timer, int registrar_id) {
	Pimpl()->addEvent(event, timer, registrar_id);
}
void EventLoop::delEvent(std::string event_type, int registrar_id) {
	Pimpl()->delEvent(event_type, registrar_id);
}
void EventLoop::sendMessage(std::shared_ptr<IMessage> message) {
	Pimpl()->sendMessage(message);
}
Connector && EventLoop::createConnector() {
	//TODO проблема - объект существует на стеке, но указателей через std::shared_ptr нету!!!
	return Connector(std::shared_ptr<EventLoop>(this), ++last_id_connector);
}
void EventLoop::run() {
	Pimpl()->run();
}
void EventLoop::stop() {
	Pimpl()->stop();
}
//TODO Возможно лучше сделать глобальную блокировку при любых манимпуляциях с хранилищем событий