#include "thread_pool.h"

ThreadPool::ThreadPool(int maxThreadSize)
{
	m_thread.resize(maxThreadSize);
	m_maxThreadSize = maxThreadSize;
}

void ThreadPool::init()
{
	m_state = RUNNING;
	for (auto& i : m_thread)
	{
		i = thread(&ThreadPool::worker, this);
	}
}

void ThreadPool::worker()
{
	function<void()> func;
	while (m_state == RUNNING)
	{
		bool ifDequeue;
		{
			unique_lock<mutex> lock(m_mutex);
			if (m_queue.empty())
			{
				//m_cv.wait(lock);
				break;
			}
			ifDequeue = m_queue.dequeue(func);
		}
		if (ifDequeue)
		{
			func();
		}
	}
}

void ThreadPool::shutdown()
{
	m_cv.notify_all();
	for (thread& th : m_thread)
	{
		if (th.joinable())
		{
			th.join();
		}
	}
	m_state = SHUTDOWN;
}
