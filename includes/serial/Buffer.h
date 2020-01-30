#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <list>
#include <string>
#include "Mutex.h"
#include <memory>


class Buffer {
public:
	Buffer();
	void push_back(std::string &str);
	std::string pop_front();

private:
	struct Node {
		Mutex mutex;
		std::string buffer;
		Node();
		Node(Node &&obj) = delete;
		Node(const Node &obj);
	};
	std::list<std::shared_ptr<Node>>::iterator get_front();
	std::list<std::shared_ptr<Node>>::iterator get_end();
	std::list<std::shared_ptr<Node>> data;
	Mutex lock_when_empty;
};


#endif