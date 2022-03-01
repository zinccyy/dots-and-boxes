#include <engine/drawable/text.hpp>

namespace eng
{
namespace draw
{
Text::Text(FontCharactersMap &fm, const glm::vec2 &pos) : StartPosition(pos), mFontMap(fm)
{
}
void Text::setText(const std::string &text)
{
    mText = text;
}
int Text::setupBuffers()
{
    int error = 0;

    // VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // VBO
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    // attrib
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return error;
}
void Text::draw(gl::ShaderProgram &program)
{
    program.use();

    // uniforms
    program.setUniform("uCharColor", Color);
    program.setUniform("uOrtho", mOrtho);

    glActiveTexture(0);
    glBindVertexArray(mVAO);

    glm::vec2 xy = StartPosition;

    for (auto &c : mText)
    {
        auto ch = mFontMap[c];

        ch.Scale = this->Scale;

        float xpos = xy.x + ch.Bearing.x * Scale;
        float ypos = xy.y - (ch.Size.y - ch.Bearing.y) * Scale;

        float w = ch.Size.x * Scale;
        float h = ch.Size.y * Scale;

        // update VBO for each character
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f}, {xpos, ypos, 0.0f, 1.0f},     {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f}, {xpos + w, ypos, 1.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 0.0f},
        };
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        ch.draw(program);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        xy.x += (ch.Advance >> 6) * Scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    // unbind all
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Text::windowResize(const glm::vec2 &win_size)
{
    mOrtho = glm::ortho(0.f, win_size.x, 0.f, win_size.y);
}
} // namespace draw
} // namespace eng