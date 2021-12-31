#pragma once

// GL
#include <GL/glew.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace utils
{
namespace gl
{
class ShaderProgram
{
  public:
    // init data
    ShaderProgram() : mVertShader(0), mFragShader(0), mShaderProgram(0)
    {
    }

    // load shaders from files
    int loadShadersFromSource(const char *vShaderPath, const char *fShaderPath);

    // bind the shader program
    inline void use() const
    {
        glUseProgram(mShaderProgram);
    }

    // return opengl shader program ID
    inline unsigned int getGLProgram() const
    {
        return mShaderProgram;
    }

    // uniform setters
    void setUniform(const char *var, const glm::mat4 &mat);
    void setUniform(const char *var, const glm::vec3 &vec);
    void setUniform(const char *var, const glm::vec2 &vec);
    void setUniform(const char *var, const float value);
    void setUniform(const char *var, const bool value);

    // delete shader program
    ~ShaderProgram();

  private:
    // helpers

    // simplify getting uniform location
    int mGetUniformLocation(const char *var);

    // check if the shader is compiled
    int mCheckCompilation(unsigned int shader);

    // check if all shaders linked correctly
    int mCheckLinking();

    // data
    unsigned int mVertShader;
    unsigned int mFragShader;
    unsigned int mShaderProgram;
};
} // namespace gl
} // namespace utils
