#pragma once

#include "OrderBook.h"

#include <vector>
#include <fstream>

class WriterFromVector
{
public:
	WriterFromVector(const char* outputPath = "..\\results.txt");

	~WriterFromVector();

	bool write(const std::vector<OrderBook>& m_vecLevelUpdates);

private:
	// Methods
	void writeCurrentOrderBook();

    // Members
	std::ofstream m_output;
	OrderBook m_currentOrderBook;
};
