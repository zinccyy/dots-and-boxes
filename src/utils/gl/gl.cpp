#include <utils/gl/gl.hpp>

#include <iostream>
#include <algorithm>
#include <cctype>

namespace utils
{
namespace gl
{
std::string toLower(const std::string &str);
int convertHexString(const std::string &color);
int convertHexChar(const char c);
glm::vec3 parseHexRGB(const std::string &rgb)
{
    // #ff5733 - example
    glm::vec3 color;

    const std::string r = toLower(rgb.substr(1, 2));
    const std::string g = toLower(rgb.substr(3, 2));
    const std::string b = toLower(rgb.substr(5, 2));

    color.r = convertHexString(r);
    color.g = convertHexString(g);
    color.b = convertHexString(b);

    return color;
}
std::string toLower(const std::string &str)
{
    std::string out;

    std::for_each(str.begin(), str.end(), [&out](const char c) { out.push_back(std::tolower(c)); });

    return out;
}
int convertHexString(const std::string &color)
{
    const auto a = color[0];
    const auto b = color[1];

    return convertHexChar(a) * 16 + convertHexChar(b);
}
int convertHexChar(const char c)
{
    if (c >= 'a' && c <= 'f')
    {
        return (c - 'a') + 10;
    }
    else if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else
    {
        return -1;
    }
}
} // namespace gl
} // namespace utils