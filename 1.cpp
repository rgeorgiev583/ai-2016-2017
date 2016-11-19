#include <cstdio>
#include <vector>
#include <stack>
#include <algorithm>

namespace Frogs
{
    enum class Frog  { None, Brown, Green };
    enum class Step  { None, JumpLeft, LeapLeft, JumpRight, LeapRight };

    struct State
    {
        std::vector<Frog> Lilies;
        int Count, BlankPos;
        Step Movement;

        State(): Count(0), BlankPos(-1), Movement(Step::None) {}
        State(int count): Lilies(2 * count + 1), Count(count), BlankPos(count), Movement(Step::None)
        {
            for (int i = 0; i < Count; ++i)
            {
                Lilies[i] = Frog::Brown;
                Lilies[2 * Count - i] = Frog::Green;
            }

            Lilies[BlankPos] = Frog::None;
        }

        bool operator==(const State& other) const
        {
            return Count == other.Count && Lilies == other.Lilies && BlankPos == other.BlankPos && Movement == other.Movement;
        }

        bool IsStuckJumpLeft()  const  { return BlankPos >= (int)Lilies.size() - 1 || Lilies[BlankPos + 1] != Frog::Green; }
        bool IsStuckLeapLeft()  const  { return BlankPos >= (int)Lilies.size() - 2 || Lilies[BlankPos + 2] != Frog::Green; }
        bool IsStuckJumpRight() const  { return BlankPos < 1                       || Lilies[BlankPos - 1] != Frog::Brown; }
        bool IsStuckLeapRight() const  { return BlankPos < 2                       || Lilies[BlankPos - 2] != Frog::Brown; }

        bool WasTargetReached() const
        {
            for (int i = 0; i < Count; ++i)
                if (Lilies[i] != Frog::Green || Lilies[2 * Count - i] != Frog::Brown)
                    return false;

            return Lilies[Count] == Frog::None;
        };

        void Print() const
        {
            for (const auto& frog: Lilies)
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

        std::pair<bool, State> Move(Step movement) const
        {
            auto canMove = false;
            auto moved = *this;
            moved.Movement = movement;

            switch (movement)
            {
                case Step::JumpLeft:
                    if (!IsStuckJumpLeft())
                    {
                        moved.Lilies[moved.BlankPos] = Frog::Green;
                        moved.BlankPos++;
                        canMove = true;
                    }
                    break;

                case Step::LeapLeft:
                    if (!IsStuckLeapLeft())
                    {
                        moved.Lilies[moved.BlankPos] = Frog::Green;
                        moved.BlankPos += 2;
                        canMove = true;
                    }
                    break;

                case Step::JumpRight:
                    if (!IsStuckJumpRight())
                    {
                        moved.Lilies[moved.BlankPos] = Frog::Brown;
                        moved.BlankPos--;
                        canMove = true;
                    }
                    break;

                case Step::LeapRight:
                    if (!IsStuckLeapRight())
                    {
                        moved.Lilies[moved.BlankPos] = Frog::Brown;
                        moved.BlankPos -= 2;
                        canMove = true;
                    }
                    break;

                default:
                    break;
            }

            if (canMove)
                moved.Lilies[moved.BlankPos] = Frog::None;
            return std::make_pair(canMove, moved);
        }

        bool UndoStep(Step prevMovement)
        {
            auto canUndoStep = false;

            switch (Movement)
            {
                case Step::JumpLeft:
                    Lilies[BlankPos] = Frog::Green;
                    BlankPos--;
                    canUndoStep = true;
                    break;

                case Step::LeapLeft:
                    Lilies[BlankPos] = Frog::Green;
                    BlankPos -= 2;
                    canUndoStep = true;
                    break;

                case Step::JumpRight:
                    Lilies[BlankPos] = Frog::Brown;
                    BlankPos++;
                    canUndoStep = true;
                    break;

                case Step::LeapRight:
                    Lilies[BlankPos] = Frog::Brown;
                    BlankPos += 2;
                    canUndoStep = true;
                    break;

                default:
                    break;
            }

            if (canUndoStep)
            {
                Lilies[BlankPos] = Frog::None;
                Movement = prevMovement;
            }
            return canUndoStep;
        };
    };
}

int main()
{
    int count;
    scanf("%u", &count);

    if (count < 0)
        return 1;

    std::vector<Frogs::State> visited;
    std::stack<Frogs::State> trace;
    std::stack<Frogs::Step> movements;
    trace.push(Frogs::State(count));

    while (!trace.empty() && !trace.top().WasTargetReached())
    {
        auto state = trace.top();
        trace.pop();

        auto move = [&](Frogs::Step movement)
        {
            auto newStateOptional = state.Move(movement);
            if (newStateOptional.first)
                trace.push(newStateOptional.second);
        };

        while (!movements.empty() && state.IsStuckJumpLeft() && state.IsStuckLeapLeft() && state.IsStuckJumpRight() && state.IsStuckLeapRight())
        {
            state.UndoStep(movements.top());
            movements.pop();
        }

        if (std::find(visited.crbegin(), visited.crend(), state) == visited.crend())
        {
            visited.push_back(state);
            movements.push(state.Movement);
            move(Frogs::Step::JumpLeft);
            move(Frogs::Step::LeapLeft);
            move(Frogs::Step::JumpRight);
            move(Frogs::Step::LeapRight);

            state.Print();
        }
    }

    if (!trace.empty())
        trace.top().Print();

    return 0;
}
