#ifndef _STATE_H_
#define _STATE_H_
#include "package.h"
class State : public Package {
public:
	State();
	State(const std::vector<Package::byte_t> &bin_buff);
	//void fromBinary(std::vector<Package::byte_t> bin_buff);
	~State();

	Package::byte_t state();
	void setState(Package::byte_t s);

	short OX();
	void  setOX(short x);

	short OY();
	void setOY(short y);

	short PWMX();
	void setPWMX(short pwmx);

	short PWMY();
	void setPWMY(short pwmy);

	short positionX();
	void setPositionX(short pos);

	short positionY();
	void setPositionY(short pos);

	 Package::byte_t  randomValue();

	// Package::byte_t getHash();
private:
    void create_all_fields();


};
#endif