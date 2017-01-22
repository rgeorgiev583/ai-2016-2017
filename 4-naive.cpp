#include <cstdio>
#include <vector>

struct Item
{
    long long Weight, Value;
};

static long long knapsack(long long maxWeight, const std::vector<Item>& items)
{
    if (items.size() == 0 || maxWeight == 0)
        return 0;

    auto lastItem = items.back();
    auto newItems = items;
    newItems.pop_back();

    if (lastItem.Weight > maxWeight)
        return knapsack(maxWeight, newItems);

    return std::max(lastItem.Value + knapsack(maxWeight - lastItem.Weight, newItems), knapsack(maxWeight, newItems));
}

int main()
{
    long long m;
    scanf("%lld", &m);
    int n;
    scanf("%d", &n);

    if (m < 0 || n < 0)
        return 1;

    std::vector<Item> items(n);

    for (auto& item: items)
    {
        scanf("%lld", &item.Value);
        scanf("%lld", &item.Weight);
    }

    printf("%lld\n", knapsack(m, items));
    return 0;
}
