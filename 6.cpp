#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <chrono>
#include <functional>
#include <algorithm>

#include <cmath>

#define SPLIT 20

struct Entry
{
    double SL, SW, PL, PW;
    std::string Class;
};

using IntDoublePair = std::pair<int, double>;

struct CompareSecond: std::binary_function<IntDoublePair, IntDoublePair, bool>
{
    bool operator()(const IntDoublePair& a, const IntDoublePair& b)
    {
        return a.second < b.second;
    }
};

int main(int argc, char** argv)
{
    if (argc < 2)
        return 1;

    // preparing data
    std::ifstream file(argv[1]);
    if (!file.is_open())
        return 1;

    std::vector<Entry> data;

    while (!file.eof())
    {
        std::string line;
        std::getline(file, line);
        if ("" == line)
            continue;
        std::istringstream linein(std::move(line));
        Entry entry;
        std::string sl, sw, pl, pw;
        std::getline(linein, sl, ',');
        entry.SL = std::stod(sl, nullptr);
        std::getline(linein, sw, ',');
        entry.SW = std::stod(sw, nullptr);
        std::getline(linein, pl, ',');
        entry.PL = std::stod(pl, nullptr);
        std::getline(linein, pw, ',');
        entry.PW = std::stod(pw, nullptr);
        std::getline(linein, entry.Class);
        data.push_back(std::move(entry));
    }

    std::set<int> testSet, trainingSet;

    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> distribution(0, (int)data.size() - 1);
    while (testSet.size() < 20)
        testSet.insert(distribution(generator));
    while (trainingSet.size() < (int)data.size() - 20)
    {
        int index = distribution(generator);
        if (!testSet.count(index))
            trainingSet.insert(index);
    }

    // generating predictions
    int correctCount = 0;

    int k;
    std::cin >> k;
    std::set<std::string> predictions;
    for (auto test: testSet)
    {
        // finding neighbors
        std::set<IntDoublePair, CompareSecond> distances;
        auto euclideanDistance = [](const Entry& entry1, const Entry& entry2)
        {
            auto dSL = entry1.SL - entry2.SL, dSW = entry1.SW - entry2.SW, dPL = entry1.PL - entry2.PL, dPW = entry1.PW - entry2.PW;
            return sqrt(dSL * dSL + dSW * dSW + dPL * dPL + dPW * dPW);
        };
        for (auto trainer: trainingSet)
            distances.insert(std::make_pair(trainer, euclideanDistance(data[test], data[trainer])));

        std::set<int> neighbors;
        auto i = distances.begin();
        for (int j = 0; j < k; ++j, ++i)
            neighbors.insert(i->first);

        // getting response
        std::map<std::string, int> classVotes;
        for (auto neighbor: neighbors)
        {
            auto response = data[neighbor].Class;
            if (classVotes.count(response))
                ++classVotes[response];
            else
                classVotes[response] = 1;
        }

        auto compareSecond = [](const auto& a, const auto& b)
        {
            return a.second < b.second;
        };
        auto responsePos = std::max_element(classVotes.begin(), classVotes.end(), compareSecond);
        auto response = responsePos->first;
        predictions.insert(response);
        std::cout << "Predicted: " << response << ", actual: " << data[test].Class << std::endl;
        if (response == data[test].Class)
            ++correctCount;
    }

    // calculating accuracy
    std::cout << "Accuracy: " << 100.0 * correctCount / testSet.size() << "%" << std::endl;
    return 0;
}
