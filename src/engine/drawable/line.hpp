#pragma once

#include <engine/drawable.hpp>

namespace eng
{
namespace draw
{
class Line : public IDrawable
{
  public:
    Line();

    int setupBuffers() override;
    void draw(gl::ShaderProgram &program) override;
};
} // namespace draw
} // namespace eng