#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <ctime> 

int main() 
{
    std::string currentLevel;
    std::ifstream fileInput("..\\huobi_dm_depth.log");

    std::ofstream fileOutput("..\\results.txt");

    size_t currentTimeStartIndex = -1, currentTimeFinishIndex = -1;
    std::string currentTime;

    size_t currentAsksStartIndex = -1, currentAsksFinishIndex = -1;
    std::string currentAsksString;

    std::map<std::string, std::string> currentAsks; // key -> price orders, value -> amount orders

    std::vector<size_t> lhsBracketArray;
    std::vector<size_t> rhsBracketArray;
    std::vector<size_t> commaArray;

    size_t currentBidsStartIndex = -1, currentBidsFinishIndex = -1;
    std::string currentBidsString;

    std::map<std::string, std::string> currentBids; // key -> price orders, value -> amount orders

    clock_t start = clock();

    while (getline(fileInput, currentLevel))
    {
        if (currentLevel.find("ping") != -1)
            continue;

        //////////////////////////////////////////////////////////////////////////////
        ///////////////////////////// Current level time /////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
        currentTimeStartIndex = currentLevel.find("current time: ") + std::size("current time: ") - 1;
        currentTimeFinishIndex = currentLevel.find(",");
        currentTime = currentLevel.substr(currentTimeStartIndex, currentTimeFinishIndex - currentTimeStartIndex);

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
                currentAsks[currentAsksString.substr(lhsBracketArray[i] + 1, commaArray[2 * i] - lhsBracketArray[i] - 1)] = currentAsksString.substr(commaArray[2 * i] + 1, rhsBracketArray[i] - commaArray[2 * i] - 1);
            else
                currentAsks.erase(currentAsksString.substr(lhsBracketArray[i] + 1, commaArray[2 * i] - lhsBracketArray[i] - 1));
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
                currentBids[currentBidsString.substr(lhsBracketArray[i] + 1, commaArray[2 * i] - lhsBracketArray[i] - 1)] = currentBidsString.substr(commaArray[2 * i] + 1, rhsBracketArray[i] - commaArray[2 * i] - 1);
            else
                currentBids.erase(currentBidsString.substr(lhsBracketArray[i] + 1, commaArray[2 * i] - lhsBracketArray[i] - 1));
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
        if (fileOutput.is_open())
        {
            fileOutput << "" << currentTime << ", " << currentBids.rbegin()->first << ", " << currentBids.rbegin()->second << ", " << currentAsks.begin()->first << ", " << currentAsks.begin()->second << "" << std::endl;
        }
    }
    clock_t end = clock();

    std::cout << "Run time = " << (end - start) / 1000 << " sec" << std::endl;

    fileInput.close();
    fileOutput.close();

    return 0;
}
