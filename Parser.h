#pragma once

#include "Files.h"
#include "OrderBook.h"
#include "UpdateTime.h"

#define FormatOrderBook 0 // 1 -> tree, 0 -> hash table

class Parser
{
public:
	Parser(const char* inputPath, const char* outputPath = "..\\results.txt");

	~Parser();

	long long getAverageOrderBookUpdateTime() const;

private:
	// methods
	int sizeFileCalculate();
	void fullReadFile();
	void updateAllOrderBooks();
	void inputBestValue();
	void fastParse();
	//void parse();

    // members
	Files m_files;
	std::string m_fileString;
	UpdateTime m_updateTime;

#if FormatOrderBook
	OrderBook_tree m_limitOrderBook;
#else
	OrderBook_hashTable m_limitOrderBook;
#endif
};
