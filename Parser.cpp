#include "Parser.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <ios>
#include <chrono>
#include <vector>
#include <algorithm>

#include "rapidjson/document.h"

using Clock = std::chrono::steady_clock;

bool comp(std::pair<double, int> lhs, std::pair<double, int> rhs)
{
    return lhs.first < rhs.first;
}

Parser::Parser(const char* inputPath, const char* outputPath) :
    m_files(inputPath, outputPath),
    m_fileString(sizeFileCalculate(), ' '),
    m_updateTime()
{
    fastParse();
}

Parser::~Parser() {};

int Parser::sizeFileCalculate()
{
    if (m_files.inputFile.is_open())
    {
        m_files.inputFile.seekg(0, std::ios::end);
        int size = m_files.inputFile.tellg();
        m_files.inputFile.seekg(0);
        return size;
    }
    else
    {
        std::cout << "Cannot open to input file in process creating Parser" << std::endl;
        return -1;
    }
}

void Parser::fullReadFile()
{
    m_files.inputFile.read(&m_fileString[0], m_fileString.size());
}

void Parser::updateAllOrderBooks()
{
    // For benchmark order book update
    Clock::time_point start, end;

    size_t currentStartIndex = -1, currentFinishIndex = -1;
    rapidjson::Document doc;

    char* currentLevel = strtok(const_cast<char*>(m_fileString.c_str()), "\n");
    std::string currentLevelString;

    if (currentLevel != NULL)
    {
        m_files.outputFile << std::fixed << std::setprecision(2)
            << "{CURRENT TIME}, {BEST BID PRICE}, {BEST BID AMOUNT}, {BEST ASK PRICE}, {BEST ASK AMOUNT}"
            << std::endl;
    }
    else
    {
        std::cout << "Input file is empty!" << std::endl;
        return;
    }
    
    std::string currentTime;
    bool updateOrderBookTime = false; // for non-accounting of the update of the first order book

    while (currentLevel != NULL)
    {
        currentLevelString = static_cast<std::string>(currentLevel);

        // Update file string
        currentLevel = strtok(NULL, "\n");

        // Ping check 
        if (currentLevelString.find("ping") != -1)
            continue;

        //////////////////////////////////////////////////////////////////////////////
        ///////////////////////// Parsing current level time /////////////////////////
        //////////////////////////////////////////////////////////////////////////////
        currentStartIndex = currentLevelString.find("current time: ") + std::size("current time: ") - 1;
        currentFinishIndex = currentLevelString.find(",");
        currentTime = currentLevelString.substr(currentStartIndex, currentFinishIndex - currentStartIndex);

        //////////////////////////////////////////////////////////////////////////////
        ///////////////////////// Parsing current level asks /////////////////////////
        //////////////////////////////////////////////////////////////////////////////
        currentStartIndex = currentLevelString.find("message: ") + std::size("message: ") - 1;

        currentLevelString = currentLevelString.substr(currentStartIndex, currentLevelString.size() - 1);

        if (doc.Parse<0>(currentLevelString.c_str()).HasParseError())
        {
            std::cout << "Json parsing error!" << std::endl;
            return;
        }

        if (!doc.HasMember("tick"))
        {
            std::cout << "There is no \"tick\" section!" << std::endl;
            return;
        }

        if (!doc["tick"].IsObject())
        {
            std::cout << "Tick is not json object!" << std::endl;
            return;
        }

        if (!doc["tick"].HasMember("asks"))
        {
            std::cout << "There is no \"asks\" section!" << std::endl;
            return;
        }

        if (!doc["tick"]["asks"].IsArray())
        {
            std::cout << "Asks is not array!" << std::endl;
            return;
        }

        auto asksArray = doc["tick"]["asks"].GetArray();

        ///////////////////////////////////////////////////////////////////////////////
        ///////////////////////// Parsing current level bids //////////////////////////
        ///////////////////////////////////////////////////////////////////////////////
        if (!doc["tick"].HasMember("bids"))
        {
            std::cout << "There is no \"bids\" section!" << std::endl;
            return;
        }

        if (!doc["tick"]["bids"].IsArray())
        {
            std::cout << "Bids is not array!" << std::endl;
            return;
        }

        auto bidsArray = doc["tick"]["bids"].GetArray();

        ///////////////////////////////////////////////////////////////////////////////
        ///////////////////////// Update current order book ///////////////////////////
        ///////////////////////////////////////////////////////////////////////////////
        start = Clock::now();

        m_limitOrderBook.currentTime = currentTime;

        for (const auto& askElem : asksArray)
        {
            /*if (!askElem.IsArray())
            {
                std::cout << "Element of asks array is not array!" << std::endl;
                return;
            }*/

            auto ask = askElem.GetArray();

            /*if (!ask[0].IsLosslessDouble())
            {
                std::cout << "Zero element of ask array (price order) is not double!" << std::endl;
                return;
            }

            if (!ask[1].IsInt())
            {
                std::cout << "First element of ask array (amount orders) is not int!" << std::endl;
                return;
            }*/

            if (ask[1].GetInt() != 0)
                m_limitOrderBook.currentAsks[ask[0].GetDouble()] = ask[1].GetInt();
            else
                m_limitOrderBook.currentAsks.erase(ask[0].GetDouble());
        }

        for (const auto& bidElem : bidsArray)
        {
            /*if (!bidElem.IsArray())
            {
                std::cout << "Element of bids array is not array!" << std::endl;
                return;
            }*/

            auto bid = bidElem.GetArray();

            /*if (!bid[0].IsLosslessDouble())
            {
                std::cout << "Zero element of bid array (price order) is not double!" << std::endl;
                return;
            }

            if (!bid[1].IsInt())
            {
                std::cout << "First element of bid array (amount orders) is not int!" << std::endl;
                return;
            }*/

            if (bid[1].GetInt() != 0)
                m_limitOrderBook.currentBids[bid[0].GetDouble()] = bid[1].GetInt();
            else
                m_limitOrderBook.currentBids.erase(bid[0].GetDouble());
        }

        end = Clock::now();

        //// Print current ASKS limit order book
        //for (const auto& pair : m_limitOrderBook.currentAsks)
        //{
        //    std::cout << std::fixed << std::setprecision(2) << pair.first << " : " << pair.second << std::endl;
        //}
        //
        //std::cout << std::endl;
        //std::cout << std::endl;
        //
        //// Print current BIDS limit order book
        //for (const auto& pair : m_limitOrderBook.currentBids)
        //{
        //    std::cout << std::fixed << std::setprecision(2) << pair.first << " : " << pair.second << std::endl;
        //}
        //
        //std::cout << std::endl;
        //std::cout << std::endl;

        if (updateOrderBookTime)
        {
            m_updateTime.totalOrderBookUpdateTime += end - start;
            ++m_updateTime.orderBookUpdatesAmount;
        }
        else
        {
            updateOrderBookTime = true;
        }

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////// File output /////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        inputBestValue();
    }
}

