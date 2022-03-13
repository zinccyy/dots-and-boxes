#pragma once

#include <vector>
#include <array>
#include <map>

namespace utils
{
namespace ds
{
struct Line
{
    std::array<int, 2> Indices;
};
struct Board
{
    int N;
    int M;

    const static int MAX_DOTS = 7;

    std::array<std::array<int, MAX_DOTS>, MAX_DOTS> AdjencyMatrix;
    std::map<std::pair<int, int>, bool> AvailableLines;

    Board();
    Board(int n, int m);
};
} // namespace ds
} // namespace utils