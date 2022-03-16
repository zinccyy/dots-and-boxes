#pragma once

#include <game/state.hpp>

#include <engine/drawable.hpp>
#include <engine/drawable/line.hpp>
#include <engine/drawable/box.hpp>

#include <array>
#include <map>
#include <list>

#define MAX_BOXES (6)
#define MAX_DOTS (MAX_BOXES + 1)
#define MAX_LINES ((MAX_BOXES * 2 + 1) * MAX_BOXES + MAX_BOXES)

namespace gm
{
namespace state
{
using LineIndices = std::pair<uint8_t, uint8_t>;
// board scoring state, adjency matrix etc.
struct BoardStateData
{
    BoardStateData() : CurrentPlayer(0), GameOver(false), Scores{0}, AdjencyMatrix{false}, BoxesDraw{false}
    {
    }

    void pickLine(std::pair<uint8_t, uint8_t> line);
    bool checkForNewBoxes(int N, int M);

    // connected dots
    std::array<std::array<bool, MAX_DOTS * MAX_DOTS>, MAX_DOTS * MAX_DOTS> AdjencyMatrix;

    // should the box be drawn or not
    std::array<std::array<bool, MAX_BOXES>, MAX_BOXES> BoxesDraw;

    // list of (x, y) indices for lines
    std::list<LineIndices> AvailableLines;

    // scores
    std::array<uint8_t, 2> Scores;

    // switch for the player
    bool CurrentPlayer;

    // is the game over -> all dots connected
    bool GameOver;
};
class BoardState : public gm::State
{
  public:
    // used mostly for minimax algorithm
    BoardStateData StateData;

    // colors
    std::array<glm::vec3, 2> PlayerColors;

    uint8_t N; // rows - dots number
    uint8_t M; // cols - dots number

    // drawable stuff
    std::array<std::array<eng::draw::Box, MAX_BOXES>, MAX_BOXES> Boxes;
    std::array<std::array<eng::draw::Dot, MAX_DOTS>, MAX_DOTS> Dots;

    // separate distances for the dots - x and y values
    glm::vec2 DotsDistance;

    // dynamically allocated because of user drawing a single line dynamically
    std::list<eng::draw::Line *> Lines;

    // currently drawn line - user trying to connect the dots
    eng::draw::Line *NewLine;

    // index + pointer to the dots
    std::pair<uint8_t, eng::draw::Dot *> PickedDot;
    std::pair<uint8_t, eng::draw::Dot *> ConnectDot;

    // constructor - init all needed values
    BoardState(Game *game);
    BoardState(Game *game, int n, int m);

    ~BoardState();

    int init() override;
    int processEvent(SDL_Event &event) override;
    int processInput() override;
    int draw() override;

  private:
    // recalculate positions of dots, boxes etc.
    void mRecalculatePositions(const glm::vec2 &win_size);

    // run minimax algorithm to get the best line option for CPU
    int mHeuristicValue(BoardStateData &state);
    std::pair<LineIndices, int8_t> mMiniMax(BoardStateData &state, uint8_t depth, int8_t alpha, int8_t beta);

    // use minimax algorithm to pick the best line for CPU
    int mCPUDrawLine();

    // calculate hovered dot - picked or connect
    void mGetHoveredDot();

    // distance between dots - x and y
    glm::vec2 mDotsDistance;

    // shadowed lines which are present until the line is drawn
    std::array<eng::draw::Line, MAX_DOTS> mPlaceholderRowLines;
    std::array<eng::draw::Line, MAX_DOTS> mPlaceholderColLines;

    // shaders
    utils::gl::ShaderProgram mDotShaderProgram;
    utils::gl::ShaderProgram mBoxShaderProgram;
    utils::gl::ShaderProgram mLineShaderProgram;
};
} // namespace state
} // namespace gm