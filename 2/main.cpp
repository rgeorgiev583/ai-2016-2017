#include <iostream>
#include <cmath>

#include "AStar.hpp"

int main()
{
    int n;
    scanf("%d", &n);
    int size = sqrt(n + 1), emptyCellY = 0, emptyCellX = 0;
    SlidingBlocks::SquareMatrix<int> beginMatrix(size);

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            scanf("%d", &beginMatrix[i][j]);

            if (beginMatrix[i][j] == 0)
            {
                emptyCellY = i;
                emptyCellX = j;
            }
        }

    SlidingBlocks::State begin(std::move(beginMatrix), emptyCellY, emptyCellX), end(size, 1);
    auto path = std::move(SlidingBlocks::FindPath(begin, end));

    printf("%d\n", path.size());

    for (auto node: path)
        switch (node->GetMovement())
        {
            case SlidingBlocks::Step::Left:
                printf("left\n");
                break;

            case SlidingBlocks::Step::Right:
                printf("right\n");
                break;

            case SlidingBlocks::Step::Down:
                printf("down\n");
                break;

            case SlidingBlocks::Step::Up:
                printf("up\n");
                break;

            default:
                break;
        }

    return 0;
}
