//\authors Ageev Aleksey

#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_
#include <memory>
#include "IMessage.h"
#include "Event.h"
#include "Timer.h"

class EventLoopImpl;
class ConnectorImpl;
class EventLoop;


/**
		Класс осуществляющий взаимодействие с Циклом событй. Он нужен для того, чтобы
		предоставить возможность отправлять сообщения и регистрировать собственные обработчики,
		при этом не допуская до экземпляра EventLoop, иначе другая часть системы, сможет остановить
		работу Цикла событий, удалять чужие обрабочики и события.
	*/
class Connector {
public:
	Connector(std::shared_ptr<EventLoop> loop_ptr, int id);
	Connector(Connector && obj);
	Connector &&operator=(Connector &&obj);

	Connector(const Connector&) = delete;
	Connector operator=(const Connector&) = delete;
	/**
		\brief Отправка сообщений в очередь событий
		\param[in] message умный указатель на класс, который реализует интерфейс IMessage

		Для того, чтобы отправлять сообщения, необходимо взять собственный класс-контейнер,
		который будет содержать данные для отправки в другу часть системы. Отнаследовать его от
		класса IMessage и реализовать метод getType. Тип сообщеня должен совпадать с типом события,
		который должен реагировать на это сообщение.
	*/
	bool sendMessage(std::shared_ptr<IMessage> message);
	/**
		\brief Регистрация обработчика на определенное событие.
		\param[in] handler указатель на функцию обработчик, для указания
		метода класса воспользуйтесь std::bind

		Регистрация обработчика на определенное событие.  Обработчиков может быть несколько.
		Для регистрации нескольких обработчиков вызовите несколько раз этот метод с разными указателями
		на обработчики, но с одинаковым типом.

	*/
	bool registerEvent(std::function<IEventHandler> handler, std::string event_type);
	bool registerEvent(std::function<IEventHandler> handler, Timer timer);
	/*
		\brief сбрасывает все обработчики события, которые были зарегестрированы через данный
		экземпляр Connector
		\param[in] event_type строчка описывающа тип события, у которого надо сбросить обработчики
	*/
	bool resetEvent(std::string &&event_type);
	bool resetEvent(const std::string &event_type);
private:
	ConnectorImpl* Pimpl();
private:
	ConnectorImpl *data;
	/**/
	//Требуется хранить те события, которые были зарегистрированы через данный коннектор
};


/**
	Класс описывающий EventLoop - это бесконечный цикл работающий с очередью событий.
	Событие имеет тип и множество обработчиков этого события. 
	В очередь добавляют сообщения (класс с данными)с типом события. 
	Когда из очереди будет извлечено сообщение, то будет вызваны соответсттвующие обработчики
	в порядке их добавления. Сообщение прокидывается от обработчика к обработчику. Таким образом,
	если некоторый обработчик модифицирует сообщение, то к следующему обработчику придет это 
	модифицированнае сообщение.
*/
class EventLoop {
public:
	EventLoop();
	~EventLoop();
	/**
		\brief Добавления события, на которые должен реагировать EventLoop. Если придет IMessage
		с типом события, который отстутствует в EventLoop, то оно будет отброшено.
	*/
    void addEvent(Event &&event, int register_id = 0);
	void addEvent(Event &&event, Timer timer, int registrar_id = 0);
	void delEvent(std::string event_type, int registrar_id = 0);
	void sendMessage(std::shared_ptr<IMessage> message);
	/**
		\brief Создает объект класса Connector, связанный с данным EventLoop. 
	*/
    Connector&& createConnector();
	/**
		\brief Запускает работу цикла событий

		Метод запустит работу цикла событий в потоке, в котором будет сделан вызов этого метода.
	*/
    void run();
	/**
		\brief Останавливает работу цикла событий
	*/
    void stop();
	EventLoopImpl* Pimpl();
private:
	int last_id_connector;
	EventLoopImpl *data;
	/**/
};

#endif