#ifndef _EVENT_H_
#define _EVENT_H_
#include <memory>
#include <vector>
#include <functional>
#include "IMessage.h"
//\authors Ageev Aleksey

/**
	\brief События в EventLoop. Содержит тип и обработчики
*/


class Event {
public:
	/**
		\brief создание события
		\param[in] event_type Тип события, по которому будет определяться на кокое сообщение реагировать
	*/
	Event(std::initializer_list<std::function<IEventHandler>> handler, std::string event_type);
	Event(std::string event_type);
	~Event();
	/**
		\brief Добавлени обработчика к событию
		\param[in] Указатель на функцию обработчик
	*/
	void addHandler(std::function<IEventHandler> handler);
	/**
		\brief Удаление конкретного обработчика у события
		\param[in] Указатель на функцию обработчик
	*/
	void removeHandler(std::function<IEventHandler> handler);
	/**
		\brief последовательный запуск всех обработчиков
		\param[in] message Сообщение передаваемое обработчику
	*/
	std::shared_ptr<IMessage> execute(std::shared_ptr<IMessage> message) const;
	//Возвращает тип события
	std::string getType() const;
private:
	std::vector<std::function<IEventHandler>> handlers;
	std::string type;
};

#endif
