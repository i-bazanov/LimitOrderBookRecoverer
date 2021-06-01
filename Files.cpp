#include "Files.h"

Files::Files(const char* inputPath, const char* outputPath) : inputFile(inputPath), outputFile(outputPath) {}

Files::~Files()
{
	inputFile.close();
	outputFile.close();
}
