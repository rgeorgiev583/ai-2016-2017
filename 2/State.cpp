#include <memory>

#include "State.hpp"


SlidingBlocks::State::State(): emptyCellY(0), emptyCellX(0)  { }

SlidingBlocks::State::State(int n, int begin): SquareMatrix<int>(n)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            data[i][j] = begin++;

    emptyCellX = emptyCellY = size - 1;
    data[emptyCellY][emptyCellX] = 0;
}

SlidingBlocks::State::State(const SquareMatrix<int>& matrix, int _emptyCellY, int _emptyCellX):
        SquareMatrix<int>(matrix), emptyCellY(_emptyCellY), emptyCellX(_emptyCellX)  { }

bool SlidingBlocks::State::operator==(const SlidingBlocks::State& other) const
{
    return SquareMatrix<int>::operator==(other) &&
           emptyCellX == other.emptyCellX && emptyCellY == other.emptyCellY;
}

std::shared_ptr<SlidingBlocks::State> SlidingBlocks::State::Move(Step movement) const
{
    auto canMove = false;
    std::shared_ptr<State> moved = std::make_shared<SlidingBlocks::State>(*this);
    moved->movement = movement;

    switch (movement)
    {
        case Step::Left:
            if (emptyCellX < Size() - 1)
            {
                (*moved)[moved->emptyCellY][moved->emptyCellX] = (*moved)[moved->emptyCellY][moved->emptyCellX + 1];
                moved->emptyCellX++;
                canMove = true;
            }
            break;

        case Step::Right:
            if (emptyCellX > 0)
            {
                (*moved)[moved->emptyCellY][moved->emptyCellX] = (*moved)[moved->emptyCellY][moved->emptyCellX - 1];
                moved->emptyCellX--;
                canMove = true;
            }
            break;

        case Step::Down:
            if (emptyCellY < Size() - 1)
            {
                (*moved)[moved->emptyCellY][moved->emptyCellX] = (*moved)[moved->emptyCellY + 1][moved->emptyCellX];
                moved->emptyCellY++;
                canMove = true;
            }
            break;

        case Step::Up:
            if (emptyCellY > 0)
            {
                (*moved)[moved->emptyCellY][moved->emptyCellX] = (*moved)[moved->emptyCellY - 1][moved->emptyCellX];
                moved->emptyCellY--;
                canMove = true;
            }
            break;
    }

    if (canMove)
        (*moved)[emptyCellY][emptyCellX] = 0;
    return canMove ? moved : nullptr;
}

void SlidingBlocks::State::Print() const
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            printf("%d ", data[i][j]);

        printf("\n");
    }
}

int SlidingBlocks::ManhattanDistance(const State& from, const State& to)
{
    int size = from.Size();

    if (size != to.Size())
        return -1;

    int count = 0;

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (from[i][j] != to[i][j])
                count++;

    return count;
}
