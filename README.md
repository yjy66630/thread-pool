# thread-pool

一个非常简单的线程池实现

> 依赖：[google-test](https://github.com/google/googletest)

本项目用来简单学习c++11的多线程库。

main.cc: 
- 主程序，测试线程池功能
test.cc:
- 由 main.cc 调用，通过使用信号量的方式创建线程池
taii.h:
- RAII方法
semaphore.h:
- 构造了一个简易的信号量