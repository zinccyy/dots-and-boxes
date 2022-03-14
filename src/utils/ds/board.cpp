#include "utils/log.hpp"
#include <utility>
#include <utils/ds/board.hpp>

#include <iostream>

namespace utils
{
namespace ds
{
std::pair<Line, int> minimax_alg(Board &board, int depth, bool maximize);
int staticEvaluate(Board &board);
std::pair<Line, int> minimax_alg(Board &board, int depth, bool maximize)
{
    std::pair<Line, int> move;

    if (maximize)
    {
        move = {{-1, -1}, -10000};
    }
    else
    {
        move = {{-1, -1}, 10000};
    }

    if (depth == 0 || board.Over)
    {
        move.second = staticEvaluate(board);
        return move;
    }

    for (auto line : board.AvailableLines)
    {
        utils::log::debug("trying line (%d, %d)", line.first, line.second);

        // clone board
        auto tmp_board = board;

        // move with available line
        tmp_board.move(line);
        tmp_board.checkForNewBoxes();

        auto heuristic_value = staticEvaluate(tmp_board);
        if (maximize)
        {
            if (heuristic_value >= tmp_board.Beta)
            {
                move = {line, heuristic_value};
                break;
            }
            else
            {
                tmp_board.Alpha = std::max(tmp_board.Alpha, heuristic_value);
            }
        }
        else
        {
            if (heuristic_value <= tmp_board.Alpha)
            {
                move = {line, heuristic_value};
                break;
            }
            else
            {
                tmp_board.Beta = std::max(tmp_board.Beta, heuristic_value);
            }
        }

        // evaluate position
        auto eval = minimax_alg(tmp_board, depth - 1, !maximize);

        // update best move
        if (maximize)
        {
            if (eval.second > move.second)
            {
                move = {line, eval.second};
            }
        }
        else
        {
            if (eval.second < move.second)
            {
                move = {line, eval.second};
            }
        }
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

    // set player scores to 0
    Scores = {0};

    // alpha/beta pruning
    Alpha = -10000;
    Beta = 10000;

    // init adjency matrix to 0
    AdjencyMatrix = {false};
    Boxes = {false};

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

            if (j == this->M - 2)
            {
                // last col -> add right side
                this->AvailableLines.push_back(std::make_pair(top_right, bottom_right));
            }

            if (i == this->N - 2)
            {
                // last row -> add bottom side
                this->AvailableLines.push_back(std::make_pair(bottom_left, bottom_right));
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

    line = minimax_alg(*this, 5, true).first;
    utils::log::debug("calculated line: (%d, %d)", line.first, line.second);

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

                utils::log::debug("New box found (%d,%d,%d,%d)", top_left, top_right, bottom_left, bottom_right);

                // CPU score
                Scores[CurrentPlayer]++;
            }
        }
    }

    if (Scores[0] + Scores[1] == ((N - 1) * (M - 1)))
    {
        // all the boxes have been drawn -> game over -> print a winner -> use imgui on next render
        Over = true;
    }
    return any_new;
}
} // namespace ds
} // namespace utils