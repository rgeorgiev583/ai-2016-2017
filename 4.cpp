#include <cstdio>
#include <vector>


namespace Knapsack
{
    class IntMatrix
    {
        long long** data;
        int rows;
        long long cols;

        void copy(const IntMatrix& other)
        {
            rows = other.rows;
            cols = other.cols;
            data = new long long*[rows];

            for (int i = 0; i < rows; ++i)
            {
                data[i] = new long long[cols];

                for (long long j = 0; j < cols; ++j)
                    data[i][j] = other.data[i][j];
            }
        }

        void destroy()
        {
            for (int i = 0; i < rows; ++i)
                delete data[i];

            delete data;
            data = nullptr;
        }

    public:
        IntMatrix(): data(nullptr), rows(0), cols(0)
        {
        }
        IntMatrix(int rows_, long long cols_): data(nullptr), rows(rows_), cols(cols_)
        {
            data = new long long*[rows];

            for (int i = 0; i < rows; ++i)
                data[i] = new long long[cols];
        }
        IntMatrix(const IntMatrix& other): data(nullptr)
        {
            copy(other);
        }
        IntMatrix& operator=(const IntMatrix& other)
        {
            if (this != &other)
            {
                destroy();
                copy(other);
            }

            return *this;
        }
        ~IntMatrix()
        {
            destroy();
        }

        const long long* operator[](int i) const  { return data[i]; }
        long long* operator[](int i)              { return data[i]; }
    };

    struct Item
    {
        long long Weight, Value;
    };

    long long Solve(long long maxWeight, const std::vector<Item>& items)
    {
        int n = items.size();
        IntMatrix knapsack(n+1, maxWeight + 1);

        for (int i = 0; i <= n; ++i)
            for (long long j = 0; j <= maxWeight; ++j)
                if (i == 0 || j == 0)
                    knapsack[i][j] = 0;
                else if (items[i-1].Weight <= j)
                    knapsack[i][j] = std::max(items[i-1].Value + knapsack[i-1][j - items[i-1].Weight], knapsack[i-1][j]);
                else
                    knapsack[i][j] = knapsack[i-1][j];

       return knapsack[n][maxWeight];
    }
}

int main()
{
    long long m;
    scanf("%d", &m);
    int n;
    scanf("%d", &n);

    if (m < 0 || n < 0)
        return 1;

    std::vector<Knapsack::Item> items(n);

    for (auto& item: items)
    {
        scanf("%d", &item.Value);
        scanf("%d", &item.Weight);
    }

    printf("%d\n", Knapsack::Solve(m, items));
    return 0;
}
