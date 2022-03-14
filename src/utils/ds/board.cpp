#include "utils/log.hpp"
#include <algorithm>
#include <utility>
#include <utils/ds/board.hpp>

#include <iostream>

namespace utils
{
namespace ds
{
std::pair<Line, int8_t> minimax_alg(Board &board, int depth, bool maximize, int8_t alpha = -100, int8_t beta = 100);
int staticEvaluate(Board &board);
std::pair<Line, int8_t> minimax_alg(Board &board, int depth, bool maximize, int8_t alpha, int8_t beta)
{
    std::pair<Line, int8_t> move = {{-1, -1}, 0};

    if (depth == 0 || board.Over)
    {
        move.second = staticEvaluate(board);
        return move;
    }

    if (maximize)
    {
        utils::log::debug("MAX");
        move.second = -100;
        for (auto line : board.AvailableLines)
        {
            utils::log::debug("trying line (%d, %d)", line.first, line.second);

            auto new_board = board;
            new_board.Simulation = true;

            new_board.move(line);
            auto any_new = new_board.checkForNewBoxes();
            if (!any_new)
            {
                // no new boxes found -> player loses turn
                new_board.CurrentPlayer = !new_board.CurrentPlayer;
            }

            auto eval = minimax_alg(new_board, depth - 1, !maximize, alpha, beta);
            if (eval.second > move.second)
            {
                // utils::log::debug("found better option for higher: %d - %d -> (%d, %d)", eval.second, move.second, line.first, line.second);
                move = {line, eval.second};
            }
            alpha = std::max(alpha, eval.second);
            if (beta <= alpha)
            {
                break;
            }
        }
    }
    else
    {
        utils::log::debug("MIN");
        move.second = 100;
        for (auto line : board.AvailableLines)
        {
            utils::log::debug("trying line (%d, %d)", line.first, line.second);

            auto new_board = board;
            new_board.Simulation = true;

            new_board.move(line);
            auto any_new = new_board.checkForNewBoxes();
            if (!any_new)
            {
                // no new boxes found -> player loses turn
                new_board.CurrentPlayer = !new_board.CurrentPlayer;
            }

            auto eval = minimax_alg(new_board, depth - 1, !maximize, alpha, beta);
            if (eval.second < move.second)
            {
                // utils::log::debug("found better option for lower: %d - %d -> (%d, %d)", eval.second, move.second, line.first, line.second);
                move = {line, eval.second};
            }
            beta = std::min(beta, eval.second);
            if (beta <= alpha)
            {
                break;
            }
        }
    }

    if (maximize)
    {
        utils::log::debug("Max value gathered: (%d, %d) = %d: %d", move.first.first, move.first.second, move.second, depth);
    }
    else
    {
        utils::log::debug("Min value gathered: (%d, %d) = %d: %d", move.first.first, move.first.second, move.second, depth);
    }

    return move;
}
int staticEvaluate(Board &board)
{
    // CPU score - player score
    return board.Scores[1] - board.Scores[0];
}
Board::Board()
{
}
Board::Board(int n, int m)
{
    N = n;
    M = m;
    Over = false;
    CurrentPlayer = false;
    Simulation = false;

    // set player scores to 0
    Scores = {0};

    // init adjency matrix to 0
    AdjencyMatrix = {false};
    Boxes = {false};

    utils::log::debug("NxM = %dx%d", N, M);

    // generate available lines to pick - state for the minimax algorithm
    for (int i = 0; i < this->N - 1; i++)
    {
        for (int j = 0; j < this->M - 1; j++)
        {
            const int top_left = this->M * i + j;
            const int top_right = this->M * i + (j + 1);
            const int bottom_left = this->M * (i + 1) + j;
            const int bottom_right = this->M * (i + 1) + (j + 1);

            this->AvailableLines.push_back(std::make_pair(top_left, top_right));
            this->AvailableLines.push_back(std::make_pair(top_left, bottom_left));

            utils::log::debug("line added: (%d, %d)", top_left, top_right);
            utils::log::debug("line added: (%d, %d)", top_left, bottom_left);

            if (j == this->M - 2)
            {
                // last col -> add right side
                this->AvailableLines.push_back(std::make_pair(top_right, bottom_right));
                utils::log::debug("line added: (%d, %d)", top_right, bottom_right);
            }

            if (i == this->N - 2)
            {
                // last row -> add bottom side
                this->AvailableLines.push_back(std::make_pair(bottom_left, bottom_right));
                utils::log::debug("line added: (%d, %d)", bottom_left, bottom_right);
            }
        }
    }

    for (auto line : AvailableLines)
    {
        utils::log::debug("Line = (%d, %d)", line.first, line.second);
    }
}
Line Board::minimax()
{
    Line line = {-1, -1};

    utils::log::debug("start score: (%d, %d)", Scores[0], Scores[1]);

    line = minimax_alg(*this, 3, true).first;

    utils::log::debug("calculated line: (%d, %d)", line.first, line.second);

    if (line == Line{-1, -1})
    {
        // pick any from the list of available because the choice wasn't made
        line = AvailableLines.back();
    }

    move(line);

    return line;
}
void Board::move(Line line)
{
    // remove line from the
    AvailableLines.remove(line);

    // check also for reverse -> player can connect dots differently
    AvailableLines.remove({line.second, line.first});

    // configure adjency matrix
    AdjencyMatrix[line.first][line.second] = true;
    AdjencyMatrix[line.second][line.first] = true;
}
bool Board::checkForNewBoxes()
{
    bool any_new = false;

    for (int i = 0; i < N - 1; i++)
    {
        for (int j = 0; j < M - 1; j++)
        {
            if (Boxes[i][j])
            {
                // box is already drawn
                continue;
            }

            const int top_left = M * i + j;
            const int top_right = M * i + (j + 1);
            const int bottom_left = M * (i + 1) + j;
            const int bottom_right = M * (i + 1) + (j + 1);

            // check if a box is formed
            if (AdjencyMatrix[top_left][top_right] && AdjencyMatrix[top_left][bottom_left] && AdjencyMatrix[top_right][bottom_right] && AdjencyMatrix[bottom_left][bottom_right])
            {
                Boxes[i][j] = true;
                any_new = true;

                utils::log::debug("Player %d++ -> (%d, %d, %d, %d)", CurrentPlayer, top_left, top_right, bottom_left, bottom_right);

                if (Simulation)
                {
                    Scores[CurrentPlayer]++;
                }
            }
        }
    }

    if (Scores[0] + Scores[1] == ((N - 1) * (M - 1)))
    {
        Over = true;
    }
    return any_new;
}
} // namespace ds
} // namespace utils