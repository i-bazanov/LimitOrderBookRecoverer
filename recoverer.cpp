#include "Parser.h"

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Recoverer is running ..." << std::endl;

    long long sumAllAverageOrderBookUpdateTime(0);
    size_t launchesNumber = 100;

    for (size_t i = 0; i < launchesNumber; ++i)
    {
        Parser parser("..\\huobi_dm_depth.log");

        sumAllAverageOrderBookUpdateTime += parser.getAverageOrderBookUpdateTime();

        std::cout << '\r' << i << " %";
    }
    std::cout << '\r' << "100 %";
    std::cout << '\r' << "Average value of average order book update time on all dataset = " << sumAllAverageOrderBookUpdateTime / launchesNumber << " nsec" << std::endl;

    return 0;
}
