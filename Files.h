#pragma once

#include <fstream>

struct Files
{
	Files(const char* inputPath, const char* outputPath);

	~Files();

	std::ifstream inputFile;
	std::ofstream outputFile;
};
