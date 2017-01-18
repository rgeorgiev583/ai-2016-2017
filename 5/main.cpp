#include <cstdio>

#include "tictactoe.hpp"


int main()
{
    auto play = [] (TicTacToe* it) {
        printf("\nChoose an option:\n"
               "(X) Play as MAX\n"
               "(O) Play as MIN\n"
               "(Q) Quit\n");

        TicTacToe::Integer human = 0;
        while (!human)
        {
            char option;
            scanf("%c", &option);
            switch (option)
            {
                case 'Q':
                case 'q':
                    return false;

                case 'X':
                case 'x':
                    human = TicTacToe::Max;
                    break;

                case 'O':
                case 'o':
                    human = TicTacToe::Min;
                    break;
            }
        }

        while (true)
        {
            printf("\n");
            it->Print();
            TicTacToe::Integer move = 0;
            if (it->Turn == human)
            {
                printf("Your move: ");
                while (true)
                {
                    int moveX, moveY;
                    scanf("%d %d", &moveX, &moveY);
                    move = moveY * 3 + moveX;
                    if (0 <= move && move < TicTacToe::Size && it->Board[move] == TicTacToe::Zero)
                        break;
                    printf("Invalid move!\n");
                }
            }
            else
            {
                if (human == TicTacToe::Max)
                {
                    TicTacToe::Integer min = +TicTacToe::Infinity;
                    for (TicTacToe::Integer p = 0; p < TicTacToe::Size; ++p)
                        if (it->Board[p] == TicTacToe::Zero)
                        {
                            TicTacToe* child = it->GetChild(p);
                            if (child->Payoff < min)
                            {
                                min = child->Payoff;
                                move = p;
                            }
                        }
                }
                else
                {
                    TicTacToe::Integer max = -TicTacToe::Infinity;
                    for (TicTacToe::Integer p = 0; p < TicTacToe::Size; ++p)
                        if (it->Board[p] == TicTacToe::Zero)
                        {
                            TicTacToe* child = it->GetChild(p);
                            if (child->Payoff > max)
                            {
                                max = child->Payoff;
                                move = p;
                            }
                        }
                }
                printf("Computer move: \n");
            }

            it = it->GetChild(move);
            if (it->Depth == TicTacToe::Size || it->IsWin())
            {
                printf("\n");
                it->Print();
                TicTacToe::Integer humanPayoff = human * it->Payoff;
                if (humanPayoff > 0)
                    printf("You win!\n");
                else if (humanPayoff < 0)
                    printf("You lose!\n");
                else
                    printf("Draw!\n");
                return true;
            }
        }
    };

    TicTacToe root;
    while (play(&root));
    return 0;
}
