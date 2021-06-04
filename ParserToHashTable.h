#pragma once

#include "OrderBook.h"

#include <unordered_map>
#include <fstream>

class ParserToHashTable
{
public:
	ParserToHashTable(const char* inputPath);

	~ParserToHashTable();

	bool parse();

	const std::unordered_map<int, OrderBook>& getLevelUpdates() const;

private:
    // Members
	std::ifstream m_input;
	// Key -> number element, Value -> OrderBook
	std::unordered_map<int, OrderBook> m_LevelUpdates; // zero element -> zero limit order book, other elements -> level updates
};
