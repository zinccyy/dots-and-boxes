#include <utils/gl/shader_program.hpp>
#include <utils/fs/fs.hpp>
#include <utils/log.hpp>

namespace utils
{
namespace gl
{
// load shaders from files
int ShaderProgram::loadShadersFromSource(const char *vShaderPath, const char *fShaderPath)
{
    int error = 0;
    auto vert_source = utils::fs::loadFileToString(vShaderPath);
    auto frag_source = utils::fs::loadFileToString(fShaderPath);

    utils::log::debug("vertex shader source: \n%s\n", vert_source->c_str());
    utils::log::debug("fragment shader source: \n%s\n", frag_source->c_str());

    if (!vert_source.has_value() || !frag_source.has_value())
    {
        return -1;
    }

    mVertShader = glCreateShader(GL_VERTEX_SHADER);
    mFragShader = glCreateShader(GL_FRAGMENT_SHADER);

    auto vs_ptr = vert_source->c_str();
    auto fs_ptr = frag_source->c_str();

    glShaderSource(mVertShader, 1, &vs_ptr, nullptr);
    glShaderSource(mFragShader, 1, &fs_ptr, nullptr);

    glCompileShader(mVertShader);
    assert(mCheckCompilation(mVertShader) == 0);
    glCompileShader(mFragShader);
    assert(mCheckCompilation(mFragShader) == 0);

    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, mVertShader);
    glAttachShader(mShaderProgram, mFragShader);
    glLinkProgram(mShaderProgram);
    assert(mCheckLinking() == 0);

    glDeleteShader(mVertShader);
    glDeleteShader(mFragShader);

    return error;
}

// uniform setters
void ShaderProgram::setUniform(const char *var, const glm::mat4 &mat)
{
    glUniformMatrix4fv(mGetUniformLocation(var), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setUniform(const char *var, const glm::vec3 &vec)
{
    glUniform3f(mGetUniformLocation(var), vec.x, vec.y, vec.z);
}

void ShaderProgram::setUniform(const char *var, const glm::vec2 &vec)
{
    glUniform2f(mGetUniformLocation(var), vec.x, vec.y);
}

void ShaderProgram::setUniform(const char *var, const float value)
{
    glUniform1f(mGetUniformLocation(var), value);
}

void ShaderProgram::setUniform(const char *var, const bool value)
{
    glUniform1i(mGetUniformLocation(var), value);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(mShaderProgram);
}

// simplify getting uniform location
int ShaderProgram::mGetUniformLocation(const char *var)
{
    return glGetUniformLocation(mShaderProgram, var);
}

// check if the shader is compiled
int ShaderProgram::mCheckCompilation(unsigned int shader)
{
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        log::error("ERROR::SHADER::COMPILATION_FAILED:\n%s\n", info_log);
        return 1;
    }
    else
    {
        return 0;
    }
}

// check if all shaders linked correctly
int ShaderProgram::mCheckLinking()
{
    int success;
    char info_log[512];
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(mShaderProgram, sizeof(info_log), NULL, info_log);
        log::error("ERROR::PROGRAM::LINKING_FAILED:\n%s\n", info_log);
        return 1;
    }
    else
    {
        return 0;
    }
}
} // namespace gl
} // namespace utils
