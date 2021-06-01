#pragma once

#include "Files.h"
#include "OrderBook.h"

class Parser
{
public:
	Parser(const char* inputPath, const char* outputPath = "..\\results.txt");

	~Parser();

private:
	// methods
	void parse();

    // members
	Files m_files;
	OrderBook m_limitOrderBook;
};
