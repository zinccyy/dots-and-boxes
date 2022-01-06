#pragma once

#include <engine/drawable.hpp>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

namespace eng
{
namespace draw
{
struct Character : public IDrawable
{
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
    char Char;
    FT_Face FontFace;

    Character();
    Character(char c, FT_Face face);

    int setupBuffers() override;
    void draw(gl::ShaderProgram &program) override;
    void windowResize(const glm::vec2 &win_size) override;
};
} // namespace draw
} // namespace eng