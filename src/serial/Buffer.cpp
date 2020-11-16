#include "Buffer.h"



Buffer::Buffer()
{
	data.push_back(std::shared_ptr<Node>(new Node));//Создание терминального элемента
}




void Buffer::push_back(std::string &str) {
	auto end_element = --get_end();
	while (true) {
		(*end_element)->mutex.lock();
		//проверяем, что до блокировки эелемента его не успели удалить или вставить новый элемент в конец
		if (end_element != --get_end()) {
			(*end_element)->mutex.release();
			end_element = get_end();
			continue;
		}
		else {
			break;
		}
	}
	std::shared_ptr<Node> new_el(new Node);
	new_el->buffer = std::move(str);
	new_el->mutex.lock();
	data.push_back(new_el);
	(*end_element)->mutex.release();
	new_el->mutex.release();
	
}

std::string Buffer::pop_front() {
	auto terminal_element = get_front();
	while (true) {
		(*terminal_element)->mutex.lock();
		auto front = terminal_element;
		++front;
		//проверяем, что мы указываем не за конец списка.
		if (front == get_end()) {
			//список содержит только териминальный элемент. Считаем, что он пуст
			return std::string("");
		}
		(*front)->mutex.lock();
		//Проверяем, что этот элемент все еще стоит за терминальным
		if (++get_front() == front) {
			data.erase(front);
			(*front)->mutex.release();
			return (*front)->buffer;
		}
	}
	data.pop_front();
	front_element->mutex.release();
	return front_element->buffer;
}

std::list<std::shared_ptr<Buffer::Node>>::iterator Buffer::get_end() {
	return data.end();
}

std::list<std::shared_ptr<Buffer::Node>>::iterator Buffer::get_front() {

	return data.begin();
}

Buffer::Node::Node() {}

Buffer::Node::Node(const Node& obj) {
	buffer = obj.buffer;
}