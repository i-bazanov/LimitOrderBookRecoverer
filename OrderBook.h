#pragma once

#include <string>
#include <map>

struct OrderBook
{
    std::map<std::string, std::string> currentAsks; // key -> price orders, value -> amount orders
    std::map<std::string, std::string> currentBids; // key -> price orders, value -> amount orders
    std::string currentTime;
};
