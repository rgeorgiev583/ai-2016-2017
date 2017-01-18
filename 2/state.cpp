#include <memory>

#include "state.hpp"


void SlidingBlocks::SquareIntMatrix::copy(const SlidingBlocks::SquareIntMatrix& other)
{
    size = other.size;
    data = new int*[size];

    for (int i = 0; i < size; ++i)
    {
        data[i] = new int[size];

        for (int j = 0; j < size; ++j)
            data[i][j] = other.data[i][j];
    }
}

void SlidingBlocks::SquareIntMatrix::destroy()
{
    for (int i = 0; i < size; ++i)
        delete data[i];

    delete data;
    data = nullptr;
}

SlidingBlocks::SquareIntMatrix::SquareIntMatrix():
        data(nullptr), size(0)
{
}

SlidingBlocks::SquareIntMatrix::SquareIntMatrix(int _size):
        data(nullptr), size(_size)
{
    data = new int*[size];

    for (int i = 0; i < size; ++i)
        data[i] = new int[size];
}

SlidingBlocks::SquareIntMatrix::SquareIntMatrix(const SlidingBlocks::SquareIntMatrix& other):
        data(nullptr)
{
    copy(other);
}

SlidingBlocks::SquareIntMatrix& SlidingBlocks::SquareIntMatrix::operator=(const SlidingBlocks::SquareIntMatrix& other)
{
    if (this != &other)
    {
        destroy();
        copy(other);
    }

    return *this;
}

SlidingBlocks::SquareIntMatrix::~SquareIntMatrix()
{
    destroy();
}

int SlidingBlocks::SquareIntMatrix::Size() const
{
    return size;
}

bool SlidingBlocks::SquareIntMatrix::operator==(const SlidingBlocks::SquareIntMatrix& other) const
{
    if (size != other.Size())
        return false;

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (data[i][j] != other.data[i][j])
                return false;

    return true;
}

const int* SlidingBlocks::SquareIntMatrix::operator[](int i) const
{
    return data[i];
}

int* SlidingBlocks::SquareIntMatrix::operator[](int i)
{
    return data[i];
}


SlidingBlocks::State::State(): emptyCellY(0), emptyCellX(0), movement(SlidingBlocks::Step::None)  { }

SlidingBlocks::State::State(int n, int begin): SquareIntMatrix(n), movement(SlidingBlocks::Step::None)
{
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            data[i][j] = begin++;

    emptyCellX = emptyCellY = size - 1;
    data[emptyCellY][emptyCellX] = 0;
}

SlidingBlocks::State::State(const SlidingBlocks::SquareIntMatrix& matrix, int _emptyCellY, int _emptyCellX):
        SquareIntMatrix(matrix), emptyCellY(_emptyCellY), emptyCellX(_emptyCellX), movement(SlidingBlocks::Step::None)
{
}

SlidingBlocks::Step SlidingBlocks::State::GetMovement() const
{
    return movement;
}

bool SlidingBlocks::State::operator==(const SlidingBlocks::State& other) const
{
    return SquareIntMatrix::operator==(other) &&
           emptyCellY == other.emptyCellY && emptyCellX == other.emptyCellX;
}

std::shared_ptr<SlidingBlocks::State> SlidingBlocks::State::Move(SlidingBlocks::Step movement) const
{
    auto canMove = false;
    auto moved = std::make_shared<SlidingBlocks::State>(*this);
    moved->movement = movement;

    switch (movement)
    {
        case Step::Left:
            if (moved->emptyCellX < Size() - 1)
            {
                moved->data[moved->emptyCellY][moved->emptyCellX] = moved->data[moved->emptyCellY][moved->emptyCellX + 1];
                ++moved->emptyCellX;
                canMove = true;
            }
            break;

        case Step::Right:
            if (moved->emptyCellX > 0)
            {
                moved->data[moved->emptyCellY][moved->emptyCellX] = moved->data[moved->emptyCellY][moved->emptyCellX - 1];
                --moved->emptyCellX;
                canMove = true;
            }
            break;

        case Step::Up:
            if (moved->emptyCellY < Size() - 1)
            {
                moved->data[moved->emptyCellY][moved->emptyCellX] = moved->data[moved->emptyCellY + 1][moved->emptyCellX];
                ++moved->emptyCellY;
                canMove = true;
            }
            break;

        case Step::Down:
            if (moved->emptyCellY > 0)
            {
                moved->data[moved->emptyCellY][moved->emptyCellX] = moved->data[moved->emptyCellY - 1][moved->emptyCellX];
                --moved->emptyCellY;
                canMove = true;
            }
            break;
    }

    if (canMove)
        moved->data[moved->emptyCellY][moved->emptyCellX] = 0;
    return canMove ? moved : nullptr;
}

void SlidingBlocks::State::Print() const
{
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
            printf("%d ", data[i][j]);
        printf("\n");
    }
}

int SlidingBlocks::ManhattanDistance(const SlidingBlocks::State& from, const SlidingBlocks::State& to)
{
    int size = from.Size();
    if (size != to.Size())
        return -1;

    int count = 0;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (from[i][j] != to[i][j])
                count++;
    return count;
}
