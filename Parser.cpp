#include "Parser.h"

#include <string>
#include <vector>
#include <iostream>

Parser::Parser(const char* inputPath, const char* outputPath): m_files(inputPath, outputPath) 
{
	parse();
}

Parser::~Parser() {};

void Parser::parse()
{
    size_t currentTimeStartIndex = -1, currentTimeFinishIndex = -1;

    size_t currentAsksStartIndex = -1, currentAsksFinishIndex = -1;
    std::string currentAsksString;

    std::vector<size_t> lhsBracketArray;
    std::vector<size_t> rhsBracketArray;
    std::vector<size_t> commaArray;

    size_t currentBidsStartIndex = -1, currentBidsFinishIndex = -1;
    std::string currentBidsString;

    std::string currentLevel;

    if (m_files.inputFile.is_open())
    {
        while (getline(m_files.inputFile, currentLevel))
        {
            if (currentLevel.find("ping") != -1)
                continue;

            //////////////////////////////////////////////////////////////////////////////
            ///////////////////////////// Current level time /////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            currentTimeStartIndex = currentLevel.find("current time: ") + std::size("current time: ") - 1;
            currentTimeFinishIndex = currentLevel.find(",");
            m_limitOrderBook.currentTime = currentLevel.substr(currentTimeStartIndex, currentTimeFinishIndex - currentTimeStartIndex);

            //////////////////////////////////////////////////////////////////////////////
            ///////////////////////////// Current level asks /////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            currentAsksStartIndex = currentLevel.find("\"asks\":") + std::size("\"asks\":") - 1;
            currentAsksFinishIndex = currentLevel.find(",\"bids\":");
            currentAsksString = currentLevel.substr(currentAsksStartIndex, currentAsksFinishIndex - currentAsksStartIndex);

            lhsBracketArray.clear();
            rhsBracketArray.clear();
            commaArray.clear();

            for (size_t i = 1; i + 1 < currentAsksString.size(); ++i)
            {
                if ('[' == currentAsksString[i])
                    lhsBracketArray.push_back(i);
                else if (']' == currentAsksString[i])
                    rhsBracketArray.push_back(i);
                else if (',' == currentAsksString[i])
                    commaArray.push_back(i);
            }

            for (size_t i = 0; i < lhsBracketArray.size(); ++i)
            {
                if (currentAsksString.substr(commaArray[2 * i] + 1, rhsBracketArray[i] - commaArray[2 * i] - 1) != "0")
                    m_limitOrderBook.currentAsks[currentAsksString.substr(lhsBracketArray[i] + 1, commaArray[2 * i] - lhsBracketArray[i] - 1)] = currentAsksString.substr(commaArray[2 * i] + 1, rhsBracketArray[i] - commaArray[2 * i] - 1);
                else
                    m_limitOrderBook.currentAsks.erase(currentAsksString.substr(lhsBracketArray[i] + 1, commaArray[2 * i] - lhsBracketArray[i] - 1));
            }

            //Print current ASKS limit order book
            /*for (const auto& pair : currentAsks)
            {
                std::cout << pair.first << " : " << pair.second << std::endl;
            }

            std::cout << std::endl;
            std::cout << std::endl;*/

            ///////////////////////////////////////////////////////////////////////////////
            ///////////////////////////// Current level bids //////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////
            currentBidsStartIndex = currentLevel.find("\"bids\":") + std::size("\"bids\":") - 1;
            currentBidsFinishIndex = currentLevel.rfind(",\"ch\":");
            currentBidsString = currentLevel.substr(currentBidsStartIndex, currentBidsFinishIndex - currentBidsStartIndex);

            lhsBracketArray.clear();
            rhsBracketArray.clear();
            commaArray.clear();

            for (size_t i = 1; i + 1 < currentBidsString.size(); ++i)
            {
                if ('[' == currentBidsString[i])
                    lhsBracketArray.push_back(i);
                else if (']' == currentBidsString[i])
                    rhsBracketArray.push_back(i);
                else if (',' == currentBidsString[i])
                    commaArray.push_back(i);
            }

            for (size_t i = 0; i < lhsBracketArray.size(); ++i)
            {
                if (currentBidsString.substr(commaArray[2 * i] + 1, rhsBracketArray[i] - commaArray[2 * i] - 1) != "0")
                    m_limitOrderBook.currentBids[currentBidsString.substr(lhsBracketArray[i] + 1, commaArray[2 * i] - lhsBracketArray[i] - 1)] = currentBidsString.substr(commaArray[2 * i] + 1, rhsBracketArray[i] - commaArray[2 * i] - 1);
                else
                    m_limitOrderBook.currentBids.erase(currentBidsString.substr(lhsBracketArray[i] + 1, commaArray[2 * i] - lhsBracketArray[i] - 1));
            }

            //Print current BIDS limit order book
            /*for (const auto& pair : currentBids)
            {
                std::cout << pair.first << " : " << pair.second << std::endl;
            }

            std::cout << std::endl;
            std::cout << std::endl;*/

            ///////////////////////////////////////////////////////////////////////
            ///////////////////////////// File output /////////////////////////////
            ///////////////////////////////////////////////////////////////////////
            if (m_files.outputFile.is_open())
            {
                m_files.outputFile << "" << m_limitOrderBook.currentTime << ", "
                    << m_limitOrderBook.currentBids.rbegin()->first << ", " << m_limitOrderBook.currentBids.rbegin()->second << ", "
                    << m_limitOrderBook.currentAsks.begin()->first << ", " << m_limitOrderBook.currentAsks.begin()->second << ""
                    << std::endl;
            }
            else
            {
                std::cout << "Cannot open to output file" << std::endl;
                return;
            }
        }
    }
    else
    {
        std::cout << "Cannot open to input file" << std::endl;
        return;
    }
}
