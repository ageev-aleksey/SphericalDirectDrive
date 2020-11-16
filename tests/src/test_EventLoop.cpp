#include "EventLoop.h"
#include "IMessage.h"
#include "gtest/gtest.h"
#include <memory>
#include <thread>
#include <string>




class testMessage : public IMessage {
public:
	testMessage() : IMessage("-1") {
	}
};
volatile int i = 0;
std::shared_ptr<IMessage> dummy_handler(std::shared_ptr<IMessage> msg) {
	i++;
	return msg;
}



void thread_create(std::shared_ptr<Connector> connector, std::string id) {
	for (int j = 0; j < 10000; j++) {
		connector->registerEvent(dummy_handler, std::to_string(i));
	}
}




void thread_send(std::shared_ptr<Connector> connector) {
	for (int j = 0; j < 10000; j++) {
		connector->sendMessage(std::make_shared<testMessage>());
	}
	
}


void thread_stop(std::shared_ptr<EventLoop> loop_ptr) {
	while (i < 20000) {
		if ((i % 100) == 0) {
			std::cout << std::dec << i << std::endl;
		}
	}
	std::cout << "stop loop: " << i << std::endl;
	loop_ptr->stop();
}

TEST(TestEventLoop, CreateAndSendMessage) {
	std::shared_ptr<EventLoop> eloop = EventLoop::create();
	eloop->addEvent(Event({ dummy_handler }, std::string("-1")));
	std::thread thr1(thread_create, eloop->createConnector(), std::string("0"));
	std::thread thr2(thread_create, eloop->createConnector(), std::string("0"));
	std::thread thr3(thread_send, eloop->createConnector());
	std::thread thr4(thread_send, eloop->createConnector());
	std::thread thr5(thread_stop, eloop);
	eloop->run();
	std::cout << "ProcessStop" << std::endl;
	thr1.join();
	thr2.join();
	thr3.join();
	thr4.join();
	thr5.join();
}