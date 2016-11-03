#include <cstdio>
#include <vector>
#include <stack>
#include <set>
#include <algorithm>

int main()
{
    using namespace std;

    enum class Frog      { None, Brown, Green };
    enum class Movement  { None, JumpLeft, LeapLeft, JumpRight, LeapRight };

    unsigned int frogCount;
    scanf("%u", &frogCount);

    vector<Frog> lilies(2 * frogCount + 1);
    lilies[frogCount] = Frog::None;
    int blankPos = frogCount;

    for (auto i = 0; i < frogCount; i++)
    {
        lilies[i] = Frog::Brown;
        lilies[2 * frogCount - i] = Frog::Green;
    }

    auto printState = [&]
    {
        for (auto frog: lilies)
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
    };

    auto wasTargetReached = [&]
    {
        for (size_t i = 0; i < frogCount; i++)
            if (lilies[i] != Frog::Green || lilies[2 * frogCount - i] != Frog::Brown)
                return false;

        return lilies[frogCount] == Frog::None;
    };

    set<vector<Frog> > visited;
    stack<Movement> trace, movements;
    trace.push(Movement::None);

    while (!trace.empty() && !wasTargetReached())
    {
        auto undoMovement = [&](Movement movement)
        {
            switch (movement)
            {
                case Movement::JumpLeft:
                    lilies[blankPos] = Frog::Green;
                    blankPos--;
                    break;

                case Movement::LeapLeft:
                    lilies[blankPos] = Frog::Green;
                    blankPos -= 2;
                    break;

                case Movement::JumpRight:
                    lilies[blankPos] = Frog::Brown;
                    blankPos++;
                    break;

                case Movement::LeapRight:
                    lilies[blankPos] = Frog::Brown;
                    blankPos += 2;
                    break;
                
                default:
                    break;
            }

            lilies[blankPos] = Frog::None;
        };

        auto isStuckJumpLeft  = [&] { return blankPos >= lilies.size() - 1 || lilies[blankPos + 1] != Frog::Green; };
        auto isStuckLeapLeft  = [&] { return blankPos >= lilies.size() - 2 || lilies[blankPos + 2] != Frog::Green; };
        auto isStuckJumpRight = [&] { return blankPos < 1 || lilies[blankPos - 1] != Frog::Brown; };
        auto isStuckLeapRight = [&] { return blankPos < 2 || lilies[blankPos - 2] != Frog::Brown; };

        while (!movements.empty() && isStuckJumpLeft() && isStuckLeapLeft() && isStuckLeapLeft() && isStuckLeapRight())
        {
            undoMovement(movements.top());
            movements.pop();
        }

        auto movement = trace.top();
        trace.pop();

        auto canMove = true;

        if (lilies[blankPos] == Frog::None)
            switch (movement)
            {
                case Movement::JumpLeft:
                    if (isStuckJumpLeft())
                        canMove = false;
                    else
                    {
                        lilies[blankPos] = Frog::Green;
                        blankPos++;
                    }
                    break;

                case Movement::LeapLeft:
                    if (isStuckLeapLeft())
                        canMove = false;
                    else
                    {
                        lilies[blankPos] = Frog::Green;
                        blankPos += 2;
                    }
                    break;

                case Movement::JumpRight:
                    if (isStuckJumpRight())
                        canMove = false;
                    else
                    {
                        lilies[blankPos] = Frog::Brown;
                        blankPos--;
                    }
                    break;

                case Movement::LeapRight:
                    if (isStuckLeapRight())
                        canMove = false;
                    else
                    {
                        lilies[blankPos] = Frog::Brown;
                        blankPos -= 2;
                    }
                    break;
                
                default:
                    break;
            }
        else
            canMove = false;

        if (canMove)
        {
            lilies[blankPos] = Frog::None;

            if (!visited.count(lilies))
            {
                visited.insert(lilies);
                movements.push(movement);

                trace.push(Movement::JumpLeft);
                trace.push(Movement::LeapLeft);
                trace.push(Movement::JumpRight);
                trace.push(Movement::LeapRight);
            }
            //else
                //undoMovement(movement);

            printState();
        }
    }

    return 0;
}
