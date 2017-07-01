#pragma once
#include <chrono>

class StopWatch
{
private:
	std::chrono::system_clock::time_point started_at_;

public:
	StopWatch()
	{
		this->initialize();
	};
	void initialize()
	{
		this->started_at_ = std::chrono::system_clock::now();
	};

	long long stop()
	{
		auto now = std::chrono::system_clock::now();
		auto elapsed = (now - this->started_at_);

		return elapsed.count();
	}
};
