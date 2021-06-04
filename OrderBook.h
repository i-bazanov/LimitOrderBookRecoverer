#pragma once

#include <map>
#include <string>

struct OrderBook
{
    std::map<double, int> currentAsks; // key -> price order, value -> amount orders
    std::map<double, int> currentBids; // key -> price order, value -> amount orders
    std::string currentTime;
};
