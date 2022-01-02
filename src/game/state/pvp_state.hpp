#pragma once

#include <game/state.hpp>

#include <engine/drawable/box.hpp>
#include <engine/drawable/dot.hpp>
#include <engine/drawable/line.hpp>

#include <utils/gl/shader_program.hpp>

#include <array>
#include <vector>
#include <memory>

namespace gm
{
namespace state
{
class PlayerVsPlayerState : public State
{
  public:
    PlayerVsPlayerState(Game *game);

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
    std::array<std::array<eng::draw::Dot, 3>, 3> mDots;

    // draw lines
    std::vector<eng::draw::Line *> mLines;

    // currently drawn line - the one trying to connect the dots
    eng::draw::Line *mNewLine;

    // dot where the mouse was pressed and is trying to connect with the other dot
    eng::draw::Dot *mPickedDot;

    // dot to connect with picked dot - once the mouse was released
    eng::draw::Dot *mConnectDot;

    // shaders
    utils::gl::ShaderProgram mDotShaderProgram;
    utils::gl::ShaderProgram mLineShaderProgram;
};
} // namespace state
} // namespace gm
