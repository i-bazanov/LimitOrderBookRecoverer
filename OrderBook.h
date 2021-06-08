#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <array>

struct OrderBook_tree
{
    std::map<double, int> currentAsks; // key -> price order, value -> amount orders
    std::map<double, int> currentBids; // key -> price order, value -> amount orders
    std::string currentTime;
};

struct OrderBook_hashTable
{
    std::unordered_map<double, int> currentAsks; // key -> price order, value -> amount orders
    std::unordered_map<double, int> currentBids; // key -> price order, value -> amount orders
    std::string currentTime;
};
