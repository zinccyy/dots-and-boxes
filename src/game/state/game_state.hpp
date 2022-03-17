#pragma once

#include <game/state.hpp>

#include <engine/drawable/box.hpp>
#include <engine/drawable/dot.hpp>
#include <engine/drawable/line.hpp>
#include <engine/drawable/character.hpp>
#include <engine/drawable/text.hpp>
#include <game/state/board_state.hpp>

#include <utils/gl/shader_program.hpp>

#include <vector>
#include <memory>
#include <map>

namespace gm
{
namespace state
{
enum class GameLevel
{
    None = -1,
    Easy = 0,
    Medium = 1,
    Hard = 2,
};
class GameState : public State
{
  public:
    GameState(Game *game);
    GameState(Game *game, int n, int m, GameLevel level);

    int init() override;
    int processEvent(SDL_Event &event) override;
    int processInput() override;
    int draw() override;

    ~GameState();

  private:
    void mRecalculatePositions(const glm::vec2 &win_size);

    // shaders
    utils::gl::ShaderProgram mFontCharacterShaderProgram;

    // font stuff
    eng::draw::FontCharactersMap mCharsMap;

    // freetype data
    FT_Library mFreeType;
    FT_Face mRobotoFace;

    // text to use
    std::array<eng::draw::Text, 2> mPlayerScoreText;
    std::array<eng::draw::Text, 2> mPlayerNameText;

    // CPU level
    GameLevel mLevel;

    // board sub state - gathering all drawables used for the board in one place
    BoardState *mBoardState;

    // is the restart menu ready - prevent flickering and drawing main menu imgui instead of restart/main menu window
    bool mRestartMenuReady;
};
} // namespace state
} // namespace gm
