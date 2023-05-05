#pragma once
#include <queue>
#include <iostream>
#include <mutex>

using namespace std;

/*
一个线程安全的队列
*/
template <typename T>
class TSQueue
{
public:
	TSQueue(){};
	~TSQueue() {
		if (!this->empty()) {
			cout << "队列析构时不为空" << endl;
		}
	}
	/*
	线程安全必须传递引用参数或返回指针
	*/
	bool dequeue(T& t);
	/*
	线程安全必须传递引用参数或返回指针
	*/
	void enqueue(T& t);
	bool empty();
	int size();
private:
	queue<T> m_queue;
	// 必须使用 recursive_mutex，而不能用 mutex，因为会重复获取多次锁
	recursive_mutex m_mutex;
};



template<typename T>
bool TSQueue<T>::dequeue(T& t) {
	unique_lock<recursive_mutex> lock(m_mutex);
	if (this->empty()) {
		return false;
	}
	t = move(m_queue.front());
	m_queue.pop();
	return true;
}

template<typename T>
void TSQueue<T>::enqueue(T& t) {
	unique_lock<recursive_mutex> lock(m_mutex);
	m_queue.emplace(t);
}

template<typename T>
bool TSQueue<T>::empty() {
	unique_lock<recursive_mutex> lock(m_mutex);
	if (m_queue.empty()) {
		return true;
	}
	return false;
}

template<typename T>
int TSQueue<T>::size() {
	unique_lock<recursive_mutex> lock(m_mutex);
	return (int)(m_queue.size());
}