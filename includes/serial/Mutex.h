#ifndef _MUTEX_H_
#define _MUTEX_H_

class MutexImpl;

class Mutex {
public:
	Mutex();
	~Mutex();
	void lock();
	void release();
	
private:
	MutexImpl *pimpl;
};



#endif