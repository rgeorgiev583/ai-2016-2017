#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <chrono>
#include <functional>
#include <algorithm>

#include <cmath>

#define NUM_FIELDS 16
#define NUM_SETS   10


namespace NaiveBayes
{
    struct Entry
    {
        std::array<double, NUM_FIELDS> Features;
        std::string Class;
    };

    struct DataSet: public std::vector<Entry>
    {
        void SplitData(std::vector<std::set<int> >& dataSets)
        {
            std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<> distribution(0, (int)size() - 1);
            auto addNextSet = [&generator, &distribution, &dataSets](int i, int size)
            {
                std::set<int> currentSet;

                while ((int)currentSet.size() < size)
                {
                    int index = distribution(generator);
                    for (const auto& dataSet: dataSets)
                        if (dataSet.count(index))
                            continue;
                    currentSet.insert(index);
                }

                dataSets.push_back(std::move(currentSet));
            };
            int setSize = size() / NUM_SETS;
            std::cout << "Splitting source dataset into " << NUM_SETS << " subsets of size " << setSize << "." << std::endl;
            for (int i = 0; i < NUM_SETS - 1; ++i)
                addNextSet(i, setSize);
            addNextSet(NUM_SETS - 1, size() % NUM_SETS);
        }
    };

    bool LoadDataset(const char* filename, DataSet& data)
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
            std::getline(linein, entry.Class, ',');
            auto setField = [&entry, &data](int i, const std::string& field)
            {
                switch (field[0])
                {
                    case 'y':
                        entry.Features[i] = 1.0;
                        break;

                    case 'n':
                        entry.Features[i] = 0.0;
                        break;

                    case '?':
                        entry.Features[i] = 0.5;
                        break;
                }
            };
            for (int i = 0; i < NUM_FIELDS; i++)
            {
                std::string value;
                std::getline(linein, value, ',');
                setField(i, value);
            }
            std::string value;
            std::getline(linein, value);
            setField(NUM_FIELDS - 1, value);
            data.push_back(std::move(entry));
        }

        return true;
    }

    struct Classifier
    {
        const DataSet& data;
        std::map<std::string, std::vector<int> > dataMap;
        std::map<std::string, int> counts;
        std::map<std::string, double> priors;
        std::map<std::string, std::vector<double> > multinomialLikelihoods;
        std::map<std::string, int> multinomialSums;
        std::map<std::string, Entry> sumX;
        std::map<std::string, std::vector<double> > means;
        std::map<std::string, std::vector<double> > variances;

        Classifier(const DataSet& source): data(source)
        {
        }

        void AddTrainingSet(const std::set<int>& trainingSet)
        {
            for (int i: trainingSet)
            {
                for (int j = 0; j < NUM_FIELDS; j++)
                {
                    sumX[data[i].Class].Features[j] += data[i].Features[j];
                    multinomialSums[data[i].Class] += data[i].Features[j];
                }

                counts[data[i].Class]++;
                dataMap[data[i].Class].push_back(i);
            }
        }

        void Train(double alpha)
        {
            for (const auto& it: sumX)
            {
                const std::string& label = it.first;
                priors[label] = (double)counts[label] / data.size();

                std::cout << "Class " << label << ", prior: " << std::setw(1) << std::setprecision(3) << priors[label] << std::endl;
                std::cout << "feature\tmean\tvar\tstddev\tmnl" << std::endl;

                // calculate means
                std::vector<double> featureMeans(NUM_FIELDS);
                for (int i = 0; i < NUM_FIELDS; i++)
                    featureMeans[i] = sumX[label].Features[i] / counts[label];

                // calculate variances
                std::vector<double> featureVariances(NUM_FIELDS);
                const auto& currentData = dataMap[label];
                for (int currentDataIndex: currentData)
                    for (int i = 0; i < NUM_FIELDS; i++)
                    {
                        double featureDeviation = data[currentDataIndex].Features[i] - featureMeans[i];
                        featureVariances[i] += featureDeviation * featureDeviation;
                    }
                for (auto& featureVariance: featureVariances)
                    featureVariance /= currentData.size();

                const auto& currentSumX = sumX[label];
                auto currentMultinomialSum = multinomialSums[label];
                auto& currentMultinomialLikelihood = multinomialLikelihoods[label];
                // calculate multinomial likelihoods
                for (double feature: currentSumX.Features)
                    currentMultinomialLikelihood.push_back((feature + alpha) / (currentMultinomialSum + alpha * featureMeans.size()));

                for (unsigned int i = 0; i < NUM_FIELDS; i++)
                    printf("%i\t%2.3f\t%2.3f\t%2.3f\t%2.3f\n",i+1,featureMeans[i],featureVariances[i],sqrt(featureVariances[i]),currentMultinomialLikelihood[i]);

                means[label] = std::move(featureMeans);
                variances[label] = std::move(featureVariances);
            }
        }

        void Classify(const std::set<int>& testSet, int decision)
        {
            int correct = 0;
            std::cout << "Classifying:" << std::endl;
            std::cout << "class\tprob\tresult" << std::endl;

            for (int i: testSet)
            {
                std::string predictedLabel;
                double maxLikelihood = 0.0;
                double denominator = 0.0;
                std::vector<double> probabilities;
                for (const auto& it: priors)
                {
                    const std::string& label = it.first;
                    double numerator = priors[label];
                    const auto& currentMultinomialLikelihood = multinomialLikelihoods[label];
                    const auto& currentMean = means[label];
                    const auto& currentVariance = variances[label];
                    for (int j = 0; j < NUM_FIELDS; j++)
                        switch (decision)
                        {
                            case 2:
                                // Multinomial
                                if (data[i].Features[j])
                                    numerator *= pow(currentMultinomialLikelihood[j], data[i].Features[j]);
                                break;

                            case 3:
                                // Bernoulli
                                numerator *= pow(currentMean[j], data[i].Features[j]) * pow(1.0 - currentMean[j], 1.0 - data[i].Features[j]);
                                break;

                            default:
                                // Gaussian
                                numerator *= 1 / sqrt(2 * M_PI * currentVariance[j]) * exp((-1 * (data[i].Features[j] - currentMean[j]) * (data[i].Features[j] - currentMean[j])) / (2 * currentVariance[j]));
                                break;
                        }

                    if (numerator > maxLikelihood)
                    {
                        maxLikelihood = numerator;
                        predictedLabel = label;
                    }
                    denominator += numerator;
                    probabilities.push_back(numerator);
                }

                std::cout << predictedLabel << "\t" << std::setw(1) << std::setprecision(3) << maxLikelihood / denominator << "\t";
                if ("" == data[i].Class)
                    std::cout << "<no label>" << std::endl;
                else if (predictedLabel == data[i].Class)
                {
                    std::cout << "correct" << std::endl;
                    ++correct;
                }
                else
                    std::cout << "incorrect" << std::endl;
            }

            printf("Accuracy: %3.2f %% (%i/%i)\n", 100.0 * correct / testSet.size(), correct, (int)testSet.size());
        }
    };
}

int main(int argc, char** argv)
{
    // smoothing factor
    const double alpha = 1.0;
    // decision rule
    const int decision = 1;

    if (argc < 2)
        return 1;

    // preparing data
    NaiveBayes::DataSet data;
    if (!NaiveBayes::LoadDataset(argv[1], data))
        return 2;

    std::vector<std::set<int> > sets;
    data.SplitData(sets);
    std::cout << std::endl;

    for (int i = 0; i < (int)sets.size(); ++i)
    {
        std::cout << "Using subset " << i << " as the test set." << std::endl;
        NaiveBayes::Classifier classifier(data);
        for (int j = 0; j < (int)sets.size(); ++j)
            if (sets[i] != sets[j])
                classifier.AddTrainingSet(sets[j]);
        classifier.Train(alpha);
        classifier.Classify(sets[i], decision);
        std::cout << std::endl << std::endl;
    }

    return 0;
}
