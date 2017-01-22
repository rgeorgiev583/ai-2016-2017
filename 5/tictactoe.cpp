#include <cstdio>
#include <iterator>

#include "tictactoe.hpp"

TicTacToe* TicTacToe::GetChild(Integer move)
{
    if (!children[move])
        children[move] = new TicTacToe(this, move, -Infinity, +Infinity);
    return children[move];
}

bool TicTacToe::IsWin() const
{
    const Integer parentTurn = parent->Turn;
    switch (move)
    {
        case 0:
            return (parentTurn == Board[1] && parentTurn == Board[2]) ||
                   (parentTurn == Board[3] && parentTurn == Board[6]) ||
                   (parentTurn == Board[4] && parentTurn == Board[8]);
        case 1:
            return (parentTurn == Board[0] && parentTurn == Board[2]) ||
                   (parentTurn == Board[4] && parentTurn == Board[7]);
        case 2:
            return (parentTurn == Board[1] && parentTurn == Board[0]) ||
                   (parentTurn == Board[5] && parentTurn == Board[8]) ||
                   (parentTurn == Board[4] && parentTurn == Board[6]);
        case 3:
            return (parentTurn == Board[4] && parentTurn == Board[5]) ||
                   (parentTurn == Board[0] && parentTurn == Board[6]);
        case 4:
            return (parentTurn == Board[3] && parentTurn == Board[5]) ||
                   (parentTurn == Board[1] && parentTurn == Board[7]) ||
                   (parentTurn == Board[0] && parentTurn == Board[8]) ||
                   (parentTurn == Board[2] && parentTurn == Board[6]);
        case 5:
            return (parentTurn == Board[4] && parentTurn == Board[3]) ||
                   (parentTurn == Board[2] && parentTurn == Board[8]);
        case 6:
            return (parentTurn == Board[7] && parentTurn == Board[8]) ||
                   (parentTurn == Board[3] && parentTurn == Board[0]) ||
                   (parentTurn == Board[4] && parentTurn == Board[2]);
        case 7:
            return (parentTurn == Board[6] && parentTurn == Board[8]) ||
                   (parentTurn == Board[4] && parentTurn == Board[1]);
        case 8:
            return (parentTurn == Board[7] && parentTurn == Board[6]) ||
                   (parentTurn == Board[5] && parentTurn == Board[2]) ||
                   (parentTurn == Board[4] && parentTurn == Board[0]);
        default:
            return false;
    }
}

TicTacToe::TicTacToe():
        parent(nullptr), move(-1), alpha(-Infinity), beta(+Infinity), Turn(Max), Depth(0)
{
    Search();
}

TicTacToe::TicTacToe(const TicTacToe* parent, Integer move, Integer alpha, Integer beta):
        parent(parent), move(move), alpha(alpha), beta(beta), Turn(-parent->Turn), Depth(parent->Depth + 1)
{
    std::copy(std::begin(parent->Board), std::end(parent->Board), Board);
    Board[move] = parent->Turn;
    bool isWin = IsWin(), isFull = Depth == Size;
    if (isWin || isFull)
        Payoff = isWin ? parent->Turn * (10 - Depth) : Zero;
    else
        Search();
}

void TicTacToe::Search()
{
    if (Max == Turn)
    {
        Integer max = -Infinity;
        for (Integer p = 0; p < Size; ++p)
            if (Zero == Board[p])
            {
                children[p] = new TicTacToe(this, p, alpha, beta);
                if (children[p]->Payoff > max)
                {
                    max = children[p]->Payoff;
                    if (max > alpha && (alpha = max) >= beta)
                        break;
                }
            }
        Payoff = max;
    }
    else
    {
        Integer min = +Infinity;
        for (Integer p = 0; p < Size; ++p)
            if (Zero == Board[p])
            {
                children[p] = new TicTacToe(this, p, alpha, beta);
                if (children[p]->Payoff < min)
                {
                    min = children[p]->Payoff;
                    if (min < beta && (beta = min) <= alpha)
                        break;
                }
            }
        Payoff = min;
    }
}

TicTacToe::~TicTacToe()
{
    for (auto child: children)
        delete child;
}

void TicTacToe::Print() const
{
    auto getPlayerSign = [](Integer turn) {
        switch (turn)
        {
            case Max:
                return 'x';

            case Min:
                return 'o';

            default:
                return ' ';
        }
    };
    printf(
        "+---+---+---+\n"
        "| %c | %c | %c |\n"
        "+---+---+---+\n"
        "| %c | %c | %c |\n"
        "+---+---+---+\n"
        "| %c | %c | %c |\n"
        "+---+---+---+\n",
        getPlayerSign(Board[0]), getPlayerSign(Board[1]), getPlayerSign(Board[2]),
        getPlayerSign(Board[3]), getPlayerSign(Board[4]), getPlayerSign(Board[5]),
        getPlayerSign(Board[6]), getPlayerSign(Board[7]), getPlayerSign(Board[8])
    );
}
