#include "Parser.h"

#include <iostream>
#include <ctime>

int main() 
{
    std::cout << "Recoverer is running ..." << std::endl;

    clock_t start = clock();

    Parser parser("..\\huobi_dm_depth.log");

    clock_t end = clock();

    std::cout << "Run time = " << (end - start) / 1000 << " sec" << std::endl;

    return 0;
}
