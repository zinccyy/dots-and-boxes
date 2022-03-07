#pragma once

#include <game/state.hpp>

#include <engine/drawable/box.hpp>
#include <engine/drawable/dot.hpp>
#include <engine/drawable/line.hpp>
#include <engine/drawable/character.hpp>
#include <engine/drawable/text.hpp>

#include <utils/gl/shader_program.hpp>

#include <vector>
#include <memory>
#include <map>

namespace gm
{
namespace state
{
class PlayerVsCPUState : public State
{
  public:
    PlayerVsCPUState(Game *game);
    PlayerVsCPUState(Game *game, int n, int m);

    int init() override;
    int processEvent(SDL_Event &event) override;
    int processInput() override;
    int draw() override;

    ~PlayerVsCPUState();

  private:
    void mRecalculateDotsPositions(const glm::vec2 &win_size);

    // check if a line has been drawn resulting in a new box
    // returns true if any new boxes have been drawn -> current player keeps the turn
    bool mCheckForNewBoxes();

    // fetch currently hovered dot -> nullptr if no dot is hovered
    eng::draw::Dot *mGetHoveredDot();

    // playing field
    std::vector<std::vector<eng::draw::Dot>> mDots;

    // boxes to draw once they have been filled
    std::vector<std::vector<eng::draw::Box>> mBoxes;

    // shadowed lines which are present until the line is drawn
    std::vector<std::vector<eng::draw::Line>> mPlaceholderRowLines;
    std::vector<std::vector<eng::draw::Line>> mPlaceholderColumnLines;

    std::vector<std::vector<bool>> mAdjencyMatrix;

    // draw lines
    std::vector<eng::draw::Line *> mLines;

    // currently drawn line - the one trying to connect the dots
    eng::draw::Line *mNewLine;

    // dot where the mouse was pressed and is trying to connect with the other dot
    eng::draw::Dot *mPickedDot;

    // i,j of the picked dot
    int mPickedIndex;

    // dot to connect with picked dot - once the mouse was released
    eng::draw::Dot *mConnectDot;

    // i,j of the connect dot
    int mConnectIndex;

    // separate distances for the dots - x and y values
    glm::vec2 mDotsDistance;

    // N x M boxes
    glm::vec2 mBoardSize;

    // 0 or 1
    bool mCurrentPlayer;

    // player scores
    std::array<int, 2> mScores;

    // shaders
    utils::gl::ShaderProgram mDotShaderProgram;
    utils::gl::ShaderProgram mBoxShaderProgram;
    utils::gl::ShaderProgram mLineShaderProgram;
    utils::gl::ShaderProgram mFontCharacterShaderProgram;

    // font stuff
    eng::draw::FontCharactersMap mCharsMap;

    // freetype data
    FT_Library mFreeType;
    FT_Face mRobotoFace;

    // text to use
    std::array<eng::draw::Text, 2> mPlayerScoreText;

    // make game over
    bool mGameFinished;

    // colors
    std::array<glm::vec3, 2> mPlayerColors;
};
} // namespace state
} // namespace gm
