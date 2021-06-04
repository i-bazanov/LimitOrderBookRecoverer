#include "ParserToVector.h"
#include "WriterFromVector.h"

#include <iostream>
#include <ctime>

int main() 
{
    std::cout << "Recoverer is running ..." << std::endl;
    clock_t start, end;

    {
        start = clock();
        ParserToVector parser("..\\huobi_dm_depth.log");

        if (!parser.parse())
            return 0;

        end = clock();
        std::cout << "Parsing to vector run time = " << (end - start) / 1000 << " sec" << std::endl;


        start = clock();
        WriterFromVector writer;

        if (!writer.write(parser.getVecLevelUpdates()))
            return 0;

        end = clock();
        std::cout << "Writing from vector run time = " << (end - start) / 1000 << " sec" << std::endl;
    }

    return 0;
}
