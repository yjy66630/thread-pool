#pragma once
#include <functional>

using namespace std;
/*
利用RAII原理生成简易包装器
*/
class Raii
{
public:
	using fun = function<void(void)>;

	Raii(fun construct = nullptr, fun destory = nullptr) {
		if (construct) {
			construct();
		}
		if (destory) {
			m_destory = destory;
		}
	}
	~Raii() {
		if (m_destory) {
			m_destory();
		}
	};
private:
	Raii() = delete;
	Raii(const Raii&) = delete;
	Raii(Raii&&) = delete;
	Raii operator=(const Raii&) = delete;
	Raii operator=(Raii&&) = delete;

	fun m_destory;
};