#pragma once

#include <engine/drawable.hpp>

namespace eng
{
namespace draw
{
class Box : public IDrawable
{
  public:
    // location of the box
    glm::vec2 Position;

    // WxH size of the box
    glm::vec2 Size;

    // color of the box
    glm::vec3 Color;

    // draw the box or not -> first needs to be rounded with lines for drawing to be enabled
    bool Draw;

    Box();
    ~Box();

    int setupBuffers() override;
    void draw(gl::ShaderProgram &program) override;
    void windowResize(const glm::vec2 &win_size) override;

  private:
    glm::mat4 mScale;
    glm::mat4 mTranslate;
    glm::mat4 mOrtho;
};
} // namespace draw
} // namespace eng