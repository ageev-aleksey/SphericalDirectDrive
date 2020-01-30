#include "IMessage.h"
#include <memory>
#include <iostream>


IMessage::IMessage(std::string _type) : type(_type)
{}

IMessage::~IMessage()
{}
/*
IMessage::IMessage(const IMessage &message) {
	type = message.type;
}
*/

std::string IMessage::getType() {
	return type;
}