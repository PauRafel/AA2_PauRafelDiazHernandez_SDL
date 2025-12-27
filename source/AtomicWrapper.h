#pragma once
#include <atomic>

//https://stackoverflow.com/questions/13193484/how-to-declare-a-vector-of-atomic-in-c

template<typename T>
struct AtomicWrapper
{
	std::atomic<T> _a;

	AtomicWrapper() : _a() {}
	AtomicWrapper(const std::atomic<T>& a) : _a(a.load()) {}
	AtomicWrapper(const AtomicWrapper& other) : _a(other._a.load()) {}

	AtomicWrapper& operator=(const AtomicWrapper& other)
	{
		_a.store(other._a.load());
		return *this;
	}
};
