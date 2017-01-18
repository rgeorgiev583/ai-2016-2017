#ifndef __STATE__
#define __STATE__

#include <memory>


namespace SlidingBlocks
{
    class SquareIntMatrix
    {
        void copy(const SquareIntMatrix& other);
        void destroy();

    protected:
        int** data;
        int size;

    public:
        SquareIntMatrix();
        SquareIntMatrix(int _size);
        SquareIntMatrix(const SquareIntMatrix& other);
        SquareIntMatrix& operator=(const SquareIntMatrix& other);
        ~SquareIntMatrix();

        int Size() const;

        bool operator==(const SquareIntMatrix& other) const;

        const int* operator[](int i) const;
        int* operator[](int i);
    };

    enum class Step  { None, Left, Right, Down, Up };

    class State: public SquareIntMatrix
    {
        int emptyCellY, emptyCellX;
        Step movement;

    public:
        State();
        State(int n, int from);
        State(const SquareIntMatrix& matrix, int emptyCellY_ = 0, int emptyCellX_ = 0);

        Step GetMovement() const;

        bool operator==(const State& other) const;

        std::shared_ptr<State> Move(Step movement) const;

        void Print() const;
    };

    int ManhattanDistance(const State& from, const State& to);
}


#endif  // __STATE__
