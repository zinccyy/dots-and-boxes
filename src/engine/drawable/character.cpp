#include <engine/drawable/character.hpp>

#include <utils/log.hpp>

namespace eng
{
namespace draw
{
Character::Character()
{
}
Character::Character(char c, FT_Face face) : Char(c), FontFace(face)
{
}

int Character::setupBuffers()
{
    int error = 0;

    if (FT_Load_Char(FontFace, Char, FT_LOAD_RENDER))
    {
        utils::log::error("unable to load char %c from font face", Char);
        return -1;
    }

    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FontFace->glyph->bitmap.width, FontFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, FontFace->glyph->bitmap.buffer);

    // setup texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    this->Size = glm::ivec2(FontFace->glyph->bitmap.width, FontFace->glyph->bitmap.rows);
    this->Bearing = glm::ivec2(FontFace->glyph->bitmap_left, FontFace->glyph->bitmap_top);
    this->Advance = FontFace->glyph->advance.x;

    return error;
}

void Character::draw(gl::ShaderProgram &program, const glm::vec2 &win_size)
{
}
} // namespace draw
} // namespace eng