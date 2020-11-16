#include "Mutex.h"
#include <gtest/gtest.h>
#include <windows.h>

struct ThreadParametrs {
	Mutex m;
	int value;
};


DWORD WINAPI ThreadProc(_In_ LPVOID lpParametr) {
	ThreadParametrs  *m = reinterpret_cast<ThreadParametrs*>(lpParametr);
	m->m.lock();
	m->value++;
	m->m.release();
	return 0;
}

TEST(MutextTest, TestLockInThread) {
	ThreadParametrs param;
	param.value = 0;
	HANDLE threads[2];
	threads[0] = CreateThread(NULL, 0, ThreadProc, &param, 0, NULL);
	threads[1] = CreateThread(NULL, 0, ThreadProc, &param, 0, NULL);
	WaitForMultipleObjects(2, threads, TRUE, INFINITE);
	ASSERT_EQ(param.value, 2);
}


TEST(MutextTest, TestLock) {
	Mutex m;
	m.lock();
	m.release();
	std::cout << "OK!" << std::endl;
}