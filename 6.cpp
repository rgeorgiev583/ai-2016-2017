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

#define SPLIT 20

struct Entry
{
    double SL, SW, PL, PW;
    std::string Class;
};

using IntPair = std::pair<int, int>;

struct CompareSecond: std::binary_function<IntPair, IntPair, bool>
{
    bool operator()(const IntPair& a, const IntPair& b)
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
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());

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

    std::uniform_int_distribution<> dist(0, data.size() - 1);
    std::set<int> trainingSet, testSet;

    for (int i = 0; i < data.size(); ++i)
        if (dist(generator) < SPLIT)
            testSet.insert(i);
        else
            trainingSet.insert(i);

    // generating predictions
    int k;
    std::cin >> k;

    std::set<std::string> predictions;

    for (auto test: testSet)
    {
        // finding neighbors
        auto compareSecond = [](const auto& a, const auto& b)
        {
            return a.second < b.second;
        };
        std::set<IntPair, CompareSecond> distances;

        auto euclideanDistance = [](const Entry& entry1, const Entry& entry2)
        {
            int dSL = entry1.SL - entry2.SL, dSW = entry1.SW - entry2.SW, dPL = entry1.PL - entry2.PL, dPW = entry1.PL - entry2.PW;
            return sqrt(dSL * dSL + dSW * dSW + dPL * dPL + dPW * dPW);
        };
        for (auto trainer: trainingSet)
            distances.insert(std::make_pair(trainer, euclideanDistance(data[test], data[trainer])));

        std::set<int> neighbors;
        auto i = distances.begin();
        for (int j = 0; j < k; ++j, ++i)
            neighbors.insert(i->first);

        std::map<std::string, int> classVotes;

        for (auto neighbor: neighbors)
        {
            auto response = data[neighbor].Class;
            if (classVotes.count(response))
                ++classVotes[response];
            else
                classVotes[response] = 1;
        }

        auto responsePos = std::max_element(classVotes.begin(), classVotes.end(), compareSecond);
        auto response = responsePos->first;
        predictions.insert(response);
        std::cout << "Predicted: " << response << ", actual: " << data[test].Class << std::endl;
    }

    int correctCount = 0;
    auto j = predictions.begin();

    for (auto i = testSet.begin(); i != testSet.end(); i++, j++)
        if (data[*i].Class == *j)
            correctCount++;

    std::cout << "Accuracy: " << 100.0 * correctCount / testSet.size() << "%";
    return 0;
}
