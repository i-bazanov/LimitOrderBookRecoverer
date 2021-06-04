#pragma once

#include "OrderBook.h"

#include <vector>
#include <fstream>

class ParserToVector
{
public:
	ParserToVector(const char* inputPath);

	~ParserToVector();

	bool parse();

	const std::vector<OrderBook>& getLevelUpdates() const;

private:
    // Members
	const char* m_inputPath;
	std::ifstream m_input;
	std::vector<OrderBook> m_LevelUpdates; // zero element -> zero limit order book, other elements -> level updates
};
