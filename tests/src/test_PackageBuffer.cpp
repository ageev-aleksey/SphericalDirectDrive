//
// Created by Aleksey on 15.03.2020.
//
#include "gtest/gtest.h"
#include "State.h"
#include "PackageBuffer.h"
#include "boost/circular_buffer.hpp"



TEST(TestPackageBuffer, TestFindPackage) {
    State s;
    s.setPWMX(232);
    s.setPWMY(488);
    s.setPositionX(10);
    s.setPositionY(240);
    s.setOY(100);
    s.setOX(111);
    auto data = s.toBinary();
    PackageBuffer buffer(s.size()+4);
    buffer.addBytes({20, 143, 19});
    buffer.addBytes(data);
	//std::cout << std::endl << buffer << std::endl << buffer.buffer.end() - buffer.buffer.begin() ;
	//std::cout << std::endl;
   State sp = buffer.formPackage();
    ASSERT_EQ(sp.PWMX(), s.PWMX());
    ASSERT_EQ(sp.PWMY(), s.PWMY());
    ASSERT_EQ(sp.positionX(), s.positionX());
    ASSERT_EQ(sp.positionY(), s.positionY());
    ASSERT_EQ(sp.OX(), s.OX());
	ASSERT_EQ(sp.OY(), s.OY());
}


TEST(TestPackageBuffer, TestFindPackage2) {
	State s1;
	s1.setPWMX(232);
	s1.setPWMY(488);
	s1.setPositionX(10);
	s1.setPositionY(240);
	s1.setOY(100);
	s1.setOX(111);
	State s2;
	s2.setPWMX(11);
	s2.setPWMY(11);
	s2.setPositionX(11);
	s2.setPositionY(11);
	s2.setOY(11);
	s2.setOX(11);
	auto data1 = s1.toBinary();
	auto data2 = s2.toBinary();
	PackageBuffer buffer(s1.size() + 5);
	buffer.addBytes(data1);
	buffer.addBytes(data2);
	std::cout << std::endl << buffer << std::endl << buffer.buffer.end() - buffer.buffer.begin() ;
	std::cout << std::endl;
	State sp = buffer.formPackage();
	ASSERT_EQ(sp.PWMX(), s2.PWMX());
	ASSERT_EQ(sp.PWMY(), s2.PWMY());
	ASSERT_EQ(sp.positionX(), s2.positionX());
	ASSERT_EQ(sp.positionY(), s2.positionY());
	ASSERT_EQ(sp.OX(), s2.OX());
	ASSERT_EQ(sp.OY(), s2.OY());
}
/*
class Integer {
public:
    Integer( int _v) {
        v =  _v;
    }
    Integer(const Integer &_v) {
        v =  _v.v;
    }
	Integer(Integer &&_v) {
		_v.is_move = true;
		v = _v.v;
	}
    Integer() {
        v = 0;
    }
    ~Integer() {
		if(!is_move)
			std::cout << "destructor: " << v << std::endl;
    }
    Integer& operator=(const Integer &_v) {
		std::cout << v << " <-- " << _v.v;
		v = _v.v;
		return *this;
    }

    int v;
	bool is_move;
};


TEST(TestCircuitBuffer, CircuitBuffer) {
    boost::circular_buffer<Integer> cb(3);
    ASSERT_EQ(cb.begin(), cb.end());
    cb.push_back(1);
    cb.push_back(2);
    std::cout << std::endl;
    for(const auto &el : cb) {
        std::cout << el.v << "-";
    }
    std::cout << std::endl;
    cb.push_back(3);

    std::cout << std::endl;
    for(const auto &el : cb) {
        std::cout << el.v << "-";
    }
    std::cout << std::endl;

    cb.push_back(4);

    std::cout << std::endl;
    for(const auto &el : cb) {
        std::cout << el.v << "-";
    }
    std::cout << std::endl;
}*/