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

#define NUM_FIELDS         4
#define NUM_TEST_INSTANCES 20


namespace KNearestNeighbors
{
    struct Entry
    {
        std::array<double, NUM_FIELDS> Attributes;
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

    bool LoadDataset(const char* filename, std::vector<Entry>& data)
    {
        std::ifstream filein(filename);
        if (!filein.is_open())
            return false;

        while (!filein.eof())
        {
            std::string line;
            std::getline(filein, line);
            if ("" == line)
                continue;
            std::istringstream linein(std::move(line));
            Entry entry;
            for (auto& attribute: entry.Attributes)
            {
                std::string value;
                std::getline(linein, value, ',');
                attribute = std::stod(std::move(value), nullptr);
            }
            std::getline(linein, entry.Class);
            data.push_back(std::move(entry));
        }

        return true;
    }

    void SplitData(const std::vector<Entry>& data, std::set<int>& trainingSet, std::set<int>& testSet)
    {
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<> distribution(0, (int)data.size() - 1);
        while (testSet.size() < NUM_TEST_INSTANCES)
            testSet.insert(distribution(generator));
        while ((int)trainingSet.size() < (int)data.size() - NUM_TEST_INSTANCES)
        {
            int index = distribution(generator);
            if (!testSet.count(index))
                trainingSet.insert(index);
        }
    }

    void GetNeighbors(const std::vector<Entry>& data, const std::set<int>& trainingSet, int testInstance, int k, std::set<int>& neighbors)
    {
        std::set<IntDoublePair, CompareSecond> distances;
        auto euclideanDistance = [](const Entry& entry1, const Entry& entry2)
        {
            double sumDeltasSquared = 0;
            for (int i = 0; i < NUM_FIELDS; i++)
            {
                double delta = entry1.Attributes[i] - entry2.Attributes[i];
                sumDeltasSquared += delta * delta;
            }
            return sqrt(sumDeltasSquared);
        };
        for (int trainer: trainingSet)
            distances.insert(std::make_pair(trainer, euclideanDistance(data[testInstance], data[trainer])));

        auto i = distances.begin();
        for (int j = 0; j < k; ++j, ++i)
            neighbors.insert(i->first);
    }

    std::string GetResponse(const std::vector<Entry>& data, const std::set<int>& neighbors)
    {
        std::map<std::string, int> classVotes;
        for (int neighbor: neighbors)
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
        return responsePos->first;
    }
}

int main(int argc, const char* const* argv)
{
    if (argc < 2)
        return 1;

    // preparing data
    std::vector<KNearestNeighbors::Entry> data;
    if (!KNearestNeighbors::LoadDataset(argv[1], data))
        return 2;

    std::set<int> trainingSet, testSet;
    KNearestNeighbors::SplitData(data, trainingSet, testSet);

    // generating predictions
    int correctCount = 0;

    int k;
    std::cin >> k;
    for (int test: testSet)
    {
        // finding neighbors
        std::set<int> neighbors;
        KNearestNeighbors::GetNeighbors(data, trainingSet, test, k, neighbors);

        // getting response
        auto response = KNearestNeighbors::GetResponse(data, neighbors);
        std::cout << "Predicted: " << response << ", actual: " << data[test].Class << std::endl;
        if (response == data[test].Class)
            ++correctCount;
    }

    // calculating accuracy
    std::cout << "Accuracy: " << 100.0 * correctCount / testSet.size() << "%" << std::endl;
    return 0;
}
