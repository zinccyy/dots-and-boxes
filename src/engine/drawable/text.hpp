#pragma once

#include <engine/drawable.hpp>
#include <engine/drawable/character.hpp>
#include <string>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

namespace eng
{
namespace draw
{
struct Text : public IDrawable
{
    glm::vec2 StartPosition;
    glm::vec3 Color;
    float Scale;

    Text(FontCharactersMap &fm, const glm::vec2 &pos);

    void setText(const std::string &text);

    int setupBuffers() override;
    void draw(gl::ShaderProgram &program) override;
    void windowResize(const glm::vec2 &win_size) override;

  private:
    std::string mText;
    glm::mat4 mOrtho;
    FontCharactersMap &mFontMap;
};
} // namespace draw
} // namespace eng