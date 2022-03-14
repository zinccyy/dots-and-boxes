#pragma once

#include <list>
#include <array>
#include <map>

namespace utils
{
namespace ds
{
using Line = std::pair<int, int>;
struct Board
{
    int N;
    int M;
    bool Over;
    bool CurrentPlayer;

    // alpha/beta pruning - minimax
    int Alpha;
    int Beta;

    // max board size: 6x6 boxes = (6+1)^2 dots
    const static int MAX_BOXES = 6;
    const static int MAX_DOTS = (MAX_BOXES + 1) * (MAX_BOXES + 1);

    std::array<int, 2> Scores;
    std::array<std::array<bool, MAX_DOTS>, MAX_DOTS> AdjencyMatrix;
    std::array<std::array<bool, MAX_BOXES>, MAX_BOXES> Boxes;
    std::list<Line> AvailableLines;

    Board();
    Board(int n, int m);

    // run minimax algorithm and return two indices for the dots to connect
    std::pair<int, int> minimax();

    // create move with the given line
    void move(Line line);
    bool checkForNewBoxes();
};
} // namespace ds
} // namespace utils