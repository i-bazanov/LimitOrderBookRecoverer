#pragma once

#include "OrderBook.h"

#include <vector>
#include <fstream>

class WriterFromVector
{
public:
	WriterFromVector(const char* outputPath = "..\\resultsVector.txt");

	~WriterFromVector();

	bool write(const std::vector<OrderBook>& m_LevelUpdates);

private:
	// Methods
	void writeCurrentOrderBook();

    // Members
	std::ofstream m_output;
	OrderBook m_currentOrderBook;
};
