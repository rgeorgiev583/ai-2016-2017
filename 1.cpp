#include <cstdio>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

enum class Frog  { None, Brown, Green };
enum class Step  { None, JumpLeft, LeapLeft, JumpRight, LeapRight };

struct FrogsState
{
    size_t Count;
    vector<Frog> Lilies;
    int BlankPos;
    Step Movement;

    FrogsState(): Count(0), BlankPos(-1), Movement(Step::None) {}
    FrogsState(size_t count): Count(count), Lilies(2 * Count + 1), BlankPos(Count), Movement(Step::None)
    {
        for (auto i = 0; i < Count; i++)
        {
            Lilies[i] = Frog::Brown;
            Lilies[2 * Count - i] = Frog::Green;
        }

        Lilies[BlankPos] = Frog::None;
    }

    bool operator==(const FrogsState& other) const
    {
        return Count == other.Count && Lilies == other.Lilies && BlankPos == other.BlankPos;
    }

    bool IsStuckJumpLeft()  const  { return BlankPos >= Lilies.size() - 1 || Lilies[BlankPos + 1] != Frog::Green; }
    bool IsStuckLeapLeft()  const  { return BlankPos >= Lilies.size() - 2 || Lilies[BlankPos + 2] != Frog::Green; }
    bool IsStuckJumpRight() const  { return BlankPos < 1                  || Lilies[BlankPos - 1] != Frog::Brown; }
    bool IsStuckLeapRight() const  { return BlankPos < 2                  || Lilies[BlankPos - 2] != Frog::Brown; }

    bool WasTargetReached() const
    {
        for (auto i = 0; i < Count; i++)
            if (Lilies[i] != Frog::Green || Lilies[2 * Count - i] != Frog::Brown)
                return false;

        return Lilies[Count] == Frog::None;
    };

    void Print() const
    {
        for (auto frog: Lilies)
            switch (frog)
            {
                case Frog::Brown:
                    printf(">");
                    break;

                case Frog::Green:
                    printf("<");
                    break;

                case Frog::None:
                    printf(" ");
                    break;
            }

        printf("\n");
    }

    bool Move(Step movement, FrogsState& moved) const
    {
        moved = *this;
        moved.Movement = movement;

        switch (movement)
        {
            case Step::JumpLeft:
                if (!IsStuckJumpLeft())
                {
                    moved.Lilies[moved.BlankPos] = Frog::Green;
                    moved.BlankPos++;
                    moved.Lilies[moved.BlankPos] = Frog::None;
                    return true;
                }
                break;

            case Step::LeapLeft:
                if (!IsStuckLeapLeft())
                {
                    moved.Lilies[moved.BlankPos] = Frog::Green;
                    moved.BlankPos += 2;
                    moved.Lilies[moved.BlankPos] = Frog::None;
                    return true;
                }
                break;

            case Step::JumpRight:
                if (!IsStuckJumpRight())
                {
                    moved.Lilies[moved.BlankPos] = Frog::Brown;
                    moved.BlankPos--;
                    moved.Lilies[moved.BlankPos] = Frog::None;
                    return true;
                }
                break;

            case Step::LeapRight:
                if (!IsStuckLeapRight())
                {
                    moved.Lilies[moved.BlankPos] = Frog::Brown;
                    moved.BlankPos -= 2;
                    moved.Lilies[moved.BlankPos] = Frog::None;
                    return true;
                }
                break;
            
            default:
                break;
        }

        return false;
    }

    bool UndoStep(Step prevMovement)
    {
        switch (Movement)
        {
            case Step::JumpLeft:
                Lilies[BlankPos] = Frog::Green;
                BlankPos--;
                Lilies[BlankPos] = Frog::None;
                Movement = prevMovement;
                return true;

            case Step::LeapLeft:
                Lilies[BlankPos] = Frog::Green;
                BlankPos -= 2;
                Lilies[BlankPos] = Frog::None;
                Movement = prevMovement;
                return true;

            case Step::JumpRight:
                Lilies[BlankPos] = Frog::Brown;
                BlankPos++;
                Lilies[BlankPos] = Frog::None;
                Movement = prevMovement;
                return true;

            case Step::LeapRight:
                Lilies[BlankPos] = Frog::Brown;
                BlankPos += 2;
                Lilies[BlankPos] = Frog::None;
                Movement = prevMovement;
                return true;
            
            default:
                break;
        }

        return false;
    };
};

/*
namespace std
{
    template <>
    struct hash<FrogsState>
    {
        size_t operator()(const FrogsState& k) const
        {
            return ((hash<size_t>()(k.Count) ^ (hash<vector<Frog> >()(k.Lilies) << 1)) >> 1) ^ (hash<int>()(k.BlankPos) << 1);
        }
    };
};
*/

int main()
{
    unsigned int count;
    scanf_s("%u", &count);

    vector<FrogsState> visited;
    stack<FrogsState> trace;
    stack<Step> movements;
    trace.push(FrogsState(count));

    while (!trace.empty() && !trace.top().WasTargetReached())
    {
        auto state = trace.top();
        trace.pop();

        auto move = [&](Step movement)
        {
            FrogsState newState;
            if (state.Move(movement, newState))
                trace.push(newState);
        };

        while (!movements.empty() && state.IsStuckJumpLeft() && state.IsStuckLeapLeft() && state.IsStuckJumpRight() && state.IsStuckLeapRight())
        {
            state.UndoStep(movements.top());
            movements.pop();
        }

        if (visited.end() == find(visited.begin(), visited.end(), state))
        {
            visited.push_back(state);
            movements.push(state.Movement);
            move(Step::JumpLeft);
            move(Step::LeapLeft);
            move(Step::JumpRight);
            move(Step::LeapRight);

            state.Print();
        }
    }

    if (!trace.empty())
        trace.top().Print();

    return 0;
}
