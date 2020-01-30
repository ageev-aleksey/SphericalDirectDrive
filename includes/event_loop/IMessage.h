//\authors Ageev Aleksey

#ifndef _IMESSAGE_H_
#define _IMESSAGE_H_
#include <string>
#include <memory>



/**
	\brief  Интерфейс сообщений, которые будут отправляться в EventLoop

	Для отправки сообщений, необходимо реализовать данный интерфейс и метод GetType должен
	возвращать тип события. По нему будет определяться какой обработчик вызывать для этого сообщения

*/
class IMessage {
public:
	IMessage(std::string _type);
	//IMessage();
	virtual ~IMessage();
	std::string getType();
private:
	std::string type;
};

using IEventHandler = std::shared_ptr<IMessage> (std::shared_ptr<IMessage> message);

//typedef std::shared_ptr<IMessage>(*IEventHandler)(std::shared_ptr<IMessage> message);

#endif
