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


namespace NaiveBayesClassifier
{
    using Entry = std::array<double, NUM_FIELDS>;

    struct State
    {
        std::map<std::string, std::vector<Entry> > data;
        std::map<std::string, int> n;
        std::map<std::string, double> priors;
        std::map<std::string, std::vector<double> > multinomialLikelihoods;
        std::map<std::string, int> multinomialSums;
        std::map<std::string, Entry > sumX;
        std::map<std::string, std::vector<double> > means;
        std::map<std::string, std::vector<double> > variances;
    };

    bool LoadDataset(const char* filename, State& state, int& total)
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
            std::string label;
            std::getline(linein, label, ',');
            Entry entry;
            auto setField = [&entry, &label, &state](int i, const std::string& field)
            {
                switch (field[0])
                {
                    case 'y':
                        entry[i] = 1.0;
                        break;

                    case 'n':
                        entry[i] = 0.0;
                        break;

                    case '?':
                        entry[i] = 0.5;
                        break;
                }
                state.sumX[label][i] += entry[i];
                state.multinomialSums[label] += entry[i];
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
            state.data[label].push_back(std::move(entry));
            state.n[label]++;
            total++;
        }

        return true;
    }

    void SplitData(const State& state, std::vector<std::set<int> >& dataSets)
    {
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<> distribution(0, (int)state.data.size() - 1);
        auto addNextSet = [&generator, &distribution, &dataSets](int i, int size)
        {
            std::set<int> currentSet;

            while (currentSet.size() < size)
            {
                int index = distribution(generator);
                for (const auto& dataSet: dataSets)
                    if (dataSet.count(index))
                        continue;
                currentSet.insert(index);
            }

            dataSets.push_back(std::move(currentSet));
        };
        for (int i = 0; i < NUM_SETS - 1; ++i)
            addNextSet(i, state.data.size() / NUM_SETS);
        addNextSet(NUM_SETS - 1, state.data.size() % NUM_SETS);
    }

    void Train(const State& state)
    {
        for (auto it = state.sumX.begin(); it != state.sumX.end(); it++)
        {
            state.priors[it->first] = (double)state.n[it->first] / total;

            std::cout << "Class " << it->first << ", prior: " << std::setw(1) << std::setprecision(3) << priors[it->first] << std::endl;
            std::cout << "feature\tmean\tvar\tstddev\tmnl" << std::endl;

            // calculate means
            std::vector<double> featureMeans(NUM_FIELDS);
            for (int i = 0; i < NUM_FIELDS; i++)
                featureMeans[i] = state.sumX[it->first][i] / state.n[it->first];

            // calculate variances
            std::vector<double> featureVariances(NUM_FIELDS);
            const auto& firstData = state.data[it->first];
            for (int i = 0; i < (int)firstData.size(); i++)
                for (int j = 0; j < NUM_FIELDS; j++)
                    featureVariances[j] += (firstData[i][j] - featureMeans[j]) * (firstData[i][j] - featureMeans[j]);
            for (int i = 0; i < NUM_FIELDS; i++)
                featureVariances[i] /= firstData.size();

            const auto& firstSumX = state.sumX[it->first];
            auto firstMultinomialSum = state.multinomialSums[it->first];
            auto& firstMultinomialLikelihood = state.multinomialLikelihoods[it->first];
            // calculate multinomial likelihoods
            for (int i = 0; i < NUM_FIELDS; i++)
            {
                double mnl = (firstSumX[i] + state.alpha) / (firstMultinomialSum + (state.alpha * featureMeans.size()));
                firstMultinomialLikelihood.push_back(mnl);
            }

            for (unsigned int i = 0; i < NUM_FIELDS; i++)
                printf("%i\t%2.3f\t%2.3f\t%2.3f\t%2.3f\n",i+1,featureMeans[i],featureVariances[i],sqrt(featureVariances[i]),firstMultinomialLikelihood[i]);

            state.means[it->first] = std::move(featureMeans);
            state.variances[it->first] = std::move(featureVariances);
        }
    }

    void Classify(const State& state, const std::string& label, const Entry& entry)
    {
        std::string predlabel;
        double maxlikelihood = 0.0;
        double denom = 0.0;
        std::vector<double> probs;
        for (auto it = state.priors.begin(); it != state.priors.end(); it++)
        {
            double numer = priors[it->first];
            const auto& firstMultinomialLikelihood = atate.multinomialLikelihoods[it->first];
            const auto& firstMean = state.means[it->first];
            const auto& firstVariance = state.variances[it->first];
            for (int j = 0; j < NUM_FIELDS; j++)
                switch (state,decision)
                {
                    case 2:
                        // Multinomial
                        if (entry[j])
                            numer *= pow(firstMultinomialLikelihood[j], entry[j]);
                        break;

                    case 3:
                        // Bernoulli
                        numer *= pow(firstMean[j], entry[j]) * pow(1.0 - firstMean[j], 1.0 - entry[j]);
                        break;

                    default:
                        // Gaussian
                        numer *= 1 / sqrt(2 * M_PI * firstVariance[j]) * exp((-1 * (entry[j] - firstMean[j]) * (entry[j] -firstMean[j])) / (2 * firstVariance[j]));
                        break;
                }

            if (numer > maxlikelihood)
            {
                maxlikelihood = numer;
                predlabel = it->first;
            }
            denom += numer;
            probs.push_back(numer);
        }

        std::cout << predlabel << "\t" << std::setw(1) << std::setprecision(3) << maxlikelihood/denom << "\t";
        if ("" == label)
            std::cout << "<no label>" << std::endl;
        else if (predlabel == label)
        {
            std::cout << "correct" << std::endl;
            correct++;
        }
        else
            std::cout << "incorrect" << std::endl;

        totalClassified++;
    }
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
    int total = 0, correct = 0, totalClassified = 0;



    int errcode = readFromFile(argv[1], false);
    if (errcode)
        return errcode;


    // classify
    std::cout << "Classifying:" << std::endl;
    std::cout << "class\tprob\tresult" << std::endl;

    errcode = readFromFile(argv[2], true);
    if (errcode)
        return errcode;
    printf("Accuracy: %3.2f %% (%i/%i)\n", 100.0 * correct / totalClassified, correct, totalClassified);

    return 0;
}