void Parser::inputBestValue()
{
#if FormatOrderBook

    m_files.outputFile << "{" << m_limitOrderBook.currentTime << "}, {"
        << m_limitOrderBook.currentBids.rbegin()->first << "}, {" << m_limitOrderBook.currentBids.rbegin()->second << "}, {"
        << m_limitOrderBook.currentAsks.begin()->first << "}, {" << m_limitOrderBook.currentAsks.begin()->second << "}"
        << std::endl;

#else

    std::vector<std::pair<double, int>> sortedAsks(m_limitOrderBook.currentAsks.begin(), m_limitOrderBook.currentAsks.end());
    std::sort(sortedAsks.begin(), sortedAsks.end(), comp);

    std::vector<std::pair<double, int>> sortedBids(m_limitOrderBook.currentBids.begin(), m_limitOrderBook.currentBids.end());
    std::sort(sortedBids.begin(), sortedBids.end(), comp);

    m_files.outputFile << "{" << m_limitOrderBook.currentTime << "}, {"
        << sortedBids.rbegin()->first << "}, {" << sortedBids.rbegin()->second << "}, {"
        << sortedAsks.begin()->first << "}, {" << sortedAsks.begin()->second << "}"
        << std::endl;

#endif
}

void Parser::fastParse()
{
    if (m_files.inputFile.is_open())
    {
        if (m_files.outputFile.is_open())
        {
            fullReadFile();

            updateAllOrderBooks();
        }
        else
        {
            std::cout << "Cannot open to output file" << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "Cannot open to input file" << std::endl;
        return;
    }
}

long long Parser::getAverageOrderBookUpdateTime() const
{
    return m_updateTime.totalOrderBookUpdateTime.count() / m_updateTime.orderBookUpdatesAmount;
}

//void Parser::parse()
//{
//    if (m_files.inputFile.is_open())
//    {
//        if (m_files.outputFile.is_open())
//        {
//            m_files.outputFile << std::fixed << std::setprecision(2)
//                << "{CURRENT TIME}, {BEST BID PRICE}, {BEST BID AMOUNT}, {BEST ASK PRICE}, {BEST ASK AMOUNT}"
//                << std::endl;
//
//            size_t currentStartIndex = -1, currentFinishIndex = -1;
//            std::string currentLevel;
//            rapidjson::Document doc;
//
//            while (getline(m_files.inputFile, currentLevel))
//            {
//                // Ping check 
//                if (currentLevel.find("ping") != -1)
//                    continue;
//
//                //////////////////////////////////////////////////////////////////////////////
//                ///////////////////////////// Current level time /////////////////////////////
//                //////////////////////////////////////////////////////////////////////////////
//                currentStartIndex = currentLevel.find("current time: ") + std::size("current time: ") - 1;
//                currentFinishIndex = currentLevel.find(",");
//                m_limitOrderBook.currentTime = currentLevel.substr(currentStartIndex, currentFinishIndex - currentStartIndex);
//
//                //////////////////////////////////////////////////////////////////////////////
//                ///////////////////////////// Current level asks /////////////////////////////
//                //////////////////////////////////////////////////////////////////////////////
//                currentStartIndex = currentLevel.find("message: ") + std::size("message: ") - 1;
//
//                currentLevel = currentLevel.substr(currentStartIndex, currentLevel.size() - 1);
//
//                if (doc.Parse<0>(currentLevel.c_str()).HasParseError())
//                {
//                    std::cout << "Json parsing error!" << std::endl;
//                    return;
//                }
//
//                if (!doc.HasMember("tick"))
//                {
//                    std::cout << "There is no \"tick\" section!" << std::endl;
//                    return;
//                }
//
//                if (!doc["tick"].IsObject())
//                {
//                    std::cout << "Tick is not json object!" << std::endl;
//                    return;
//                }
//
//                if (!doc["tick"].HasMember("asks"))
//                {
//                    std::cout << "There is no \"asks\" section!" << std::endl;
//                    return;
//                }
//
//                if (!doc["tick"]["asks"].IsArray())
//                {
//                    std::cout << "Asks is not array!" << std::endl;
//                    return;
//                }
//
//                auto asksArray = doc["tick"]["asks"].GetArray();
//
//                for (const auto& askElem : asksArray)
//                {
//                    if (!askElem.IsArray())
//                    {
//                        std::cout << "Element of asks array is not array!" << std::endl;
//                        return;
//                    }
//
//                    auto ask = askElem.GetArray();
//
//                    if (!ask[0].IsLosslessDouble())
//                    {
//                        std::cout << "Zero element of ask array (price order) is not double!" << std::endl;
//                        return;
//                    }
//
//                    if (!ask[1].IsInt())
//                    {
//                        std::cout << "First element of ask array (amount orders) is not int!" << std::endl;
//                        return;
//                    }
//
//                    if (ask[1].GetInt() != 0)
//                        m_limitOrderBook.currentAsks[ask[0].GetDouble()] = ask[1].GetInt();
//                    else
//                        m_limitOrderBook.currentAsks.erase(ask[0].GetDouble());
//                }
//
//                //// Print current ASKS limit order book
//                //for (const auto& pair : m_limitOrderBook.currentAsks)
//                //{
//                //    std::cout << std::fixed << std::setprecision(2) << pair.first << " : " << pair.second << std::endl;
//                //}
//
//                //std::cout << std::endl;
//                //std::cout << std::endl;
//
//                ///////////////////////////////////////////////////////////////////////////////
//                ///////////////////////////// Current level bids //////////////////////////////
//                ///////////////////////////////////////////////////////////////////////////////
//                if (!doc["tick"].HasMember("bids"))
//                {
//                    std::cout << "There is no \"bids\" section!" << std::endl;
//                    return;
//                }
//
//                if (!doc["tick"]["bids"].IsArray())
//                {
//                    std::cout << "Bids is not array!" << std::endl;
//                    return;
//                }
//
//                auto bidsArray = doc["tick"]["bids"].GetArray();
//
//                for (const auto& bidElem : bidsArray)
//                {
//                    if (!bidElem.IsArray())
//                    {
//                        std::cout << "Element of bids array is not array!" << std::endl;
//                        return;
//                    }
//
//                    auto bid = bidElem.GetArray();
//
//                    if (!bid[0].IsLosslessDouble())
//                    {
//                        std::cout << "Zero element of bid array (price order) is not double!" << std::endl;
//                        return;
//                    }
//
//                    if (!bid[1].IsInt())
//                    {
//                        std::cout << "First element of bid array (amount orders) is not int!" << std::endl;
//                        return;
//                    }
//
//                    if (bid[1].GetInt() != 0)
//                        m_limitOrderBook.currentBids[bid[0].GetDouble()] = bid[1].GetInt();
//                    else
//                        m_limitOrderBook.currentBids.erase(bid[0].GetDouble());
//                }
//
//                //// Print current BIDS limit order book
//                //for (const auto& pair : m_limitOrderBook.currentBids)
//                //{
//                //    std::cout << std::fixed << std::setprecision(2) << pair.first << " : " << pair.second << std::endl;
//                //}
//
//                //std::cout << std::endl;
//                //std::cout << std::endl;
//
//                ///////////////////////////////////////////////////////////////////////
//                ///////////////////////////// File output /////////////////////////////
//                ///////////////////////////////////////////////////////////////////////
//                m_files.outputFile << "{" << m_limitOrderBook.currentTime << "}, {"
//                    << m_limitOrderBook.currentBids.rbegin()->first << "}, {" << m_limitOrderBook.currentBids.rbegin()->second << "}, {"
//                    << m_limitOrderBook.currentAsks.begin()->first << "}, {" << m_limitOrderBook.currentAsks.begin()->second << "}"
//                    << std::endl;
//            }
//        }
//        else
//        {
//            std::cout << "Cannot open to output file" << std::endl;
//            return;
//        }
//    }
//    else
//    {
//        std::cout << "Cannot open to input file" << std::endl;
//        return;
//    }
//}
