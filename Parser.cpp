#include "Parser.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <ios>
#include "rapidjson/document.h"

Parser::Parser(const char* inputPath, const char* outputPath): m_files(inputPath, outputPath) 
{
	parse();
}

Parser::~Parser() {};

void Parser::parse()
{
    if (m_files.inputFile.is_open())
    {
        if (m_files.outputFile.is_open())
        {
            m_files.outputFile << std::fixed << std::setprecision(2)
                << "{CURRENT TIME}, {BEST BID PRICE}, {BEST BID AMOUNT}, {BEST ASK PRICE}, {BEST ASK AMOUNT}"
                << std::endl;

            size_t currentStartIndex = -1, currentFinishIndex = -1;
            std::string currentLevel;
            rapidjson::Document doc;

            while (getline(m_files.inputFile, currentLevel))
            {
                // Ping check 
                if (currentLevel.find("ping") != -1)
                    continue;

                //////////////////////////////////////////////////////////////////////////////
                ///////////////////////////// Current level time /////////////////////////////
                //////////////////////////////////////////////////////////////////////////////
                currentStartIndex = currentLevel.find("current time: ") + std::size("current time: ") - 1;
                currentFinishIndex = currentLevel.find(",");
                m_limitOrderBook.currentTime = currentLevel.substr(currentStartIndex, currentFinishIndex - currentStartIndex);

                //////////////////////////////////////////////////////////////////////////////
                ///////////////////////////// Current level asks /////////////////////////////
                //////////////////////////////////////////////////////////////////////////////
                currentStartIndex = currentLevel.find("message: ") + std::size("message: ") - 1;

                currentLevel = currentLevel.substr(currentStartIndex, currentLevel.size() - 1);

                if (doc.Parse<0>(currentLevel.c_str()).HasParseError())
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

                for (const auto& askElem : asksArray)
                {
                    if (!askElem.IsArray())
                    {
                        std::cout << "Element of asks array is not array!" << std::endl;
                        return;
                    }

                    auto ask = askElem.GetArray();

                    if (!ask[0].IsLosslessDouble())
                    {
                        std::cout << "Zero element of ask array (price order) is not double!" << std::endl;
                        return;
                    }

                    if (!ask[1].IsInt())
                    {
                        std::cout << "First element of ask array (amount orders) is not int!" << std::endl;
                        return;
                    }

                    if (ask[1].GetInt() != 0)
                        m_limitOrderBook.currentAsks[ask[0].GetDouble()] = ask[1].GetInt();
                    else
                        m_limitOrderBook.currentAsks.erase(ask[0].GetDouble());
                }

                //// Print current ASKS limit order book
                //for (const auto& pair : m_limitOrderBook.currentAsks)
                //{
                //    std::cout << std::fixed << std::setprecision(2) << pair.first << " : " << pair.second << std::endl;
                //}

                //std::cout << std::endl;
                //std::cout << std::endl;

                ///////////////////////////////////////////////////////////////////////////////
                ///////////////////////////// Current level bids //////////////////////////////
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

                for (const auto& bidElem : bidsArray)
                {
                    if (!bidElem.IsArray())
                    {
                        std::cout << "Element of bids array is not array!" << std::endl;
                        return;
                    }

                    auto bid = bidElem.GetArray();

                    if (!bid[0].IsLosslessDouble())
                    {
                        std::cout << "Zero element of bid array (price order) is not double!" << std::endl;
                        return;
                    }

                    if (!bid[1].IsInt())
                    {
                        std::cout << "First element of bid array (amount orders) is not int!" << std::endl;
                        return;
                    }

                    if (bid[1].GetInt() != 0)
                        m_limitOrderBook.currentBids[bid[0].GetDouble()] = bid[1].GetInt();
                    else
                        m_limitOrderBook.currentBids.erase(bid[0].GetDouble());
                }

                //// Print current BIDS limit order book
                //for (const auto& pair : m_limitOrderBook.currentBids)
                //{
                //    std::cout << std::fixed << std::setprecision(2) << pair.first << " : " << pair.second << std::endl;
                //}

                //std::cout << std::endl;
                //std::cout << std::endl;

                ///////////////////////////////////////////////////////////////////////
                ///////////////////////////// File output /////////////////////////////
                ///////////////////////////////////////////////////////////////////////
                m_files.outputFile << "{" << m_limitOrderBook.currentTime << "}, {"
                    << m_limitOrderBook.currentBids.rbegin()->first << "}, {" << m_limitOrderBook.currentBids.rbegin()->second << "}, {"
                    << m_limitOrderBook.currentAsks.begin()->first << "}, {" << m_limitOrderBook.currentAsks.begin()->second << "}"
                    << std::endl;
            }
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
