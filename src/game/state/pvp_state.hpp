#pragma once

#include <game/state.hpp>

#include <engine/drawable/box.hpp>
#include <engine/drawable/dot.hpp>
#include <engine/drawable/line.hpp>
#include <engine/drawable/character.hpp>

#include <utils/gl/shader_program.hpp>

#include <vector>
#include <memory>
#include <map>

namespace gm
{
namespace state
{
typedef std::map<char, eng::draw::Character> FontCharactersMap;
class PlayerVsPlayerState : public State
{
  public:
    PlayerVsPlayerState(Game *game);
    PlayerVsPlayerState(Game *game, int n, int m);

    int init() override;
    int processEvent(SDL_Event &event) override;
    int processInput() override;
    int draw() override;

    ~PlayerVsPlayerState();

  private:
    void mRecalculateDotsPositions(const glm::vec2 &win_size);

    // fetch currently hovered dot -> nullptr if no dot is hovered
    eng::draw::Dot *mGetHoveredDot();

    // playing field
    std::vector<std::vector<eng::draw::Dot>> mDots;

    // shadowed lines which are present until the line is drawn
    std::vector<std::vector<eng::draw::Line>> mPlaceholderRowLines;
    std::vector<std::vector<eng::draw::Line>> mPlaceholderColumnLines;

    // draw lines
    std::vector<eng::draw::Line *> mLines;

    // currently drawn line - the one trying to connect the dots
    eng::draw::Line *mNewLine;

    // dot where the mouse was pressed and is trying to connect with the other dot
    eng::draw::Dot *mPickedDot;

    // dot to connect with picked dot - once the mouse was released
    eng::draw::Dot *mConnectDot;

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
    utils::gl::ShaderProgram mLineShaderProgram;
    utils::gl::ShaderProgram mFontCharacterShaderProgram;

    // font stuff
    FontCharactersMap mCharsMap;

    // freetype data
    FT_Library mFreeType;
    FT_Face mRobotoFace;
};
} // namespace state
} // namespace gm
