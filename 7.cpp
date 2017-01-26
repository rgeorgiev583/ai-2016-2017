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

using Entry = std::array<double, NUM_FIELDS>;

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
    std::map<std::string, std::vector<Entry> > data;
    std::map<std::string, int> n;
    std::map<std::string, double> priors;
    std::map<std::string, std::vector<double> > multinomialLikelihoods;
    std::map<std::string, int> multinomialSums;
    std::map<std::string, Entry > sumX;
    std::map<std::string, std::vector<double> > means;
    std::map<std::string, std::vector<double> > variances;

    auto classify = [&](const std::string& label, const Entry& entry)
    {
        std::string predlabel;
        double maxlikelihood = 0.0;
        double denom = 0.0;
        std::vector<double> probs;
        for (auto it = priors.begin(); it != priors.end(); it++)
        {
            double numer = priors[it->first];
            const auto& firstMultinomialLikelihood = multinomialLikelihoods[it->first];
            const auto& firstMean = means[it->first];
            const auto& firstVariance = variances[it->first];
            for (int j = 0; j < NUM_FIELDS; j++)
                switch (decision)
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
    };

    auto readFromFile = [&](const char* filename, bool isClassification)
    {
        std::ifstream file(argv[1]);
        if (!file.is_open())
            return 1;

        while (!file.eof())
        {
            std::string line;
            std::getline(file, line);
            if ("" == line)
                continue;
            std::istringstream linein(std::move(line));
            std::string label;
            std::getline(linein, label, ',');
            Entry entry;
            auto setField = [&entry, &label, &sumX, &multinomialSums, isClassification](int i, const std::string& field)
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
                if (!isClassification)
                {
                    sumX[label][i] += entry[i];
                    multinomialSums[label] += entry[i];
                }
            };
            for (int i = 0; i < NUM_FIELDS; i++)
            {
                std::string field;
                std::getline(linein, field, ',');
                setField(i, field);
            }
            std::string field;
            std::getline(linein, field);
            setField(NUM_FIELDS - 1, field);
            if (!isClassification)
            {
                data[label].push_back(std::move(entry));
                n[label]++;
                total++;
            }
            else
                classify(label, entry);
        }

        return 0;
    };

    int errcode = readFromFile(argv[1], false);
    if (errcode)
        return errcode;

    for (auto it = sumX.begin(); it != sumX.end(); it++)
    {
        priors[it->first] = (double)n[it->first] / total;

        std::cout << "Class " << it->first << ", prior: " << std::setw(1) << std::setprecision(3) << priors[it->first] << std::endl;
        std::cout << "feature\tmean\tvar\tstddev\tmnl" << std::endl;

        // calculate means
        std::vector<double> featureMeans(NUM_FIELDS);
        for (int i = 0; i < NUM_FIELDS; i++)
            featureMeans[i] = sumX[it->first][i] / n[it->first];

        // calculate variances
        std::vector<double> featureVariances(NUM_FIELDS);
        const auto& firstData = data[it->first];
        for (int i = 0; i < (int)firstData.size(); i++)
            for (int j = 0; j < NUM_FIELDS; j++)
                featureVariances[j] += (firstData[i][j] - featureMeans[j]) * (firstData[i][j] - featureMeans[j]);
        for (int i = 0; i < NUM_FIELDS; i++)
            featureVariances[i] /= firstData.size();

        const auto& firstSumX = sumX[it->first];
        auto firstMultinomialSum = multinomialSums[it->first];
        auto& firstMultinomialLikelihood = multinomialLikelihoods[it->first];
        // calculate multinomial likelihoods
        for (int i = 0; i < NUM_FIELDS; i++)
        {
            double mnl = (firstSumX[i] + alpha) / (firstMultinomialSum + (alpha * featureMeans.size()));
            firstMultinomialLikelihood.push_back(mnl);
        }

        for (unsigned int i = 0; i < NUM_FIELDS; i++)
            printf("%i\t%2.3f\t%2.3f\t%2.3f\t%2.3f\n",i+1,featureMeans[i],featureVariances[i],sqrt(featureVariances[i]),firstMultinomialLikelihood[i]);

        means[it->first] = std::move(featureMeans);
        variances[it->first] = std::move(featureVariances);
    }

    // classify
    std::cout << "Classifying:" << std::endl;
    std::cout << "class\tprob\tresult" << std::endl;

    errcode = readFromFile(argv[2], true);
    if (errcode)
        return errcode;
    printf("Accuracy: %3.2f %% (%i/%i)\n", 100.0 * correct / totalClassified, correct, totalClassified);

    return 0;
}
