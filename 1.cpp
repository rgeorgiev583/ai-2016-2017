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
        std::vector<Frog> Lilies;
        int Count, BlankPos;
        Step Movement;

    public:
        State(): Count(0), BlankPos(-1), Movement(Step::None)  { }
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

        Step GetMovement()      const  { return Movement; }

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

        std::shared_ptr<State> Move(Step movement) const
        {
            bool canMove = false;
            auto moved = std::make_shared<State>(*this);
            moved->Movement = movement;

            switch (movement)
            {
                case Step::JumpLeft:
                    if (!IsStuckJumpLeft())
                    {
                        moved->Lilies[moved->BlankPos] = Frog::Green;
                        moved->BlankPos++;
                        canMove = true;
                    }
                    break;

                case Step::LeapLeft:
                    if (!IsStuckLeapLeft())
                    {
                        moved->Lilies[moved->BlankPos] = Frog::Green;
                        moved->BlankPos += 2;
                        canMove = true;
                    }
                    break;

                case Step::JumpRight:
                    if (!IsStuckJumpRight())
                    {
                        moved->Lilies[moved->BlankPos] = Frog::Brown;
                        moved->BlankPos--;
                        canMove = true;
                    }
                    break;

                case Step::LeapRight:
                    if (!IsStuckLeapRight())
                    {
                        moved->Lilies[moved->BlankPos] = Frog::Brown;
                        moved->BlankPos -= 2;
                        canMove = true;
                    }
                    break;

                default:
                    break;
            }

            if (canMove)
                moved->Lilies[moved->BlankPos] = Frog::None;
            return canMove ? moved : nullptr;
        }

        std::shared_ptr<State> UndoStep(Step prevMovement) const
        {
            bool canUndoStep = false;
            auto undone = std::make_shared<State>(*this);

            switch (Movement)
            {
                case Step::JumpLeft:
                    undone->Lilies[BlankPos] = Frog::Green;
                    undone->BlankPos--;
                    canUndoStep = true;
                    break;

                case Step::LeapLeft:
                    undone->Lilies[BlankPos] = Frog::Green;
                    undone->BlankPos -= 2;
                    canUndoStep = true;
                    break;

                case Step::JumpRight:
                    undone->Lilies[BlankPos] = Frog::Brown;
                    undone->BlankPos++;
                    canUndoStep = true;
                    break;

                case Step::LeapRight:
                    undone->Lilies[BlankPos] = Frog::Brown;
                    undone->BlankPos += 2;
                    canUndoStep = true;
                    break;

                default:
                    break;
            }

            if (canUndoStep)
            {
                undone->Lilies[BlankPos] = Frog::None;
                undone->Movement = prevMovement;
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
