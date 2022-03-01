#pragma once

#include <engine/drawable.hpp>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

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

    // setup position (starting position of the character) before the draw() call
    glm::vec2 Position;
    float Scale;

    Character();
    Character(char c, FT_Face face);

    int setupBuffers() override;
    void draw(gl::ShaderProgram &program) override;
    void windowResize(const glm::vec2 &win_size) override;
};
using FontCharactersMap = std::map<char, Character>;
} // namespace draw
} // namespace eng