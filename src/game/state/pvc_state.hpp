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
    Easy = 0,
    Medium = 1,
    Hard = 2,
};
class PlayerVsCPUState : public State
{
  public:
    PlayerVsCPUState(Game *game);
    PlayerVsCPUState(Game *game, int n, int m, GameLevel level);

    int init() override;
    int processEvent(SDL_Event &event) override;
    int processInput() override;
    int draw() override;

    ~PlayerVsCPUState();

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

    // CPU level
    GameLevel mLevel;

    // board sub state - gathering all drawables used for the board in one place
    BoardState *mBoardState;
};
} // namespace state
} // namespace gm
