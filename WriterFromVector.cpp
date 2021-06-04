#include "WriterFromVector.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

#include "rapidjson/document.h"

WriterFromVector::WriterFromVector(const char* outputPath)
    : m_output(outputPath)
{}

WriterFromVector::~WriterFromVector()
{
    m_output.close();
}

bool WriterFromVector::write(const std::vector<OrderBook>& m_vecLevelUpdates)
{
    if (m_output.is_open())
    {
        m_currentOrderBook = m_vecLevelUpdates[0];

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////// File output /////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        m_output << std::fixed << std::setprecision(2)
            << "{CURRENT TIME}, {BEST BID PRICE}, {BEST BID AMOUNT}, {BEST ASK PRICE}, {BEST ASK AMOUNT}"
            << std::endl;

        writeCurrentOrderBook();
        
        for (size_t i = 1; i < m_vecLevelUpdates.size(); ++i)
        {
            m_currentOrderBook.currentTime = m_vecLevelUpdates[i].currentTime;

            for (const auto& ask : m_vecLevelUpdates[i].currentAsks)
            {
                m_currentOrderBook.currentAsks[ask.first] = ask.second;
            }

            for (const auto& bid : m_vecLevelUpdates[i].currentBids)
            {
                m_currentOrderBook.currentBids[bid.first] = bid.second;
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

void WriterFromVector::writeCurrentOrderBook()
{
    m_output << "{" << m_currentOrderBook.currentTime << "}, {"
        << m_currentOrderBook.currentBids.rbegin()->first << "}, {" << m_currentOrderBook.currentBids.rbegin()->second << "}, {"
        << m_currentOrderBook.currentAsks.begin()->first << "}, {" << m_currentOrderBook.currentAsks.begin()->second << "}"
        << std::endl;
}
