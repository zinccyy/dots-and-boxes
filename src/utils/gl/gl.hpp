#pragma once

#include <glm/glm.hpp>

#include <string>

namespace utils
{
namespace gl
{
glm::vec3 parseHexRGB(const std::string &rgb);
}
} // namespace utils