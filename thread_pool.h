#pragma once

#include <future>
#include <vector>
#include <thread>
#include <functional>
#include "TSQueue.h"

using namespace std;

enum ThreadPoolState
{
	RUNNING,
	SHUTDOWN
};

class ThreadPool
{
public:
	/***
	 * @brief : 线程池的初始化函数
	 * @param maxThreadSize : 线程池的最大并行线程数量
	 */
	explicit ThreadPool(int maxThreadSize);

	ThreadPool() = delete;

	/***
	@brief 将函数提交到线程池中，等待执行
	@return 通过future包装的函数句柄，使得可以在恰当的时候计算出结果
	*/
	template<typename F, typename... Args>
	shared_ptr<packaged_task<void()>> submit(F f, Args... args);

	/***
	@brief 线程池的初始化函数
	*/
	void init();

	/**
	 * @brief 工作用
	 * */
	void worker();

	/***
	@brief 线程池的关闭函数
	*/
	void shutdown();

private:
	vector<thread> m_thread; // 工作中的线程
	TSQueue<function<void()>> m_queue; // 线程队列
	int m_maxThreadSize; // 最大线程数量
	ThreadPoolState m_state;
	mutex m_mutex;
	condition_variable m_cv;
};

template<typename F, typename... Args>
shared_ptr<packaged_task<void()>> ThreadPool::submit(F f, Args... args)
{
	if (m_state != RUNNING)
	{
		throw runtime_error("提交任务时线程池状态必须为RUNNING");
	}
	unique_lock<mutex> lock(m_mutex);
	m_cv.wait(lock, [&]()
		{
			return m_queue.size() <= m_maxThreadSize;
		});
	// 将任务函数转换为function包装器的形式
	function<decltype(f(args...))()> func = bind(forward<F>(f), forward<Args>(args)...);
	// 这是为了方便在worker里面调用，因为在wrapper_func中难以直接调用func
	shared_ptr < packaged_task<decltype(f(args...))()>> task_ptr = make_shared<packaged_task<decltype(f(args...))()>>(func);
	// 但是队列中为 function<void()> 的形式，这是为了统一格式
	function<void()> wrapper_func = [&task_ptr]()
	{
		(*task_ptr)();
	};

	m_queue.enqueue(wrapper_func);
	m_cv.notify_one();
	return task_ptr;
}