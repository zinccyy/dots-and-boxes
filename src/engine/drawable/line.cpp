#include <engine/drawable/line.hpp>

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <math.h>
#include <utils/gl/shader_program.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>

namespace eng
{
namespace draw
{
Line::Line()
{
}
int Line::setupBuffers()
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
void Line::draw(utils::gl::ShaderProgram &program, const glm::vec2 &win_size)
{
    // enable shaders
    program.use();

    // calculate length
    const auto len = glm::distance(StartPosition, EndPosition) / 2;
    const auto angle = glm::orientedAngle(glm::normalize(glm::vec2(StartPosition.x + 1, StartPosition.y) - StartPosition), glm::normalize(EndPosition - StartPosition));

    auto start = StartPosition - glm::vec2(StartPosition.x / 2, 0);
    auto end = StartPosition + glm::vec2(StartPosition.x / 2, 0);

    auto scale = glm::scale(glm::mat4(1), glm::vec3(len, Height, 0));
    auto rotate = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));
    auto rot_start = glm::vec2(rotate * glm::vec4(start, 0, 1));
    auto rot_end = glm::vec2(rotate * glm::vec4(end, 0, 1));
    auto unit = (rot_end - rot_start) / glm::distance(rot_end, rot_start);
    auto trans_vec = StartPosition + unit * len;
    auto translate = glm::translate(glm::mat4(1), glm::vec3(trans_vec, 0));
    auto ortho = glm::ortho(0.f, win_size.x, win_size.y, 0.f);

    program.setUniform("uScale", scale);
    program.setUniform("uTranslate", translate);
    program.setUniform("uRotate", rotate);
    program.setUniform("uOrtho", ortho);
    program.setUniform("uColor", Color);

    // bind VAO
    glBindVertexArray(mVAO);

    // draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // unbind VAO for later
    glBindVertexArray(0);
}
void Line::updatePositions()
{
    StartPosition = ConnectedDots.first->Position;
    EndPosition = ConnectedDots.second->Position;
}

Line::~Line()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}
} // namespace draw
} // namespace eng