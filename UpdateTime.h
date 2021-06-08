#pragma once

#include <chrono>

struct UpdateTime
{
	UpdateTime();

	~UpdateTime();

	std::chrono::nanoseconds totalOrderBookUpdateTime;
	size_t orderBookUpdatesAmount;
};
