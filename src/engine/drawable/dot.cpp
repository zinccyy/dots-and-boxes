#include <engine/drawable/dot.hpp>

#include <glm/gtx/string_cast.hpp>
#include <utils/gl/shader_program.hpp>
#include <utils/log.hpp>

#include <iostream>

namespace eng
{
namespace draw
{
Dot::Dot() : InnerRadius(0), OuterRadius(0), Size(0), Hovered(false), Smoothstep(false)
{
}
int Dot::setupBuffers()
{
    int error = 0;

    float vertices[] = {
        -1.f, -1.f, // BL
        1.f,  -1.f, // BR
        1.f,  1.f,  // TR
        -1.f, 1.f,  // TL
    };

    unsigned int indices[] = {
        0, 1, 2, //
        0, 3, 2, //
    };

    // VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // VBO
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO
    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // unbind
    glBindVertexArray(0);

    return error;
}
void Dot::windowResize(const glm::vec2 &win_size)
{
    mScale = glm::scale(glm::mat4(1), glm::vec3(glm::vec2(Size), 0));
    mTranslate = glm::translate(glm::mat4(1), glm::vec3(Position, 0));

    // reverse Y coordinate
    mOrtho = glm::ortho(0.f, win_size.x, win_size.y, 0.f);
}
void Dot::draw(utils::gl::ShaderProgram &program)
{
    // enable shaders
    program.use();

    program.setUniform("uPosition", Position);
    program.setUniform("uInnerColor", InnerColor);
    program.setUniform("uOuterColor", OuterColor);
    program.setUniform("uInnerRadius", InnerRadius);
    program.setUniform("uOuterRadius", OuterRadius);
    program.setUniform("uHovered", Hovered);
    program.setUniform("uSmoothstep", Smoothstep);
    program.setUniform("uScale", mScale);
    program.setUniform("uTranslate", mTranslate);
    program.setUniform("uOrtho", mOrtho);

    // bind VAO
    glBindVertexArray(mVAO);

    // draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // unbind VAO for later
    glBindVertexArray(0);
}
Dot::~Dot()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}
} // namespace draw
} // namespace eng