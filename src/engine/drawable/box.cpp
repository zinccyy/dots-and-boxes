#include <engine/drawable/box.hpp>

#include <utils/gl/shader_program.hpp>

namespace eng
{
namespace draw
{
Box::Box() : mPosition(0, 0), mSize(0)
{
}
int Box::setupBuffers()
{
    int error = 0;
    return error;
}
void Box::draw(utils::gl::ShaderProgram &program, const glm::vec2 &win_size)
{
}
} // namespace draw
} // namespace eng