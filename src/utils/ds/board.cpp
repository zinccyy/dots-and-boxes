#include <utils/ds/board.hpp>

#include <iostream>

namespace utils
{
namespace ds
{
Board::Board()
{
}
Board::Board(int n, int m)
{
    N = n;
    M = m;

    // init adjency matrix to null
    AdjencyMatrix = {false};

    // generate available lines to pick
}

} // namespace ds
} // namespace utils