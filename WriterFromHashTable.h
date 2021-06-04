#pragma once

#include "OrderBook.h"

#include <unordered_map>
#include <fstream>

class WriterFromHashTable
{
public:
	WriterFromHashTable(const char* outputPath = "..\\resultsHashTable.txt");

	~WriterFromHashTable();

	bool write(const std::unordered_map<int, OrderBook>& m_vecLevelUpdates);

private:
	// Methods
	void writeCurrentOrderBook();

    // Members
	std::ofstream m_output;
	OrderBook m_currentOrderBook;
};
