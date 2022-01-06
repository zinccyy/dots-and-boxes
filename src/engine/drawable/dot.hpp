#pragma once

#include <engine/drawable.hpp>

namespace eng
{
namespace draw
{
struct Dot : public IDrawable
{
  public:
    // position of the dot
    glm::vec2 Position;

    // dot color
    glm::vec3 InnerColor;

    // border color
    glm::vec3 OuterColor;

    // radius of the dot
    float InnerRadius;

    // + border radius
    float OuterRadius;

    // enable smoothstep in the shader or not
    bool Smoothstep;

    // quad size
    float Size;

    // is the dot hovered or not -> shade with gray if it is
    bool Hovered;

    Dot();

    int setupBuffers() override;
    void draw(gl::ShaderProgram &program) override;
    void windowResize(const glm::vec2 &win_size) override;

    ~Dot();

  private:
    glm::mat4 mScale;
    glm::mat4 mTranslate;
    glm::mat4 mOrtho;
};
} // namespace draw
} // namespace eng