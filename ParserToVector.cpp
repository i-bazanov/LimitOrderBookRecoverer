#include "ParserToVector.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#include "rapidjson/document.h"

ParserToVector::ParserToVector(const char* inputPath)
    : m_inputPath(inputPath),
      m_input(inputPath)
{}

ParserToVector::~ParserToVector() 
{
    m_input.close();
}

bool ParserToVector::parse()
{
    if (m_input.is_open())
    {
        std::string currentLevel;

        size_t numberLevelUpdate = 0;

        while (getline(m_input, currentLevel))
        {
            // Ping check 
            if (currentLevel.find("ping") != -1)
                continue;

            ++numberLevelUpdate;
        }
        m_vecLevelUpdates.resize(numberLevelUpdate);

        // For second file reading
        m_input.close();
        m_input.open(m_inputPath);

        if(m_input.is_open())
        {
            size_t currentStartIndex = -1, currentFinishIndex = -1;

            rapidjson::Document doc;

            size_t limitOrderBooksCounter = 0;

            while (getline(m_input, currentLevel))
            {
                // Ping check 
                if (currentLevel.find("ping") != -1)
                    continue;

                //////////////////////////////////////////////////////////////////////////////
                ///////////////////////////// Current level time /////////////////////////////
                //////////////////////////////////////////////////////////////////////////////
                currentStartIndex = currentLevel.find("current time: ") + std::size("current time: ") - 1;
                currentFinishIndex = currentLevel.find(",");
                m_vecLevelUpdates[limitOrderBooksCounter].currentTime = currentLevel.substr(currentStartIndex, currentFinishIndex - currentStartIndex);

                //////////////////////////////////////////////////////////////////////////////
                ///////////////////////////// Current level asks /////////////////////////////
                //////////////////////////////////////////////////////////////////////////////
                currentStartIndex = currentLevel.find("message: ") + std::size("message: ") - 1;

                currentLevel = currentLevel.substr(currentStartIndex, currentLevel.size() - 1);

                if (doc.Parse<0>(currentLevel.c_str()).HasParseError())
                {
                    std::cout << "Json parsing error!" << std::endl;
                    return 0;
                }

                if (!doc.HasMember("tick"))
                {
                    std::cout << "There is no \"tick\" section!" << std::endl;
                    return 0;
                }

                if (!doc["tick"].IsObject())
                {
                    std::cout << "Tick is not json object!" << std::endl;
                    return 0;
                }

                if (!doc["tick"].HasMember("asks"))
                {
                    std::cout << "There is no \"asks\" section!" << std::endl;
                    return 0;
                }

                if (!doc["tick"]["asks"].IsArray())
                {
                    std::cout << "Asks is not array!" << std::endl;
                    return 0;
                }

                auto asksArray = doc["tick"]["asks"].GetArray();

                for (const auto& askElem : asksArray)
                {
                    if (!askElem.IsArray())
                    {
                        std::cout << "Element of asks array is not array!" << std::endl;
                        return 0;
                    }

                    auto ask = askElem.GetArray();

                    if (!ask[0].IsLosslessDouble())
                    {
                        std::cout << "Zero element of ask array (price order) is not double!" << std::endl;
                        return 0;
                    }

                    if (!ask[1].IsInt())
                    {
                        std::cout << "First element of ask array (amount orders) is not int!" << std::endl;
                        return 0;
                    }

                    m_vecLevelUpdates[limitOrderBooksCounter].currentAsks[ask[0].GetDouble()] = ask[1].GetInt();
                }

                ///////////////////////////////////////////////////////////////////////////////
                ///////////////////////////// Current level bids //////////////////////////////
                ///////////////////////////////////////////////////////////////////////////////
                if (!doc["tick"].HasMember("bids"))
                {
                    std::cout << "There is no \"bids\" section!" << std::endl;
                    return 0;
                }

                if (!doc["tick"]["bids"].IsArray())
                {
                    std::cout << "Bids is not array!" << std::endl;
                    return 0;
                }

                auto bidsArray = doc["tick"]["bids"].GetArray();

                for (const auto& bidElem : bidsArray)
                {
                    if (!bidElem.IsArray())
                    {
                        std::cout << "Element of bids array is not array!" << std::endl;
                        return 0;
                    }

                    auto bid = bidElem.GetArray();

                    if (!bid[0].IsLosslessDouble())
                    {
                        std::cout << "Zero element of bid array (price order) is not double!" << std::endl;
                        return 0;
                    }

                    if (!bid[1].IsInt())
                    {
                        std::cout << "First element of bid array (amount orders) is not int!" << std::endl;
                        return 0;
                    }

                    m_vecLevelUpdates[limitOrderBooksCounter].currentBids[bid[0].GetDouble()] = bid[1].GetInt();
                }

                ++limitOrderBooksCounter;
            }
        }
        else
        {
            std::cout << "Cannot open input file to fill vector of limit order books" << std::endl;
            return 0;
        }
    }
    else
    {
        std::cout << "Cannot open input file to count the number of rows" << std::endl;
        return 0;
    }

    return 1;
}

const std::vector<OrderBook>& ParserToVector::getVecLevelUpdates() const
{
    return m_vecLevelUpdates;
}
