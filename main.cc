#include "thread_pool.h"
#include "gtest/gtest.h"
#include <Windows.h>

using namespace std;

recursive_mutex fun_mutex;

void add(int& ans, int x, int y)
{
	lock_guard<recursive_mutex> lock(fun_mutex);
	ans = x + y;
	cout << "begin: " << __FUNCTION__ << ", " << ans << endl;
	Sleep(1000);
	cout << "end: " << __FUNCTION__ << ", " << ans << endl;
	return;
}

class TestThreadPool : public testing::Test
{
};

TEST_F(TestThreadPool, Test1) {
	ThreadPool pool(2);
	pool.init();
	int ans = 0;
	auto f1 = pool.submit(add, ans, 1, 2);
	auto f2 = pool.submit(add, ans, 3, 4);
	auto f3 = pool.submit(add, ans, 5, 6);
	pool.shutdown();
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}