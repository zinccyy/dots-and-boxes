#pragma once

#include <engine/drawable.hpp>

namespace eng
{
namespace draw
{
class Box : public IDrawable
{
  public:
    Box();

    int setupBuffers() override;
    void draw(gl::ShaderProgram &program) override;

  private:
    // position of the dot
    glm::vec2 mPosition;

    // size of the box -> w = h = size
    float mSize;
};
} // namespace draw
} // namespace eng