#include <cstdio>
#include <vector>
#include <stack>
#include <algorithm>
#include <memory>

namespace Frogs
{
    enum class Frog  { None, Brown, Green };
    enum class Step  { None, JumpLeft, LeapLeft, JumpRight, LeapRight };

    class State
    {
        std::vector<Frog> lilies;
        int count, blankPos;
        Step movement;

    public:
        State(): count(0), blankPos(-1), movement(Step::None)
        {
        }

        State(int count): lilies(2 * count + 1), count(count), blankPos(count), movement(Step::None)
        {
            for (int i = 0; i < count; ++i)
            {
                lilies[i] = Frog::Brown;
                lilies[2 * count - i] = Frog::Green;
            }

            lilies[blankPos] = Frog::None;
        }

        bool operator==(const State& other) const
        {
            return count == other.count && lilies == other.lilies && blankPos == other.blankPos && movement == other.movement;
        }

        Step GetMovement()      const  { return movement; }

        bool IsStuckJumpLeft()  const  { return blankPos >= (int)lilies.size() - 1 || lilies[blankPos + 1] != Frog::Green; }
        bool IsStuckLeapLeft()  const  { return blankPos >= (int)lilies.size() - 2 || lilies[blankPos + 2] != Frog::Green; }
        bool IsStuckJumpRight() const  { return blankPos < 1                       || lilies[blankPos - 1] != Frog::Brown; }
        bool IsStuckLeapRight() const  { return blankPos < 2                       || lilies[blankPos - 2] != Frog::Brown; }

        bool WasTargetReached() const
        {
            for (int i = 0; i < count; ++i)
                if (lilies[i] != Frog::Green || lilies[2 * count - i] != Frog::Brown)
                    return false;

            return lilies[count] == Frog::None;
        };

        void Print() const
        {
            for (const auto& frog: lilies)
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

        std::shared_ptr<State> Move(Step movement) const
        {
            bool canMove = false;
            auto moved = std::make_shared<State>(*this);
            moved->movement = movement;

            switch (movement)
            {
                case Step::JumpLeft:
                    if (!IsStuckJumpLeft())
                    {
                        moved->lilies[moved->blankPos] = Frog::Green;
                        moved->blankPos++;
                        canMove = true;
                    }
                    break;

                case Step::LeapLeft:
                    if (!IsStuckLeapLeft())
                    {
                        moved->lilies[moved->blankPos] = Frog::Green;
                        moved->blankPos += 2;
                        canMove = true;
                    }
                    break;

                case Step::JumpRight:
                    if (!IsStuckJumpRight())
                    {
                        moved->lilies[moved->blankPos] = Frog::Brown;
                        moved->blankPos--;
                        canMove = true;
                    }
                    break;

                case Step::LeapRight:
                    if (!IsStuckLeapRight())
                    {
                        moved->lilies[moved->blankPos] = Frog::Brown;
                        moved->blankPos -= 2;
                        canMove = true;
                    }
                    break;

                default:
                    break;
            }

            if (canMove)
                moved->lilies[moved->blankPos] = Frog::None;
            return canMove ? moved : nullptr;
        }

        std::shared_ptr<State> UndoStep(Step prevMovement) const
        {
            bool canUndoStep = false;
            auto undone = std::make_shared<State>(*this);

            switch (movement)
            {
                case Step::JumpLeft:
                    undone->lilies[blankPos] = Frog::Green;
                    undone->blankPos--;
                    canUndoStep = true;
                    break;

                case Step::LeapLeft:
                    undone->lilies[blankPos] = Frog::Green;
                    undone->blankPos -= 2;
                    canUndoStep = true;
                    break;

                case Step::JumpRight:
                    undone->lilies[blankPos] = Frog::Brown;
                    undone->blankPos++;
                    canUndoStep = true;
                    break;

                case Step::LeapRight:
                    undone->lilies[blankPos] = Frog::Brown;
                    undone->blankPos += 2;
                    canUndoStep = true;
                    break;

                default:
                    break;
            }

            if (canUndoStep)
            {
                undone->lilies[blankPos] = Frog::None;
                undone->movement = prevMovement;
            }
            return canUndoStep ? undone : nullptr;
        };
    };
}

int main()
{
    int count;
    scanf("%u", &count);

    if (count < 0)
        return 1;

    std::vector<std::shared_ptr<Frogs::State> > visited;
    std::stack<std::shared_ptr<Frogs::State> > trace;
    std::stack<Frogs::Step> movements;
    trace.push(std::make_shared<Frogs::State>(count));

    while (!trace.empty() && !trace.top()->WasTargetReached())
    {
        auto state = trace.top();
        trace.pop();

        auto move = [&](Frogs::Step movement)
        {
            auto newState = state->Move(movement);
            if (newState)
                trace.push(newState);
        };

        while (!movements.empty() && state && state->IsStuckJumpLeft() && state->IsStuckLeapLeft() && state->IsStuckJumpRight() && state->IsStuckLeapRight())
        {
            state = state->UndoStep(movements.top());
            movements.pop();
        }

        if (!state)
            continue;

        if (std::find(visited.crbegin(), visited.crend(), state) == visited.crend())
        {
            visited.push_back(state);
            movements.push(state->GetMovement());
            move(Frogs::Step::JumpLeft);
            move(Frogs::Step::LeapLeft);
            move(Frogs::Step::JumpRight);
            move(Frogs::Step::LeapRight);

            state->Print();
        }
    }

    if (!trace.empty())
        trace.top()->Print();

    return 0;
}
