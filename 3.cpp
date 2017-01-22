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

            for (int i = 0; i < rows.size(); ++i)
                if (i != col && (rows[i] == row || abs(rows[i] - row) == abs(i - col)))
                    count++;

            return count;
        }

    public:
        Board(unsigned seed, int n, int interval): generator(seed), rows(n), refillInterval(interval)
        {
            Refill();
        }

        void Refill()
        {
            for (int i = 0; i < rows.size(); ++i)
                rows[i] = i;

            for (int i = 0; i < rows.size(); ++i)
                std::swap(rows[i], rows[generator() % rows.size()]);
        }

        void Solve()
        {
            int moveCount = 0;

            while (true)
            {
                std::vector<int> candidates;

                int totalConflictCount = 0;
                for (int i = 0; i < rows.size(); ++i)
                {
                    int conflictCount = getConflictCount(rows[i], i);
                    if (conflictCount > 0)
                        candidates.push_back(i);
                    totalConflictCount += conflictCount;
                }
                if (0 == totalConflictCount)
                    return;

                int worstQueenColumn = candidates[generator() % candidates.size()];
                int minConflictCount = rows.size();
                candidates.clear();
                for (int i = 0; i < rows.size(); ++i)
                {
                    int conflictCount = getConflictCount(i, worstQueenColumn);
                    if (conflictCount < minConflictCount)
                    {
                        minConflictCount = conflictCount;
                        candidates.clear();
                    }
                    if (conflictCount == minConflictCount)
                        candidates.push_back(i);
                }

                if (!candidates.empty())
                    rows[worstQueenColumn] = candidates[generator() % candidates.size()];

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
            for (int i = 0; i < rows.size(); ++i)
            {
                for (int j = 0; j < rows.size(); ++j)
                    printf(rows[j] == i ? "*" : "_");

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

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    NQueens::Board queens(seed, n, 2 * n);
    queens.Solve();
    queens.Print();
    return 0;
}
