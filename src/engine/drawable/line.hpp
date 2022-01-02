#pragma once

#include <engine/drawable.hpp>
#include <engine/drawable/dot.hpp>

namespace eng
{
namespace draw
{
class Line : public IDrawable
{
  public:
    // position where the line starts from - one of the dots positions
    glm::vec2 StartPosition;

    // position of the mouse or an other dot
    glm::vec2 EndPosition;

    // line height
    float Height;

    // dots which the line connects - used to update positions once the dot positions change
    std::pair<Dot *, Dot *> ConnectedDots;

    Line();

    int setupBuffers() override;
    void draw(gl::ShaderProgram &program, const glm::vec2 &win_size) override;

    void updatePositions();

    ~Line();
};
} // namespace draw
} // namespace eng