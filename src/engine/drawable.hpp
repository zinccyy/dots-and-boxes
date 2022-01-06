#pragma once

#include <utils/gl/shader_program.hpp>

namespace eng
{

using namespace utils;

class IDrawable
{
  public:
    // setup buffers (vertex array buffer, element buffer, data buffer) etc. - opengl stuff
    virtual int setupBuffers() = 0;

    // draw object to the opengl context
    virtual void draw(gl::ShaderProgram &program) = 0;

    // update matrices and other needed stuff once the window resizes - performance boost instead of recalculating matrices each draw() call
    virtual void windowResize(const glm::vec2 &win_size) = 0;

  protected:
    unsigned int mVBO;
    unsigned int mEBO;
    unsigned int mVAO;
};
} // namespace eng