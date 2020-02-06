#include  "EventLoop.h"
#include <iostream>
#include <queue>
#include<list>
#include <unordered_map>
#include "Mutex.h"
#include<chrono>

using namespace std::chrono;






class EventLoopImpl {
public:
	EventLoopImpl() : queue(), time_events(), events_map(),
		mutex_global(), isRun(false)
	{}

	void addEvent(const Event &event, int register_id = 0) {
		mutex_global.lock();

		events_map.insert({ event.getType(), { std::move(event), register_id } });

		mutex_global.release();
	}

	void addEvent(const Event &event,const Timer &timer, int registrar_id = 0) {
		mutex_global.lock();

		time_events.push_back({ {event, registrar_id}, timer });

		mutex_global.release();
	}

	void delEvent(std::string event_type, int registrar_id = 0) {
		mutex_global.lock(); 

		for (auto itr = time_events.begin(); itr != time_events.end(); itr++) {
			if ((itr->description.id_conn == registrar_id) && (itr->description.event.getType() == event_type)) {
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

		mutex_global.release();
	}

	void sendMessage(std::shared_ptr<IMessage> message) {
		mutex_global.lock();

		queue.push(message);

		mutex_global.release();
	}

	void run() {
		isRun = true;//нужна ли блокировка?
		while (isRun) {
			mutex_global.lock();

			for (auto &v : time_events) {
				if (v.timer.isRang()) {
					mutex_global.release();
	
					v.description.event.execute(nullptr);

					mutex_global.lock();
				}
			}

			if (!queue.empty()) {
				std::shared_ptr<IMessage> message = queue.front();
				queue.pop();
				auto elist = events_map.equal_range(message->getType());
				for (auto itr = elist.first; itr != elist.second; itr++) {
					mutex_global.release();
					//TODO ≈лси менеджер потока прервет этот птоток и запустит другой поток
					//который добавит новый тип событи€, что приведет к реалокации таблицы,
					// то тогда итератор будет невалиден!!
					message = itr->second.event.execute(message);

					mutex_global.lock();
				}
			}

			mutex_global.release();

			//std::this_thread::sleep_for(CYCLE_TYME_SLEEP);
		}
	}

	void stop() {
		isRun = false; //нужна ли блокировка?
	}


private:
	const std::chrono::milliseconds CYCLE_TYME_SLEEP = 2ms; //TODO ќпределитьс€ с константой усыплени€ бесконечного цикла в EventLoop 

	struct event_desc {
		Event event;
		int id_conn;
	};
	struct time_event {
		struct event_desc description;
		Timer timer;
	};
	std::queue< std::shared_ptr<IMessage> > queue;
	std::list<struct time_event> time_events;
	std::unordered_multimap<std::string, struct event_desc> events_map;
	Mutex mutex_global;
	bool isRun;
public:
	std::weak_ptr<EventLoop> self_event_loop;
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
std::shared_ptr<Connector> EventLoop::createConnector() {
	//TODO проблема - объект существует на стеке, но указателей через std::shared_ptr нету!!!
	return std::make_shared<Connector>(Pimpl()->self_event_loop, ++last_id_connector);
}
void EventLoop::run() {
	Pimpl()->run();
}
void EventLoop::stop() {
	Pimpl()->stop();
}
std::shared_ptr<EventLoop> EventLoop::create() {
	std::shared_ptr<EventLoop> loop_ptr = std::shared_ptr<EventLoop>(new EventLoop());
	loop_ptr->Pimpl()->self_event_loop = loop_ptr;
	return std::move(loop_ptr);
 }

