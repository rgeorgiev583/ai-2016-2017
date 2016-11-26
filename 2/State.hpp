#ifndef __STATE__
#define __STATE__

#include <memory>

namespace SlidingBlocks
{
    template <typename T>
    class SquareMatrix
    {
        void copy(const SquareMatrix& other)
        {
            size = other.size;
            data = new T*[size];

            for (int i = 0; i < size; i++)
            {
                data[i] = new T[size];

                for (int j = 0; j < size; j++)
                    data[i][j] = other.data[i][j];
            }
        }

        void destroy()
        {
            for (int i = 0; i < size; i++)
                delete data[i];

            delete data;
            data = nullptr;
        }

    protected:
        T** data;
        int size;

    public:
        SquareMatrix(): data(nullptr), size(0)  { }
        SquareMatrix(int _size): data(nullptr), size(_size)
        {
            data = new T*[size];

            for (int i = 0; i < size; i++)
                data[i] = new T[size];
        }
        SquareMatrix(const SquareMatrix& other): data(nullptr)
        {
            copy(other);
        }
        SquareMatrix& operator=(const SquareMatrix& other)
        {
            if (&other != this)
            {
                destroy();
                copy(other);
            }

            return *this;
        }
        ~SquareMatrix()
        {
            destroy();
        }

        bool operator==(const SquareMatrix& other) const
        {
            if (size != other.Size())
                return false;

            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    if (data[i][j] != other.data[i][j])
                        return false;

            return true;
        }

        int Size() const                  { return size; }

        const T* operator[](int i) const  { return data[i]; }
        T* operator[](int i)              { return data[i]; }
    };

    enum class Step  { Left, Right, Down, Up };

    class State: public SquareMatrix<int>
    {
        int emptyCellY, emptyCellX;
        Step movement;

    public:
        State();
        State(int n, int from);
        State(const SquareMatrix<int>& matrix, int emptyCellY_ = 0, int emptyCellX_ = 0);

        bool operator==(const State& other) const;
        std::shared_ptr<State> Move(Step movement) const;

        Step GetMovement() const  { return movement; }

        void Print() const;
    };

    int ManhattanDistance(const State& from, const State& to);
}

#endif  // __STATE__
