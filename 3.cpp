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

        int getConflictCount(int row, int col) const
        {
            int count = 0, n = rows.size();

            for (int i = 0; i < n; ++i)
                if (i != col)
                {
                    int j = rows[i];
                    if (j == row || abs(j - row) == abs(i - col))
                        count++;
                }

            return count;
        }

    public:
        Board(unsigned seed, int n): generator(seed), rows(n)
        {
            Refill();
        }

        void Refill()
        {
            int n = rows.size();

            for (int i = 0; i < n; ++i)
                rows[i] = i;

            for (int i = 0; i < n; ++i)
            {
                int j = generator() % n;
                std::swap(rows[i], rows[j]);
            }
        }

        void Solve()
        {
            int moveCount = 0, n = rows.size();
            std::vector<int> candidates;

            while (true)
            {
                int maxConflicts = 0;
                candidates.clear();
                for (int i = 0; i < n; ++i)
                {
                    int conflictCount = getConflictCount(rows[i], i);
                    if (conflictCount == maxConflicts)
                        candidates.push_back(i);
                    else if (conflictCount > maxConflicts)
                    {
                        maxConflicts = conflictCount;
                        candidates.clear();
                        candidates.push_back(i);
                    }
                }

                if (maxConflicts == 0)
                    return;

                int worstQueenColumn = candidates[generator() % candidates.size()];

                int minConflictCount = rows.size();
                candidates.clear();
                for (int i = 0; i < n; ++i)
                {
                    int conflictCount = getConflictCount(i, worstQueenColumn);
                    if (conflictCount == minConflictCount)
                        candidates.push_back(i);
                    else if (conflictCount < minConflictCount)
                    {
                        minConflictCount = conflictCount;
                        candidates.clear();
                        candidates.push_back(i);
                    }
                }

                if (!candidates.empty())
                    rows[worstQueenColumn] = candidates[generator() % candidates.size()];

                moveCount++;
                if (moveCount == n * 2)
                {
                    Refill();
                    moveCount = 0;
                }
            }
        }

        void Print() const
        {
            int n = rows.size();
            for (int i = 0; i < n; ++i)
            {
                for (int j = 0; j < n; ++j)
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
    NQueens::Board queens(seed, n);
    queens.Solve();
    queens.Print();
    return 0;
}
