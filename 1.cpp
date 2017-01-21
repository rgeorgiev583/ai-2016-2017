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

    public:
        State(): count(0), blankPos(-1)
        {
        }
        State(int count): lilies(2 * count + 1), count(count), blankPos(count)
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
            return count == other.count && lilies == other.lilies && blankPos == other.blankPos;
        }

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
        }

        std::shared_ptr<State> Move(Step movement) const
        {
            bool canMove = false;
            auto moved = std::make_shared<State>(*this);

            switch (movement)
            {
                case Step::JumpLeft:
                    if (!moved->IsStuckJumpLeft())
                    {
                        moved->lilies[moved->blankPos] = Frog::Green;
                        moved->blankPos++;
                        canMove = true;
                    }
                    break;

                case Step::LeapLeft:
                    if (!moved->IsStuckLeapLeft())
                    {
                        moved->lilies[moved->blankPos] = Frog::Green;
                        moved->blankPos += 2;
                        canMove = true;
                    }
                    break;

                case Step::JumpRight:
                    if (!moved->IsStuckJumpRight())
                    {
                        moved->lilies[moved->blankPos] = Frog::Brown;
                        moved->blankPos--;
                        canMove = true;
                    }
                    break;

                case Step::LeapRight:
                    if (!moved->IsStuckLeapRight())
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
                        printf("_");
                        break;
                }

            printf("\n");
        }
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

        if (!state)
            continue;

        if (visited.crend() == std::find(visited.crbegin(), visited.crend(), state))
        {
            visited.push_back(state);
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
