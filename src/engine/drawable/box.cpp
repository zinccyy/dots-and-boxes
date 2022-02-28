#include <engine/drawable/box.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <utils/gl/shader_program.hpp>

namespace eng
{
namespace draw
{

Box::Box() : Position(0), Size(0), Color(0), Draw(false)
{
}

int Box::setupBuffers()
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

void Box::draw(utils::gl::ShaderProgram &program)
{
    if (!Draw)
    {
        return;
    }

    program.use();

    program.setUniform("uColor", Color);
    program.setUniform("uScale", mScale);
    program.setUniform("uTranslate", mTranslate);
    program.setUniform("uOrtho", mOrtho);

    glBindVertexArray(mVAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Box::windowResize(const glm::vec2 &win_size)
{
    mScale = glm::scale(glm::mat4(1), glm::vec3(Size, 0));
    mTranslate = glm::translate(glm::mat4(1), glm::vec3(Position, 0));

    // reverse Y coordinate
    mOrtho = glm::ortho(0.f, win_size.x, win_size.y, 0.f);
}

Box::~Box()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

} // namespace draw
} // namespace eng