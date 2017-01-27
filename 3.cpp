#include <cstdio>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>


namespace NQueens
{
    class Board
    {
        std::default_random_engine generator;
        std::vector<int> rows;
        int refillInterval;

        int getConflictCount(int row, int col) const
        {
            int count = 0;

            for (int i = 0; i < (int)rows.size(); ++i)
                if (i != col && (rows[i] == row || abs(rows[i] - row) == abs(i - col)))
                    ++count;

            return count;
        }

    public:
        Board(unsigned seed, int size, int interval): generator(seed), rows(size), refillInterval(interval)
        {
            Refill();
        }

        void Refill()
        {
            for (int i = 0; i < (int)rows.size(); ++i)
                rows[i] = i;

            for (int row: rows)
            {
                std::uniform_int_distribution<> distribution(0, (int)rows.size() - 1);
                std::swap(row, rows[distribution(generator)]);
            }
        }

        void Solve()
        {
            int moveCount = 0;

            while (true)
            {
                std::vector<int> candidates;

                for (int i = 0; i < (int)rows.size(); ++i)
                    if (getConflictCount(rows[i], i) > 0)
                        candidates.push_back(i);
                if (candidates.empty())
                    return;

                std::uniform_int_distribution<> worstDistribution(0, (int)candidates.size() - 1);
                int worstColumn = candidates[worstDistribution(generator)];
                candidates.clear();
                int minConflictCount = (int)rows.size();
                for (int i = 0; i < (int)rows.size(); ++i)
                {
                    int conflictCount = getConflictCount(i, worstColumn);
                    if (conflictCount < minConflictCount)
                    {
                        minConflictCount = conflictCount;
                        candidates.clear();
                    }
                    if (conflictCount == minConflictCount)
                        candidates.push_back(i);
                }
                if (!candidates.empty())
                {
                    std::uniform_int_distribution<> bestDistribution(0, (int)candidates.size() - 1);
                    rows[worstColumn] = candidates[bestDistribution(generator)];
                }

                ++moveCount;
                if (refillInterval == moveCount)
                {
                    Refill();
                    moveCount = 0;
                }
            }
        }

        void Print() const
        {
            for (int i = 0; i < (int)rows.size(); ++i)
            {
                for (int row: rows)
                    printf(row == i ? "*" : "_");

                printf("\n");
            }
        }
    };
}

int main()
{
    int n;
    scanf("%d", &n);
    if (n < 0)
        return 1;

    NQueens::Board queens(std::chrono::system_clock::now().time_since_epoch().count(), n, 2 * n);
    queens.Solve();
    queens.Print();
    return 0;
}
