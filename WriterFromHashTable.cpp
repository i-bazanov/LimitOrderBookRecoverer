#include "WriterFromHashTable.h"

#include <string>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <iomanip>

#include "rapidjson/document.h"

WriterFromHashTable::WriterFromHashTable(const char* outputPath)
    : m_output(outputPath)
{}

WriterFromHashTable::~WriterFromHashTable()
{
    m_output.close();
}

bool WriterFromHashTable::write(const std::unordered_map<int, OrderBook>& m_vecLevelUpdates)
{
    if (m_output.is_open())
    {
        m_currentOrderBook = m_vecLevelUpdates.at(0);

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////// File output /////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        m_output << std::fixed << std::setprecision(2)
            << "{CURRENT TIME}, {BEST BID PRICE}, {BEST BID AMOUNT}, {BEST ASK PRICE}, {BEST ASK AMOUNT}"
            << std::endl;

        writeCurrentOrderBook();
        
        for (size_t i = 1; i < m_vecLevelUpdates.size(); ++i)
        {
            m_currentOrderBook.currentTime = m_vecLevelUpdates.at(i).currentTime;

            for (const auto& ask : m_vecLevelUpdates.at(i).currentAsks)
            {
                // Checking number of ask orders for zero value
                if (ask.second != 0)
                    m_currentOrderBook.currentAsks[ask.first] = ask.second;
                else
                    m_currentOrderBook.currentAsks.erase(ask.first);
            }

            for (const auto& bid : m_vecLevelUpdates.at(i).currentBids)
            {
                // Checking number of bid orders for zero value
                if(bid.second != 0)
                    m_currentOrderBook.currentBids[bid.first] = bid.second;
                else
                    m_currentOrderBook.currentBids.erase(bid.first);
            }

            writeCurrentOrderBook();
        }
    }
    else
    {
        std::cout << "Cannot open output file" << std::endl;
        return 0;
    }

    return 1;
}

void WriterFromHashTable::writeCurrentOrderBook()
{
    m_output << "{" << m_currentOrderBook.currentTime << "}, {"
        << m_currentOrderBook.currentBids.rbegin()->first << "}, {" << m_currentOrderBook.currentBids.rbegin()->second << "}, {"
        << m_currentOrderBook.currentAsks.begin()->first << "}, {" << m_currentOrderBook.currentAsks.begin()->second << "}"
        << std::endl;
}
