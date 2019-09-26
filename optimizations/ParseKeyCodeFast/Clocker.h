#pragma once
#include <chrono>

// T iselementof { std::chrono::steady_clock, std::chrono::high_resolution_clock }.

template<class T=std::chrono::steady_clock>
struct Clocker {
	typename T::time_point start;

	Clocker() : start{T::now()} {}
	
	long long get() const {
		auto const end = T::now();
		auto const dt_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		return dt_ms;
	}
	
	void reset() {
		start = T::now();
	}
};
