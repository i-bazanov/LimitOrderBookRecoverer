#include "ParserToVector.h"
#include "WriterFromVector.h"

#include "ParserToHashTable.h"
#include "WriterFromHashTable.h"

#include <iostream>
#include <ctime>

int main() 
{
    std::cout << "Recoverer is running ..." << std::endl;
    std::cout << std::endl;
    clock_t start, end;

    {
        start = clock();
        ParserToVector parser("..\\huobi_dm_depth.log");

        if (!parser.parse())
            return 0;

        end = clock();
        std::cout << "Parsing to vector run time = " << (end - start) << " msec" << std::endl;


        start = clock();
        WriterFromVector writer;

        if (!writer.write(parser.getLevelUpdates()))
            return 0;

        end = clock();
        std::cout << "Writing from vector run time = " << (end - start) << " msec" << std::endl;
    }

    std::cout << std::endl;

    {
        start = clock();
        ParserToHashTable parser("..\\huobi_dm_depth.log");

        if (!parser.parse())
            return 0;

        end = clock();
        std::cout << "Parsing to hash table run time = " << (end - start) << " msec" << std::endl;


        start = clock();
        WriterFromHashTable writer;

        if (!writer.write(parser.getLevelUpdates()))
            return 0;

        end = clock();
        std::cout << "Writing from hash table run time = " << (end - start) << " msec" << std::endl;
    }

    return 0;
}
