#include "semaphore.h"
#include "raii.h"
#include "TSQueue.h"
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>
#include "gtest/gtest.h"

using namespace std;

/***
 * @brief : 包装信号量的测试函数
 * @param num : 信号量允许的同时运行的最大线程数
 * @param max_thread : 共需要运行的最大线程数
 */
class Foo
{
public:
	Foo(int num, int max_thread) {
		m_num = num;
		m_sem.setCount(m_num);
		m_max_thread = max_thread;
	}
	void run();
private:
	void fun();
	int m_num;
	int m_max_thread;
	Semaphore m_sem;
};

void Foo::fun() {
	mutex mutex_;
	Raii raii([&]() {
		m_sem.wait();
		},
		[&]() {
			m_sem.require();
		});
	lock_guard<mutex> lock(mutex_);
	this_thread::sleep_for(chrono::milliseconds(2000));
	cout << this_thread::get_id() << endl;
};

void Foo::run() {
	vector<thread> th;

	for (int i = 0; i < m_max_thread; i++) {
		th.emplace_back(&Foo::fun, this);
	}
	for (auto& th_ : th) {
		th_.join();
	}
}

class TestSemaphore : public testing::Test
{
protected:
	Semaphore sem;
};


TEST_F(TestSemaphore, Test1) {
	Foo foo(4, 7);
	foo.run();
}

TEST_F(TestSemaphore, Test2) {
	Foo foo(4, 1);
	foo.run();
}

TEST_F(TestSemaphore, Test3) {
	Foo foo(1, 4);
	foo.run();
}

TEST_F(TestSemaphore, Test4) {
	Foo foo(4, 4);
	foo.run();
}

TEST_F(TestSemaphore, TestError) {
	Semaphore sem;
	EXPECT_THROW(sem.require(), out_of_range);
	EXPECT_THROW(sem.wait(), out_of_range);
	sem.setCount(-1);
	EXPECT_THROW(sem.wait(), out_of_range);
}

class TestTSQueue: public testing::Test
{
};

TEST_F(TestTSQueue, testQueue) {
	TSQueue<int> queue;
	vector<int> numbers(0, 100);
	thread th1([&]() {
		for (auto& number : numbers) {
			queue.dequeue(number);
			cout << "dequeue " << number << endl;
		}
		});
	thread th2([&]() {
		for (int i = 0; i < (int)(numbers.size()); i++) {
			queue.enqueue(i);
			cout << "enqueue " << i << endl;
		}
		});
	th1.join();
	th2.join();
	EXPECT_EQ(queue.empty(), true);
}