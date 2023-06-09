#pragma once

#include <mutex>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <cassert>

using namespace std;
/*
信号量
*/
class Semaphore {
public:
	Semaphore() : ifSetCount(false) {};
	void setCount(int count) { m_count = count; ifSetCount = true; }
	bool require(); // 信号量自增
	bool wait(); // 信号量自减
private:
	int m_count;
	bool ifSetCount;
	mutex m_mutex;
	condition_variable m_cv;
};

bool Semaphore::require() {
	lock_guard<mutex> lock(m_mutex);
	if (!ifSetCount) {
		out_of_range err("请先调用setCount函数设置同时运行的最大线程数\n");
		throw err;
	}
	m_count++;
	m_cv.notify_one();
	return m_count;
}

bool Semaphore::wait() {
	unique_lock<mutex> lock(m_mutex);
	if (!ifSetCount) {
		out_of_range err("请先调用setCount函数设置同时运行的最大线程数\n");
		throw err;
	}
	if (m_count <= -1) {
		out_of_range err("最大线程数范围错误\n");
		throw err;
	}
	m_cv.wait(lock, [&]() {
		return m_count > 0;
		});
	m_count--;
	return m_count;
}